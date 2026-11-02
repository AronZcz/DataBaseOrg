#pragma once
#include <string.h>
#include "Values.h"
#include <stdexcept>
#include <unordered_map>


struct RelationColumn
{
	std::string name;
	DataType type;
};

using RelationRow = std::vector<Value>;

class Relation
{
	std::vector<RelationColumn> columns;
	std::vector<RelationRow> rows;

public:
	Relation(std::vector<RelationColumn> columns)
		: columns(std::move(columns))
	{
	}

	const std::vector<RelationColumn>& getColumns() const
	{
		return columns;
	}

	void addRow(RelationRow row)
	{
		if (row.size() != columns.size())
			throw std::invalid_argument("Nieprawidlowa liczba wartosci");

		rows.push_back(std::move(row));
	}

	auto begin() const { return rows.cbegin(); }
	auto end() const { return rows.cend(); }

	std::size_t size() const { return rows.size(); }
};