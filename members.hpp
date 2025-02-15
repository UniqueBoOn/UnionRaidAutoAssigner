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

    friend class Assigner;
};

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // MEMBERS_HPP

///////////////////////////////////////////////////////////////////////////////////////////////
