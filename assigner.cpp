///////////////////////////////////////////////////////////////////////////////////////////////

#include "assigner.hpp"

#include "global.hpp"
#include "trace.hpp"

#include "bosses.hpp"
#include "teams.hpp"
#include "members.hpp"

#include "csvencoder.hpp"
#include "iassign.hpp"
#include "assignnaive.hpp"

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
    , m_assigner(nullptr)
{
    FNC1 << "Assigner()";
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Assigner::init()
{
    FNC2 << "init()";

    for (int i = 0; i < m_members.size(); i++)
    {
        for (Team& team : m_members[i].teams)
        {
            m_teamsElements[team.element].push_back(&team);

            // Reference list so team can remove itself from list quickly
            team.elmentListPtr = &m_teamsElements[team.element];
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Assigner::initAssignNaive()
{
    FNC2 << "initNaive()";

    ASSERT(m_assigner == nullptr);
    m_assigner = make_unique<AssignNaive>(m_bosses, m_teamsElements);
    m_assigner->init();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Assigner::assign()
{
    FNC2 << "assign()";

    ASSERT(m_assigner != nullptr);
    m_assigner->assign();
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
