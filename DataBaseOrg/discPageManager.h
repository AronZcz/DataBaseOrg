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
public:
	virtual void init(serializerRule<T> serializer) = 0;

	virtual T readPage(int pageID) = 0;

	virtual void writePage(int pageID, T& obj) = 0;

	virtual void deletePage(int pageID) = 0;

	virtual void saveChanges() = 0;

	virtual void modifyPage(int pageID, T& obj) = 0;

	virtual void clear() = 0;
};