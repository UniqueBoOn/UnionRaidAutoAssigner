///////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>

#include "global.hpp"
#include "trace.hpp"
#include "base.hpp"

#include "assigner.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

string loadCSV(string path)
{
    filesystem::path csvPath(path);
    ASSERT(filesystem::exists(csvPath));

    ifstream csvFile(csvPath);
    ASSERT(csvFile.is_open());

    string csvString((istreambuf_iterator<char>(csvFile)), istreambuf_iterator<char>());

    return move(csvString);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void initGlobal()
{
    FNC2 << "Load and init teams, members, dmg and bosses";

    auto bossData = loadCSV("csv/aa_bosses.csv");
    Global::instance().initBosses(bossData);

    auto teamsData = loadCSV("csv/aa_teams.csv");
    Global::instance().initTeams(teamsData);

    auto membersData = loadCSV("csv/aa_teams.csv"); // aa_teams.csv contains also member dmg
    Global::instance().initMembers(membersData);
}

///////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
#if 1
    FILE* tracefile;
    freopen_s(&tracefile, "report.trc", "w", stdout);
    freopen_s(&tracefile, "report.trc", "w", stderr);
#endif

    initGlobal();

    Assigner assigner;
    assigner.init();

    // Currently just a single pass
    //do
    {
        auto start = std::chrono::high_resolution_clock::now();
        LOG_I << "Assign teams of member to all bosses";
        assigner.assign();
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> elapsed = end - start;
        LOG_I << "Assigning took " << base_toString(elapsed.count()) << "ms";

        //LOG_I << "Verify correctness of team assignments";
        //assigner.verify(); // TODO

        //LOG_I << "Grade result";
        // TODO

        start = std::chrono::high_resolution_clock::now();
        LOG_I << "Writeback result";
        assigner.writeback();
        end = std::chrono::high_resolution_clock::now();

        elapsed = end - start;
        LOG_I << "Writeback took " << base_toString(elapsed.count()) << "ms";
    }
    //while (true); // TODO define some goal like amount of iterations or time
}

///////////////////////////////////////////////////////////////////////////////////////////////
