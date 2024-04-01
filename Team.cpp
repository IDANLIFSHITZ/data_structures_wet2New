//
// Created by yuval on 3/27/2024.
//

#include "Team.h"

Team::Team(int id) : id(id), playersList(new LinkedList()),
                     playersTree(new AVL<Player*, Pair<int,int>>()), numOfPlayers(0)
{
}


Team::~Team()
{
    delete playersList;
    playersTree->clear();
    delete playersTree;
}

StatusType Team::addPlayer(int strength)
{
    try
    {
        Player* player = new Player(numOfPlayers, strength);
        numOfPlayers++;
        playersList->push(player);
        Pair<int, int> pair(strength, player->getId());
        playersTree->insert(player, pair);
    }
    catch (const std::bad_alloc& e)
    {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType Team::removeNewestPlayer()
{
    if (numOfPlayers == 0)
    {
        return StatusType::FAILURE;
    }
    Pair<int,int> pair(playersList->head->player->getStrength(), playersList->head->player->getId());
    playersList->pop();
    playersTree->remove(pair);
    numOfPlayers--;
    return StatusType::SUCCESS;
}

int Team::getId() const
{
    return id;
}

int Team::getNumOfPlayers() const
{
    return numOfPlayers;
}

StatusType Team::uniteTeams(Team* other){
    playersList->uniteLists(other->playersList);

    int mySize = playersTree->get_size();
    int otherSize = other->playersTree->get_size();
    int unitedSize = mySize + otherSize;

    Player** myPlayersArray;
    Player** otherPlayersArray;
    Pair<int,int>* myKeysArray;
    Pair<int,int>* otherKeysArray;

    try
    {
        myPlayersArray = new Player*[mySize];
    }
    catch (const std::bad_alloc& e)
    {
        return StatusType::ALLOCATION_ERROR;
    }
    try
    {
        otherPlayersArray = new Player*[otherSize];
    }
    catch (const std::bad_alloc& e)
    {
        delete[] myPlayersArray;
        return StatusType::ALLOCATION_ERROR;
    }
    try
    {
        myKeysArray = new Pair<int,int>[mySize];
    }
    catch (const std::bad_alloc& e)
    {
        delete[] myPlayersArray;
        delete[] otherPlayersArray;
        return StatusType::ALLOCATION_ERROR;
    }
    try
    {
        otherKeysArray = new Pair<int,int>[otherSize];
    }
    catch (const std::bad_alloc& e)
    {
        delete[] myPlayersArray;
        delete[] otherPlayersArray;
        delete[] myKeysArray;
        return StatusType::ALLOCATION_ERROR;
    }

    Pair<Player**, Pair<int,int>* > myArrays (myPlayersArray, myKeysArray);
    Pair<Player**, Pair<int,int>* > otherArrays (otherPlayersArray, otherKeysArray);

    Player** unitedPlayersArray;
    Pair<int,int>* unitedKeysArray;


    playersTree->tree_to_sorted_array(myArrays.get_first(), myArrays.get_second());


    other->playersTree->tree_to_sorted_array(otherArrays.get_first(), otherArrays.get_second());

    try
    {
        unitedPlayersArray = new Player*[mySize + otherSize];
    }
    catch (const std::bad_alloc& e)
    {
        deleteArraysFromPair(&myArrays);
        deleteArraysFromPair(&otherArrays);
        return StatusType::ALLOCATION_ERROR;
    }
    try
    {
        unitedKeysArray = new Pair<int,int>[mySize + otherSize];
    }
    catch (const std::bad_alloc& e)
    {
        deleteArraysFromPair(&myArrays);
        deleteArraysFromPair(&otherArrays);
        delete[] unitedPlayersArray;
        return StatusType::ALLOCATION_ERROR;
    }

    mergeArrays(myArrays.get_first(), mySize, otherArrays.get_first(), otherSize, unitedPlayersArray, unitedKeysArray);

    deleteArraysFromPair(&myArrays);
    deleteArraysFromPair(&otherArrays);

    delete playersTree;

    try
    {
        playersTree = new AVL<Player *, Pair<int, int>>(unitedPlayersArray, unitedKeysArray, unitedSize);
    }
    catch (const std::bad_alloc& e)
    {
        delete[] unitedPlayersArray;
        delete[] unitedKeysArray;
        return StatusType::ALLOCATION_ERROR;
    }

    delete other->playersList;
    other->playersList = nullptr;

    delete other->playersTree;
    other->playersTree = nullptr;

    delete[] unitedPlayersArray;
    delete[] unitedKeysArray;

    return StatusType::SUCCESS;
}

StatusType deleteArraysFromPair(const Pair<Player**, Pair<int,int>* >* pair)
{
    Player** playersArray = pair->get_first();
    Pair<int,int>* keysArray = pair->get_second();

    delete[] playersArray;
    delete[] keysArray;

    return StatusType::SUCCESS;
}


StatusType mergeArrays(Player** arr1, int size1, Player** arr2, int size2, Player** mergedPlayerArr, Pair<int,int>* mergedKeysArr){
    int i = 0, j = 0, k = 0;
    while (i < size1 && j < size2){
        if (arr1[i] < arr2[j]){
            mergedPlayerArr[k] = arr1[i];
            Pair<int,int> pair(arr1[i]->getStrength(), arr1[i]->getId());
            mergedKeysArr[k] = pair;
            i++;
        } else {
            mergedPlayerArr[k] = arr2[j];
            Pair<int,int> pair(arr1[j]->getStrength(), arr1[j]->getId());
            mergedKeysArr[k] = pair;
            j++;
        }
        k++;
    }
    while (i < size1){
        mergedPlayerArr[k] = arr1[i];
        Pair<int,int> pair(arr1[i]->getStrength(), arr1[i]->getId());
        mergedKeysArr[k] = pair;
        i++;
        k++;
    }
    while (j < size2){
        mergedPlayerArr[k] = arr2[j];
        Pair<int,int> pair(arr2[j]->getStrength(), arr2[j]->getId());
        mergedKeysArr[k] = pair;
        j++;
        k++;
    }
    return StatusType::SUCCESS;
}

int Team::getStrength() const
{
    if (numOfPlayers == 0)
    {
        return 0;
    }
    Player* player = playersTree->get_median();
    return player->getStrength();
}