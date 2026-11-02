#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <stdexcept>
#include <algorithm>

#include "Engine.h"
#include "pageManager.h"
#include "StoredDataBases.h"
#include "DataBase.h"


int main()
{
    try
    {
        constexpr int recordCount = 100;

        auto require = [](bool condition, const std::string& message)
            {
                if (!condition)
                    throw std::runtime_error(message);
            };

        auto printRelation = [](const std::string& title, const Relation& relation)
            {
                std::cout << "\n=== " << title
                    << " | rekordow: " << relation.size() << " ===\n";

                for (const auto& column : relation.getColumns())
                    std::cout << column.name << '\t';
                std::cout << '\n';

                for (const auto& row : relation)
                {
                    for (const auto& value : row)
                    {
                        if (const auto* v = std::get_if<std::int64_t>(&value))
                            std::cout << *v;
                        else if (const auto* v = std::get_if<std::string>(&value))
                            std::cout << *v;
                        else
                            throw std::runtime_error("Nieoczekiwany typ danych");

                        std::cout << '\t';
                    }
                    std::cout << '\n';
                }
            };

        // Nowa nazwa chroni przed dopisaniem danych do poprzedniego testu.
        const auto databases = Engine::showDatabases();
        std::string databaseName = "EngineTest";
        int suffix = 1;

        while (std::find(databases.begin(), databases.end(), databaseName)
            != databases.end())
        {
            databaseName = "EngineTest_" + std::to_string(suffix++);
        }

        Engine::createDatabase(databaseName);
        Engine::useDatabase(databaseName);

        require(Engine::usedDB >= 0,
            "Engine wymaga ID bazy > 0. Sprawdz obsluge bazy o ID 0.");

        const std::vector<std::string> tables = { "TabelaA", "TabelaB" };

        for (const auto& tableName : tables)
        {
            Engine::createTable(tableName);

            {
                auto database = pageManager::get<DataBase>(Engine::usedDB);
                const int tableID = database.read().getTableID(tableName);

                auto tableHandler = pageManager::get<Table>(tableID);
                auto& table = tableHandler.get();
                table.addColum("id", DataType::Int, columnMetadata{});
                table.addColum("nazwa", DataType::String, columnMetadata{});
            }

            pageManager::flushAll();

            for (int id = 1; id <= recordCount; ++id)
            {
                Engine::insert(tableName, {
                    {
                        DataType::Int,
                        Value{ static_cast<std::int64_t>(id) },
                        "id"
                    },
                    {
                        DataType::String,
                        Value{ tableName + "_" + std::to_string(id) },
                        "nazwa"
                    }
                    });
            }
        }

        require(Engine::showTables().size() == 2,
            "Baza powinna zawierac dokladnie dwie tabele");

        // Sprawdza liczbe, unikalnosc i wartosci rekordow.
        auto verify = [&](const Relation& relation,
            const std::string& tableName,
            int divisor)
            {
                require(relation.size() == recordCount / divisor,
                    tableName + ": niepoprawna liczba rekordow");

                const auto& columns = relation.getColumns();
                require(columns.size() == 2,
                    tableName + ": niepoprawna liczba kolumn");
                require(columns[0].name == "id" &&
                    columns[0].type == DataType::Int &&
                    columns[1].name == "nazwa" &&
                    columns[1].type == DataType::String,
                    tableName + ": niepoprawny schemat");

                std::unordered_set<std::int64_t> ids;

                for (const auto& row : relation)
                {
                    require(row.size() == 2, "Niepoprawny rozmiar rekordu");

                    const auto id = std::get<std::int64_t>(row.at(0));
                    const auto& name = std::get<std::string>(row.at(1));

                    require(id >= 1 && id <= recordCount && id % divisor == 0,
                        tableName + ": niepoprawne ID");
                    require(ids.insert(id).second,
                        tableName + ": powtorzone ID");
                    require(name == tableName + "_" + std::to_string(id),
                        tableName + ": niepoprawna nazwa");
                }
            };

        // Wypisanie obu tabel przed pierwszym select.
        for (const auto& tableName : tables)
        {
            const Relation before = Engine::selectAll(tableName);
            printRelation(tableName + " PRZED SELECT", before);
            verify(before, tableName, 1);
        }

        for (std::size_t i = 0; i < tables.size(); ++i)
        {
            const auto& tableName = tables[i];

            // TabelaA: parzyste ID -> 2500 rekordow.
            // TabelaB: ID podzielne przez 5 -> 1000 rekordow.
            const int divisor = (i == 0) ? 2 : 5;

            const Relation result = Engine::select(
                tableName,
                { "id", "nazwa" },
                [divisor](const RelationRow& row)
                {
                    return std::get<std::int64_t>(row.at(0)) % divisor == 0;
                }
            );

            printRelation(tableName + " PO SELECT (wynik)", result);
            verify(result, tableName, divisor);
        }

        // SELECT nie powinien zmieniac zadnej tabeli zrodlowej.
        for (const auto& tableName : tables)
            verify(Engine::selectAll(tableName), tableName, 1);

        std::cout << "\nTEST OK: dwie tabele po 5000 rekordow."
            << "\nSELECT: 2500 rekordow z TabelaA i 1000 z TabelaB.\n";

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "\nTEST NIEUDANY: " << error.what() << '\n';
        return 1;
    }
}