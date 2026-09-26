#include "discPageManager.h"

// Initialize the discPageManager with the given serializerRule, this will set the position and offset to 0
template <typename T>
discPageManager <T>::discPageManager(serializerRule<T> serializer, int pos, int off) {}

// Read the page with the given pageID, if the page does not exist, return nullptr
template <typename T>
T* discPageManager <T>:: recreateObject(int pageID){}

// Create a new page with the given pageID and store the object in it, if the page already exists, overwrite it
template <typename T>
void discPageManager <T>::createPage(int pageID, T& obj){}
	
// Delete the page with the given pageID, if the page does not exist, do nothing
template <typename T>
void discPageManager <T>::deletePage(int pageID);

// Modify the page with the given pageID, if the page does not exist, create a new page with the given pageID and store the object in it
template <typename T>
void discPageManager <T>::modifyPage(int pageID, T& obj);

// Clear all pages from the file, this will delete all data in the file and reset the position and offset
template <typename T>
void discPageManager <T>::clear();

// Get the next available pageID for a new page, if no page is available, return -1
template <typename T>
int discPageManager <T>::getNextPageID();

// Find an empty page in the file and return its pageID, if no empty page is found, return -1
template <typename T>
int discPageManager <T>::findEmptyPage();
