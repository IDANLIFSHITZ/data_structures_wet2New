//
// Created by magshimim on 3/27/2024.
//

#include <iostream>
#include <functional>
#include "wet2util.h"
#include "Pair.h"

#ifndef WET24_TO_PUBLISH_AVL_H
#define WET24_TO_PUBLISH_AVL_H

template<class valT, class keyT>
class AVL {
private:
    class alreadyInTree : public std::exception {
    };

    class Node {
    public:
        /*
         * Node members.
         */
        keyT key;
        valT data;
        int height; //height of node (null is -1).
        int subtreeSize;
        Node* parent;
        Node* left; //left child.
        Node* right; //right child.
        int extra;
        int maxSubtreeRank;


        explicit Node(valT data, const keyT& key, Node* parent = nullptr) :
            key(key),
            data(data),
            height(0),
            subtreeSize(1),
            parent(parent),
            left(nullptr),
            right(nullptr),
            extra(0),
            maxSubtreeRank(0){}

        ~Node()
        {
            delete this->left;
            delete this->right;
        }

        /*
         * operators on Node class:
         * assumes keyT has operators: ==, !=, <, >.
         */
        bool operator==(const Node &node) {
            return (this->key == node.key);
        }

        bool operator!=(const Node &node) {
            return (this->key != node.key);
        }

        bool operator<(const Node &node) {
            return (this->key < node.key);
        }

        bool operator>(const Node &node) {
            return (this->key > node.key);
        }
        ///////////////////////////////////////

        /*
        * returns height of node.
        */
        static int get_height(Node *node) {
            if (node == nullptr) {
                return -1;
            }
            return node->height;
        }

        /*
        * updates height of node;
        */
        static void update_node_height(Node* node)
        {
            if (node != nullptr)
            {
                node->height = max(get_height(node->left), get_height(node->right)) + 1;
            }
        }

        /*
        * returns subtreeSize of node.
        */
        static int get_subtreeSize(Node *node) {
            if (node == nullptr) {
                return 0;
            }
            return node->subtreeSize;
        }

        /*
         * updates subtreeSize of node.
         */
        static void update_subtreeSize(Node* node)
        {
            if (node != nullptr)
            {
                node->subtreeSize = Node::get_subtreeSize(node->left) + Node::get_subtreeSize(node->right) + 1;
            }
        }

        /*
         * calculates and returns balance factor.
         */
        int get_balance() const {
            if (this->height == 0) {
                return 0;
            }
            return (get_height(this->left) - get_height(this->right));
        }
    };

    Node *root; //root of AVL tree.
    int numOfNodes;

    Node* minNode; //ptr to smallest node in the tree.
    Node* maxNode; //ptr to biggest node in the tree.

    /*
    * updates maxSubtreeRank of node.
    */
    void update_maxSubtreeRank(Node* node)
    {
        int currRank = this->calc_power(node->data);
        int maxRankLeft = (node->left != nullptr) ? // get left maxSubtreeRank.
                          node->left->maxSubtreeRank : 0;
        int maxRankRight = (node->right != nullptr) ? // get right maxSubtreeRank.
                           node->right->maxSubtreeRank : 0;

        node->maxSubtreeRank = AVL::max(currRank, AVL::max(maxRankLeft, maxRankRight)); //calc maxSubtreeRank.
        node->maxSubtreeRank += (node->maxSubtreeRank != 0) ?
                                node->extra : 0;
    }

