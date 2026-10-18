#pragma once
#include <cstdint> 

#include "Index.h"
#include "B_Tree.h"
#include "Values.h"

enum class columnFlag : std::uint8_t
{
    PK = 1u << 0, // klucz g³ówny
    UNIQUE = 1u << 1, // wartoœci bez duplikatów
    NOT_NULL = 1u << 2, // wartoœæ wymagana
    AUTO_INC = 1u << 3, // automatyczne nadawanie liczby
    FK = 1u << 4, // klucz obcy
    HAS_DEFAULT = 1u << 5, // ma wartoœæ domyœln¹
    GENERATED = 1u << 6, // wartoœæ obliczana
    READ_ONLY = 1u << 7  // nie mo¿na zmieniæ jej bezpoœrednio
};

struct columnMetadata
{
	std::int8_t flag;

    columnMetadata(std::vector<columnFlag> flags) : flag{ 0 }
    {
        for (columnFlag f : flags)
            setFlag(f);
    }

    columnMetadata() : flag{0}
    {

    }

    void setFlag(columnFlag f)
    {
        flag |= static_cast<std::uint8_t>(f);
    }

    void remFlag(columnFlag f)
    {
        //A & ~B set to 0 when flag is 1 if flad is 0 set 0
        flag &= ~(static_cast<std::uint8_t>(f));
    }

    bool is_PK()
    {
        return (flag & (static_cast<std::uint8_t>(columnFlag::PK))) != 0;
    }
    bool is_UNIQUE()
    {
       return (flag & (static_cast<std::uint8_t>(columnFlag::UNIQUE))) != 0;
    }
    bool is_NOT_NULL()
    {
        return (flag & (static_cast<std::uint8_t>(columnFlag::NOT_NULL))) != 0;
    }
    bool is_AUTO_INC()
    {
        return (flag & (static_cast<std::uint8_t>(columnFlag::AUTO_INC))) != 0;
    }
    bool is_FK()
    {
        return (flag & (static_cast<std::uint8_t>(columnFlag::FK))) != 0;
    }
    bool is_HAS_DEFAULT()
    {
        return (flag & (static_cast<std::uint8_t>(columnFlag::HAS_DEFAULT))) != 0;
    }
    bool is_GENERATED()
    {
        return (flag & (static_cast<std::uint8_t>(columnFlag::GENERATED))) != 0;
    }
    bool is_READ_ONLY()
    {
        return (flag & (static_cast<std::uint8_t>(columnFlag::READ_ONLY))) != 0;
    }
};

// Table stores data about used rowIDs
class Column 
{
public:
    columnMetadata metaData;
    int usedTreeID;
    DataType dType;

    Column();
    // For loading Column if not exist;
    Column(int usedTreeID, columnMetadata metaData);

    // For creating new tree
    Column(DataType dType, columnMetadata metaData);

    void addRecord(Value& v, int rowID);

    bool removeRecord(Value& v,int rowID);

    bool modifyRecord(Value& old_, Value new_, int rowID);

    void addFlag(columnFlag f);

    void dropFlag(columnFlag f);

    // Universal func to call if value that is added matches req
    bool checkFlagReq(Value& v);
};