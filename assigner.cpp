///////////////////////////////////////////////////////////////////////////////////////////////

#include "assigner.hpp"

#include "global.hpp"

#include "trace.hpp"

#include "bosses.hpp"
#include "teams.hpp"
#include "members.hpp"

#include "csvencoder.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

// static
Assigner& Assigner::instance()
{
    static Assigner assigner;

    return assigner;
}

///////////////////////////////////////////////////////////////////////////////////////////////

Assigner::Assigner()
    : m_bosses(Global::instance().getBosses().m_bosses)
    , m_teams(Global::instance().getTeams().m_teams)
    , m_members(Global::instance().getMembers().m_members)
    , m_teamsElements(eElement_Size)
{
    FNC1 << "Assigner()";
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Assigner::init()
{
    FNC2 << "init()";

    // Access Global and do some magic
    Global::instance();

    LOG_I << "Sort teams of members to lists with specific elements";

    for (int i = 0; i < m_members.size(); i++)
    {
        for (Team& team : m_members[i].teams)
        {
            m_teamsElements[team.element].push_back(&team);

            // Reference list so team can remove itself from list quickly
            team.elmentListPtr = &m_teamsElements[team.element];
        }
    }

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

void Assigner::assign()
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
            boss.lifeCurrent -= (*team)->dmg * 1000000;

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
                boss.lifeCurrent -= team.dmg * 1000000;

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
                boss.lifeCurrent -= team.dmg * 1000000;

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

void Assigner::removeConflictingTeams(Team& team)
{
    for (auto conflicting : team.conflictPtrs)
    {
        conflicting->elmentListPtr->remove(conflicting);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Assigner::writeback()
{
    CSVEncoder csvEncoder;

    for (auto& boss : m_bosses)
    {
        csvEncoder.pushLine();

        csvEncoder.pushBack(boss.name);
        csvEncoder.pushBack(boss.life);

        string overKill = base_toString((boss.lifeCurrent * 100) / (boss.life)) + "%";
        csvEncoder.pushBack(overKill);

        for (auto& team : boss.teamsAssigned)
        {
            csvEncoder.pushBack(team->memberPtr->name);
            string s = team->name + "_" + elementToString(team->element);
            csvEncoder.pushBack(s);
            csvEncoder.pushBack(team->dmg);
        }
    }

    csvEncoder.buildCSV();
    string csv = csvEncoder.getCSV();

    // Write the CSV data into file
    {
        filesystem::path csvPath("csv/aa_result.csv");
        ofstream csvFile(csvPath);
        ASSERT(csvFile.is_open());

        csvFile << csv;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Assigner::fillTeamElementLists(Member& member)
{
    FNC2 << "fillTeamElementLists()";

    for (Team& team : member.teams)
    {
        m_teamsElements[team.element].push_back(&team);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
