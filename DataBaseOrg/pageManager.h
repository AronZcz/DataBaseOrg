#pragma once

#include <vector>
#include "cachePageManager.h"
#include "discPageManager.h"

#include "B_Tree.h"


class pageManager
{
	cachePageManager<BTreeNode> cachedNodes;
	discPageManager<BTreeNode> discNodes;

	// Later saving metadata about databases, settings, etc. can be added here

	BTreeNode getNodeFromCache(int pageID);
	BTreeNode getNodeFromDisc(int pageID);

	// Get node from cache or disc;
	BTreeNode getStoredNode(int pageID);

	// Get pointer to node in cache
	BTreeNode* getNodePointer(int pageID);

	// Store node in cache
	void storeInCacheNode(int pageID, BTreeNode node);

	// Store node on disc and mark it as clean
	void storeCachedNode(int pageID);

	// Free memory cached node and remove it from cache, if it is dirty, save it to disc first
	void removeFromCacheNode(int pageID);

	// Remove node from disc, if it is dirty, save it to disc first
	void removeFromDiscNode(int pageID);

public:
	
	// Save all changes to disk
	void dumpChanges();

	// Get node from cache or disc, if not found return nullptr
	BTreeNode* getNode(int pageID);

	// store node in cache and disc, if it already exists in cache, modify it and mark it as clean
	void storeNode(int pageID, BTreeNode node);

	// Remove node from cache and disc, if it exists
	void removeNode(int pageID);

	// Zmniejsza licznik przypiêæ
	void releaseNode(int pageID);    

	// Oznacza zmianê przez wskaŸnik
	void markNodeDirty(int pageID);  
};