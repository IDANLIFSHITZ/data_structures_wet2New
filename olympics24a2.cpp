#include "olympics24a2.h"
#include "Pair.h"

/*
 * private functions:
 */

void olympics_t::increase_win(Team* team, int change)
{
    Pair<int,int> teamKey = Pair<int,int>(team->getStrength(), -team->getId());
    this->teamsTree->update_extra(teamKey, change);
    int numOfSmallerTeams = this->teamsTree->get_num_of_smaller_nodes( teamKey);
    output_t<Team*> nextSmallestTeam = this->teamsTree->search_number_of_smaller_nodes(numOfSmallerTeams-1);

    if (nextSmallestTeam.status() == StatusType::SUCCESS)
    {
        Pair<int,int> nextSmallestTeamKey = Pair<int,int>(nextSmallestTeam.ans()->getStrength(), -nextSmallestTeam.ans()->getId());
        this->teamsTree->update_extra(nextSmallestTeamKey, -1*change);
    }
}

/*
 * public functions:
 */

olympics_t::olympics_t(): teamsTable(new hashTable()),
                          teamsTree(new AVL<Team*, Pair<int,int>>([](const Team* team){if(team->getNumOfPlayers()== 0){return 0;}return team->getStrength();})){}

olympics_t::~olympics_t()
{
    // deletes the teams competing
    teamsTree->clear();

    // deletes the array of trees and their nodes
    delete teamsTable;

    // deletes the tree and its nodes
    delete teamsTree;
}


StatusType olympics_t::add_team(int teamId)
{
    if (teamId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }
    if (teamsTable->find(teamId).status() == StatusType::SUCCESS)
    {
        return StatusType::FAILURE;
    }
    try
    {
        Team* team = new Team(teamId);
        StatusType status = teamsTable->add(team, teamId);
        if (status != StatusType::SUCCESS)
        {
            return status;
        }
        return teamsTree->insert(team, Pair<int,int>(team->getStrength(), -teamId));
    }
    catch (const std::bad_alloc& err)
    {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType olympics_t::remove_team(int teamId)
{
    // invalid input
    if (teamId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }

    // find the team to delete
    output_t<Team*> out = teamsTable->find(teamId);

    // team is not in the table, cannot remove it
    if (out.status() != StatusType::SUCCESS)
    {
        return StatusType::FAILURE;
    }

    Team* teamToDelete = out.ans();

    // remove the team from the table
    StatusType status = teamsTable->remove(teamId);
    if (status != StatusType::SUCCESS)
    {
        return status;
    }

    // remove the team from the tree
    status = teamsTree->remove(Pair<int,int>(out.ans()->getStrength(), -teamId));
    if (status != StatusType::SUCCESS)
    {
        return status;
    }

    // free the players of the team
    delete teamToDelete;
    return StatusType::SUCCESS;
}

StatusType olympics_t::add_player(int teamId, int playerStrength)
{
    // invalid input
    if (teamId <= 0 || playerStrength <= 0)
    {
        return StatusType::INVALID_INPUT;
    }

    // check if the team with the given id exists
    output_t<Team*> out = teamsTable->find(teamId);
    if (out.status() != StatusType::SUCCESS)
    {
        return StatusType::FAILURE;
    }

    // the team we need to add to
    Team* teamToAddPlayer = out.ans();
    Pair<int,int> teamKey = Pair<int,int>(teamToAddPlayer->getStrength(), -teamId);
    int saveWins = this->teamsTree->calc_extra_in_path(teamKey);

    // remove the team from the tree, so it can be reinserted with the new strength
    this->teamsTree->remove(teamKey);

    // add the player to the team
    StatusType status = teamToAddPlayer->addPlayer(playerStrength);
    if (status != StatusType::SUCCESS)
    {
        this->teamsTree->insert(teamToAddPlayer, teamKey);
        return status;
    }

    // reinsert the team to the tree with the new strength
    teamKey.set_first(teamToAddPlayer->getStrength());
    status = teamsTree->insert(teamToAddPlayer, teamKey);
    Pair<int,int> lowKey = Pair<int,int>(teamKey.get_first(), teamKey.get_second()-1);
    teamsTree->update_extra_in_range(lowKey, teamKey, saveWins);

	return status;
}

StatusType olympics_t::remove_newest_player(int teamId)
{
    // invalid input
    if (teamId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }

    // check if the team with the given id exists
    output_t<Team*> out = teamsTable->find(teamId);
    if (out.status() != StatusType::SUCCESS)
    {
        return StatusType::FAILURE;
    }

    // the team we need to remove the player from
    Team* teamToRemovePlayer = out.ans();

    //save number of wins of team and remove team from teamsTree.
    Pair<int,int> teamKey = Pair<int,int>(teamToRemovePlayer->getStrength(), -teamId);
    int saveWins = this->teamsTree->calc_extra_in_path(teamKey);
    this->teamsTree->remove(teamKey);

    // remove the player from the team
    StatusType status = teamToRemovePlayer->removeNewestPlayer();

    //return team to teamsTree.
    teamKey.set_first(teamToRemovePlayer->getStrength());
    this->teamsTree->insert(teamToRemovePlayer, teamKey);
    if (status != StatusType::SUCCESS)
    {
        return status;
    }

    this->increase_win(teamToRemovePlayer, saveWins);
	return StatusType::SUCCESS;
}

output_t<int> olympics_t::play_match(int teamId1, int teamId2)
{
    if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2) //if invalid input.
    {
        return StatusType::INVALID_INPUT;
    }

    //find teams.
    output_t<Team*> outTeam1 = teamsTable->find(teamId1);
    output_t<Team*> outTeam2 = teamsTable->find(teamId2);
    if (outTeam1.status() != StatusType::SUCCESS || outTeam2.status() != StatusType::SUCCESS)
    {
        return StatusType::FAILURE;
    }
    Team* team1 = outTeam1.ans();
    Team* team2 = outTeam2.ans();
    if (team1->getNumOfPlayers() == 0 || team2->getNumOfPlayers() == 0)
    {
        return StatusType::FAILURE;
    }

    //get teams strength and increase win to winning team.
    int team1Strength = team1->getStrength();
    int team2Strength = team2->getStrength();
    if (team1Strength > team2Strength || (team1Strength == team2Strength && teamId1 < teamId2))
    {
        increase_win(team1);
        return teamId1;
    }
    increase_win(team2);
    return teamId2;
}

output_t<int> olympics_t::num_wins_for_team(int teamId)
{
    if (teamId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }

    output_t<Team*> team = this->teamsTable->find(teamId);
    if (team.status() != StatusType::SUCCESS)
    {
        return team.status();
    }
    Pair<int,int> teamKey = Pair<int,int>(team.ans()->getStrength(), -(team.ans()->getId()));
    return this->teamsTree->calc_extra_in_path(teamKey);
}

output_t<int> olympics_t::get_highest_ranked_team()
{
	if (this->teamsTable->isEmpty())
    {
        return -1;
    }
    return output_t<int>(this->teamsTree->get_maxSubtreeRank());
}

StatusType olympics_t::unite_teams(int teamId1, int teamId2)
{
    // invalid input
    if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2)
    {
        return StatusType::INVALID_INPUT;
    }

    // team1 is in table
    output_t<Team*> outTeam1 = teamsTable->find(teamId1);
    if (outTeam1.status() != StatusType::SUCCESS)
    {
        return StatusType::FAILURE;
    }

    // team2 is in table
    output_t<Team*> outTeam2 = teamsTable->find(teamId2);
    if (outTeam2.status() != StatusType::SUCCESS)
    {
        return StatusType::FAILURE;
    }

    Team* team1 = outTeam1.ans();
    Team* team2 = outTeam2.ans();

    // remove team2 from the table and the tree
    teamsTable->remove(teamId2);
    teamsTree->remove(Pair<int,int>(team2->getStrength(), -teamId2));

    // remove team1 from the table and the tree, so it can go in with the new strength
    Pair<int,int> team1Key = Pair<int,int>(team1->getStrength(), -teamId1);
    int saveWins = this->teamsTree->calc_extra_in_path(team1Key);
    this->teamsTree->remove(team1Key);

    // unite the teams
    team1->uniteTeams(team2);

    // reinsert team1 to the table and the tree with the new strength
    team1Key.set_first(team1->getStrength());
    StatusType status = teamsTree->insert(team1, team1Key);
    this->increase_win(team1, saveWins);

    return status;
}