    /*
     * executes right rotate. returns original left node.
     */
    Node *right_rotate(Node *nodeToRotate)
    {
        /*
         * part 1: rotate.
        */
        Node* leftNode = nodeToRotate->left;
        int nodeToRotateExtra = nodeToRotate->extra; // save extra of nodeToRotate.
        int leftNodeExtra = leftNode->extra; // save extra of nodeToRotate->left.

        //update extra field.
        nodeToRotate->extra = -1 * leftNodeExtra;
        leftNode->extra += nodeToRotateExtra;

        // updates new left child parent and extra.
        nodeToRotate->left = leftNode->right;
        if (leftNode->right != nullptr) // if left child has right child (new left child).
        {
            leftNode->right->extra += leftNodeExtra;
            leftNode->right->parent = nodeToRotate;
        }

        //update leftNode to be new nodeToRotate.
        leftNode->right = nodeToRotate;
        leftNode->parent = nodeToRotate->parent;
        nodeToRotate->parent = leftNode;


        // updates org parent children.
        if (leftNode->parent != nullptr) // if nodeToRotate had parent.
        {
            if (nodeToRotate->key < leftNode->parent->key) // if was left child.
            {
                leftNode->parent->left = leftNode;
            }
            else  // else was right child.
            {
                leftNode->parent->right = leftNode;
            }
        }

        /*
         * part 2: update heights, subtreeSizeRank, max.
        */
        // update nodeToRotate:
        //update height.
        Node::update_node_height(nodeToRotate);

        //update subtreeSize.
        Node::update_subtreeSize(nodeToRotate);

        //update maxSubtreeRank.
        if (nodeToRotate->left != nullptr)
        {
            this->update_maxSubtreeRank(nodeToRotate->left);
        }
        this->update_maxSubtreeRank(nodeToRotate);
        ///////////////////////////////////////////////////////////////////

        // update leftNode:
        //update height.
        Node::update_node_height(leftNode);

        //update subtreeSize.
        Node::update_subtreeSize(leftNode);

        //update maxSubtreeRank.
        this->update_maxSubtreeRank(leftNode);
        ///////////////////////////////////////////////////////////////////

        return leftNode; //return new nodeToRotate.
    }

    /*
     * executes right rotate. returns original right node.
     */
    Node* left_rotate(Node *nodeToRotate) {
        /*
         * part 1: rotate.
        */
        Node* rightNode = nodeToRotate->right;
        int nodeToRotateExtra = nodeToRotate->extra; // save extra of nodeToRotate.
        int rightNodeExtra = rightNode->extra; // save extra of nodeToRotate->left.

        //update extra field.
        nodeToRotate->extra = -1 * rightNodeExtra;
        rightNode->extra += nodeToRotateExtra;

        // updates new right child parent and extra.
        nodeToRotate->right = rightNode->left;
        if (rightNode->left != nullptr) // if left child has right child (new left child).
        {
            rightNode->left->extra += rightNodeExtra;
            rightNode->left->parent = nodeToRotate;
        }

        //update rightNode to be new nodeToRotate.
        rightNode->left = nodeToRotate;
        rightNode->parent = nodeToRotate->parent;
        nodeToRotate->parent = rightNode;


        // updates org parent children.
        if (rightNode->parent != nullptr) // if nodeToRotate had parent.
        {
            if (nodeToRotate->key < rightNode->parent->key) // if was left child.
            {
                rightNode->parent->left = rightNode;
            }
            else  // else was right child.
            {
                rightNode->parent->right = rightNode;
            }
        }

        /*
         * part 2: update heights, subtreeSizeRank, max.
        */
        // update nodeToRotate:
        //update height.
        Node::update_node_height(nodeToRotate);

        //update subtreeSize.
        Node::update_subtreeSize(nodeToRotate);

        //update maxSubtreeRank.
        if (nodeToRotate->right != nullptr)
        {
            this->update_maxSubtreeRank(nodeToRotate->right);
        }
        this->update_maxSubtreeRank(nodeToRotate);
        ///////////////////////////////////////////////////////////////////

        // update rightNode:
        //update height.
        Node::update_node_height(rightNode);

        //update subtreeSize.
        Node::update_subtreeSize(rightNode);

        //update maxSubtreeRank.
        this->update_maxSubtreeRank(rightNode);
        ///////////////////////////////////////////////////////////////////

        return rightNode; //return new nodeToRotate.
    }

