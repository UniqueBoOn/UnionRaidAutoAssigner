///////////////////////////////////////////////////////////////////////////////////////////////
#if 0
#include "base.hpp"
#include "trace.hpp"
#include "traceconfig.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////

// static
volatile bool Trace::s_globalIsOn = true;

///////////////////////////////////////////////////////////////////////////////////////////////

Trace::Trace(const char* file, int line, int level)
   : m_doit(isOn() && isOn(file, line, level))
   , m_header()
{
   if (m_doit)
   {
      m_header = getHeader(file, line, level, 0);
      Thread::getCurrent().incrDepth();
   }
};

///////////////////////////////////////////////////////////////////////////////////////////////

bool Trace::isDoit()
{
   return m_doit;
}

///////////////////////////////////////////////////////////////////////////////////////////////

ostream& Trace::doit()
{
   return Thread::getCurrent().getTraceStream() << m_header << "++ ";
}

///////////////////////////////////////////////////////////////////////////////////////////////

Trace::~Trace()
{
   if (m_doit)
   {
      Thread::getCurrent().decrDepth();
      c_cerr < Thread::getCurrent().getTraceStream() << m_header << "--";
   }
};

///////////////////////////////////////////////////////////////////////////////////////////////

// static
bool Trace::isOn(const char* file, int line, int level)
{
   return TraceConfig::instance().isOn(file, line, level);
}

///////////////////////////////////////////////////////////////////////////////////////////////

// static
string Trace::getHeader(const char* file, int line, int level, int diff)
{
   // this method is called _very_ often. Therefore exceptionally const char* is used
   // instead of string (originating from text-segment-char* there has been no copy
   // whatsoever until now).
   // now the c-string-handling is neccessary:
   // - remove ending ".cpp"
   string::size_type filenameSize = strlen(file);
   if (strcmp(file + filenameSize - 4, ".cpp") == 0) filenameSize -= 4;

   // strip preceding directories away
   // - these exist only in DispQt (Qt-Make) and in some inline-parts, so it's easier to
   //   look specific than search '/'
   if (strncmp(file, "src/", 4) == 0) { file += 4; filenameSize -= 4; }
   if (strncmp(file, "../../H12_BaseOS_src/base/", 26) == 0) { file += 26; filenameSize -= 26; }

   // - take only 13 chars anyway
   if (filenameSize > 13) filenameSize = 13;

   Timestamp now = Timestamp::now();
   ostringstream s;
   s << "["
#if 1
     << now.toLocal() // localtime-Darstellung des timestamps, sehr platzaufwändig
#else
     << now // seit geraumer Zeit nicht mehr gewollt: timestamp nur so, sec+microsecs
#endif
     << "] "
     << "["
     << left << setw(13) << string(file, filenameSize)
     << right << setw(4) << line
     << "] "
     << level << " "
     << "["
     << Thread::getCurrent().getName()
     << "] "
     << setw(Thread::getCurrent().getDepth(diff)) << "";
   return s.str();
}
// changes in format must reflect in c_prefixTraceSize:
const int c_prefixTraceSize = (0
#if 1
                               + 1 + 4+1+2+1+2 + 1 + 2+1+2+1+2 + 1 + 6 + 1  // dateTime
#else
                               + 1 + 10 + 1 + 6 + 1  // timestamp
#endif
                               + 1
                               + 1 + 13 + 4 + 1      // file+line
                               + 1
                               + 1                   // level
                               + 1
                               + 1 + 10 + 1          // thread-name
                               + 1);

///////////////////////////////////////////////////////////////////////////////////////////////

// static
void operator<(int c, ostream& s)
{
   ostringstream& my = (ostringstream&)s;
   string line = my.str();
   Thread::getCurrent().returnTraceStream(my);

   // trace
   TraceConfig::instance().getWriterTrace().write(line);

   // log
   if (c != c_cerr)
   {
      assert((signed)line.size() >= c_prefixTraceSize);
      string toLog = string("(") + (char)c + ") " + base_trim(line.substr(c_prefixTraceSize));
      TraceConfig::instance().getWriterLog().write(toLog);
   }
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////
