#include <string>
#include <vector>
#include <memory>
#include "Values.h"

#pragma once
/*
	DESCRIPTION:

	ustawia indexy do wierszy wed³ug wartoœci komórki w danej kolumnie

    korzysta ze zewnêtrznych klas dziedzicz¹cych po "IndexInterface*
*/

struct Label
{
    Value val;
    std::vector<int> tableIndex;
};

class IndexInterface
{
public:

    // Ustaw index do kontenera wed³ug jego wartoœci  
    virtual void insert(const Value& key, int rowID) = 0;

    // Wszystkie wiersze maj¹ce wartoœæ
    virtual std::vector<int> retrieve(const Value& key) const = 0;

    // usuwa wed³ug wartoœci jeden wiersz
    virtual bool remove(const Value& key, int rowID) = 0;

    // usuwa wszystkie wyst¹pienia 
    virtual std::size_t removeByVal(const Value& key) = 0;

    // edytuje wartoœæ 
    virtual void edit(const Value& oldVal, const Value& newVal, int rowID) = 0;

    virtual void clear() = 0;

    virtual std::size_t size() const = 0;
};