#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <chrono>
#include <thread>
#include "Table.h"
#include "Relation.h"
#include "functional"






namespace Engine
{
	extern int usedDB;


	void createDatabase(std::string);	// OK
	void useDatabase(std::string);		// OK
	std::vector<std::string> showDatabases();	// OK

	void createTable(std::string);	//OK EMPTY
	void createTable(std::string, std::vector<metadataFormat_t>, rowFormat_t); //TO IMPLEMENT

	std::vector<std::string> showTables();	//OK

	// Only modify table
	void modifyTable(std::string, std::string); //TO IMPLEMENT

	void dropTable(std::string);//TO IMPLEMENT

	// Only modify column
	void modifyColumn(std::string, std::string = "", columnMetadata, DataType); //TO IMPLEMENT

	// Data
	void insert(std::string, std::vector<insertionData>);	//OK
	Relation selectAll(std::string);	//OK
	Relation select(std::string, std::vector<std::string>, std::function<bool(const RelationRow&)> predicate); // OK

	// UPDATE age = 1 WHERE ...
	void update(Value, std::string, std::function<bool(const RelationRow&)> predicate);//TO IMPLEMENT

	// DELETE age WHERE ...
	void delete_(std::string, std::function<bool(const RelationRow&)> predicate);//TO IMPLEMENT


	// Workers loop

	void writerLoop();

};