    /*
     * checks if NodeToBalance needs to be balanced and balances it.
     */
    Node* balance(Node* nodeToBalance)
    {
        int balanceFactor = nodeToBalance->get_balance();

        if (balanceFactor == 2)
        {
            //LL rotate.
            if (nodeToBalance->left->get_balance() >= 0)
            {
                nodeToBalance = right_rotate(nodeToBalance);
            }
            //LR rotate.
            else
            {
                nodeToBalance->left = left_rotate(nodeToBalance->left);
                nodeToBalance = right_rotate(nodeToBalance);
            }
        }
        else if (balanceFactor == -2)
        {
            //RR rotate.
            if (nodeToBalance->right->get_balance() <= 0)
            {
                nodeToBalance = left_rotate(nodeToBalance);
            }
            //RL rotate.
            else
            {
                nodeToBalance->right = right_rotate(nodeToBalance->right);
                nodeToBalance = left_rotate(nodeToBalance);
            }
        }
        return nodeToBalance;
    }

    /*
     * returns extra of path from root to node with key=key.
     */
    int get_extra_from_path(keyT keyToSearch)
    {
        int extra = 0;
        Node* currNode = this->root;
        while (currNode != nullptr)
        {
            extra += currNode->extra;
            if (currNode->key > keyToSearch) //if belongs in left subtree.
            {
                currNode = currNode->left;
            }
            else if (currNode->key < keyToSearch) //if belongs in left subtree.
            {
                currNode = currNode->right;
            }
            else
            {
                return extra;
            }
        }
        return 0;
    }

    /*
    * returns Node with key=key if exists, else return nullptr.
    */
    Node* search_extra_aux(Node *currNode, const keyT& key, int& extra) const {
        while (currNode != nullptr) //if in empty leaf.
        {
            extra += currNode->extra;
            if (currNode->key > key) //if key is in left subtree.
            {
               currNode = currNode->left;
            }
            else if (currNode->key < key) //if key is in right subtree.
            {
                currNode = currNode->right;
            }
            else //currNode is the node to find.
            {
                return currNode;
            }
        }
        return nullptr;
    }

    /*
    * returns Node with key=key if exists, else return nullptr.
    */
    Node* search_aux(Node *currNode, const keyT& key) const
    {
        int dm = 0;
        return search_extra_aux(currNode, key, dm); //currNode is the node to find.
    }

    /*
     * returns first Node with force=force.
     */
    Node* search_by_force(Node* currNode, int force, int& numOfSmaller) const
    {
        while (currNode != nullptr) //if in empty leaf.
        {
            if (this->calc_power(currNode->data) > force) //if force is in left subtree.
            {
                if (currNode->left == nullptr)
                {
                    return  currNode;
                }
                currNode = currNode->left;
            }
            else
            {
                int leftSubtreeSize = (currNode->left != nullptr) ?
                                      currNode->left->subtreeSize : 0;
                numOfSmaller += leftSubtreeSize + 1;
                if (this->calc_power(currNode->data) < force) //if force is in right subtree.
                {
                    if (currNode->right == nullptr)
                    {
                        return  currNode;
                    }
                    currNode = currNode->right;
                }
                else //currNode is the node to find.
                {
                    return currNode;
                }
            }
        }
        return nullptr;
    }

    /*
     * adds a new node to the tree recursively.
     * returns the node that in supposed to be the current node in the recursion after insertion and rotations.
     */
    Node *insert_aux(Node *currNode, Node *newNode)
    {
        /* part 1: normal tree insertion. */

        if (currNode == nullptr) //if arrived at leaf.
        {
            this->update_maxSubtreeRank(newNode);
            return newNode;
        }

        //subtract current node extra.
        newNode->extra -= currNode->extra;

        if (*newNode < *currNode) //if newNode belong in left subtree.
        {
            currNode->left = insert_aux(currNode->left, newNode);
            if (currNode->left != nullptr) //update new left child parent.
            {
                currNode->left->parent = currNode;
            }
        }
        else if (*newNode > *currNode) //if newNode belong in right subtree.
        {
            currNode->right = insert_aux(currNode->right, newNode);
            if (currNode->right != nullptr) //update new right child parent.
            {
                currNode->right->parent = currNode;
            }
        }
        else // newNode already in AVL tree.
        {
            throw alreadyInTree();
        }

        /* part 2: update height, subtreeSize, maxSubtreeRank: */

        Node::update_node_height(currNode);
        Node::update_subtreeSize(currNode);
        this->update_maxSubtreeRank(currNode);

        /* part 3: calc balance factor and perform rotations if tree  is unbalanced */
        return this->balance(currNode);
    }

