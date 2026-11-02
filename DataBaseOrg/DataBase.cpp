#include "DataBase.h"
#include "pageManager.h"


// DB OPERS

// CREATE DATABASE {NAME}
DataBase::DataBase(std::string name)
{
	this->name = name;
}

// TB OPERS after DB selection
// names of tables in the current database
//std::vector<std::string> tableNames;
// Map for resolving id by name 
//std::unordered_map<std::string, int> tableCatalog;
// CREATE TABLE {NAME} {(DTYPE_0, {COL_0_NAME}), (DTYPE_1, {COL_1_NAME}), ... , (DTYPE_n, {COL_n_NAME})}
void DataBase::createTB(std::string name)
{
	if(!tableCatalog.contains(name))
	{
		Table table;
		int id = pageManager::create(table);

		tableNames.push_back(name);
		tableCatalog[name] = id;
	}
}

// DROP TABLE {NAME}
void DataBase::dropTB(std::string name)
{
	if(tableCatalog.contains(name))
	{
		pageManager::remove<Table>(tableCatalog[name]);
	}

	tableCatalog.erase(name);
	int pos{};
	for(std::string s : tableNames)
	{
		if(s == name)
		{
			tableNames.erase(tableNames.begin() + pos);
			break;
		}
		pos += 1;
	}
}

std::vector <std::string> DataBase::showTB() const
{
	return tableNames;
}

int DataBase::getTableID(std::string name) const
{
	if(tableCatalog.contains(name))
	{
		return tableCatalog.at(name);
	}
	return -1;
}