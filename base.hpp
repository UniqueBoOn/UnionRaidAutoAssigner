///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef BASE_HPP
#define BASE_HPP

///////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <cassert>
#include <utility>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

class ExcBase { };
class ExcBaseSys : public ExcBase { };
class ExcBaseFile : public ExcBase { };

///////////////////////////////////////////////////////////////////////////////////////////////

class Hookable
{
public:
   virtual void errorCallback() = 0;
   virtual void fatalCallback() = 0;
   virtual ~Hookable() {};
};

Hookable* base_setHooks(Hookable* newHooks);
void base_setErrorFatal(bool isOn);
bool base_isErrorFatal();

///////////////////////////////////////////////////////////////////////////////////////////////

extern Hookable* g_hooks;
extern bool g_isAlreadyDead;
extern bool g_isErrorFatal;

#define DO_FATAL(msg)                \
   do                                \
   {                                 \
      LOG_F << msg;                  \
      g_isAlreadyDead = true;        \
      if (g_hooks)                   \
      {                              \
         g_hooks -> fatalCallback(); \
      }                              \
      _exit(8);                      \
   }                                 \
   while(false)

#define DO_ERROR(msg)                \
   do                                \
   {                                 \
      LOG_F << "Non fatal " << msg;  \
      if (g_hooks)                   \
      {                              \
         g_hooks -> errorCallback(); \
      }                              \
   }                                 \
   while(false)

#define DEAD_END(msg)                \
   do                                \
   {                                 \
      DO_FATAL(msg);                 \
   }                                 \
   while(false)

#define ASSERT_MSG(cond, msg)                            \
   do                                                    \
   {                                                     \
      if (cond)                                          \
      {                                                  \
         /* nop */                                       \
      }                                                  \
      else                                               \
      {                                                  \
         if (!g_isAlreadyDead)                           \
         {                                               \
            ostringstream NAME(smsg);                    \
            NAME(smsg) << " (\"" << msg << "\")";        \
            string NAME(msgstr) = NAME(smsg).str();      \
                                                         \
            ostringstream NAME(s);                       \
            NAME(s) << "assertion \"" #cond "\" failed"  \
                    << (NAME(msgstr).size() > 5          \
                        ? NAME(msgstr)                   \
                        : "");                           \
                                                         \
            if (g_isErrorFatal)                          \
            {                                            \
               DO_FATAL(NAME(s).str());                  \
            }                                            \
            else                                         \
            {                                            \
               DO_ERROR(NAME(s).str());                  \
            }                                            \
         }                                               \
      }                                                  \
   }                                                     \
   while(false)

#define ASSERT_WARNONLY(cond)                           \
   do                                                   \
   {                                                    \
      if (cond)                                         \
      {                                                 \
         /* nop */                                      \
      }                                                 \
      else                                              \
      {                                                 \
         ostringstream NAME(s);                         \
         NAME(s) << "assertion \"" #cond "\" failed";   \
         LOG_W << "WARNONLY:" << (NAME(s).str());       \
      }                                                 \
   }                                                    \
   while(false)

#define ASSERT(cond) ASSERT_MSG(cond, "")

#define VALUE_OF(x) "value of \'" << #x << "\' is \'" << x << "\'"

// following macroes have intentional no braces around the parameters
// to keep compiler-sign-mismatch-warnings

#define ASSERT_EQUAL(a, b)                                     \
   do                                                          \
   {                                                           \
      if (a == b)                                              \
      {                                                        \
         /* nop */                                             \
      }                                                        \
      else                                                     \
      {                                                        \
         if (!g_isAlreadyDead)                                 \
         {                                                     \
            ostringstream NAME(s);                             \
            NAME(s) << "assertion \"" #a " == " #b "\" failed" \
                    << " (" << VALUE_OF(a)                     \
                    << " / " << VALUE_OF(b) << ")";            \
                                                               \
            if (g_isErrorFatal)                               \
            {                                                  \
               DO_FATAL(NAME(s).str());                        \
            }                                                  \
            else                                               \
            {                                                  \
               DO_ERROR(NAME(s).str());                        \
            }                                                  \
         }                                                     \
      }                                                        \
   }                                                           \
   while(false)

#define ASSERT_RANGE(a, val, b)                                \
   do                                                          \
   {                                                           \
      if (a <= val && val <= b)                                \
      {                                                        \
         /* nop */                                             \
      }                                                        \
      else                                                     \
      {                                                        \
         if (!g_isAlreadyDead)                                 \
         {                                                     \
            ostringstream NAME(s);                             \
            NAME(s) << "assertion \"" #a " <= " #val " && " #val " <= " #b "\" failed" \
                    << " (" << VALUE_OF(a)                     \
                    << " / " << VALUE_OF(val)                  \
                    << " / " << VALUE_OF(b) << ")";            \
                                                               \
            if (g_isErrorFatal)                               \
            {                                                  \
               DO_FATAL(NAME(s).str());                        \
            }                                                  \
            else                                               \
            {                                                  \
               DO_ERROR(NAME(s).str());                        \
            }                                                  \
         }                                                     \
      }                                                        \
   }                                                           \
   while(false)

///////////////////////////////////////////////////////////////////////////////////////////////

#if 0
#define SCOPE_ON(flag) _InternalScope UNIQUE_NAME(flag)

