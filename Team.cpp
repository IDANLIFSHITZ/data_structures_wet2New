//
// Created by yuval on 3/27/2024.
//

#include "Team.h"

Team::Team(int id) : id(id),
                     playersList(new LinkedList()),
                     playersTree(new AVL<Player*, Pair<int,int>>()),
                     numOfPlayers(0){}


Team::~Team()
{
    delete playersList;
    if (playersTree != nullptr)
    {
        playersTree->clear();
    }
    delete playersTree;
}

StatusType Team::addPlayer(int strength)
{
    try
    {
        Player* player = new Player(this->numOfPlayers, strength);
        numOfPlayers++;
        playersList->push(player);
        Pair<int, int> pair(strength, player->getId());
        playersTree->insert(player, pair);
    }
    catch (const std::bad_alloc& err)
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

    Player** myPlayersArray = nullptr;
    Player** otherPlayersArray = nullptr;
    Pair<int,int>* myKeysArray = nullptr;
    Pair<int,int>* otherKeysArray = nullptr;

    try
    {
        myPlayersArray = new Player*[mySize];
        otherPlayersArray = new Player*[otherSize];
        myKeysArray = new Pair<int,int>[mySize];
        otherKeysArray = new Pair<int,int>[otherSize];
    }
    catch (const std::bad_alloc& err)
    {
        delete[] myPlayersArray;
        delete[] otherPlayersArray;
        delete[] myKeysArray;
        delete[] otherKeysArray;
        return StatusType::ALLOCATION_ERROR;
    }

    //fill arrays from trees.
    this->playersTree->tree_to_sorted_array(myPlayersArray, myKeysArray);
    other->playersTree->tree_to_sorted_array(otherPlayersArray, otherKeysArray);

    //create new arrays for merge.
    Player** unitedPlayersArray = nullptr;
    Pair<int,int>* unitedKeysArray = nullptr;
    try
    {
        unitedPlayersArray = new Player*[mySize + otherSize];
        unitedKeysArray = new Pair<int,int>[mySize + otherSize];
    }
    catch (const std::bad_alloc& err)
    {
        delete[] myPlayersArray;
        delete[] otherPlayersArray;
        delete[] myKeysArray;
        delete[] otherKeysArray;
        delete[] unitedPlayersArray;
        delete[] unitedKeysArray;
        return StatusType::ALLOCATION_ERROR;
    }

    //merge players arrays and delete old arrays after merge.
    mergeArrays(myPlayersArray, mySize, otherPlayersArray, otherSize, unitedPlayersArray, unitedKeysArray);

    delete[] myPlayersArray;
    delete[] otherPlayersArray;
    delete[] myKeysArray;
    delete[] otherKeysArray;

    //delete old tree to replace with merged tree and update size.
    delete this->playersTree;
    try
    {
        this->playersTree = new AVL<Player *, Pair<int, int>>(unitedPlayersArray, unitedKeysArray, unitedSize);
        this->numOfPlayers = this->playersList->size;
    }
    catch (const std::bad_alloc& err)
    {
        delete[] unitedPlayersArray;
        delete[] unitedKeysArray;
        return StatusType::ALLOCATION_ERROR;
    }

    //delete other team.
    delete other->playersTree;
    other->playersTree = nullptr;
    delete other;

    delete[] unitedPlayersArray;
    delete[] unitedKeysArray;
    return StatusType::SUCCESS;
}

StatusType mergeArrays(Player** arr1, int size1, Player** arr2, int size2, Player** mergedPlayerArr, Pair<int,int>* mergedKeysArr){
    int indexArr1 = 0, indexArr2 = 0, indexNew = 0;
    while (indexArr1 < size1 && indexArr2 < size2)
    {

        if (*arr1[indexArr1] < *arr2[indexArr2])
        {
            mergedPlayerArr[indexNew] = arr1[indexArr1];
            Pair<int,int> pair(arr1[indexArr1]->getStrength(), arr1[indexArr1]->getId());
            mergedKeysArr[indexNew++] = pair;
            indexArr1++;
        }
        else
        {
            mergedPlayerArr[indexNew] = arr2[indexArr2];
            Pair<int,int> pair(arr2[indexArr2]->getStrength(), arr2[indexArr2]->getId());
            mergedKeysArr[indexNew++] = pair;
            indexArr2++;
        }
    }
    while (indexArr1 < size1)
    {
        mergedPlayerArr[indexNew] = arr1[indexArr1];
        Pair<int,int> pair(arr1[indexArr1]->getStrength(), arr1[indexArr1]->getId());
        mergedKeysArr[indexNew] = pair;
        indexArr1++;
        indexNew++;
    }
    while (indexArr2 < size2)
    {
        mergedPlayerArr[indexNew] = arr2[indexArr2];
        Pair<int,int> pair(arr2[indexArr2]->getStrength(), arr2[indexArr2]->getId());
        mergedKeysArr[indexNew] = pair;
        indexArr2++;
        indexNew++;
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
    return player->getStrength() * numOfPlayers;
}