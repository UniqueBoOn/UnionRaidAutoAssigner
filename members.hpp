///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MEMBERS_HPP
#define MEMBERS_HPP

///////////////////////////////////////////////////////////////////////////////////////////////

#include "base.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

class Teams;

///////////////////////////////////////////////////////////////////////////////////////////////

class Members
{
public:
    Members(string csvData);

private:

    Teams& m_teams;

    vector<Member> m_members;

    void fillConflictList(Team& team, vector<Team>& teams);
    bool hasTeamConflicts(Team& currentTeam, Team& team);

    friend class Assigner;
};

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // MEMBERS_HPP

///////////////////////////////////////////////////////////////////////////////////////////////
