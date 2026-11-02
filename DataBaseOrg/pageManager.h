#pragma once

#include <iostream>
#include <vector>
#include <type_traits>
#include "cachePageManager.h"
#include "discPageManager.h"
#include "B_Tree.h"
#include "Column.h"
#include "Table.h"
#include "DataBase.h"


// First look up in cache then in disc, if not found return nullptr

// Adding first occurs in disc then in cache because id must be given by disc manager.

// LATER if id exceeds max id then new file must be created with new menager to that file
static class pageManager
{
	static inline SerializerBTreeNode bTreeNodeSerializer {};
	static inline SerializerBTree bTreeSerializer {};
	static inline SerializerColumn columnSerializer{};
	static inline SerializerTable tableSerializer{};
	static inline SerializerDataBase databaseSerializer{};
    static inline SerializerStoredDataBases storedDataBasesSerializer{};
    static inline cachePageManager<StoredDataBases> cachedDatabaseCatalogs{};
    static inline discPageManager<StoredDataBases> storedDatabaseCatalogs{
        &storedDataBasesSerializer, "SerializedDatabaseCatalogs.bin", 256 };

	static inline cachePageManager<BTreeNode> cachedNodes{};
	static inline discPageManager<BTreeNode> storedNodes{ discPageManager<BTreeNode>(&bTreeNodeSerializer, "SerializedNodes.bin", 512) };

	static inline cachePageManager<B_Tree> cachedTrees{};
	static inline discPageManager<B_Tree> storedTrees{ discPageManager<B_Tree>(&bTreeSerializer, "SerializedTrees.bin", 512) };

	static inline cachePageManager<Column> cachedColumns{};
	static inline discPageManager<Column> storedColumns{ discPageManager<Column>(&columnSerializer, "SerializedColumns.bin", 512) };

	static inline cachePageManager<Table> cachedTables{};
	static inline discPageManager<Table> storedTables{ discPageManager<Table>(&tableSerializer, "SerializedTables.bin", 512) };
	
	static inline cachePageManager<DataBase> cachedDatabases{};
	static inline discPageManager<DataBase> storedDatabases{ discPageManager<DataBase>(&databaseSerializer, "SerializedDatabases.bin", 512)};
	// Later saving metadata about databases, settings, etc. can be added here
	// Later change to four funcs that can distinguish where what goes

public:
	// Generic page operations
	template <typename>
	static constexpr bool unsupportedType = false;

	template <typename T>
	static auto& cache()
	{
		if constexpr (std::is_same_v<T, BTreeNode>)
			return cachedNodes;
		else if constexpr (std::is_same_v<T, B_Tree>)
			return cachedTrees;
		else if constexpr (std::is_same_v<T, Column>)
			return cachedColumns;
		else if constexpr (std::is_same_v<T, Table>)
			return cachedTables;
		else if constexpr (std::is_same_v<T, DataBase>)
			return cachedDatabases;
		else if constexpr (std::is_same_v<T, StoredDataBases>)
			return cachedDatabaseCatalogs;
		else
			static_assert(unsupportedType<T>, "Nieobslugiwany typ");
	}

	template <typename T>
	static auto& storage()
	{
		if constexpr (std::is_same_v<T, BTreeNode>)
			return storedNodes;
		else if constexpr (std::is_same_v<T, B_Tree>)
			return storedTrees;
		else if constexpr (std::is_same_v<T, Column>)
			return storedColumns;
		else if constexpr (std::is_same_v<T, Table>)
			return storedTables;
		else if constexpr (std::is_same_v<T, DataBase>)
			return storedDatabases;
		else if constexpr (std::is_same_v<T, StoredDataBases>)
			return storedDatabaseCatalogs;
		else
			static_assert(unsupportedType<T>, "Nieobslugiwany typ");
	}

	template <typename T>
	static RAIIhandler<T> get(int pageID)
	{
		T* obj{ nullptr };
		T liveObj;
		// CHECK cache
		obj = cache<T>().get(pageID);
		if (obj != nullptr)
		{
			// IS in cache
			return RAIIhandler<T>(obj, &cache<T>(), pageID);
		}
		// TRY disc
		else
		{
			if (storage<T>().deserializeObject(pageID, liveObj))
			{
				// Added to cache
				cache<T>().load(liveObj, pageID);
				obj = cache<T>().get(pageID);
				return RAIIhandler<T>(obj, &cache<T>(), pageID);
			}
			else
			{
				// DO not exist
				return RAIIhandler<T>(nullptr, &cache<T>(), pageID);
			}
		}
	}

	template <typename T>
	static void store(int pageID)
	{
		T* obj{};
		obj = cache<T>().get(pageID);

		if (obj != nullptr)
		{
			storage<T>().modifyPage(pageID, *obj);
			cache<T>().markClean(pageID);
		}
	}

	template <typename T>
	static int create(T& obj)
	{
		int pageID = storage<T>().createPage(obj);
		cache<T>().insert(obj, pageID);

		return pageID;
	}

	template <typename T>
	static void remove(int pageID)
	{
		if (!cache<T>().remove(pageID))
			throw std::runtime_error("Cannot remove a pinned, locked or dirty page");

		storage<T>().deletePage(pageID);
	}

	// for emergency only
	static void forceRemoveNode(int pageID)
	{
		cachedNodes.forceRemove(pageID);
	}
	static void forceRemoveTree(int pageID)
	{
		cachedTrees.forceRemove(pageID);
	}
	static void forceRemoveColumn(int pageID)
	{
		cachedColumns.forceRemove(pageID);
	}
	static void forceRemoveCTable(int pageID)
	{
		cachedTables.forceRemove(pageID);
	}

	static void clearCache()
	{
		cachedNodes.clear();
		cachedTrees.clear();
		cachedColumns.clear();
		cachedTables.clear();
		cachedDatabases.clear();
		cachedDatabaseCatalogs.clear();
	}

	// Just save all
	static void flushAll()
	{
		std::vector<int> dirtyNodes{ cachedNodes.getDirtyPages() };
		std::vector<int> dirtyTrees{ cachedTrees.getDirtyPages() };
		std::vector<int> dirtyColumns{ cachedColumns.getDirtyPages() };
		std::vector<int> dirtyTables{ cachedTables.getDirtyPages() };

		// save nodes

		for (int id : dirtyNodes)
		{
			store<BTreeNode>(id);
		}

		for (int id : dirtyTrees)
		{
			store<B_Tree>(id);
		}

		for (int id : dirtyColumns)
		{
			store<Column>(id);
		}

		for (int id : dirtyTables)
		{
			store<Table>(id);
		}

		for (int id : cachedDatabases.getDirtyPages())
		{
			store<DataBase>(id);
		}

        for (int id : cachedDatabaseCatalogs.getDirtyPages())
            store<StoredDataBases>(id);
	}
};