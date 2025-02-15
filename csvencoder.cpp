///////////////////////////////////////////////////////////////////////////////////////////////

#include "csvencoder.hpp"

#include "trace.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

CSVEncoder::CSVEncoder()
    : m_delimeter(";")
    , m_csvTable("")
    , m_currentLine(-42)
    , m_lines()
{
    LOG_I << "CSVEncoder()";
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CSVEncoder::pushLine()
{
    csvLine line;
    line.data = "";//.clear();
    line.elementCount = 0;

    m_lines.push_back(line);

    if (m_currentLine != -42) m_currentLine++;
    else m_currentLine = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CSVEncoder::pushBack()
{
    ASSERT(m_currentLine != -42);

    m_lines[m_currentLine].data += m_delimeter;
    m_lines[m_currentLine].elementCount++;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CSVEncoder::pushBack(int value)
{
    ASSERT(m_currentLine != -42);

    m_lines[m_currentLine].data += base_toString(value) + m_delimeter;
    m_lines[m_currentLine].elementCount++;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CSVEncoder::pushBack(long long int value)
{
    ASSERT(m_currentLine != -42);

    m_lines[m_currentLine].data += base_toString(value) + m_delimeter;
    m_lines[m_currentLine].elementCount++;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CSVEncoder::pushBack(double value)
{
    ASSERT(m_currentLine != -42);

    m_lines[m_currentLine].data += base_toString(value) + m_delimeter;
    m_lines[m_currentLine].elementCount++;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CSVEncoder::pushBack(string value)
{
    ASSERT(m_currentLine != -42);

    m_lines[m_currentLine].data += value + m_delimeter;
    m_lines[m_currentLine].elementCount++;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CSVEncoder::buildCSV()
{
    ASSERT(!m_lines.empty());

    stringstream ss;
    for (auto& line : m_lines)
    {
        ss << line.data << endl;
    }

    m_csvTable = ss.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////

string CSVEncoder::getCSV()
{
    ASSERT(!m_csvTable.empty());

    return m_csvTable;
}

///////////////////////////////////////////////////////////////////////////////////////////////
