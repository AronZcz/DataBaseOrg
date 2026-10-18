#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "B_Tree.h"
#include "pageManager.h"
#include "discPageManager.h"
#include "Table.h"



int main()
{
    rowFormat_t rowFormat;

    rowFormat["Name"] = DataType::String;
    rowFormat["Age"] = DataType::Int;

    std::vector<std::string> names = std::vector<std::string>({ "Name", "Age" });

    metadataFormat_t metadataFormat;
    metadataFormat["Name"].setFlag(columnFlag::NOT_NULL);
    metadataFormat["Age"].setFlag(columnFlag::NOT_NULL);

    Table table = Table(names, rowFormat, metadataFormat);

    insertionData nameVal;
    nameVal.dType = DataType::String;
    nameVal.name = "Name";
    nameVal.v = "Aron";

    insertionData ageVal;
    ageVal.dType = DataType::Int;
    ageVal.name = "Age";
    ageVal.v = 23;


    std::vector<insertionData> newRow;
    newRow.push_back(nameVal);
    newRow.push_back(ageVal);

    table.pushDataRow(newRow);
}