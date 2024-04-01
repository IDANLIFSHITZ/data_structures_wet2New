#include "olympics24a2.h"
#include "Pair.h"

/*
 * private functions:
 */

void olympics_t::increase_win(Team* team, int change)
{
    Pair<int,int> teamKey = Pair<int,int>(team->getStrength(), team->getId());
    this->teamsTree->update_extra(teamKey, change);
    int numOfSmallerTeams = this->teamsTree->get_num_of_smaller_nodes( teamKey);
    auto nextSmallestTeam = this->teamsTree->search_number_of_smaller_nodes(numOfSmallerTeams-1);
    if (nextSmallestTeam.status() == StatusType::SUCCESS)
    {
        Pair<int,int> nextSmallestTeamKey = Pair<int,int>(nextSmallestTeam.ans()->getStrength(), nextSmallestTeam.ans()->getId());
        this->teamsTree->update_extra(nextSmallestTeamKey, -1*change);
    }
}

/*
 * public functions:
 */

olympics_t::olympics_t(): teamsTable(new hashTable()),
                          teamsTree(new AVL<Team*, Pair<int,int>>([](const Team* team){if (team->getNumOfPlayers() == 0)
                                                                      {
                                                                          return 0;
                                                                      }return team->getStrength();})),
                          numOfTeams(0)
{
}

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
        status = teamsTree->insert(team, Pair<int,int>(team->getStrength(), teamId));
        if (status != StatusType::SUCCESS)
        {
            return status;
        }
        numOfTeams++;
    }
    catch (const std::bad_alloc& e)
    {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
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
    status = teamsTree->remove(Pair<int,int>(out.ans()->getStrength(), teamId));
    if (status != StatusType::SUCCESS)
    {
        return status;
    }

    // free the players of the team
    delete teamToDelete;

    numOfTeams--;

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

    // remove the team from the tree, so it can be reinserted with the new strength
    StatusType status = teamsTree->remove(Pair<int,int>(teamToAddPlayer->getStrength(), teamId));
    if (status != StatusType::SUCCESS)
    {
        return status;
    }
    // add the player to the team
    status = teamToAddPlayer->addPlayer(playerStrength);
    if (status != StatusType::SUCCESS)
    {
        return status;
    }

    // reinsert the team to the tree with the new strength
    status = teamsTree->insert(teamToAddPlayer, Pair<int,int>(teamToAddPlayer->getStrength(), teamId));
    if (status != StatusType::SUCCESS)
    {
        return status;
    }
	return StatusType::SUCCESS;
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

    // remove the player from the team
    StatusType status = teamToRemovePlayer->removeNewestPlayer();
    if (status != StatusType::SUCCESS)
    {
        return status;
    }
	return StatusType::SUCCESS;
}

output_t<int> olympics_t::play_match(int teamId1, int teamId2)
{
    if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2)
    {
        return StatusType::INVALID_INPUT;
    }
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
    int team1Strength = team1->getStrength();
    int team2Strength = team2->getStrength();

    if (team1Strength > team2Strength)
    {
        increase_win(team1);
        return teamId1;
    }
    else if (team1Strength < team2Strength)
    {
        increase_win(team2);
        return teamId2;
    }
    else
    {
        int idOfTeamWithLowerId = team1->getId() > team2->getId() ? teamId2 : teamId1;
        increase_win(teamsTable->find(idOfTeamWithLowerId).ans(), 1);
        return idOfTeamWithLowerId;
    }
}

output_t<int> olympics_t::num_wins_for_team(int teamId)
{
    if (teamId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }

    auto team = this->teamsTable->find(teamId);
    if (team.status() != StatusType::SUCCESS)
    {
        return team.status();
    }
    return this->teamsTree->calc_extra_in_path(Pair<int,int>(team.ans()->getStrength(), team.ans()->getId()));
}

output_t<int> olympics_t::get_highest_ranked_team()
{
	if (this->teamsTable->isEmpty())
    {
        return -1;
    }
    if (this->teamsTree->get_size() == 0)
    {
        return 0;
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
    output_t<Team*> outTeam1 = teamsTable->find(teamId1);
    if (outTeam1.status() != StatusType::SUCCESS)
    {
        return StatusType::FAILURE;
    }

    output_t<Team*> outTeam2 = teamsTable->find(teamId2);
    if (outTeam2.status() != StatusType::SUCCESS)
    {
        return StatusType::FAILURE;
    }
    Team* team1 = outTeam1.ans();
    Team* team2 = outTeam2.ans();

    StatusType status =  teamsTable->remove(teamId2);
    if (status != StatusType::SUCCESS)
    {
        return status;
    }
    status = teamsTree->remove(Pair<int,int>(team2->getStrength(), teamId2));
    if (status != StatusType::SUCCESS)
    {
        return status;
    }
    team1->uniteTeams(team2);

    numOfTeams--;

    return StatusType::SUCCESS;
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
    int numOfSmallerLow = this->teamsTree->get_number_of_smaller_nodes_by_power_min(lowPower);
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
        Pair<int,int> lowTeamKey = Pair<int,int>(lowTeam->getStrength(), lowTeam->getId());
        Pair<int,int> highTeamKey = Pair<int,int>(highTeam->getStrength(), highTeam->getId());
        teamsTree->update_extra_in_range(lowTeamKey, highTeamKey, 1);
    }
    return teamsTree->search_number_of_smaller_nodes(numOfSmallerHigh).ans()->getId();
}
