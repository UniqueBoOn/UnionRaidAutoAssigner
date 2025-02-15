///////////////////////////////////////////////////////////////////////////////////////////////

#include "bosses.hpp"

#include "trace.hpp"

#include "global.hpp"
#include "csvdecoder.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

Bosses::Bosses(string csvData)
    : m_decoder(csvData)
    , m_bosses()
{
    FNC2 << "Bosses()";

    for (int i = 0; i < 15; i++)
    {
        ASSERT(m_decoder.nextLine());
        initBoss();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Bosses::initBoss()
{
    Boss boss;
    boss.name        = m_decoder.fetchString();
    boss.element     = stringToElement(m_decoder.fetchString());
    boss.life        = m_decoder.fetchLong();
    boss.lifeCurrent = boss.life;
    boss.factor      = m_decoder.fetchDouble();

    boss.teamsAssigned.clear();

    cout << "Read in boss " << boss.name
                     << " " << base_toString(boss.element)
                     << " " << base_toString(boss.life)
                     << " " << base_toString(boss.factor)
                     << endl;

    m_bosses.push_back(boss);
}

///////////////////////////////////////////////////////////////////////////////////////////////
