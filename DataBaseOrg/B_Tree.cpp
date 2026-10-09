#include "B_Tree.h"
#include <stdexcept>
#include "pageManager.h"


B_Tree::B_Tree(DataType type, int degree, int rootNodeId)
{
    this->rootNodeId = rootNodeId;
    this->type = type;
    this->t = degree;

    this->n = 2 * degree - 1;
}

void B_Tree::insert(const Value& key, int rowID)
{
    // OBSŁUGA braku
    if (rootNodeId == -1)
    {
        BTreeNode root;
        // pustego roota,
        // pusty root → utwórz pierwszy Label, ustaw root.leaf = true
        root.leaf = true;

        Label l;
        l.val = key;
        l.tableIndex.push_back(rowID);
        l.type = this->type;

        root.keys.push_back(l);

        rootNodeId = pageManager::createNode(root);
        return;
    }

    auto rootHandler = pageManager::getNode(rootNodeId);
    BTreeNode &root = rootHandler.get();

    // niepełnego roota,
    if (root.keys.size() >= this->n)
    {
        // pełnego roota
        // pełny root → utwórz newRoot, dodaj stary root jako Children[0],
        // zrób splitChild(newRoot, 0), potem root = newRoot
        

        // Przebuduj
        BTreeNode newRoot;
        newRoot.leaf = false;


        newRoot.ChildrensId.push_back(rootNodeId);
        splitChild(newRoot, 0);

        rootNodeId = pageManager::createNode(newRoot);
        auto rootHandler = pageManager::getNode(rootNodeId);
        BTreeNode& root = rootHandler.get();

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
        lt.type = this->type;

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

        auto childHandler = pageManager::getNode(node.ChildrensId[childIndex]);
        BTreeNode &child = childHandler.get();


        if(child.keys.size() >= this->n)
        {
            // 3. jeśli pełne -> splitChild(node, index)
            splitChild(node, childIndex);

            // 4. po splicie jeszcze raz zdecyduj,
            //   czy iść do lewego czy prawego dziecka 
            goto decideAgain;
        }
        
        // 5. wywołaj rekurencyjnie insertNonFull(...)

        insertNonFull(child, key, rowID);
    }
}

void B_Tree::splitChild(BTreeNode& parent, std::size_t childIndex)
{
    auto childHandler = pageManager::getNode(parent.ChildrensId[childIndex]);
    BTreeNode& child = childHandler.get();
    // wybrać środkowy Label,
    int midPoint = child.keys.size() / 2;

    Label midLabel = child.keys[midPoint];

    // utworzyć nowy node,
    BTreeNode newNode;

    newNode.leaf = child.leaf;

    std::vector<Label> rightPart(
        child.keys.begin() + midPoint + 1,
        child.keys.end()
    );

    std::vector<Label> leftPart(
        child.keys.begin(),
        child.keys.begin() + midPoint
    );

    // przenieść prawą połowę kluczy do nowego noda,
    

    for (Label& l : rightPart)
    {
        newNode.keys.push_back(l);
    }

   
    // zostawić lewą połowę w starym,
    child.keys.clear();
    for (Label& l : leftPart)
    {
        child.keys.push_back(l);
    }
    
    // wstawić środkowy klucz do parent.keys,

    parent.keys.insert(parent.keys.begin() + childIndex, midLabel);

    // Warunek sprawdzający, czy dzielony node nie jest liściem.

    if (!child.leaf)
    {
        // tutaj bierzesz vector Children splitowanego noda
        
        std::vector<int> rightPart(
            child.ChildrensId.begin() + midPoint + 1,
            child.ChildrensId.end()
            );

        std::vector<int> leftPart(
            child.ChildrensId.begin(),
            child.ChildrensId.begin() + midPoint + 1
        );

        // pierwszą część zostawiasz w starym nodzie

        child.ChildrensId.clear();

        for (int& l : leftPart)
        {
            child.ChildrensId.push_back(l);
        }

        // drugą część przenosisz do newNode.Children

        for (int& r : rightPart)
        {
            newNode.ChildrensId.push_back(r);
        }
    }


    // wstawić nowego childa do parent.Children zaraz za starym childem.

    int newNodeID = pageManager::createNode(newNode);


    parent.ChildrensId.insert(
        parent.ChildrensId.begin() + childIndex + 1,
        newNodeID
    );
}

std::vector<int> B_Tree::retrieve(const Value& key) const
{
    int heldNodeID = rootNodeId;

    if (rootNodeId == -1) return {};

    while (true)
    {
        auto heldNodeHandler = pageManager::getNode(heldNodeID);
        BTreeNode heldNode = heldNodeHandler.read();

        size_t childIndex = heldNode.keys.size();

        for (size_t i = 0; i < heldNode.keys.size(); i++)
        {
            int cmp = Values::compare(key, heldNode.keys[i].val, this->type);

            if (cmp == 0)
            {
                return heldNode.keys[i].tableIndex;
            }

            if (cmp < 0)
            {
                childIndex = i;
                break;
            }
        }

        if (heldNode.leaf)
        {
            return {};
        }

       
        heldNodeID = heldNode.ChildrensId[childIndex];
    }
}

bool B_Tree::remove(const Value& key, int rowID) 
{   
    int heldNodeID = rootNodeId;

    while (true)
    {
        auto heldNodeHandler = pageManager::getNode(heldNodeID);
        BTreeNode heldNode = heldNodeHandler.read();


        size_t childIndex = heldNode.keys.size();

        for (size_t i = 0; i < heldNode.keys.size(); i++)
        {
            int cmp = Values::compare(key, heldNode.keys[i].val, this->type);

            if (cmp == 0)
            {
                for (size_t j = 0; j < heldNode.keys[i].tableIndex.size(); j++)
                {
                    if(heldNode.keys[i].tableIndex[j] == rowID) 
                    {
                        heldNode.keys[i].tableIndex.erase(
                            heldNode.keys[i].tableIndex.begin() + j
                        );

                        return true;
                    }
                }
            }

            if (cmp < 0)
            {
                childIndex = i;
                break;
            }
        }

        if (heldNode.leaf)
        {
            return false;
        }

       

        heldNodeID = heldNode.ChildrensId[childIndex];
    }
}

std::vector<int> B_Tree::removeByVal(const Value& key) 
{
    int heldNodeID = rootNodeId;

    while (true)
    {
        auto heldNodeHandler = pageManager::getNode(heldNodeID);
        BTreeNode heldNode = heldNodeHandler.read();

        size_t childIndex = heldNode.keys.size();

        for (size_t i = 0; i < heldNode.keys.size(); i++)
        {
            int cmp = Values::compare(key, heldNode.keys[i].val, this->type);

            if (cmp == 0)
            {
                std::vector<int> tmp = std::move(heldNode.keys[i].tableIndex);
                heldNode.keys[i].tableIndex.clear();

                return tmp;
            }

            if (cmp < 0)
            {
                childIndex = i;
                break;
            }
        }

        if (heldNode.leaf)
        {
            return {};
        }

        heldNodeID = heldNode.ChildrensId[childIndex];
    }
}

bool B_Tree::edit(const Value& oldVal, const Value& newVal, int rowID) 
{
	if (remove(oldVal, rowID))
	{
		insert(newVal, rowID);
        return true;
	}
    return false;
}

void B_Tree::clear() { throw std::logic_error("not implemented"); };

std::size_t B_Tree::size() const { throw std::logic_error("not implemented"); };