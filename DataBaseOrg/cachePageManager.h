#pragma once
#include "Values.h"
#include <unordered_map>
#include <stdexcept>

template <typename T>
class cachePageManager;

// Cache manager trzyma wskaüniki do obiektÛw w pamiÍci, a RAIIhandler jest odpowiedzialny za zarzπdzanie tymi wskaünikami.
// Kiedy raii jest out of scope , automatycznie odpinamy obiekt z cachePageManager i go zapisujemy jeúli jest dirty.


// RAII pilnuje przed kopiowaniem w≥aúciwego wskaünika, a takøe automatycznie odpinanie obiektu z cachePageManager przy wyjúciu z zakresu
template <typename T>
class RAIIhandler
{
	T* ptr;
	cachePageManager<T>* manager;
	int pageID;
public:
	// Only allow construction with a pointer to T
	explicit RAIIhandler(T* p, cachePageManager<T>* m, int id) : ptr(p), manager(m), pageID(id) {}

	// Automatically delete the managed object when RAIIHandler goes out of scope
	~RAIIhandler()
	{
		manager->release(pageID);
	}
	
	const T& read()
	{
		return *ptr;
	}

	T& get() 
	{
		manager->markDirty(pageID);
		return *ptr;
	}

	RAIIhandler(const RAIIhandler&) = delete;
	RAIIhandler& operator=(const RAIIhandler&) = delete;
};

template <typename T>
class cachePageManager
{
	// Cache for storing objects in memory
	std::unordered_map<int, T> cacheMap;
	// Mark for pinned objects that should not be evicted from cache
	std::unordered_map<int, int> pinnedPages;
	// Mark for locked objects that should not fetched for other threads
	std::unordered_map<int, bool> lockedPages;
	// Mark for locked objects that should not fetched for other threads
	std::unordered_map<int, bool> dirtyPages;
public:
	// Find stored object and return handler
	const RAIIhandler<T> get(int pageID) 
	{
		if (cacheMap.find(pageID) == cacheMap.end())
		{
			throw std::runtime_error("Trying to get a page that is not in cache");
		}

		if (lockedPages.contains(pageID))
		{
			throw std::runtime_error("Trying to get a locked page");
		}

		// Pin the page to prevent eviction while in use
		pinnedPages[pageID]++;

		auto it = cacheMap.find(pageID);
		return RAIIhandler<T>(&it->second, this, pageID);
	}

	void release(int pageID)
	{
		auto it = cacheMap.find(pageID);
		if (it == cacheMap.end())
			throw std::runtime_error("Page is not in cache");

		if (lockedPages.contains(pageID))
			lockedPages.erase(pageID); // koniec edit()
		else if (pinnedPages.contains(pageID))
			unpin(pageID);             // koniec jednego get()
		else
			throw std::runtime_error("Page is not in use");

		if(dirtyPages.contains(pageID))
		{
			// Here you would typically write the dirty page back to disk or database
			// For this example, we'll just mark it as clean
			dirtyPages.erase(pageID);
		}
	}

	void markClean(int pageID) 
	{
		if (cacheMap.find(pageID) == cacheMap.end())
		{
			throw std::runtime_error("Trying to mark clean a page that is not in cache");
		}
		if (dirtyPages.contains(pageID))
		{
			dirtyPages.erase(pageID); // Mark the page as clean
		}
	}

	// Add object to cache, returns the pageID of the inserted object
	void insert(T obj, int pageID) 
	{
		if (cacheMap.contains(pageID))
		{
			throw std::runtime_error("Trying to insert a page that is already in cache");
		}

		dirtyPages[pageID] = true; // Mark the page as dirty

		cacheMap[pageID] = obj;
	}

	// Remove object from cache
	bool remove(int pageID)
	{
		if (pinnedPages.contains(pageID) || lockedPages.contains(pageID) || dirtyPages.contains(pageID))
		{
			return 0; // Cannot remove pinned, locked or dirty pages
		}
		else 
		{
			cacheMap.erase(pageID);
			return 1;
		}
	}

	// Mark an object as changed
	void markDirty(int pageID) 
	{
		if (cacheMap.find(pageID) == cacheMap.end())
		{
			throw std::runtime_error("Trying to mark dirty a page that is not in cache");
		}

		if (lockedPages.contains(pageID))
		{
			throw std::runtime_error("Trying to mark dirty a locked page");
		}

		dirtyPages[pageID] = true; // Mark the page as dirty
	}

	// Mark an object as pinned (not to be evicted)
	void pin(int pageID)
	{
		if (cacheMap.find(pageID) == cacheMap.end())
		{
			throw std::runtime_error("Cannot pin a page that is not in cache");
		}

		++pinnedPages[pageID];
	}

	// Mark an object as unpinned (can be evicted)
	void unpin(int pageID) 
	{
		if (cacheMap.find(pageID) == cacheMap.end())
		{
			throw std::runtime_error("Cannot unpin a page that is not in cache");
		}

		if (!pinnedPages.contains(pageID))
		{
			throw std::runtime_error("Cannot unpin a page that is not pinned");
		}

		--pinnedPages[pageID];

		if (pinnedPages[pageID] <= 0)
		{
			pinnedPages.erase(pageID);
		}
	}

	std::vector<int> getDirtyPages();
};