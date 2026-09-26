#pragma once
#include "Values.h"
#include <unordered_map>


template <typename T>
class cachePagerManager
{
	std::unordered_map<int, T> cacheMap;
	std::vector<int> dirtyPages;
	std::vector<int> pinnedPages;
public:
	// Find stored object
	virtual T* get(int pageID) = 0;

	// Add object to cache
	virtual void insert(int pageID, T obj) = 0;

	// Remove object from cache
	virtual void remove(int pageID) = 0;

	// Clear all objects from cache
	virtual void clear() = 0;

	// Save all changed objects to disk
	virtual void dumpChanges() = 0;

	// Mark an object as changed
	virtual void markDirty(int pageID) = 0;

	// Mark an object as pinned (not to be evicted)
	virtual void pin(int pageID) = 0;

	// Mark an object as unpinned (can be evicted)
	virtual void unpin(int pageID) = 0;

	virtual ~cachePagerManager() = default;
};