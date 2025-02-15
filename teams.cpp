///////////////////////////////////////////////////////////////////////////////////////////////

#include "teams.hpp"

#include "trace.hpp"

#include "csvdecoder.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

Teams::Teams(string csvData)
    : m_teams()
{
    FNC2 << "Teams()";

    CSVDecoder csvDecoder(csvData);

    // Everything depends on being in order!!

    ///////////////////////////////////////////////////////////////////////////
    LOG_I << "Load team names";

    ASSERT(csvDecoder.nextLine());
    csvDecoder.fetchNone(); // Skip first;

    vector<string> teamNames;
    do
    {
        string name = csvDecoder.fetchString();
        teamNames.push_back(name);

        LOG_I << "Name " << name;
    }
    while (!csvDecoder.isLineEmpty());

    ///////////////////////////////////////////////////////////////////////////
    LOG_I << "Load team elements";

    ASSERT(csvDecoder.nextLine());
    csvDecoder.fetchNone(); // Skip first;
    
    vector<Element> teamElements;
    do
    {
        Element element = stringToElement(csvDecoder.fetchString());
        teamElements.push_back(element);
    }
    while (!csvDecoder.isLineEmpty());

    ///////////////////////////////////////////////////////////////////////////
    LOG_I << "Load team conflict";

    ASSERT(csvDecoder.nextLine());
    csvDecoder.fetchNone(); // Skip first

    vector<string> teamConflict;
    do
    {
        string conflict = csvDecoder.fetchString();
        teamConflict.push_back(conflict);
    }
    while (!csvDecoder.isLineEmpty());

    // Ensures everthing make sense
    ASSERT(teamNames.size() == teamElements.size());
    ASSERT(teamElements.size() == teamConflict.size());

    ///////////////////////////////////////////////////////////////////////////
    LOG_I << "Merge into single team";

    vector<Team> teams;
    for (size_t i = 0; i < teamNames.size(); i++)
    {
        Team team;
        team.name         = teamNames[i];
        team.element      = teamElements[i];
        team.conflictUnit = teamConflict[i];

        // Filled in Member class
        team.dmg        = -42;
        team.memberPtr  = nullptr;
        team.conflictPtrs.clear();

        m_teams.push_back(team);
    }

    // decoder bla bla
}

///////////////////////////////////////////////////////////////////////////////////////////////
