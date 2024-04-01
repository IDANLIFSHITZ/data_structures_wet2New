//
// Created by yuval on 3/27/2024.
//

#include "Player.h"

Player::Player(int id, int strength) : id(id), strength(strength)
{
}

int Player::getId() const
{
    return id;
}

int Player::setId(int newId)
{
    this->id = newId;
}

int Player::getStrength() const
{
    return strength;
}

bool Player::operator<(const Player& other) const
{
    if (this->strength == other.strength)
    {
        return this->id < other.id;
    }
    return this->strength < other.strength;
}