class _InternalScope
{
public:
   inline _InternalScope(bool flag) : m_flag(flag) { assert(m_flag == false); m_flag = true; }
   inline ~_InternalScope()                        { assert(m_flag == true); m_flag = false; }
private:
   bool& m_flag;
};
#endif

///////////////////////////////////////////////////////////////////////////////////////////////

#define UNIQUE_NAME NAME(FILE_IDENTIFIER)
#define NAME(id) CONCAT(id, __LINE__)

#define CONCAT(a, b) _CONCAT(a, b)
#define _CONCAT(a, b) a##b

#define FILE_IDENTIFIER _FILE_IDENTIFIER_DEFAULT
#define _FILE_IDENTIFIER_DEFAULT unique_identifier

// UNIQUE_NAME is used in macroes which create objects (named) on the stack
// - in normal cases the __LINE__-macro is sufficient for uniqueness, if not
//   (multiple inclusions of code in same file), one can define FILE_IDENTIFIER
//   to different values

///////////////////////////////////////////////////////////////////////////////////////////////

// static assert
template<bool>
struct STATIC_ASSERTION_FAILED;
template<>
struct STATIC_ASSERTION_FAILED<true>
{
   enum { value = 1 };
};
template<int>
struct MyTest
{
};
#define STATIC_ASSERT(T) typedef MyTest<sizeof(STATIC_ASSERTION_FAILED<(T)>)> MyTypedefName

///////////////////////////////////////////////////////////////////////////////////////////////
// New Enums, Container and Functions

class Boss;
class Teams;
class Member;

enum Element
{
    eElement_Electric,
    eElement_Fire,
    eElement_Iron,
    eElement_Water,
    eElement_Wind,
    eElement_Neutral,
    eElement_Size
};

Element stringToElement(string s);
string elementToString(Element element);

struct Team
{
    string name;
    Element element;
    vector<string> conflictUnits;
    long long int dmg;

    Member* memberPtr;
    vector<Team*> conflictPtrs;
    list<Team*>* elmentListPtr;
};

struct Boss
{
    string name;
    Element element;

    long long int life;
    long long int lifeCurrent;

    double factor;

    vector<Team*> teamsAssigned;
};

struct Member
{
    string name;
    int remaningHits;
    vector<Team> teams;
};

///////////////////////////////////////////////////////////////////////////////////////////////

string base_dirname(string name); // only works with absolute pathes!
string base_basename(string name);

void base_system(string name);

void base_writeFile(string fileName, string content);
bool base_existsFile(string fileName);
void base_removeFile(string fileName);

string base_toString(int value);
string base_toString(long long int value);
string base_toString(double value);
int base_toInt(string value);
long long int base_toLong(string value);
double base_toDouble(string value);

string base_quoteString(string text);
string base_unquoteString(string quotedString);
string base_detachString(string& line, string separator);     // does not trim!
string base_detachStringTrim(string& line, string separator); // splits at 1st seperator, trims
string base_detachFirstWord(string& line);                    // splits at 1st whitespace, trims
string base_detachFirstLine(string& lines);                   // splits at 1st newline, trims

int base_to4BytesAligned(int in);
long long int base_to4BytesAligned(long long int in);

string base_toHex(uint8_t value);
string base_toHex(uint16_t value);
string base_toHex(uint32_t value);
string base_toHex(int8_t value);
string base_toHex(int16_t value);
string base_toHex(int32_t value);

int base_log2(uint32_t value); // ermittelt Nummer des höchstwertigsten gesetzten Bits (oder -1)

int base_roundDown(double x);
int base_roundUp(double x);
int base_round(double x);
int base_divRoundDown(int numerator, int denominator);
int base_divRoundUp(int numerator, int denominator);
int base_divRound(int numerator, int denominator);
int base_dmm2px(double length /* in dmm */, int resolution /* in dpi */);
int base_px2dmm(int pixel /* in px */, int resolution /* in dpi */);
double base_sin(double x);
double base_cos(double x);
double base_tan(double x);
double base_asin(double x);
double base_acos(double x);
double base_atan(double x);
double base_sqrt(double x);
double base_deg2rad(double deg);
double base_rad2deg(double rad);

unsigned char base_crc8(unsigned char* data, int size, unsigned char polynom);
bool base_crc8IsValid(unsigned char* data, int size, unsigned char polynom);

unsigned short base_crc16(unsigned char* data, int size, unsigned short polynom);
bool base_crc16IsValid(unsigned char* data, int size, unsigned short polynom);

int base_encodeBase16(istream& is, ostream& os, int maxBytes = -1);      // z5% -> 7a3525
int base_decodeBase16(istream& is, ostream& os, int maxBytes = -1);      //     <-

int base_encodeBase32(istream& is, ostream& os, map<int, char>& encodeMap, int maxBytes = -1);
int base_decodeBase32(istream& is, ostream& os, map<char, int>& decodeMap, int maxBytes = -1);

int base_encodeBase64(istream& is, ostream& os, int maxBytes = -1);
int base_decodeBase64(istream& is, ostream& os, int maxBytes = -1);

bool base_startsWith(string line, string prefix);
bool base_endsWith(string line, string suffix);
string base_trim(string line, string whitespace = " \n\r\t\f");
string base_shorten(string line, int maxLen);

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // BASE_HPP

///////////////////////////////////////////////////////////////////////////////////////////////
