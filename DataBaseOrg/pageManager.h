#pragma once

#include <iostream>
#include <vector>
#include "cachePageManager.h"
#include "discPageManager.h"
#include "B_Tree.h"


// First look up in cache then in disc, if not found return nullptr

// Adding first occurs in disc then in cache because id must be given by disc manager.

// LATER if id exceeds max id then new file must be created with new menager to that file


static class pageManager
{
	static inline SerializerBTreeNode bTreeNodeSerializer {};
	static inline SerializerBTree bTreeSerializer {};

	static inline cachePageManager<BTreeNode> cachedNodes{};
	static inline discPageManager<BTreeNode> storedNodes{ discPageManager<BTreeNode>(&bTreeNodeSerializer, "SerializedNodes.bin", 4096) };

	static inline cachePageManager<B_Tree> cachedTrees{};
	static inline discPageManager<B_Tree> storedTrees{ discPageManager<B_Tree>(&bTreeSerializer, "SerializedTrees.bin", 4096) };

	// Later saving metadata about databases, settings, etc. can be added here

	// Later change to four funcs that can distinguish where what goes

public:
	// NODES // 

	// Get node from cache or disc, if not found return nullptr.
	static RAIIhandler<BTreeNode> getNode(int pageID)
	{
		BTreeNode* node;
		BTreeNode liveNode;
		// CHECK cache
		node = cachedNodes.get(pageID);
		if(node != nullptr)
		{
			// IS in cache
			return RAIIhandler<BTreeNode>(node, &cachedNodes, pageID);
		}
		// TRY disc
		else 
		{
			if(storedNodes.deserializeObject(pageID, liveNode))
			{
				// Added to cache
				cachedNodes.load(liveNode, pageID);
				node = cachedNodes.get(pageID);
				return RAIIhandler<BTreeNode>(node, &cachedNodes, pageID);
			}else
			{
				// DO not exist
				return RAIIhandler<BTreeNode>(nullptr, &cachedNodes, pageID);
			}
		}
	}

	// store node in disc form cache, mark it as clean.
	static void storeNode(int pageID)
	{
		BTreeNode* node {};
		node = cachedNodes.get(pageID);

		if(node != nullptr)
		{
			storedNodes.modifyPage(pageID, *node);
			cachedNodes.markClean(pageID);
		}
	}

	// Remove node from cache and disc, if it exists.
	static void removeNode(int pageID);

	// Creates page in sorage and then put it in cache.
	static int createNode(BTreeNode &node)
	{
		int pageID = storedNodes.createPage(node);
		cachedNodes.insert(node, pageID);

		return pageID;
	}

	
	// TREES // 

	//Get tree from cache or disc, if not found return nullptr.
	static RAIIhandler<B_Tree> getTree(int pageID)
	{
		B_Tree* tree {nullptr};
		B_Tree liveTree;
		// CHECK cache
		tree = cachedTrees.get(pageID);
		if (tree != nullptr)
		{
			// IS in cache
			return RAIIhandler<B_Tree>(tree, &cachedTrees, pageID);
		}
		// TRY disc
		else
		{
				if (storedTrees.deserializeObject(pageID, liveTree))
				{
					// Added to cache
					cachedTrees.load(liveTree, pageID);
					tree = cachedTrees.get(pageID);
					return RAIIhandler<B_Tree>(tree, &cachedTrees, pageID);
				}
				else
				{
					// DO not exist
					return RAIIhandler<B_Tree>(nullptr, &cachedTrees, pageID);
				}
			
		}
	}

	//// store node in disc form cache, mark it as clean.
	static void storeTree(int pageID)
	{
		B_Tree* tree{};
		tree = cachedTrees.get(pageID);

		if (tree != nullptr)
		{
			storedTrees.modifyPage(pageID, *tree);
			cachedTrees.markClean(pageID);
		}
	}

	//// Remove node from cache and disc, if it exists.
	static void removeTree(int pageID);

	//// Creates page in sorage and then put it in cache.
	static int createTree(B_Tree& tree)
	{
		int pageID = storedTrees.createPage(tree);
		cachedTrees.insert(tree, pageID);

		return pageID;
	}

	static void clearCache()
	{
		cachedNodes.clear();
		cachedTrees.clear();
	}

	static void flushAll()
	{
		std::vector<int> dirtyNodes{ cachedNodes.getDirtyPages() };
		std::vector<int> dirtyTrees{ cachedTrees.getDirtyPages() };

		// save nodes

		for (int id : dirtyNodes)
		{
			storeNode(id);
		}

		for (int id : dirtyTrees)
		{
			storeTree(id);
		}
	}
};