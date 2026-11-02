#include "relationAlgebra.h"


// Take a relation and a predicate function, and return a new relation containing only the rows that satisfy the predicate

Relation relationAlgebra::Selection(const Relation& relation, std::function<bool(const RelationRow&)> predicate)
{
    Relation newRelation(relation.getColumns());
    for (const RelationRow& row : relation)
    {
        if (predicate(row))
		{
			newRelation.addRow(row);
		}
    }

    return newRelation;
}

Relation relationAlgebra::Projection(const Relation& relation, const std::vector<std::string>& columnNames)
{
    // Build new columns based on the provided column names
    std::vector<RelationColumn> newColumns;
    
    for (const auto& columnName : columnNames)
    {
    	for (const auto& column : relation.getColumns())
		{
			if (column.name == columnName)
			{
				newColumns.push_back(column);
				break;
			}
		}
    }

	// New relation with the selected columns
    Relation newRelation(newColumns);

	// Fill the new relation with the corresponding values from the original relation
    for (const RelationRow& row : relation)
	{
		RelationRow newRow;
		for (const auto& columnName : columnNames)
		{
			for (std::size_t i = 0; i < relation.getColumns().size(); ++i)
			{
				if (relation.getColumns()[i].name == columnName)
				{
					newRow.push_back(row[i]);
					break;
				}
			}
		}
		newRelation.addRow(newRow);
	}

	return newRelation;
}