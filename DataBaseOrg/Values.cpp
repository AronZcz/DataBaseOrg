#include "Values.h"


template <typename T>
static int Values::miniCompare(T a, T b)
{
    if (a > b) { return 1; }
    else if (a < b) { return -1; }
    else { return 0; }
}


int8_t Values::compare(const Value a, const Value b, DataType type)
{
    bool aNull = std::holds_alternative<std::monostate>(a);
    bool bNull = std::holds_alternative<std::monostate>(b);

    if (aNull && bNull) return 0;
    if (aNull) return -1;
    if (bNull) return 1;

    switch (type)
    {
    case DataType::Int:
        return miniCompare(std::get<int64_t>(a), std::get<int64_t>(b));
    case DataType::Float:
        return miniCompare(std::get<float>(a), std::get<float>(b));
    case DataType::Double:
        return miniCompare(std::get<double>(a), std::get<double>(b));
    case DataType::String:
        return miniCompare(std::get<std::string>(a), std::get<std::string>(b));
    case DataType::Boolean:
        return miniCompare(std::get<bool>(a), std::get<bool>(b));
    default:
        return 2;
        break;
    }
}

bool Values::isNull(const Value v)
{
    return std::holds_alternative<std::monostate>(v);
}