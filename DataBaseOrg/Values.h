#pragma once

#include <variant>
#include <string>
#include <vector>


// integers for special: 0 = NULL, 1 = NaN, 2 = Infinity, 3 = -Infinity

enum class DataType
{
    Int,
    Float,
    Double,
    String,
    Boolean,
    Special,
};


using Value = std::variant<std::int64_t, float, double, std::string, bool>;


static class  Values
{
public:
    //  0, a == b
    //  1, a > b
    // -1, a < b
    // Unknown DataType ew Throw 
    template <typename T>
    static int miniCompare(T a, T b);

    static int8_t compare(const Value a, const Value b, DataType type);
};


struct Page 
{
    int pageId;
    // B size
    int pageSize;
    int offset;
};