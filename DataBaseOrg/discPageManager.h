#pragma once
#include "Values.h"
#include "B_Tree.h"
#include "Table.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>


// "C:\Users\Aron\source\repos\DataBaseOrg\x64\node"

template <typename T>
class Serializer
{
public:
	virtual std::vector<char> serialize(const T& obj, int pageSize) = 0;
	virtual T deserialize(const std::vector<char>& data, int pageSize) = 0;
};

class SerializerInt : public Serializer<int>
{
public:
	std::vector<char> serialize(const int& obj, int pageSize)  override
	{
		// Bierze bity z inta i kopiuje je do wektora charów, który jest zwracany. Wektor ma rozmiar pageSize, a pozosta³e bajty s¹ wype³nione zerami.
		std::vector<char> data(pageSize, '\0');
		std::memcpy(data.data(), &obj, sizeof(int));

		return data;
	}
	int deserialize(const std::vector<char>& data, int pageSize) override
	{
		int obj;
		std::memcpy(&obj, data.data(), sizeof(int));

		return obj;
	}
};

class SerializerBTree : public Serializer<B_Tree>
{
public:
	virtual std::vector<char> serialize(const B_Tree& obj, int pageSize) 
	{
		std::vector<char> data(pageSize, '\0');

		std::memcpy(data.data(), &obj.rootNodeId, sizeof(int));
		std::memcpy(data.data() + sizeof(int), &obj.t, sizeof(int));
		std::memcpy(data.data() + sizeof(int) * 2, &obj.type, sizeof(int));

		return data;
	}
	virtual B_Tree deserialize(const std::vector<char>& data, int pageSize)
	{
		int rootNodeId, t, type;

		std::memcpy(&rootNodeId, data.data(), sizeof(int));
		std::memcpy(&t, data.data() + sizeof(int), sizeof(int));
		std::memcpy(&type, data.data() + sizeof(int) * 2, sizeof(int));

		B_Tree obj = B_Tree(static_cast<DataType>(type), t, rootNodeId);
		return obj;
	}
};

class SerializerLabel : public Serializer<Label>
{
public:
	std::vector<char> serialize(const Label& obj, int pageSize) override
	{
		std::vector<char> data(pageSize, '\0');
		std::size_t pos = 0;

		int type = static_cast<int>(obj.type);
		std::memcpy(data.data() + pos, &type, sizeof(type));
		pos += sizeof(type);

		int count = static_cast<int>(obj.tableIndex.size());
		std::memcpy(data.data() + pos, &count, sizeof(count));
		pos += sizeof(count);

		for (int rowID : obj.tableIndex)
		{
			std::memcpy(data.data() + pos, &rowID, sizeof(rowID));
			pos += sizeof(rowID);
		}
		// Special case for string and null

		if(obj.type == DataType::String)
		{
			std::string tmp = std::get<std::string>(obj.val);
			int length = tmp.length();

			std::memcpy(data.data() + pos, &length, sizeof(length));
			pos += sizeof(length);

			for (char c : tmp)
			{
				std::memcpy(data.data() + pos, &c, sizeof(c));
				pos += sizeof(c);
			}
		}
		else if(obj.type == DataType::Null || Values::isNull(obj.val))
		{
			bool value = Values::isNull(obj.val);
		}
		else
		{
			std::int64_t value = std::get<std::int64_t>(obj.val);
			std::memcpy(data.data() + pos, &value, sizeof(value));
		}


		return data;
	}

	Label deserialize(
		const std::vector<char>& data,
		int pageSize
	) override
	{
		Label result;
		std::size_t pos = 0;

		int type;
		std::memcpy(&type, data.data() + pos, sizeof(type));
		pos += sizeof(type);
		result.type = static_cast<DataType>(type);

		int count;
		std::memcpy(&count, data.data() + pos, sizeof(count));
		pos += sizeof(count);

		for (int i = 0; i < count; ++i)
		{
			int rowID;
			std::memcpy(&rowID, data.data() + pos, sizeof(rowID));
			pos += sizeof(rowID);
			result.tableIndex.push_back(rowID);
		}

		// Special case for string
		if (result.type == DataType::String)
		{
			std::string tmp {};
			int length;
			char c;

			std::memcpy(&length, data.data() + pos, sizeof(length));
			pos += sizeof(length);

			for (int i = 0; i < length; ++i)
			{
				char c;
				std::memcpy(&c, data.data() + pos, sizeof(char));
				pos += sizeof(c);
				tmp += c;
			}

			result.setVal(tmp);
		}
		else if (result.type == DataType::Null)
		{
			std::monostate value;
			result.setVal(value);
		}
		else
		{
			std::int64_t value;
			std::memcpy(&value, data.data() + pos, sizeof(value));
			result.setVal(value);
		}

		return result;
	}
};

