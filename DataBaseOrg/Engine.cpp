#include "Engine.h"
#include "DataBase.h"
#include "pageManager.h"
#include "StoredDataBases.h"
#include "relationAlgebra.h"

int Engine::usedDB = -1;

void Engine::createDatabase(std::string name)
{
	auto storedDataBases = pageManager::get<StoredDataBases>(0);

	if (!storedDataBases.isEmpty())
	{
		StoredDataBases& liveObj = storedDataBases.get();

		if (!liveObj.databaseCatalog.contains(name))
		{
			DataBase db;

			liveObj.databaseCatalog[name] = pageManager::create<DataBase>(db);
			liveObj.storedDB.push_back(name);

			pageManager::flushAll();
		}
	}
	else
	{
		StoredDataBases sdb;
		DataBase db;

		sdb.databaseCatalog[name] = pageManager::create<DataBase>(db);
		sdb.storedDB.push_back(name);

		pageManager::create(sdb);
		pageManager::flushAll();
	}
}

std::vector<std::string> Engine::showDatabases()
{
	auto storedDataBases = pageManager::get<StoredDataBases>(0);

	if (storedDataBases.isEmpty())
	{
		return {};
	}

	const StoredDataBases& liveObj = storedDataBases.read();
	return liveObj.storedDB;
}

void Engine::useDatabase(std::string name)
{
	auto storedDataBases = pageManager::get<StoredDataBases>(0);

	if (!storedDataBases.isEmpty())
	{
		const StoredDataBases& liveObj = storedDataBases.read();
		if(liveObj.databaseCatalog.contains(name))
		{
			Engine::usedDB = liveObj.databaseCatalog.at(name);
		}
	}
}

void Engine::createTable(std::string name)
{
	if(Engine::usedDB >= 0)
	{
		auto dataBaseHandler = pageManager::get<DataBase>(usedDB);
		auto &dataBase = dataBaseHandler.get();

		dataBase.createTB(name);

		pageManager::flushAll();
	}
	else
	{
		throw std::exception("DB not selected");
	}
}

std::vector<std::string> Engine::showTables()
{
	if (Engine::usedDB >= 0)
	{
		auto dataBaseHandler = pageManager::get<DataBase>(usedDB);
		auto &dataBase = dataBaseHandler.read();

		return dataBase.showTB();
	}
	else
	{
		throw std::exception("DB not selected");
	}
}

void Engine::insert(std::string dest, std::vector<insertionData> row)
{
	if (Engine::usedDB >= 0)
	{
		auto dataBaseHandler = pageManager::get<DataBase>(usedDB);
		auto &dataBase = dataBaseHandler.get();

		int tableID = dataBase.getTableID(dest);

		auto tableHandler = pageManager::get<Table>(tableID);
		auto &table = tableHandler.get();

		table.pushDataRow(row);

		pageManager::flushAll();
	}
	else
	{
		throw std::exception("DB not selected");
	}
}

Relation Engine::selectAll(std::string dest)
{
	if (Engine::usedDB >= 0)
	{
		auto dataBaseHandler = pageManager::get<DataBase>(usedDB);
		auto &dataBase = dataBaseHandler.read();

		int tableID = dataBase.getTableID(dest);

		auto tableHandler = pageManager::get<Table>(tableID);
		auto &table = tableHandler.get();

		
		return table.getRelation();
	}
	else
	{
		throw std::exception("DB not selected");
	}
}

Relation Engine::select(std::string dest, std::vector<std::string> cols, std::function<bool(const RelationRow&)> predicate)
{
	Relation relation = Engine::selectAll(dest);

	if(cols.size() > 0)
	{
		relation = relationAlgebra::Projection(relation, cols);
	}

	return relationAlgebra::Selection(relation, predicate);

	pageManager::flushAll();
}

void Engine::writerLoop(std::chrono::milliseconds maxWait, std::size_t maxChanges)
{

}
