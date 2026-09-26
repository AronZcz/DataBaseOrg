#pragma once
#include <vector>
#include "Values.h"
#include "Index.h"


/*
	DESCRIPTION


	Podstawowy algorytm indeksowania
*/


struct BTreeNode {
    int PageID;
    std::vector<Label> keys;
    std::vector <BTreeNode> Children;
    bool leaf{ false };
};

class B_Tree : public IndexInterface
{
    // minimalne wychylenie
    int t;
    // l el
    int n;

    DataType type;
    BTreeNode root;

public:

    B_Tree(DataType type, int degree);  //IMP
    
    void insert(const Value& key, int rowID) override;  //IMP

    void insertNonFull(BTreeNode& node, const Value& key, int rowID);   //IMP

    void splitChild(BTreeNode& parent, std::size_t childIndex); //IMP

    std::vector<int> retrieve(const Value& key) const override; //IMP

    bool remove(const Value& key, int rowID) override;  //IMP

    std::vector<int> removeByVal(const Value& key) override; //IMP

    bool edit(const Value& oldVal, const Value& newVal, int rowID) override;    //NIMP

    void clear() override;  //NIMP

    std::size_t size() const override;  //NIMP
};