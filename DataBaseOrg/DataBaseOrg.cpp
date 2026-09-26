#include <iostream>
#include "Values.h"
#include "B_Tree.h"


int main()
{
    B_Tree tree = B_Tree(DataType::Int, 4);
    IndexInterface* index = &tree;

    // INSERT
    index->insert(Value{ int64_t{10} }, 1);
    index->insert(Value{ int64_t{20} }, 2);
    index->insert(Value{ int64_t{30} }, 3);
    index->insert(Value{ int64_t{40} }, 4);
    index->insert(Value{ int64_t{50} }, 5);

    // Duplikaty tej samej wartości
    index->insert(Value{ int64_t{20} }, 6);
    index->insert(Value{ int64_t{20} }, 7);

    // Usinięcie 

    index->remove(Value{ int64_t{20} }, 2);

    index->edit(Value{ int64_t{10} }, Value{ int64_t{20} }, 1);

    // RETRIEVE
    std::vector<int> result = index->retrieve(Value{ int64_t{20} });

    std::cout << "Rows for value 20:\n";

    for (int rowID : result)
    {
        std::cout << rowID << "\n";
    }

    // Test nieistniejącej wartości
    std::vector<int> missing = index->retrieve(Value{ int64_t{999} });

    std::cout << "\nRows for value 999: "
        << missing.size() << "\n";



    return 0;
}