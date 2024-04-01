//
// Created by yuval on 3/27/2024.
//

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "wet2util.h"
#include "Player.h"

class LinkedList {
private:
    class Node {
    public:
        Player* player;
        Node* next;
        Node(Player* player, Node* next) : player(player), next(next) {}
        ~Node() = default;
    };
    Node* head;
    int size;
    friend class Team;
public:
    // constructor
    LinkedList();
    // destructor
    ~LinkedList();
    // copy constructor
    LinkedList(const LinkedList& other) = delete;
    // assignment operator
    LinkedList& operator=(const LinkedList& other) = delete;
    // push
    StatusType push(Player* player);
    // pop
    StatusType pop();
    // get size
    int getSize() const;

    StatusType uniteLists(LinkedList* other);
};



#endif // LINKEDLIST_H
