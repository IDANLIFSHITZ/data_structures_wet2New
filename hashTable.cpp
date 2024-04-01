
//
// Created by yuval on 3/27/2024.
//

#include "hashTable.h"
#include "wet2util.h"

int hashTable::hashFunction(int key, int size)
{
    if (size != 0)
        return key % size;
    return 0;
}

hashTable::hashTable() : maxSize(0), currSize(0), table(nullptr)
{
}

hashTable::~hashTable()
{
    if (table == nullptr)
    {
        return;
    }
    for (int i = 0; i < maxSize; i++)
    {
        delete table[i];
    }
    delete[] table;
}

StatusType addAux(AVL<Team*, int>** table, int key, Team* value, int index)
{
    try
    {
        if (table[index] == nullptr)
        {
            table[index] = new AVL<Team*, int>();
        }
        table[index]->insert(value, key);
    } catch (const std::bad_alloc& e)
    {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}



StatusType hashTable::add(Team* value, int key)
{
    // if the table is empty, create a new table of size 1
    if (table == nullptr)
    {
        AVL<Team*, int>* pointerToTree;
        try
        {
            table = new AVL<Team *, int> *[1];
        }
        catch (const std::bad_alloc& e)
        {
            return StatusType::ALLOCATION_ERROR;
        }
        try
        {
            pointerToTree = new AVL<Team*, int>();
            pointerToTree->insert(value, key);
            table[0] = pointerToTree;
        }
        catch (const std::bad_alloc& e)
        {
            delete[] table;
            return StatusType::ALLOCATION_ERROR;
        }
        maxSize++;
        currSize++;
        return StatusType::SUCCESS;
    }

    // if the element is already in the table
    if (find(key).status() == StatusType::SUCCESS)
    {
        return StatusType::FAILURE;
    }

    // check resize
    if (currSize >= maxSize/2)
    {
        StatusType s = resize(maxSize * 2);
        if (s == StatusType::ALLOCATION_ERROR)
        {
            return StatusType::ALLOCATION_ERROR;
        }
    }

    // the cell is empty or has a tree with a few teams in it
    int index = hashFunction(key, maxSize);
    StatusType status = addAux(table, key, value, index);

    if (status != StatusType::SUCCESS)
    {
        return status;
    }
    currSize++;

    return status;
}

StatusType hashTable::remove(int key)

{
    int index = hashFunction(key, maxSize);

    // if the table is empty
    if (table == nullptr)
    {
        return StatusType::FAILURE;
    }

//     if the element is not in the table
//    if (find(key).status() == StatusType::FAILURE)
//    {
//        return StatusType::FAILURE;
//    }


    StatusType status = table[index]->remove(key);
    if (status != StatusType::SUCCESS)
    {
        return status;
    }
    currSize--;

    // it one of the trees has emptied, delete it
    if (table[index]->get_size() == 0)
    {
        delete table[index];
        table[index] = nullptr;
    }

    if (currSize <= maxSize / 4)
    {
        StatusType statusType = resize(maxSize / 2);
        if (statusType == StatusType::ALLOCATION_ERROR)
        {
            return StatusType::ALLOCATION_ERROR;
        }
    }
    return StatusType::SUCCESS;
}

StatusType hashTable::resize(int newSize)
{
    if (newSize == 0)
    {
        delete[] table;
        table = nullptr;
        maxSize = 0;
        currSize = 0;
    }

    Team** TeamsArray;
    int* keysArray;
    AVL<Team *, int>** newTable;

    try
    {
        newTable = new AVL<Team *, int> *[newSize];
        for (int i = 0; i < newSize; i++)
        {
            newTable[i] = nullptr;
        }
    }
    catch (const std::bad_alloc& e)
    {
        return StatusType::ALLOCATION_ERROR;
    }

    // iterating over the old table and inserting the elements to the new table
    for (int i = 0; i < maxSize; i++)
    {
        if (table[i] == nullptr)
            // assert (table[i]->get_size() == 0)
        {
            delete table[i];
            continue;
        }
        AVL<Team*, int>* current = table[i];

        try
        {
            TeamsArray = new Team*[current->get_size()];
        }
        catch (const std::bad_alloc& e)
        {
            delete[] newTable;
            return StatusType::ALLOCATION_ERROR;
        }
        try
        {
            keysArray = new int[current->get_size()];
        }
        catch (const std::bad_alloc& e)
        {
            delete[] newTable;
            delete[] TeamsArray;
            return StatusType::ALLOCATION_ERROR;
        }
        current->tree_to_sorted_array(TeamsArray, keysArray);

        for (int j = 0; j < current->get_size(); j++)
        {
            Team* value = TeamsArray[j];
            int key = keysArray[j];
            int index = hashFunction(key, newSize);
            StatusType status = addAux(newTable, key, value, index);
            if (status != StatusType::SUCCESS)
            {
                return status;
            }
        }
        delete[] TeamsArray;
        delete[] keysArray;
        delete current;
    }

    delete[] table;
    table = newTable;
    maxSize = newSize;
    return StatusType::SUCCESS;
}


output_t<Team*> hashTable::find(int key)
{
    if (currSize == 0)
    {
        return output_t<Team*>( StatusType::FAILURE);
    }

    int index = hashFunction(key, maxSize);
    if (table[index] == nullptr)
    {
        return output_t<Team*>( StatusType::FAILURE);
    }
    Team* team = table[index]->search(key);
    return (team != nullptr) ? output_t<Team*>(team) : output_t<Team*>(StatusType::FAILURE);
}

int hashTable::getSize() const
{
    return currSize;
}

int hashTable::isEmpty() const
{
    return currSize == 0;
}
