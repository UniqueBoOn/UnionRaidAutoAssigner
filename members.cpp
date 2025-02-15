///////////////////////////////////////////////////////////////////////////////////////////////

#include "members.hpp"

#include "trace.hpp"

#include "global.hpp"

#include "csvdecoder.hpp"

#include "teams.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

Members::Members(string csvData)
    : m_teams(Global::instance().getTeams())
    , m_members()
{
    FNC2 << "Members()";

    CSVDecoder csvDecoder(csvData);

    // Skip first 3 lines which contains member infos
    ASSERT(csvDecoder.skipLine());
    ASSERT(csvDecoder.skipLine());
    ASSERT(csvDecoder.skipLine());

    ASSERT(csvDecoder.nextLine());
    do
    {
        ///////////////////////////////////////////////////////////////////////////
        Member member;
        member.name = csvDecoder.fetchString();
        member.remaningHits = 3;
        ASSERT(!csvDecoder.isLineEmpty());

        LOG_I << "Init member " << member.name;

        vector<long long int> memberDmg;
        do
        {
            memberDmg.push_back(csvDecoder.fetchLong());
        }
        while (!csvDecoder.isLineEmpty());

        vector<Team>& teams = m_teams.m_teams;

        ASSERT(memberDmg.size() == teams.size());

        for (size_t i = 0; i < teams.size(); i++)
        {
            Team team = teams[i];
            ASSERT(team.dmg == -42);
            team.dmg = memberDmg[i];

            // Skip missing dmg numbers
            if (team.dmg != -1) member.teams.push_back(team);
        }

        m_members.push_back(member);
        LOG_I << "Init member " << member.name << " finished with " << member.teams.size() << " teams";
    }
    while (csvDecoder.nextLine());

    // Add member and conflict references
    for (auto& member : m_members)
    {
        LOG_I << "Fill conflicting teams of " << member.name;
        for (auto& team : member.teams)
        {
            team.memberPtr = &member;

            // Either unit or element conflic can exists.
            fillConflictList(team, member.teams);
        }
    }

    // Print conflicts
    for (auto& member : m_members)
    {
        LOG_I << "Check conflicting teams of " << member.name;
        for (auto& team : member.teams)
        {
            if (!team.conflictPtrs.empty())
            {
                for (auto conflictingTeam : team.conflictPtrs)
                {
                    stringstream ss;
                    ss << team.name << "_" << elementToString(team.element) << " conflicts with team: ";
                    ss << conflictingTeam->name << "_" << elementToString(conflictingTeam->element);
                    if (team.conflictUnit != "no") ss << " conflict unit " << team.conflictUnit;
                    LOG_I << ss.str();
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Members::fillConflictList(Team& currentTeam, vector<Team>& teams)
{
    FNC2 << "fillConflictList(" << currentTeam.name <<")";

    for (auto& team : teams)
    {
        if (&currentTeam != &team) // Only for different teams in the list
        {
            bool hasConflictingUnit = currentTeam.conflictUnit != "no";
            bool isCleaner = currentTeam.name == "Cleaner"; // Will never conflict

            if (!isCleaner
                && currentTeam.name == team.name) // Neutral teams are conflicting
            {
                // Only one allowed to have neutral element
                ASSERT(team.element == eElement_Neutral ^ currentTeam.element == eElement_Neutral);

                currentTeam.conflictPtrs.push_back(&team);
            }
            else if (hasConflictingUnit && !isCleaner
                     && currentTeam.conflictUnit == team.conflictUnit) // Units are conflicting
            {
                currentTeam.conflictPtrs.push_back(&team);
            }
            // else // No conflict
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