    /*
     * returns smallest node in tree with root of currNode.
     * assumes tree is not empty.
     */
    Node* get_min_node(Node *node) {
        while (node->left != nullptr) //while there is a left child.
        {
            node = node->left;
        }
        return node;
    }

    /*
     * returns biggest node in tree with root of currNode.
     * assumes tree is not empty.
     */
    Node* get_max_node(Node *node) {
        while (node->right != nullptr) //while there is a right child.
        {
            node = node->right;
        }
        return node;
    }

    /*
     * dislodges node from tree.
     * assumes node doesn't have 2 children.
     */
    void remove_node(Node* nodeToRemove)
    {
        Node* childNode = (nodeToRemove->left != nullptr) ?
                          nodeToRemove->left : nodeToRemove->right;
        Node* parentNode = nodeToRemove->parent;
        if (parentNode == nullptr) //if nodeToRemove is root of tree.
        {
            this->root = childNode; //update root.
        }
        else if(parentNode->left == nodeToRemove) //if nodeToRemove is left child.
        {
            parentNode->left = childNode;
        }
        else //else nodeToRemove is right child.
        {
            parentNode->right = childNode;
        }

        if (childNode != nullptr) //if nodeToRemove had child.
        {
            childNode->parent = parentNode;
            childNode->extra += nodeToRemove->extra;
            this->update_maxSubtreeRank(childNode);
        }
    }

    /*
     * adds current node to Array at index and adds subtrees.
     * returns updated index after all insertions.
     */
    int tree_to_sorted_array_aux(valT* values, keyT* keys, int index, Node* currNode) const {
        if (currNode == nullptr)
        {
            return index;
        }
        index = tree_to_sorted_array_aux(values, keys, index, currNode->left); //adds left subtree to Array.
        values[index] = currNode->data;
        keys[index++] = currNode->key;
        index = tree_to_sorted_array_aux(values, keys, index, currNode->right); //adds right subtree to Array.
        return index;
    }

    /*
     * builds tree from sorted array, returns root of recursive tree.
     */
    Node* sorted_array_to_tree_aux(valT* values, keyT* keys, int start, int end)
    {
        if (start > end)
        {
            return nullptr;
        }
        Node* newRoot;
        int mid = (start + end) / 2;
        try
        {
            newRoot = new Node(values[mid], keys[mid]); //create new root of recursive tree.
            int leftSubtreeSize = 0, rightSubtreeSize = 0;

            //create left child subtree from array.
            newRoot->left = sorted_array_to_tree_aux(values, keys, start, mid - 1); //create left subtree recursively.
            if (newRoot->left != nullptr) //update new left child parent.
            {
                newRoot->left->parent = newRoot;
                leftSubtreeSize = newRoot->left->subtreeSize;
            }

            //create right child subtree from array.
            newRoot->right = sorted_array_to_tree_aux(values, keys, mid + 1, end); //create right subtree recursively.
            if (newRoot->right != nullptr) //update new right child parent.
            {
                newRoot->right->parent = newRoot;
                rightSubtreeSize = newRoot->right->subtreeSize;
            }

            //update newRoot.
            Node::update_node_height(newRoot);
            newRoot->subtreeSize = leftSubtreeSize + rightSubtreeSize + 1;

            return newRoot;
        }
        catch (std::bad_alloc& err)
        {
            if (newRoot != nullptr)
            {
                if (newRoot->parent != nullptr) //if already updated parent.
                {
                    if (newRoot->parent->left == newRoot) //if left child.
                    {
                        newRoot->parent->left = nullptr;
                    }
                    else //is right child.
                    {
                        newRoot->parent->right = nullptr;
                    }
                }
                delete newRoot;
            }
            throw std::bad_alloc();
        }
        return nullptr;
    }

    /*
     * clears all nodes in the tree with root of currRoot recursively.
     */
    void clear_aux(Node *currRoot) {
        if (currRoot == nullptr) {
            return;
        }
        clear_aux(currRoot->left); //clears left subtree.
        clear_aux(currRoot->right); //clears right subtree.
        delete currRoot->data;
    }

