#include <iostream>
#include "Values.h"
#include "B_Tree.h"
#include "cachePageManager.h"


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


    // Test cache
    cachePageManager<int> cacheManager;
    cacheManager.insert(890, 1);
    cacheManager.markClean(1);

    
    {
        auto handler = cacheManager.get(1);
        handler.set(900);
        std::cout << handler.get() << '\n';
        std::cout << cacheManager.remove(1) << '\n'; // 0: strona przypięta
    }

    std::cout << cacheManager.remove(1);

    return 0;
}