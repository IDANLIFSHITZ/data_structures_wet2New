//
// Created by yuval on 3/27/2024.
//

#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "AVL.h"
#include "Team.h"


// start of class HashTable
// a dynamic array of avl trees, with one hash function (modulus of the key by the size of the array)
// the hash table will resize itself when it is more than 50% full or less than 25% full
class hashTable {
private:
    int x;
    int currSize;
    int maxSize;
    AVL<Team*, int>** table;

    /**
     * private functions: resize
     */

    /* resize the hash table
    * @param newSize
    */
    StatusType resize(int newSize);

public:

    /*
     * the hash function to use
     * @param key
     * @param size
     * @return the index in the hash table
     */
    static int hashFunction(int key, int size);



    /**
     * public functions:
     * default constructor
     * copy constructor, destructors and assignment operators
     * add, remove, find
     * getSize, isEmpty
     */

    /*
     * default constructor for the hash table
     */
    hashTable();

    /**
     * the big three- copy constructor, destructor and assignment operator
     */


    /* copy constructor for the hash table
     * @param other
     */
    hashTable(const hashTable& other) = delete;

    /* destructor for the hash table
     */
    ~hashTable();

    /* assignment operator for the hash table
     */
    hashTable& operator=(const hashTable& other) = delete;

    /**
     * public functions: add, remove, find
     */

    /* add a new element to the hash table
     * @param element
     * @param key
     * @note if the table is more than 50% full, resize it
     */
    StatusType add(Team* element, int key);

    /** remove an element from the hash table
     * @param element
     * @param key
     * @note if the table is less than 25% full, resize it
     */
    StatusType remove(int key);

    /** find an element in the hash table
     * @param key
     * @return the element if found, nullptr otherwise
     */
    output_t<Team*> find(int key);

    /** get the current size of the hash table
     * @return the current size of the hash table
     */
    int getSize() const;

    /** get the maximum size of the hash table
     * @return the maximum size of the hash table
     */
    int isEmpty() const;
};


#endif //HASHTABLE_H