    /*
     * creates new tree and copies all values and keys from tree.
     */
    Node* hard_copy(Node* currNode, Node* newParent)
    {
        if (currNode == nullptr)
        {
            return nullptr;
        }
        Node* newCopy = nullptr;
        try
        {
            int leftSubtreeSize = 0, rightSubtreeSize = 0;
            newCopy = new Node(currNode->data, currNode->key, newParent);

            //hard copy left subtree.
            newCopy->left = hard_copy(currNode->left, newCopy);
            if (newCopy->left != nullptr)
            {
                leftSubtreeSize = newCopy->left->subtreeSize;
            }

            //hard copy right subtree.
            newCopy->right = hard_copy(currNode->right, newCopy);
            if (newCopy->right != nullptr)
            {
                rightSubtreeSize = newCopy->right->subtreeSize;
            }

            //update newCopy.
            Node::update_node_height(newCopy);
            newCopy->subtreeSize = leftSubtreeSize + rightSubtreeSize + 1;

            return newCopy;
        }
        catch (std::bad_alloc& err)
        {
            if (newCopy != nullptr)
            {
                if (newCopy->parent != nullptr) //if already updated parent.
                {
                    if (newCopy->parent->left == newCopy) //if left child.
                    {
                        newCopy->parent->left = nullptr;
                    }
                    else //is right child.
                    {
                        newCopy->parent->right = nullptr;
                    }
                }
                delete newCopy;
            }
            throw std::bad_alloc();
        }
    }

    /*
     * returns number of smaller node from next team with force=force.
     * min case.
     */
    int get_number_of_smaller_nodes_of_next_min_force(Node* currNode, int forceToSearch) const
    {
        if (currNode == nullptr)
        {
            return 0;
        }

        int numOfSmaller = 0;
        currNode = currNode->left;
        while (currNode != nullptr && this->calc_power(currNode->data) != forceToSearch)
        {
            numOfSmaller += (currNode->left != nullptr) ?
                            currNode->left->subtreeSize+1 : 1;
            currNode = currNode->right;
        }
        return numOfSmaller + get_number_of_smaller_nodes_of_next_min_force(currNode, forceToSearch);
    }

    /*
     * returns number of smaller node from next team with force=force.
     * max case.
     */
    int get_number_of_smaller_nodes_of_next_max_force(Node* currNode, int forceToSearch) const
    {
        if (currNode == nullptr)
        {
            return 0;
        }

        int numOfSmaller = 0;
        currNode = currNode->right;
        if (currNode == nullptr)
        {
            return 0;
        }
        numOfSmaller = (currNode->left != nullptr) ?
                       currNode->left->subtreeSize+1 : 1;
        while (currNode != nullptr && this->calc_power(currNode->data) != forceToSearch)
        {
            currNode = currNode->left;
        }
        return numOfSmaller + get_number_of_smaller_nodes_of_next_max_force(currNode, forceToSearch);
    }

public:
    /*
     * public members:
     */
    std::function<int(const valT&)> calc_power;

    ////////////////////////////////

    /*
     * AVL constructor for non team AVL trees that don't need calc_power function.
     */
    AVL<valT, keyT>() : root(nullptr),
                        numOfNodes(0),
                        minNode(nullptr),
                        maxNode(nullptr),
                        calc_power([](const valT&)->int{return 0;}){}

    /*
     * AVL constructor for team AVL trees.
     */
    explicit AVL<valT, keyT>(std::function<int(const valT&)> calc_power) : root(nullptr),
                                                                  numOfNodes(0),
                                                                  minNode(nullptr),
                                                                  maxNode(nullptr),
                                                                  calc_power(calc_power){}

    /*
     * AVL constructor from sorted Array.
     * time complexity of O(array.size).
     */

    AVL<valT, keyT>(valT* values, keyT* keys, int size) : numOfNodes(size),
                                                          calc_power([](const valT&)->int{return 0;})
    {
        this->root = this->sorted_array_to_tree_aux(values, keys, 0, this->numOfNodes-1); //builds tree from sorted array.

        //get min and max nodes from built tree.
        this->minNode = this->get_min_node(this->root);
        this->maxNode = this->get_max_node(this->root);
    }

