// 
// 234218 Data Structures 1.
// Semester: 2023B (spring).
// Wet Exercise #1.
// 
// Recommended TAB size to view this file: 8.
// 
// The following main file is necessary to link and run your code.
// This file is READ ONLY: even if you submit something else, the compiler ..
// .. WILL use our file.
// 

#include "olympics24a2.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

void print(ofstream&, string cmd, StatusType res);

void print(ofstream&, string cmd, output_t<int> res);

int main(int argc, char* argv[])
{
    ifstream input;
    string inputName = string(argv[1]);
    input.open(inputName);

    ofstream output;
    string outputName = string(argv[2]);
    output.open(outputName);



    int d1, d2;

    // Init
    olympics_t* obj = new olympics_t();

    // Execute all commands in file
    string op;
    int counter = 1;
    while (input >> op)
    {
        if (!op.compare("add_team"))
        {
            input >> d1;
            print(output, op, obj->add_team(d1));
        }
        else if (!op.compare("remove_team"))
        {
            input >> d1;
            print(output, op, obj->remove_team(d1));
        }
        else if (!op.compare("add_player"))
        {
            input >> d1 >> d2;
            print(output, op, obj->add_player(d1, d2));
        }
        else if (!op.compare("remove_newest_player"))
        {
            input >> d1;
            print(output, op, obj->remove_newest_player(d1));
        }
        else if (!op.compare("play_match"))
        {
            input >> d1 >> d2;
            print(output, op, obj->play_match(d1, d2));
        }
        else if (!op.compare("num_wins_for_team"))
        {
            input >> d1;
            print(output, op, obj->num_wins_for_team(d1));
        }
        else if (!op.compare("get_highest_ranked_team"))
        {
            print(output, op, obj->get_highest_ranked_team());
        }
        else if (!op.compare("unite_teams"))
        {
            input >> d1 >> d2;
            print(output, op, obj->unite_teams(d1, d2));
        }
        else if (!op.compare("play_tournament"))
        {
            input >> d1 >> d2;
            print(output, op, obj->play_tournament(d1, d2));
        }
        else
        {
            output << "Unknown command: " << op << endl;
            return -1;
        }
        // Verify no faults
        if (input.fail())
        {
            output << "Invalid input format" << endl;
            return -1;
        }
        counter++;
    }

    // Quit
    delete obj;
    return 0;
}

// Helpers
static const char* StatusTypeStr[] = {"SUCCESS", "ALLOCATION_ERROR", "INVALID_INPUT", "FAILURE"};

void print(ofstream& out, string cmd, StatusType res)
{
    out << cmd << ": " << StatusTypeStr[(int)res] << endl;
}

void print(ofstream& out, string cmd, output_t<int> res)
{
    if (res.status() == StatusType::SUCCESS)
    {
        out << cmd << ": " << StatusTypeStr[(int)res.status()] << ", " << res.ans() << endl;
    }
    else
    {
        out << cmd << ": " << StatusTypeStr[(int)res.status()] << endl;
    }
}

