///////////////////////////////////////////////////////////////////////////////////////////////

#include "assigner.hpp"

#include "global.hpp"

#include "trace.hpp"

#include "bosses.hpp"
#include "teams.hpp"
#include "members.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

// static
Global& Global::instance()
{
    static Global global;

    return global;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Global::initBosses(string csvData)
{
    FNC2 << "initBosses()";

    ASSERT(m_bosses == nullptr);

    m_bosses = new Bosses(csvData);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Bosses& Global::getBosses()
{
    FNC2 << "getBosses()";

    assert(m_bosses != nullptr);

    return *m_bosses;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Global::initTeams(string csvData)
{
    FNC2 << "initTeams()";

    assert(m_teams == nullptr);

    m_teams = new Teams(csvData);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Teams& Global::getTeams()
{
    FNC2 << "getTeams()";

    assert(m_teams != nullptr);

    return *m_teams;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Global::initMembers(string csvData)
{
    FNC2 << "initMembers()";
    
    assert(m_members == nullptr);

    m_members = new Members(csvData);
}

///////////////////////////////////////////////////////////////////////////////////////////////

Members& Global::getMembers()
{
    FNC2 << "getMembers()";

    assert(m_members != nullptr);

    return *m_members;
}

///////////////////////////////////////////////////////////////////////////////////////////////

Global::Global()
    : m_bosses(nullptr)
    , m_teams(nullptr)
    , m_members(nullptr)
{
    FNC2 << "Global()";
}

///////////////////////////////////////////////////////////////////////////////////////////////