    /*
     * copy constructor for tree. hard copies other into this.
     */
    AVL& operator=(const AVL<valT, keyT>& other)
    {
        if (this == &other)
        {
            return *this;
        }
        this->numOfNodes = other.numOfNodes;
        this->root = this->hard_copy(other.root, nullptr);
        this->minNode = this->get_min_node(this->root);
        this->maxNode = this->get_max_node(this->root);

        return *this;
    }

    /*
     * destructor, destructs the tree recursively from root.
     */
    ~AVL()
    {
        delete this->root;
    }

    /*
     * destroys tree content.
     */
    void clear()
    {
        this->clear_aux(this->root);
        this->root = nullptr;
        this->minNode = nullptr;
        this->maxNode = nullptr;
    }

    /*
     * returns max of to integers.
     */
    static int max(int x, int y) {
        if (x > y) {
            return x;
        }
        return y;
    }

    /*
     * returns true if tree is empty, else false.
     */
    bool is_empty() const {
        return (numOfNodes == 0);
    }

    /*
     * returns number of nodes in tree.
     */
    int get_size() const {
        return this->numOfNodes;
    }

    /*
     * finds and returns data of node in tree with key=key if exists, returns nullptr if not.
     * time complexity of O(log(tree.nunOfNodes)).
     */
    valT search(keyT key) const {
        Node *nodeToFind = search_aux(this->root, key);
        return nodeToFind != nullptr ? nodeToFind->data
                                     : nullptr; //if nodeToFind is not nullptr returns nodeToFind->data, else returns nullptr.
    }

    /*
     * creates new node with parameters T,S and adds it to the tree.
     * return: SUCCESS- node inserted successfully.
     *         ALLOCATION_ERROR- error in memory allocation for new node creation.
     *         FAILURE- the node is already in the tree.
     * time complexity of O(log(tree.nunOfNodes)).
     */
    StatusType insert(valT data, keyT key)
    {
        try {
            Node *newNode = new Node(data, key); //create new node.
            this->root = insert_aux(this->root, newNode); //insert node to tree.
            if (this->numOfNodes == 0) //if first node in tree.
            {
                this->minNode = newNode;
                this->maxNode = newNode;
            }
            else //general case.
            {
                if (*newNode < *this->minNode) //if new node is smallest node in the tree.
                {
                    this->minNode = newNode;
                }
                if (*newNode > *this->maxNode) //if new node is biggest node in the tree.
                {
                    this->maxNode = newNode;
                }
            }

            this->numOfNodes++;
            return StatusType::SUCCESS;
        }
        catch (std::bad_alloc& err) //allocation of new node failed.
        {
            return StatusType::ALLOCATION_ERROR;
        }
        catch (alreadyInTree& err) //node already in tree.
        {
            return StatusType::FAILURE;
        }
    }

