#pragma once
#include "Column.h"
#include <unordered_map>

// For assigning dType to names
typedef std::unordered_map<std::string, DataType> rowFormat_t;
// For assigning names to metadata goes to column
typedef std::unordered_map<std::string, columnMetadata> metadataFormat_t;
// Builderd row for result
typedef std::unordered_map<int, std::vector<Value>> Row_t;
// Result array
typedef std::unordered_map <std::string, std::vector<Row_t>> resultArray_t;


// 1. TABLE is input and output for relational algebra 

// 2. resultArray is output for user to read data. Last product of Query.


// for INSERT
struct insertionData
{
	DataType dType; // Type
	Value v;	// What
	std::string name; // Where
};


// name to stored id pagrMenager relation
typedef std::unordered_map<std::string, int> columnsID_t;

class Table
{
	rowFormat_t rowFormat;
	std::vector<std::string> names;
	columnsID_t columnsIDs;

	// index for new row;
	int nextID{};

public:
	// Creating new Table
	Table(std::vector<std::string> names, rowFormat_t rowFormat, metadataFormat_t metadataFormat);

	// Loading table
	Table(std::vector<std::string> names, std::vector<int> columnsIDs, int nextID);

	// ALTER TABLE oper
	void addColum(std::string name, DataType dType, columnMetadata cm);

	// DROP COLUMN oper
	void dropColumn(std::string name);

	// Edycja 
	void changeColumnName(std::string old_, std::string new_);

	// INSERT
	void pushDataRow(std::vector<insertionData> row);

	// LATER
	void changeColumnMetadata(std::string name, metadataFormat_t);

	// ALTER TABLE oper More complicated tha that need casting
	void changeColumnType(std::string name, DataType);

	// Check for rows that are empty and shrink nextID;
	void defragmentation();


};
