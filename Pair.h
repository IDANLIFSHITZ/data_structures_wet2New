//
// Created by magshimim on 3/29/2024.
//

#ifndef WET24_TO_PUBLISH_PAIR_H
#define WET24_TO_PUBLISH_PAIR_H


template<class T, class S>
class Pair {
private:
    T first;
    S second;

public:
    Pair() = default;

    Pair(T first, S second) : first(first),
                              second(second){}

    Pair(const Pair<T, S>& pair)
    {
        this->first = pair.first;
        this->second = pair.second;
    }

    ~Pair() = default;

    /*
    * operators on Pair class:
    */

    bool operator==(const Pair<T, S>& pair)
    {
        return ((this->first == pair.first) && (this->second == pair.second));
    }

    bool operator!=(const Pair<T, S>& pair)
    {
        return ((this->first != pair.second) || (this->second != pair.second));
    }

    bool operator<(const Pair<T, S>& pair)
    {
        if (this->first != pair.first)
        {
            return (this->first < pair.first);
        }
        return (this->second < pair.second);
    }

    bool operator>(const Pair<T, S>& pair)
    {
        return (this->first != pair.first) ? (this->first > pair.first) : (this->second > pair.second);
    }

    /*
     * assumes S and T have copy constructors.
     */
    Pair<T, S>& operator=(Pair<T, S> other)
    {
        this->first = other.first;
        this->second = other.second;
        return *this;
    }
    ////////////////////////////////////////////

    /*
     * returns first item in pair.
     */
    T get_first() const
    {
        return this->first;
    }

    /*
     * returns second item in pair.
     */
    S get_second() const
    {
        return this->second;
    }
};

#endif //WET24_TO_PUBLISH_PAIR_H