class SerializerColumn : public Serializer<Column> 
{
public:
	SerializerColumn(){}

	std::vector<char> serialize(const Column& obj, int pageSize) override
	{
		std::vector<char> data(pageSize, '\0');
		std::size_t pos = 0;

		int usedTreeID = static_cast<int>(obj.usedTreeID);
		std::int8_t metadata = static_cast<std::int8_t>(obj.metaData.flag);

		std::memcpy(data.data() + pos, &usedTreeID, sizeof(usedTreeID));
		pos += sizeof(usedTreeID);

		std::memcpy(data.data() + pos, &metadata, sizeof(metadata));
		pos += sizeof(metadata);

		return data;
	}

	Column deserialize(const std::vector<char>& data, int pageSize) override 
	{
		Column result;
		std::size_t pos = 0;

		int usedTreeID = {};
		std::int8_t metadataFlag = {};


		std::memcpy(&usedTreeID, data.data() + pos, sizeof(usedTreeID));
		pos += sizeof(usedTreeID);

		std::memcpy(&metadataFlag, data.data() + pos, sizeof(metadataFlag));
		pos += sizeof(metadataFlag);

		columnMetadata metadata;
		metadata.flag = metadataFlag;

		result = Column(usedTreeID, metadata);

		return result;
	}
};

class SerializerBTreeNode : public Serializer<BTreeNode>
{
	SerializerLabel labelSerializer;

public:
	std::vector<char> serialize(
		const BTreeNode& obj,
		int pageSize
	) override
	{
		std::vector<char> data(pageSize, '\0');
		std::size_t pos = 0;

		int leaf = obj.leaf ? 1 : 0;
		std::memcpy(data.data() + pos, &leaf, sizeof(leaf));
		pos += sizeof(leaf);

		int childCount = static_cast<int>(obj.ChildrensId.size());
		std::memcpy(data.data() + pos, &childCount, sizeof(childCount));
		pos += sizeof(childCount);

		for (int childID : obj.ChildrensId)
		{
			std::memcpy(data.data() + pos, &childID, sizeof(childID));
			pos += sizeof(childID);
		}

		int keyCount = static_cast<int>(obj.keys.size());
		std::memcpy(data.data() + pos, &keyCount, sizeof(keyCount));
		pos += sizeof(keyCount);

		for (const Label& key : obj.keys)
		{
			int valueSize;

			if (key.type == DataType::String)
			{
				// SerializerLabel zapisuje: int length + bajty tekstu
				valueSize =
					static_cast<int>(sizeof(int)) +
					static_cast<int>(
						std::get<std::string>(key.val).size()
						);
			}
			else if (key.type == DataType::Null)
			{
				valueSize = 0;
			}
			else if (key.type == DataType::Int)
			{
				valueSize = static_cast<int>(sizeof(std::int64_t));
			}
			else
			{
				throw std::invalid_argument(
					"Nieobslugiwany typ Label"
				);
			}

			int labelSize =
				static_cast<int>(2 * sizeof(int)) +
				static_cast<int>(key.tableIndex.size() * sizeof(int)) +
				valueSize;

			std::vector<char> labelData =
				labelSerializer.serialize(key, labelSize);

			std::memcpy(
				data.data() + pos,
				&labelSize,
				sizeof(labelSize)
			);
			pos += sizeof(labelSize);

			std::memcpy(
				data.data() + pos,
				labelData.data(),
				labelData.size()
			);
			pos += labelData.size();
		}

		return data;
	}

