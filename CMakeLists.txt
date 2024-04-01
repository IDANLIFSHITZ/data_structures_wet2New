cmake_minimum_required(VERSION 3.24)
project(a)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(a
        AVL.h
        hashTable.cpp
        hashTable.h
        LinkedList.cpp
        LinkedList.h
        main24a2.cpp
        olympics24a2.cpp
        olympics24a2.h
        Pair.h
        Player.cpp
        Player.h
        Team.cpp
        Team.h
        wet2util.h)
