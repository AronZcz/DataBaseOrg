#pragma once
#include <unordered_map>
#include <vector>
#include "Table.h"



// Main database instance 

class DataBase
{
    friend class SerializerDataBase;

    std::string name;
    std::vector<std::string> tableNames;
    std::unordered_map<std::string, int> tableCatalog;

public:
    DataBase() = default;
    DataBase(std::string name);

    void createTB(std::string name);
    void dropTB(std::string name);
    std::vector<std::string> showTB() const;
    int getTableID(std::string name) const;
};