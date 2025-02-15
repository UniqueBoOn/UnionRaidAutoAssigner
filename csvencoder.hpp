///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CSVENCODER_HPP
#define CSVENCODER_HPP

///////////////////////////////////////////////////////////////////////////////////////////////

#include "base.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

class CSVEncoder
{
public:
    CSVEncoder();

    void pushLine();

#if 1
    // Works on current
    void pushBack(); // Pushes empty space
    void pushBack(int value);
    void pushBack(long long int value);
    void pushBack(double value);
    void pushBack(string value);
#else
    void pushBack(int line); // Pushes empty space
    void pushBack(int value, int line);
    void pushBack(long long int value, int line);
    void pushBack(double value, int line);
    void pushBack(string value, int line);
#endif

    void buildCSV(); // No changes allowed after call
    string getCSV(); // Returns the final table

private:
    struct csvLine
    {
        int elementCount;
        string data;
    };

    string m_delimeter;
    string m_csvTable;
    int m_currentLine;

    vector<csvLine> m_lines;
};

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // CSVENCODER_HPP

///////////////////////////////////////////////////////////////////////////////////////////////
