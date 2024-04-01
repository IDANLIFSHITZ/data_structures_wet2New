//
// Created by yuval on 3/27/2024.
//

#include "LinkedList.h"
#include "wet2util.h"

LinkedList::LinkedList() : head(nullptr), size(0)
{
}

LinkedList::~LinkedList()
{
    Node* current = head;
    while (current != nullptr)
    {
        Node* temp = current->next;
        delete current;
        current = temp;
    }
    head = nullptr;
}

StatusType LinkedList::push(Player* player){
    try
    {
        Node* newNode = new Node(player, head);
        head = newNode;
        size++;
    }
    catch (const std::bad_alloc& e)
    {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType LinkedList::pop()
{
    if (head == nullptr)
    {
        return StatusType::FAILURE;
    }
    Node* temp = head;
    head = head->next;
    delete temp;
    size--;
    return StatusType::SUCCESS;
}

int LinkedList::getSize() const
{
    return size;
}

StatusType LinkedList::uniteLists(LinkedList *other) {
    // put the first nodes of the lists other in front of my list and override my list player id
    if (other->head == nullptr)
    {
        return StatusType::FAILURE;
    }

    Node* otherCurrent = other->head;

    while (otherCurrent->next != nullptr)
    {
        otherCurrent = otherCurrent->next;
    }
    otherCurrent->next = head;
    // the lists are now connected

    // override the id of the players in the list
    Node* current = other->head;
    int idInc = 1;
    while (current != nullptr)
    {
        current->player->setId(idInc+other->size);
        idInc++;
        current = current->next;
    }

    // end of function updates
    head = other->head;
    size += other->size;
    other->head = nullptr;
    return StatusType::SUCCESS;
}