	BTreeNode deserialize(
		const std::vector<char>& data,
		int pageSize
	) override
	{
		BTreeNode result{};
		result.PageID = -1;

		std::size_t pos = 0;

		int leaf;
		std::memcpy(&leaf, data.data() + pos, sizeof(leaf));
		pos += sizeof(leaf);
		result.leaf = (leaf != 0);

		int childCount;
		std::memcpy(&childCount, data.data() + pos, sizeof(childCount));
		pos += sizeof(childCount);

		for (int i = 0; i < childCount; ++i)
		{
			int childID;
			std::memcpy(&childID, data.data() + pos, sizeof(childID));
			pos += sizeof(childID);

			result.ChildrensId.push_back(childID);
		}

		int keyCount;
		std::memcpy(&keyCount, data.data() + pos, sizeof(keyCount));
		pos += sizeof(keyCount);

		for (int i = 0; i < keyCount; ++i)
		{
			int labelSize;
			std::memcpy(&labelSize, data.data() + pos, sizeof(labelSize));
			pos += sizeof(labelSize);

			std::vector<char> labelData(
				data.begin() + pos,
				data.begin() + pos + labelSize
			);
			pos += labelSize;

			result.keys.push_back(
				labelSerializer.deserialize(labelData, labelSize)
			);
		}

		return result;
	}
};

class SerializeTable : public Serializer<Table>
{};

template <typename T>
class discPageManager
{
	Serializer<T>* serializer;

	int offset = 32;

	std::fstream fileManager;
	int pageSize = 256;

	// Read the page with the given pageID, if the page does not exist, return an empty vector
	std::vector<char> readPage(int pageID)
	{
		if (pageID < 0) return {};

		std::vector<char> buffer(pageSize);

		fileManager.clear();
		fileManager.seekg(
			static_cast<std::streamoff>(pageID) * pageSize + offset,
			std::ios::beg
		);

		if (!fileManager.read(buffer.data(), pageSize))
			return {};

		return buffer;
	}

public:
	// Initialize the discPageManager with the given serializerRule, this will set the position and offset to 0
	discPageManager(Serializer<T> *serializer, std::string path , int off) : serializer(serializer), pageSize(off)
	{
		{
			std::ofstream create(path, std::ios::app | std::ios::binary);

			if (!create)
				throw std::runtime_error("Nie mozna utworzyc lub otworzyc pliku");
		}

		fileManager.open(
			path,
			std::ios::in | std::ios::out | std::ios::binary
		);

		if (!fileManager)
			throw std::runtime_error("Nie mozna otworzyc pliku");
	}

	// Read the page with the given pageID, if the page does not exist, return nullptr
	bool deserializeObject(int pageID, T& obj)
	{
		std::vector<char> buff = readPage(pageID);
		if (buff.empty())
		{
			return false;
		}
		else 
		{
			obj = serializer->deserialize(buff, pageSize);
			return true;
		}
	}

	// Create a new page with the given pageID and store the object in it
	// Creation of a new page will return the pageID of the newly created page
	int createPage(T obj)
	{
		int pageID = findEmptyPage();
		std::vector<char> data = serializer->serialize(obj, pageSize);

		fileManager.seekp(pageID * pageSize + offset, std::ios::beg);
		fileManager.write(data.data(), pageSize);

		return pageID;
	}

	// Delete the page with the given pageID, if the page does not exist, do nothing
	void deletePage(int pageID)
	{
		// Fill vect with zeros and paste it in destined space
		std::vector<char> emptyData(pageSize, '\0');

		fileManager.seekp(pageID * pageSize + offset, std::ios::beg);
		fileManager.write(emptyData.data(), pageSize);
	}

	// Modify the page with the given pageID
	void modifyPage(int pageID, T& obj)
	{
		std::vector<char> data = serializer->serialize(obj, pageSize);

		fileManager.seekp(pageID * pageSize + offset, std::ios::beg);
		fileManager.write(data.data(), pageSize);
	}

	// Find an empty page in the file and return its pageID, if no empty page is found, return next available pageID
	// For reducing fragmentation, this function will search for empty pages in the file and return the first one it finds
	int findEmptyPage() 
	{
		int pageCount = getPageCount();
		for (int i = 0; i < pageCount; i++)
		{
			std::vector<char> data = readPage(i);
			if (std::all_of(data.begin(), data.end(), [](char c) { return c == '\0'; }))
			{
				return i;
			}
		}
		return pageCount; // If no empty page is found, return next available pageID
	}

	int getPageCount()
	{
		fileManager.clear();
		fileManager.flush();

		if (!fileManager)
			throw std::runtime_error("Blad zapisu pliku");

		fileManager.seekg(0, std::ios::end);
		auto end = fileManager.tellg();

		if (end == std::streampos(-1))
			throw std::runtime_error("Nie mozna odczytac rozmiaru pliku");

		auto size = static_cast<std::streamoff>(end);

		if (size <= pageSize)
			return 0;

		return static_cast<int>((size - offset) / pageSize);
	}
};