    /*
     * if exists, deletes node with key=key from the tree.
     * return: SUCCESS- node deleted successfully.
     *         ALLOCATION_ERROR- error in freeing of deleted node data.
     *         FAILURE- the node is not in the tree.
     * time complexity of O(log(tree.nunOfNodes)).
     */
    StatusType remove(keyT key)
    {
        int extra = 0;
        Node* nodeToRemove = search_extra_aux(this->root, key, extra); //find node to remove in tree.

        if (nodeToRemove == nullptr) //if there is no node with key=key in tree.
        {
            return StatusType::FAILURE;
        }

        if (nodeToRemove->left == nullptr || nodeToRemove->right == nullptr) //if doesn't have two children.
        {
            this->remove_node(nodeToRemove);
            Node* deleteNode = nodeToRemove;
            nodeToRemove = nodeToRemove->parent;

            //delete deleteNode.
            deleteNode->left = nullptr;
            deleteNode->right = nullptr;
            extra -= deleteNode->extra;
            delete deleteNode;
        }
        else //has both children.
        {
            Node* nextBiggest = this->get_min_node(nodeToRemove->right); //get next biggest node in tree.
            int extraRightChild = (nodeToRemove->right != nullptr) ?
                                  nodeToRemove->right->extra : 0;
            int nextBiggestExtra = this->get_extra_from_path(nextBiggest->key);

            nodeToRemove->key = nextBiggest->key;
            nodeToRemove->data = nextBiggest->data;
            nodeToRemove->extra += nextBiggestExtra - extra;

            if (nodeToRemove->left != nullptr)
            {
                nodeToRemove->left->extra -= nextBiggestExtra - extra;
                this->update_maxSubtreeRank(nodeToRemove->left);
            }
            if (nodeToRemove->right != nullptr)
            {
                nodeToRemove->right->extra -= nextBiggestExtra - extra;
                this->update_maxSubtreeRank(nodeToRemove->right);
            }

            this->remove_node(nextBiggest);
            nodeToRemove = nextBiggest->parent;
            nextBiggest->left = nullptr;
            nextBiggest->right = nullptr;
            extra = nextBiggestExtra;

            delete nextBiggest;
        }

        // climb up the tree to the root and update and balance.
        while (nodeToRemove != nullptr)
        {
            Node::update_node_height(nodeToRemove);
            Node::update_subtreeSize(nodeToRemove);
            this->update_maxSubtreeRank(nodeToRemove);
            this->balance(nodeToRemove);

            nodeToRemove = nodeToRemove->parent;
        }

        this->numOfNodes--;
        if (numOfNodes == 0)
        {
            this->root = nullptr;
        }
        else
        {
            this->minNode = this->get_min_node(this->root);
            this->maxNode = this->get_max_node(this->root);
        }

        return StatusType::SUCCESS;
    }

    /*
     * converts tree to Array and returns the Array.
     * time complexity of O(tree.numOfNodes).
     */
    void tree_to_sorted_array(valT* values, keyT* keys)
    {
        tree_to_sorted_array_aux(values, keys, 0, this->root); //creates the Array from the tree recursively.
    }

    /*
     * updates extra field to every node with key<keyToFind.
     * and conserves integrity of tree.
     */
    void update_extra(keyT keyToFind, int change)
    {
        keyT prevBiggestKey;
        Node* currNode = this->root;
        Node* lastRightTurn = nullptr;

        //this while finds biggest key in tree which is smaller than keyToFind.
        while(currNode != nullptr)
        {
            if (currNode->key > keyToFind)  //if keyToFind belongs in left subtree.
            {
                if (currNode->left == nullptr) //if has no left child.
                {
                    if (lastRightTurn == nullptr) //if only went left on tree.
                    {
                        return;
                    }
                    prevBiggestKey = lastRightTurn->key;
                }
                currNode = currNode->left;
            }
            else if (currNode->key < keyToFind) //if keyToFind belongs in right subtree.
            {
                if (currNode->right == nullptr)
                {
                    prevBiggestKey = currNode->key;
                }
                lastRightTurn = currNode;
                currNode = currNode->right;
            }
            else //if node in tree.
            {
                prevBiggestKey = currNode->key;
                break;
            }
        }

        //
        currNode = root;
        bool leftTurnLast = true;
        while (currNode != nullptr && currNode->key != prevBiggestKey)
        {
            if (currNode->key > prevBiggestKey)
            {
                if (!leftTurnLast)
                {
                    currNode->extra -= change;
                    leftTurnLast = true;
                }
                currNode = currNode->left;
            }
            else
            {
                if (leftTurnLast)
                {
                    currNode->extra += change;
                    leftTurnLast = false;
                }
                currNode = currNode->right;
            }
        }

        if (currNode != nullptr && currNode->key == prevBiggestKey)
        {
            if (leftTurnLast)
            {
                currNode->extra += change;
            }
            if (currNode->right != nullptr)
            {
                currNode->right->extra -= change;
                currNode = currNode->right;
            }
        }

        //update maxSubtreeRank in every node in search roure
        while (currNode != nullptr)
        {
            this->update_maxSubtreeRank(currNode);
            currNode = currNode->parent;
        }
    }

    /*
     * updates extra in all nodes in range between low and high keys.
     */
    void update_extra_in_range(keyT lowKey, keyT highKet, int change)
    {
        this->update_extra(lowKey, -change);
        this->update_extra(highKet, change);
    }

