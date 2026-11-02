#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class StoredDataBases
{
public:
    std::vector<std::string> storedDB;
    std::unordered_map<std::string, int> databaseCatalog;
};
