#include "B_Tree.h"
#include <stdexcept>

B_Tree::B_Tree(DataType type, int degree)
{
    this->root;
    this->type = type;
    this->t = degree;

    this->n = 2 * degree - 1;
}

void B_Tree::insert(const Value& key, int rowID)
{
    // OBSŁUGA
    
    if (root.keys.empty())
    {
        // pustego roota,
        // pusty root → utwórz pierwszy Label, ustaw root.leaf = true
        root.leaf = true;

        Label l;
        l.val = key;
        l.tableIndex.push_back(rowID);

        root.keys.push_back(l);
        return;
    }
    // niepełnego roota,
    else if (root.keys.size() >= this->n)
    {
        // pełnego roota
        // pełny root → utwórz newRoot, dodaj stary root jako Children[0],
        // zrób splitChild(newRoot, 0), potem root = newRoot
        

        // Przebuduj
        BTreeNode newRoot;
        newRoot.leaf = false;
        newRoot.Children.push_back(root);
        splitChild(newRoot, 0);
        root = newRoot;

        // Wstaw Label
        insertNonFull(root, key, rowID);
    }
    else
    {
        // niepełny root
        // niepełny root → insertNonFull(root, key, rowID)
        insertNonFull(root, key, rowID);
    }
}

void B_Tree::insertNonFull(BTreeNode& node, const Value& key, int rowID)
{
    // Sprawdz czy jest miejsce 
    if (!node.keys.empty())
    {
        for (Label& l : node.keys)
        {
            if (Values::compare(l.val, key, this->type) == 0)
            {
                l.tableIndex.push_back(rowID);
                return;
            }
        }
    }

    if(node.leaf)
    {
        // Jeśli nie dodaj nowy label
        Label lt;
        lt.val = key;
        lt.tableIndex.push_back(rowID);

        for (size_t i{ 0 }; i < node.keys.size(); i++)
        {
            if (Values::compare(lt.val, node.keys[i].val, this->type) < 0)
            {
                // Rozszerza vektor
                node.keys.insert(node.keys.begin() + i, lt);
                return;
            }
        }

        // największy dodany na końcu.
        node.keys.push_back(lt);
    }
    else
    {
        decideAgain:
        // 1. znajdź indeks dziecka, do którego trzeba zejść
        size_t childIndex = node.keys.size();

        for (size_t i = 0; i < node.keys.size(); i++)
        {
            if (Values::compare(key, node.keys[i].val, type) < 0)
            {
                childIndex = i;
                break;
            }
        }

        // 2. sprawdź czy to dziecko jest pełne
        if(node.Children[childIndex].keys.size() >= this->n)
        {
            // 3. jeśli pełne -> splitChild(node, index)
            splitChild(node, childIndex);

            // 4. po splicie jeszcze raz zdecyduj,
            //   czy iść do lewego czy prawego dziecka 
            goto decideAgain;
        }
        
        // 5. wywołaj rekurencyjnie insertNonFull(...)
        insertNonFull(node.Children[childIndex], key, rowID);
    }
}

void B_Tree::splitChild(BTreeNode& parent, std::size_t childIndex)
{
    // wybrać środkowy Label,
    int midPoint = parent.Children[childIndex].keys.size() / 2;
    Label midLabel = parent.Children[childIndex].keys[midPoint];

    // utworzyć nowy node,
    BTreeNode newNode;

    newNode.leaf = parent.Children[childIndex].leaf;

    std::vector<Label> rightPart(
        parent.Children[childIndex].keys.begin() + midPoint + 1,
        parent.Children[childIndex].keys.end()
    );

    std::vector<Label> leftPart(
        parent.Children[childIndex].keys.begin(),
        parent.Children[childIndex].keys.begin() + midPoint
    );

    // przenieść prawą połowę kluczy do nowego noda,
    

    for (Label& l : rightPart)
    {
        newNode.keys.push_back(l);
    }

   
    // zostawić lewą połowę w starym,
    parent.Children[childIndex].keys.clear();
    for (Label& l : leftPart)
    {
        parent.Children[childIndex].keys.push_back(l);
    }
    
    // wstawić środkowy klucz do parent.keys,

    parent.keys.insert(parent.keys.begin() + childIndex, midLabel);

    // Warunek sprawdzający, czy dzielony node nie jest liściem.

    if (!parent.Children[childIndex].leaf)
    {
        // tutaj bierzesz vector Children splitowanego noda
        
        std::vector<BTreeNode> rightPart(
                parent.Children[childIndex].Children.begin() + midPoint + 1,
                parent.Children[childIndex].Children.end()
            );

        std::vector<BTreeNode> leftPart(
            parent.Children[childIndex].Children.begin(),
            parent.Children[childIndex].Children.begin() + midPoint + 1
        );

        // pierwszą część zostawiasz w starym nodzie

        parent.Children[childIndex].Children.clear();

        for (BTreeNode& l : leftPart)
        {
            parent.Children[childIndex].Children.push_back(l);
        }

        // drugą część przenosisz do newNode.Children

        for (BTreeNode& r : rightPart)
        {
            newNode.Children.push_back(r);
        }
    }


    // wstawić nowego childa do parent.Children zaraz za starym childem.

    parent.Children.insert(
        parent.Children.begin() + childIndex + 1,
        newNode
    );
}

std::vector<int> B_Tree::retrieve(const Value& key) const
{
    const BTreeNode* heldNode = &root;

    while (true)
    {
        size_t childIndex = heldNode->keys.size();

        for (size_t i = 0; i < heldNode->keys.size(); i++)
        {
            int cmp = Values::compare(key, heldNode->keys[i].val, this->type);

            if (cmp == 0)
            {
                return heldNode->keys[i].tableIndex;
            }

            if (cmp < 0)
            {
                childIndex = i;
                break;
            }
        }

        if (heldNode->leaf)
        {
            return {};
        }

        heldNode = &heldNode->Children[childIndex];
    }
}

bool B_Tree::remove(const Value& key, int rowID) { throw std::logic_error("not implemented"); };

std::size_t B_Tree::removeByVal(const Value& key) { throw std::logic_error("not implemented"); };

void B_Tree::edit(const Value& oldVal, const Value& newVal, int rowID) { throw std::logic_error("not implemented"); };

void B_Tree::clear() { throw std::logic_error("not implemented"); };

std::size_t B_Tree::size() const { throw std::logic_error("not implemented"); };