    /*
     * returns sum of extras on path to node with key=key.
     */
    int calc_extra_in_path(keyT key)
    {
        int extraSum = 0;
        Node* currNode = this->root;

        while (currNode != nullptr)
        {
            extraSum += currNode->extra;
            if (currNode->key > key)
            {
                currNode = currNode->left;
            }
            else if (currNode->key < key)
            {
                currNode = currNode->right;
            }
            else
            {
                return extraSum;
            }
        }
        return 0;
    }

    /*
     * returns subtree size of node with key=key
     */
    int get_num_of_smaller_nodes(keyT keyToSearch) const
    {
        int numOfSmaller = 0;
        Node* currNode = this->root;

        while (currNode != nullptr)
        {
            if (currNode->key > keyToSearch)
            {
                currNode = currNode->left;
            }
            else
            {
                int leftSubtreeSize = (currNode->left != nullptr) ?
                                      currNode->left->subtreeSize : 0;
                numOfSmaller += leftSubtreeSize + 1;
                if (currNode->key < keyToSearch)
                {
                    currNode = currNode->right;
                }
                else
                {
                    return  numOfSmaller;
                }
            }
        }
        return 0;
    }

    /*
     * searches node in tree with subtreeSize=subtreeSize and returns its data.
     */

    output_t<valT> search_number_of_smaller_nodes(int numOfSmaller) const
    {
        if (numOfSmaller <= 0 || numOfSmaller > this->numOfNodes)
        {
            return output_t<valT>(StatusType::FAILURE);
        }
        Node* currNode = this->root;

        int currNumOfSmallerSize = (currNode->left != nullptr) ?
                              currNode->left->subtreeSize+1 : 1;
        while (currNode != nullptr)
        {
            if (currNumOfSmallerSize < numOfSmaller)
            {
                int add = (currNode->right->left != nullptr) ?
                          currNode->right->left->subtreeSize + 1 : 1;
                currNode = currNode->right;
                currNumOfSmallerSize += add;
            }
            else if (currNumOfSmallerSize > numOfSmaller)
            {
                int subtract = (currNode->left->right != nullptr) ?
                          currNode->left->right->subtreeSize + 1 : 1;
                currNode = currNode->left;
                currNumOfSmallerSize -= subtract;
            }
            else
            {
                return currNode->data;
            }
        }
        return output_t<valT>(StatusType::FAILURE);
    }

    /*
     * returns maxSubtreeRank in tree.
     */
    int get_maxSubtreeRank()
    {
        return (this->root != nullptr) ? this->root->maxSubtreeRank : 0;
    }

    /*
     * returns value if median key.
     */
    valT get_median()
    {
        return this->search_number_of_smaller_nodes((this->numOfNodes/2) + 1).ans();
    }

    /*
     * transfers node with key=keyToTransfer from other tree to this tree.
     * returns status of transfer.
     */
    StatusType transfer_node(AVL<valT, keyT>* other, keyT keyToTransfer) {
        valT dataToTransfer = other->search(keyToTransfer);
        if (dataToTransfer == nullptr) {
            return StatusType::FAILURE;
        }
        other->remove(keyToTransfer);
        return this->insert(dataToTransfer, keyToTransfer);
    }

    /*
     * returns number of smaller teams of min node with power=
     */
    int get_number_of_smaller_nodes_by_power_min(int forceToSearch) const
    {
        int numOfSmaller = 0;
        Node* currNode = this->search_by_force(this->root, forceToSearch, numOfSmaller);
        return  numOfSmaller + this->get_number_of_smaller_nodes_of_next_min_force(currNode, forceToSearch);
    }

    /*
     * returns number of smaller teams of max node with power=
     */
    int get_number_of_smaller_nodes_by_power_max(int forceToSearch) const
    {
        int numOfSmaller = 0;
        Node* currNode = this->search_by_force(this->root, forceToSearch, numOfSmaller);
        return numOfSmaller + this->get_number_of_smaller_nodes_of_next_max_force(currNode, forceToSearch);
    }
};



#endif //WET24_TO_PUBLISH_AVL_H
