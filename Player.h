//
// Created by yuval on 3/27/2024.
//

#ifndef PLAYER_H
#define PLAYER_H


class Player {
private:
    int id;
    int strength;
public:

    /**
     * constructors: default constructor, constructor with parameters
     */

    /*
     * constructor for the player
     * @param id the id of the player
     * @param strength the strength of the player
     */
    Player(int id, int strength);

    /*
     * default constructor for the player
     */
    Player() = default;

    /**
     * the big three- copy constructor, destructor and assignment operator
     */

    /*
     * destructor for the player
     */
    ~Player() = default;

    /*
     * copy constructor for the player
     * @param other
     */
    Player(const Player& other) = delete;

    /*
     * assignment operator for the player
     * @param other
     * @return
     */
    Player& operator=(const Player& other) = delete;

    /**
     * public functions: getId, setId getStrength
     */

    /*
     * get the id of the player
     * @return the id of the player
     */
    int getId() const;

    /*
     * set the id of the player
     * @param id the id to set
     * @return
     */
    int setId(int id);
    /*
     * get the strength of the player
     * @return the strength of the player
     */
    int getStrength() const;

    /*
     * operator < for the player
     */
    bool operator<(const Player& other) const;
};


#endif // PLAYER_H
