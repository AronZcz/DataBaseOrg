#include "Table.h"
#include "pageManager.h"

Table::Table(std::vector<std::string> names, rowFormat_t rowFormat, metadataFormat_t metadataFormat)
{
	this->rowFormat = rowFormat;
	for (std::string s : names)
	{
		this->names.push_back(s);

		Column col = Column(rowFormat[s], metadataFormat[s]);
		int columnID = pageManager::createColumn(col);

		columnsIDs[s] = columnID;
	}
}

Table::Table(std::vector<std::string> names, std::vector<int> columnsIDs, int nextID)
{
	this->nextID = nextID;

	int el {};
	for (std::string s : names)
	{
		// ASSIGN
		this->names.push_back(s);
		this->columnsIDs[s] = columnsIDs[el];

		// CHECK DType
		RAIIhandler<Column> columnHandler = pageManager::getColumn(columnsIDs[el]);
		const Column& column = columnHandler.read();
		
		rowFormat[s] = column.dType;
		el += 1;
	}
}

void Table::addColum(std::string name, DataType dType, columnMetadata cm)
{
	this->rowFormat[name] = dType;
	this->names.push_back(name);

	Column col = Column(dType, cm);
	int columnID = pageManager::createColumn(col);

	columnsIDs[name] = columnID;
}

void Table::dropColumn(std::string name)
{
	this->rowFormat.erase(name);
	this->columnsIDs.erase(name);
}

void Table::changeColumnName(std::string old_, std::string new_)
{
	this->columnsIDs[new_] = this->columnsIDs[old_];
	this->rowFormat[new_] = this->rowFormat[old_];

	this->columnsIDs.erase(old_);
	this->rowFormat.erase(old_);
}

void Table::pushDataRow(std::vector<insertionData> row)
{
	std::vector < std::pair<Value, int>> tmpRow;
	std::vector<std::string> usedColNames;

	for(insertionData inDat : row)
	{
		auto it = rowFormat.find(inDat.name);
		if(it != rowFormat.end() && it->second == inDat.dType)
		{
			auto tmpID = columnsIDs.at(inDat.name);

			// CHECK DType
			RAIIhandler<Column> columnHandler = pageManager::getColumn(tmpID);
			Column& column = columnHandler.get();

			usedColNames.push_back(inDat.name);
			column.addRecord(inDat.v, this->nextID);
			pageManager::storeColumn(tmpID);
		}
		else
		{
			throw std::exception{ "Data Type missmatch" };
		}
	}

	// Fill empty columns with NULL
	for(std::string n : names)
	{
		bool wasUsed = false;
		// Standard search for now
		for (std::string un : usedColNames)
		{
			if(n == un)
			{
				wasUsed = true;
				break;
			}
		}
		if (!wasUsed) 
		{
			auto tmpID = columnsIDs.at(n);
			RAIIhandler<Column> columnHandler = pageManager::getColumn(tmpID);
			Column& column = columnHandler.get();

			// NULL VAL
			Value v = Value{ std::monostate{} };


			column.addRecord(v, this->nextID);
			pageManager::storeColumn(tmpID);
		}
	}
	

	nextID += 1;
}

void Table::changeColumnMetadata(std::string name, metadataFormat_t)
{}

void Table::changeColumnType(std::string name, DataType)
{}

void Table::defragmentation()
{

}
 