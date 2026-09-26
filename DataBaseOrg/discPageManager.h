#pragma once
#include "Values.h"
#include <vector>

template <typename T>
class serializerRule 
{
public:
	virtual std::vector<char> serialize(const T& obj) = 0;

	virtual T deserialize(const std::vector<char>& data) = 0;
};

template <typename T>
class discPageManager
{
	int position;
	int offset;

	// Read the page with the given pageID, if the page does not exist, return an empty vector
	std::vector<char> readPage(int pageID);
public:

	// Initialize the discPageManager with the given serializerRule, this will set the position and offset to 0
	discPageManager(serializerRule<T> serializer, int pos, int off);

	// Read the page with the given pageID, if the page does not exist, return nullptr
	T* recreateObject(int pageID);

	// Create a new page with the given pageID and store the object in it, if the page already exists, overwrite it
	void createPage(int pageID, T& obj);

	// Delete the page with the given pageID, if the page does not exist, do nothing
	void deletePage(int pageID);

	// Modify the page with the given pageID, if the page does not exist, create a new page with the given pageID and store the object in it
	void modifyPage(int pageID, T& obj);

	// Clear all pages from the file, this will delete all data in the file and reset the position and offset
	void clear();

	// Get the next available pageID for a new page, if no page is available, return -1
	int getNextPageID();

	// Find an empty page in the file and return its pageID, if no empty page is found, return -1
	int findEmptyPage();
};