/*
 * returns true if x is a power of 2.
 */
bool isPowerOf2(int x)
{
    if (x < 1)
    {
        return false;
    }
    while (x != 1)
    {
        if (x % 2 != 0)
        {
            return false;
        }
        x = x/2;
    }
    return true;
}

output_t<int> olympics_t::play_tournament(int lowPower, int highPower)
{
    if (lowPower <= 0 || highPower <= 0 || highPower <= lowPower)
    {
        return StatusType::INVALID_INPUT;
    }
    int numOfSmallerLow = this->teamsTree->get_number_of_smaller_nodes_by_power_min(lowPower-1);
    int numOfSmallerHigh = this->teamsTree->get_number_of_smaller_nodes_by_power_max(highPower);
    int numOfTeams = numOfSmallerHigh - numOfSmallerLow;

    if (!isPowerOf2(numOfTeams)) //if number of teams in tournament is not a power of 2.
    {
        return StatusType::FAILURE;
    }

    if (numOfTeams == 1) //if only one team in tournament.
    {
        return this->teamsTree->search_number_of_smaller_nodes(numOfSmallerHigh).ans()->getId();
    }

    for (int round = numOfTeams /2; round >= 1; round /= 2)
    {
        Team* lowTeam = this->teamsTree->search_number_of_smaller_nodes(numOfSmallerHigh-round).ans();
        Team* highTeam = this->teamsTree->search_number_of_smaller_nodes(numOfSmallerHigh).ans();
        Pair<int,int> lowTeamKey = Pair<int,int>(lowTeam->getStrength(), -lowTeam->getId());
        Pair<int,int> highTeamKey = Pair<int,int>(highTeam->getStrength(),- highTeam->getId());
        teamsTree->update_extra_in_range(lowTeamKey, highTeamKey, 1);
    }
    return teamsTree->search_number_of_smaller_nodes(numOfSmallerHigh).ans()->getId();
}
