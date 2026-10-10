#include <iostream>
#include "Values.h"
#include "B_Tree.h"
#include "pageManager.h"



int main()
{
    try 
    {
        auto treeHandler = pageManager::getTree(0);

        if(!treeHandler.isEmpty())
        {
            B_Tree &tree = treeHandler.get();

            IndexInterface* index = &tree;

            //index->insert(Value{ int64_t{10} }, 1);
            //index->insert(Value{ int64_t{20} }, 2);
            //index->insert(Value{ int64_t{30} }, 3);
            //index->insert(Value{ int64_t{40} }, 7);
            //index->insert(Value{ int64_t{120} }, 8);

            
            // RETRIEVE
            std::vector<int> result = index->retrieve(Value{ int64_t{40} });

            std::cout << "Rows for value 20:\n";

            for (int rowID : result)
            {
                std::cout << rowID << "\n";
            }

            // Err here
            // Save tree;
            //pageManager::flushAll();

            //pageManager::clearCache();
        }
    }
    catch (std::exception e)
    {
        std::cout << e.what();
        return 0;
    }
    
    

    
    
    //// Usunięcie 

    //index->remove(Value{ int64_t{20} }, 2);

    //index->edit(Value{ int64_t{10} }, Value{ int64_t{20} }, 1);

    //// RETRIEVE
    //std::vector<int> result = index->retrieve(Value{ int64_t{20} });

    //std::cout << "Rows for value 20:\n";

    //for (int rowID : result)
    //{
    //    std::cout << rowID << "\n";
    //}

    //// Test nieistniejącej wartości
    //std::vector<int> missing = index->retrieve(Value{ int64_t{999} });

    //std::cout << "\nRows with value 999: "
    //    << missing.size() << "\n";

    //// Test cache
    //cachePageManager<int> cacheManager;
    //cacheManager.insert(890, 1);
    //cacheManager.markClean(1);

    //
    //{
    //    auto handler = cacheManager.get(1);
    //    std::cout << handler.get() << '\n';
    //    std::cout << cacheManager.remove(1) << '\n'; // 0: strona przypięta
    //}

    //std::cout << cacheManager.remove(1);

    //// Test disc

    //Serializer<int>* serializer = new SerializerInt();
    //discPageManager<int> discManager(serializer,"C:\\Users\\Aron\\source\\repos\\DataBaseOrg\\x64\\Debug\\testfile.dat", 0);

    //int obj = discManager.deserializeObject(2);

    //std::cout << "\n\nDeserialized object: " << obj << std::endl;

    return 0;
}