#pragma once

#include "Relation.h"
#include "functional"



namespace relationAlgebra 
{
	// Main functions for relational algebra operations

	// WHERE
	Relation Selection(const Relation& relation, std::function<bool(const RelationRow&)> predicate);

	// SELECT name, age 
	Relation Projection(const Relation& relation, const std::vector<std::string>& columnNames);


	Relation Union(const Relation& relation1, const Relation& relation2);


	Relation Difference(const Relation& relation1, const Relation& relation2);


	Relation CartesianProduct(const Relation& relation1, const Relation& relation2);

	// AS
	Relation Rename(const Relation& relation, const std::unordered_map<std::string, std::string>& renameMap);

	// Additional functions for relational algebra operations
}