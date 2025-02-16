///////////////////////////////////////////////////////////////////////////////////////////////

#include "assignnaive.hpp"

#include "trace.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

AssignNaive::AssignNaive(vector<Boss>& bosses, vector<list<Team*>>& teamsElements)
    : m_bosses(bosses)
    , m_teamsElements(teamsElements)
{
    FNC2 << "AssignNaive()";
}

///////////////////////////////////////////////////////////////////////////////////////////////

void AssignNaive::init()
{
    FNC2 << "init()";

    // Highest dmg members first
    for (auto& elementList : m_teamsElements)
    {
        elementList.sort([](const Team* a, const Team* b)
        {
            return a->dmg > b->dmg; // Sorting in descending order
        });
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void AssignNaive::assign()
{
    FNC2 << "assign()";

    for (auto& boss : m_bosses)
    {
        auto& teams = m_teamsElements[boss.element];

        // Assign cleaner first (This here will just skip if no cleaner found)
        auto team = find_if(teams.begin(), teams.end(), [](const Team* team)
        {
            return team->name == "Cleaner";
        });
        if (team != teams.end())
        {
            boss.teamsAssigned.push_back(*team);
            boss.lifeCurrent -= (*team)->dmg * m_scaleFromMillion;

            teams.remove(*team);
        }

        // Assign more teams until current boss hp <= 0
        while (boss.lifeCurrent > 0 && !teams.empty())
        {
            auto& team = *teams.front();
            teams.pop_front();

            if (team.memberPtr->remaningHits > 0)
            {
                boss.teamsAssigned.push_back(&team);
                boss.lifeCurrent -= team.dmg * m_scaleFromMillion;

                // After assinging we must clean up unuseable teams
                removeConflictingTeams(team);

                team.memberPtr->remaningHits--;
            }
        }

        auto& teamsNeutral = m_teamsElements[eElement_Neutral];
        while (boss.lifeCurrent > 0 && !teamsNeutral.empty())
        {
            auto& team = *teamsNeutral.front();
            teamsNeutral.pop_front();

            if (team.memberPtr->remaningHits > 0)
            {
                boss.teamsAssigned.push_back(&team);
                boss.lifeCurrent -= team.dmg * m_scaleFromMillion;

                // After assinging we must clean up unuseable teams
                removeConflictingTeams(team);

                team.memberPtr->remaningHits--;
            }
        }

        // Stop when there are no teams left
        if (teams.empty() && teamsNeutral.empty()) break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void AssignNaive::removeConflictingTeams(Team& team)
{
    for (auto conflicting : team.conflictPtrs)
    {
        conflicting->elmentListPtr->remove(conflicting);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////