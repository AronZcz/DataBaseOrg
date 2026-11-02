#include "Column.h"
#include "pageManager.h"

Column::Column(){}

// Get existing
Column::Column(int usedTreeID, columnMetadata metaData)
{
    this->usedTreeID = usedTreeID;
    this->metaData = metaData;
    auto handler = pageManager::get<B_Tree>(usedTreeID);
   
    if(handler.isEmpty())
    {
        this->usedTreeID = -1;
        throw std::exception("Tree doesn't exists. Data is corrupted");
    }
    else
    {
        const B_Tree& index = handler.read();
        this->dType = index.type;
    }
}


// Create new
Column::Column(DataType dType, columnMetadata metaData)
{
    this->metaData = metaData;
    this->dType = dType;
    B_Tree index;
    index = B_Tree(dType, 3, -1);

    usedTreeID = pageManager::create(index);
}

void Column::addRecord(Value& v, int rowID)
{
    auto handler = pageManager::get<B_Tree>(usedTreeID);
    B_Tree& index = handler.get();

    if(checkFlagReq(v))
    {
        index.insert(v, rowID);
        pageManager::store<B_Tree>(usedTreeID);
    }
    else 
    {
        throw std::exception("Error occurred while adding data to Column");
    }
}

bool Column::removeRecord(Value& v,int rowID)
{
    auto handler = pageManager::get<B_Tree>(usedTreeID);
    B_Tree& index = handler.get();

    if(index.remove(v, rowID))
    {
        pageManager::store<B_Tree>(usedTreeID);
        return true;
    }
    else 
    {
        pageManager::store<B_Tree>(usedTreeID);
        return false;
    }
}

bool Column::modifyRecord(Value& old_, Value new_, int rowID)
{
    auto handler = pageManager::get<B_Tree>(usedTreeID);
    B_Tree& index = handler.get();

    if (checkFlagReq(new_))
    {
        if (index.edit(old_, new_, rowID))
        {
            pageManager::store<B_Tree>(usedTreeID);
            return true;
        }
        else
        {
            pageManager::store<B_Tree>(usedTreeID);
            return false;
        }
    }
}

void Column::addFlag(columnFlag f)
{
    metaData.setFlag(f);
}

void Column::dropFlag(columnFlag f)
{
    metaData.remFlag(f);
}

Value Column::getValueByRowID(int rowID) const
{
    return this->getAllRecords().at(rowID);
}

allData_t Column::getAllRecords() const
{
    auto handler = pageManager::get<B_Tree>(usedTreeID);
    const B_Tree& index = handler.read();

    return index.getAllData();
}

bool Column::checkFlagReq(Value& v)
{
    RAIIhandler<B_Tree> handler = pageManager::get<B_Tree>(usedTreeID);
    const B_Tree& index = handler.read();

    // UNIQUE & PK
    if(metaData.is_PK() || metaData.is_UNIQUE())
    {
        if(index.retrieve(v).size() > 0)
        {
            return false;
        }
    }

    if (metaData.is_NOT_NULL())
    {
        // IF 0 then 0 = 0 so v is 0
        if (Values::isNull(v))
        {
            return false;
        }
    }


    return true;
}
