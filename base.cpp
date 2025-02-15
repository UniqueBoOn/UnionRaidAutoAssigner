///////////////////////////////////////////////////////////////////////////////////////////////

#include "base.hpp"

#include "trace.hpp"

//#include <rpc/des_crypt.h> // ecb_crypt, des_setparity, DES_FAILED

///////////////////////////////////////////////////////////////////////////////////////////////
// New
Element stringToElement(string s)
{
    //LOG_I << "stringToElement()";

    Element element = s == "Electric"  ? eElement_Electric
                      : s == "Wind"    ? eElement_Wind
                      : s == "Water"   ? eElement_Water
                      : s == "Fire"    ? eElement_Fire
                      : s == "Iron"    ? eElement_Iron
                      : s == "Neutral" ? eElement_Neutral
                      : (Element)-42;

    ASSERT(element != -42);

    return element;
}

string elementToString(Element element)
{
    //LOG_I << "elementToString()";

    string s = element == eElement_Electric ? "Electric"
               : element == eElement_Wind ? "Wind"
               : element == eElement_Water ? "Water"
               : element == eElement_Fire ? "Fire"
               : element == eElement_Iron ? "Iron"
               : element == eElement_Neutral ? "Neutral"
               : "Unknown";

    ASSERT(s != "Unknown");

    return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////

Hookable* g_hooks = NULL;
bool g_isAlreadyDead = false;
bool g_isErrorFatal = true;

Hookable* base_setHooks(Hookable* newHooks)
{
   Hookable* ret = g_hooks;
   g_hooks = newHooks;
   return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void base_setErrorFatal(bool isOn)
{
   LOG_I << __FUNCTION__ << " is now: " << isOn;
   g_isErrorFatal = isOn;
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool base_isErrorFatal()
{
   return g_isErrorFatal;
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_dirname(string name)
{
   int size = name.size();
   int pos = size;
   while (--pos >= 0 && name[pos] != '/')
   {
   }
   return name.substr(0, pos + 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_basename(string name)
{
   int size = name.size();
   int pos = size;
   while (--pos >= 0 && name[pos] != '/')
   {
   }
   return name.substr(pos + 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void base_system(string name)
{
   int ret = ::system(name.c_str());
   if (ret != 0)
   {
      TRC << "base_system tried \"" << name << "\" - ret = " << ret;
      throw ExcBaseSys();
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void base_writeFile(string fileName, string content)
{
   //   FNC3 << "base_writeFile(" << fileName << ", " << content << ")";
   ofstream s(fileName.c_str());
   s << content;
   if (!s) throw ExcBaseFile();
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool base_existsFile(string fileName)
{
   ifstream s(fileName.c_str());
   return !s.fail();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void base_removeFile(string fileName)
{
   base_system("rm " + fileName);
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_toString(int value)
{
   ostringstream s;
   s << value;
   return s.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_toString(long long int value)
{
    ostringstream s;
    s << value;
    return s.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_toString(double value)
{
    ostringstream s;
    s << value;
    return s.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_toInt(string value)
{
   int ret;
   ASSERT(value.size());    // no digits at all?
   istringstream s(value);
   ASSERT(s);
   s >> ret;
   ASSERT(s);   // value beginnt nicht mit int
   string dummy;
   s >> dummy;
   ASSERT(!s);  // hinter Zahl kommt noch was ...
   return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

long long int base_toLong(string value)
{
    long long int ret;
    ASSERT(value.size());    // no digits at all?
    istringstream s(value);
    ASSERT(s);
    s >> ret;
    ASSERT(s);   // value beginnt nicht mit int
    string dummy;
    s >> dummy;
    ASSERT(!s);  // hinter Zahl kommt noch was ...
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

double base_toDouble(string value)
{
   double ret;
   ASSERT(value.size());    // no digits at all?
   istringstream s(value);
   ASSERT(s);
   s >> ret;
   ASSERT(s);   // value beginnt nicht mit double
   string dummy;
   s >> dummy;
   ASSERT(!s);  // hinter Zahl kommt noch was ...
   return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

const char c_quote = '"';    // quote char is "
const char c_escape = '\\';  // escape char is \ ...

///////////////////////////////////////////////////////////////////////////////////////////////

string base_quoteString(string text)
{
   string escapedInner;
   for (int i = 0; i < (signed)text.size(); ++i)
   {
      char c = text[i];
      switch (c)
      {
         case c_quote:
         case c_escape:
            // escape all specials
            escapedInner += c_escape;
         default:
            // transit char
            escapedInner += c;
            break;
      }
   }
   return c_quote + escapedInner + c_quote;
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_unquoteString(string quotedString)
{
   string ret;

   ASSERT(quotedString.size() >= 2);
   ASSERT(quotedString[0] == c_quote);
   ASSERT(quotedString[quotedString.size()-1] == c_quote);

   bool inQuote = true;
   int index = 1;
   while (inQuote)
   {
      // LOG_I << "drin";
      switch (quotedString[index])
      {
         case c_quote:
            inQuote = false;
            break;
         case c_escape:
            ++index; // skip escape-char itself
            ASSERT(index < (signed)quotedString.size());  // escape is last char of quotedStr?
            // weiter zu default
         default:
            ret += quotedString[index];
            ++index;
            break;
      }
   }

   ++index;
   ASSERT(index == (signed)quotedString.size()); // string continues after end-quote?
   return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_detachStringIntern(string& line, string separator, bool doTrim)
{
   // returns prefix until first found separator
   // changes line to suffix
   // quoting is respected: abc"def\"xyz\\123"ghi jkl -> returns 'abc"..."ghi' and line is 'jkl'
   // doTrim means:
   // - found separator is removed (not in retVal and not in line)
   // - line is base_trim-ed before return (with respect to separators)
   // - typically used with 'whitespace' ... many ws-characters separate only once ...
   string ret;
   int index = 0;
   bool quoting = false;
   bool sepFound = false;
   while (!sepFound)
   {
      //      LOG_I << "drin";
      if (index >= (signed)line.size())
      {
         index = string::npos;
      }
      if (index == (signed)string::npos)
      {
         // no separator found
         ASSERT(!quoting); // open-quote-char without close-quote?
         sepFound = true; // quit loop
         break;
      }

      ASSERT(index != (signed)string::npos);
      ASSERT_RANGE(0, index, (signed)line.size()-1);
      index = line.find_first_of(separator + c_quote + c_escape, index);
      char c = line[index];
      switch(c)
      {
         case c_quote:
            quoting = !quoting;
            index++; // skip this quote
            break;
         case c_escape:
            index++; // skip this escape
            ASSERT(index < (signed)line.size()); // hinter escape-character kommt nix mehr?
            index++; // skip the escaped-char
            break;
         default: // separator found
            if (quoting)
            {
               index++; // skip and ignore this separator
               break;
            }
            else
            {
               sepFound = true; // quit loop
            }
            break;
      }
   }

   //   LOG_I << "raus";
   if (index == (signed)string::npos)
   {
      ret = line;
      line = "";
   }
   else
   {
      ASSERT(0 <= index);
      ret = line.substr(0, index);
      line = line.substr(index);
      if (doTrim)
      {
         line = base_trim(line, /* whitespace = */ separator);
      }
   }
   return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_detachString(string& line, string separator)
{
   return base_detachStringIntern(line, separator, /* doTrim = */ false);
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_detachStringTrim(string& line, string separator)
{
   return base_detachStringIntern(line, separator, /* doTrim = */ true);
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_detachFirstWord(string& line)
{
   return base_detachStringIntern(line, /* separator = */ " \n\r\t\f", /* doTrim = */ true);
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_detachFirstLine(string& lines)
{
   return base_detachStringIntern(lines, /* separator = */ "\n\r", /* doTrim = */ true);
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_to4BytesAligned(int in)
{
   return ((in + 3) & -4);
}

///////////////////////////////////////////////////////////////////////////////////////////////

long long int base_to4BytesAligned(long long int in)
{
   return ((in + 3) & -4);
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_toHex(uint8_t value)
{
   ostringstream s;
   s << "0x" << hex << setw(2) << setfill('0') << (int)value;
   return s.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_toHex(uint16_t value)
{
   ostringstream s;
   s << "0x" << hex << setw(4) << setfill('0') << value;
   return s.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_toHex(uint32_t value)
{
   ostringstream s;
   s << "0x" << hex << setw(8) << setfill('0') << value;
   return s.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_toHex(int8_t value)
{
   return base_toHex(uint8_t(value));
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_toHex(int16_t value)
{
   return base_toHex(uint16_t(value));
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_toHex(int32_t value)
{
   return base_toHex(uint32_t(value));
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_log2(uint32_t value)
{
   int ret = -1;  // return -1 iff value == 0 (value is smaller than 2^0)
   while (value)
   {
      value >>= 1;
      ret++;
   }
   return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_divRoundDown(int numerator, int denominator)
{
   ASSERT(numerator >= 0); // calculating with negative numbers would have to be different
   ASSERT(denominator > 0);
   TRC << fixed << setprecision(5) << VALUE_OF(numerator / denominator);
   return numerator / denominator;
}
int base_divRoundUp(int numerator, int denominator)
{
   return base_divRoundDown(numerator + denominator - 1, denominator);
}
int base_divRound(int numerator, int denominator)
{
   return base_divRoundDown(numerator + denominator / 2, denominator);
}


const double c_epsilon = 0.00000001;
int base_roundDown(double x)
{
   if (x < 0.0) return -base_roundDown(-x);
   int ret = x;
   return ret;
}
int base_roundUp(double x)
{
   if (x < 0.0) return -base_roundUp(-x);
   int ret = x + 1.0 - c_epsilon;
   return ret;
}
int base_round(double x)
{
   if (x < 0.0) return -base_round(-x);
   int ret = x + 0.5;
   return ret;
}

int base_dmm2px(double length /* in dmm */, int resolution /* in dpi */)
{
   const int c_dmm_per_inch = 254;
   int px = base_roundUp(length * resolution / c_dmm_per_inch);
   return px;
}
int base_px2dmm(int pixel /* in px */, int resolution /* in dpi */)
{
   const int c_dmm_per_inch = 254;
   int length = base_roundUp(pixel * c_dmm_per_inch / resolution);
   return length;
}

#include <fenv.h>

#define DEF_MATH(mathFunc)                                              \
   double base_##mathFunc(double x)                                     \
   {                                                                    \
      ASSERT(!isnan(x));                                                \
      errno = 0;                                                        \
      feclearexcept(FE_ALL_EXCEPT);                                     \
      double ret = mathFunc(x);                                         \
      ASSERT(errno == 0);                                               \
      ASSERT(fetestexcept(FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID) == 0); \
      return ret;                                                       \
   }                                                                    \

DEF_MATH(sin);
DEF_MATH(cos);
DEF_MATH(tan);
DEF_MATH(asin);
DEF_MATH(acos);
DEF_MATH(atan);
DEF_MATH(sqrt);

const double c_pi = base_acos(-1);

double base_deg2rad(double deg)
{
   ASSERT_RANGE(0.0 - c_epsilon, deg, 360.0 + c_epsilon);
   double rad = deg * c_pi / 180.0;
   return rad;
}
double base_rad2deg(double rad)
{
   ASSERT_RANGE(0.0 - c_epsilon, rad, 2 * c_pi + c_epsilon);
   double deg = rad * 180.0 / c_pi;
   return deg;
}

///////////////////////////////////////////////////////////////////////////////////////////////

unsigned char base_crc8(unsigned char* data, int size, unsigned char polynom)
{
   unsigned short remainder = 0;
   for (int i = 0; i < size; i++)
   {
      remainder |= (data[i] << 8);

      for (int j = 0; j < 8; j++)
      {
         if ((remainder & 0x0001) == 1)
         {
            remainder = (remainder >> 1) ^ polynom;
         }
         else
         {
            remainder = (remainder >> 1);
         }
      }
   }

   return (unsigned char)remainder;
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool base_crc8IsValid(unsigned char* data, int size, unsigned char polynom)
{
   return base_crc8(data, size, polynom) == 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

unsigned short base_crc16(unsigned char* data, int size, unsigned short polynom)
{
   ASSERT(size > 0);

   unsigned long crc = 0;
   for (int i = 0; i < size; i++)
   {
      crc |= (data[i] << 16);

      for (int j = 0; j < 8; j++)
      {
         if (crc & 0x00000001)
         {
            crc = (crc >> 1) ^ polynom;
         }
         else
         {
            crc = (crc >> 1);
         }
      }
   }

   return (unsigned short)crc;
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool base_crc16IsValid(unsigned char* data, int size, unsigned short polynom)
{
   return base_crc16(data, size, polynom) == 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_encodeBase16(istream& is, ostream& os, int maxBytes)
{
   int encodedBytes = 0;
   while (maxBytes == -1 || encodedBytes < maxBytes)
   {
      int c = is.get();                                // read next byte
      if (!is) break;                                  // break on end of stream
      os << hex << setw(2) << setfill('0') << c;       // code byte as 2 hex digits
      ++encodedBytes;
   }
   return encodedBytes;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_decodeBase16(istream& is, ostream& os, int maxBytes)
{
   int decodedBytes = 0;
   while (maxBytes == -1 || decodedBytes < maxBytes)
   {
      char buf[2 + 1];
      is.get(buf, 2 + 1);
      if (!is) break;                                  // break on end of stream

      ASSERT(strlen(buf) == 2);
      istringstream myIs(buf);
      unsigned int byte;
      myIs >> hex >> byte;

      os << (unsigned char)(byte);
      ++decodedBytes;
   }
   return decodedBytes;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_encodeBase32(istream& is, ostream& os, map<int, char>& encodeMap, int maxBytes)
{
   ASSERT(encodeMap.size() == 32);

   int encodedBytes = 0;
   int missingBytes = 0;

   while (maxBytes == -1 || encodedBytes < maxBytes)
   {
      int b1 = is.get();
      if (!is) { break; }

      int b2 = is.get();
      if (!is) { b2 = 0; missingBytes++; }

      int b3 = is.get();
      if (!is) { b3 = 0; missingBytes++; }

      int b4 = is.get();
      if (!is) { b4 = 0; missingBytes++; }

      int b5 = is.get();
      if (!is) { b5 = 0; missingBytes++; }

#if 0
      // This is the correct definition, but we cant use it because this
      // function will be used for scanner option codes. Those option
      // codes use mirrored bits for the character encoding (see #else).

      int c1 =   (b1 >> 3) & (16 + 8 + 4 + 2 + 1);     // Bits 7,6,5,4,3       of Byte 1

      int c2 = (((b1 << 2) & (16 + 8 + 4        )) +   // Bits           2,1,0 of Byte 1
                ((b2 >> 6) & (             2 + 1)));   // Bits 7,6             of Byte 2

      int c3 =   (b2 >> 1) & (16 + 8 + 4 + 2 + 1);     // Bits     5,4,3,2,1   of Byte 2

      int c4 = (((b2 << 4) & (16                )) +   // Bits               0 of Byte 2
                ((b3 >> 4) & (     8 + 4 + 2 + 1)));   // Bits 7,6,5,4         of Byte 3

      int c5 = (((b3 << 1) & (16 + 8 + 4 + 2    )) +   // Bits         3,2,1,0 of Byte 3
                ((b4 >> 7) & (                 1)));   // Bits 7               of Byte 4

      int c6 =   (b4 >> 2) & (16 + 8 + 4 + 2 + 1);     // Bits   6,5,4,3,2     of Byte 4

      int c7 = (((b4 << 3) & (16 + 8            )) +   // Bits             1,0 of Byte 4
                ((b5 >> 5) & (         4 + 2 + 1)));   // Bits 7,6,5           of Byte 5

      int c8 =   (b5 >> 0) & (16 + 8 + 4 + 2 + 1);     // Bits       4,3,2,1,0 of Byte 5
#else
      // Scanner option code character encoding ...

      int c1 =   (b1 >> 0) & (16 + 8 + 4 + 2 + 1);     // Bits       4,3,2,1,0 of Byte 1

      int c2 = (((b1 >> 5) & (         4 + 2 + 1)) +   // Bits 7,6,5           of Byte 1
                ((b2 << 3) & (16 + 8            )));   // Bits             1,0 of Byte 2

      int c3 =   (b2 >> 2) & (16 + 8 + 4 + 2 + 1);     // Bits   6,5,4,3,2     of Byte 2

      int c4 = (((b2 >> 7) & (                 1)) +   // Bits 7               of Byte 2
                ((b3 << 1) & (16 + 8 + 4 + 2    )));   // Bits         3,2,1,0 of Byte 3

      int c5 = (((b3 >> 4) & (     8 + 4 + 2 + 1)) +   // Bits 7,6,5,4         of Byte 3
                ((b4 << 4) & (16                )));   // Bits               0 of Byte 4

      int c6 =   (b4 >> 1) & (16 + 8 + 4 + 2 + 1);     // Bits     5,4,3,2,1   of Byte 4

      int c7 = (((b4 >> 6) & (             2 + 1)) +   // Bits 7,6             of Byte 4
                ((b5 << 2) & (16 + 8 + 4        )));   // Bits           2,1,0 of Byte 5

      int c8 =   (b5 >> 3) & (16 + 8 + 4 + 2 + 1);     // Bits 7,6,5,4,3       of Byte 5
#endif

      os <<                            encodeMap.at(c1)
         <<                            encodeMap.at(c2)
         << (missingBytes >= 4 ? '=' : encodeMap.at(c3))
         << (missingBytes >= 4 ? '=' : encodeMap.at(c4))
         << (missingBytes >= 3 ? '=' : encodeMap.at(c5))
         << (missingBytes >= 2 ? '=' : encodeMap.at(c6))
         << (missingBytes >= 2 ? '=' : encodeMap.at(c7))
         << (missingBytes >= 1 ? '=' : encodeMap.at(c8));

      encodedBytes += 5;

      if (!is) break;
   }

   return encodedBytes;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_decodeBase32(istream& is, ostream& os, map<char, int>& decodeMap, int maxBytes)
{
   ASSERT(decodeMap.size() == 32);

   int decodedBytes = 0;
   int missingBytes = 0;

   char buf;
   while (maxBytes == -1 || decodedBytes < maxBytes)
   {
      is.get(buf);
      if (!is) { break; }
      int c1 = decodeMap.at(buf);

      is.get(buf);
      int c2 = decodeMap.at(buf);

      is.get(buf);
      int c3 = 0;
      if (buf != '=') { c3 = decodeMap.at(buf); }
      else { missingBytes++; }

      is.get(buf);
      int c4 = 0;
      if (buf != '=') { c4 = decodeMap.at(buf); }

      is.get(buf);
      int c5 = 0;
      if (buf != '=') { c5 = decodeMap.at(buf); }
      else {  missingBytes++; }

      is.get(buf);
      int c6 = 0;
      if (buf != '=') { c6 = decodeMap.at(buf); }
      else { missingBytes++; }

      is.get(buf);
      int c7 = 0;
      if (buf != '=') { c7 = decodeMap.at(buf); }

      is.get(buf);
      int c8 = 0;
      if (buf != '=') { c8 = decodeMap.at(buf); }
      else { missingBytes++; }

#if 0
      // This is the correct definition, but we cant use it because this
      // function will be used for scanner option codes. Those option
      // codes use mirrored bits for the character encoding (see #else).

      int b1 = (((c1 << 3) & (128 + 64 + 32 + 16 + 8            )) +
                ((c2 >> 2) & (                         4 + 2 + 1)));

      int b2 = (((c2 << 6) & (128 + 64                          )) +
                ((c3 << 1) & (           32 + 16 + 8 + 4 + 2    )) +
                ((c4 >> 4) & (                                 1)));

      int b3 = (((c4 << 4) & (128 + 64 + 32 + 16                )) +
                ((c5 >> 1) & (                     8 + 4 + 2 + 1)));

      int b4 = (((c5 << 7) & (128                               )) +
                ((c6 << 2) & (      64 + 32 + 16 + 8 + 4        )) +
                ((c7 >> 3) & (                             2 + 1)));

      int b5 = (((c7 << 5) & (128 + 64 + 32                     )) +
                ((c8 >> 0) & (                16 + 8 + 4 + 2 + 1)));
#else
      // Scanner option code character decoding ...

      int b1 = (((c1 << 0) & (                16 + 8 + 4 + 2 + 1)) +
                ((c2 << 5) & (128 + 64 + 32                     )));

      int b2 = (((c2 >> 3) & (                             2 + 1)) +
                ((c3 << 2) & (      64 + 32 + 16 + 8 + 4        )) +
                ((c4 << 7) & (128                               )));

      int b3 = (((c4 >> 1) & (                     8 + 4 + 2 + 1)) +
                ((c5 << 4) & (128 + 64 + 32 + 16                )));

      int b4 = (((c5 >> 4) & (                                 1)) +
                ((c6 << 1) & (           32 + 16 + 8 + 4 + 2    )) +
                ((c7 << 6) & (128 + 64                          )));

      int b5 = (((c7 >> 2) & (                         4 + 2 + 1)) +
                ((c8 << 3) & (128 + 64 + 32 + 16 + 8            )));
#endif

      /* */                 { os << (unsigned char)b1; decodedBytes++; }
      if (missingBytes < 4) { os << (unsigned char)b2; decodedBytes++; }
      if (missingBytes < 3) { os << (unsigned char)b3; decodedBytes++; }
      if (missingBytes < 2) { os << (unsigned char)b4; decodedBytes++; }
      if (missingBytes < 1) { os << (unsigned char)b5; decodedBytes++; }
   }

   return decodedBytes;
}

///////////////////////////////////////////////////////////////////////////////////////////////

char base_encodeBase64char(int c)
{
   ASSERT_RANGE(0, c, 63);

   return (c < 26 ? 'A' + c -  0 :
           c < 52 ? 'a' + c - 26 :
           c < 62 ? '0' + c - 52 :
           c == 62 ? '+' : '/');
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_decodeBase64char(char c)
{
   if (c >= 'A' && c <= 'Z') return  0 + (c - 'A');
   if (c >= 'a' && c <= 'z') return 26 + (c - 'a');
   if (c >= '0' && c <= '9') return 52 + (c - '0');
   if (c == '+') return 62;
   if (c == '/') return 63;

   ASSERT(c == '=');
   return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_encodeBase64(istream& is, ostream& os, int maxBytes)
{
   if (maxBytes != -1)
   {
      maxBytes -= maxBytes % 3;
      ASSERT(maxBytes % 3 == 0);
   }
   int encodedBytes = 0;
   int appendedZeros = 0;
   while (maxBytes == -1 || encodedBytes < maxBytes)
   {
      int b1 = is.get();
      if (!is) { break; }

      int b2 = is.get();
      if (!is) { b2 = 0; ++appendedZeros; }

      int b3 = is.get();
      if (!is) { b3 = 0; ++appendedZeros; }

      int c1 =   (b1 >> 2) & (32 + 16 + 8 + 4 + 2 + 1);     // Bits 7,6,5,4,3,2     of Byte 1
      int c2 = (((b1 << 4) & (32 + 16                )) +   // Bits             1,0 of Byte 1
                ((b2 >> 4) & (          8 + 4 + 2 + 1)));   // Bits 7,6,5,4         of Byte 2
      int c3 = (((b2 << 2) & (32 + 16 + 8 + 4        )) +   // Bits         3,2,1,0 of Byte 2
                ((b3 >> 6) & (                  2 + 1)));   // Bits 7,6             of Byte 3
      int c4 =   (b3 >> 0) & (32 + 16 + 8 + 4 + 2 + 1);     // Bits     5,4,3,2,1,0 of Byte 3

      os <<                            base_encodeBase64char(c1)
         <<                            base_encodeBase64char(c2)
         << (appendedZeros > 1 ? '=' : base_encodeBase64char(c3))
         << (appendedZeros > 0 ? '=' : base_encodeBase64char(c4));

      encodedBytes += 3;

      if (!is) break;
   }
   return encodedBytes;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int base_decodeBase64(istream& is, ostream& os, int maxBytes)
{
   if (maxBytes != -1)
   {
      maxBytes -= maxBytes % 3;
      ASSERT(maxBytes % 3 == 0);
   }
   int decodedBytes = 0;
   int appendedZeros = 0;
   while (maxBytes == -1 || decodedBytes < maxBytes)
   {
      char buf[4 + 1];
      is.get(buf, 4 + 1);
      if (!is) break;                                  // break on end of stream

      ASSERT(strlen(buf) == 4);

      int c1 = base_decodeBase64char(buf[0]);
      ASSERT(c1 != -1);
      int c2 = base_decodeBase64char(buf[1]);
      ASSERT(c2 != -1);
      int c3 = base_decodeBase64char(buf[2]);
      if (c3 == -1) { c3 = 0; appendedZeros ++; }
      int c4 = base_decodeBase64char(buf[3]);
      if (c4 == -1) { c4 = 0; appendedZeros ++; }


      int b1 = ( ((c1 << 2) & (128 + 64 + 32 + 16 + 8 + 4        )) +
                 ((c2 >> 4) & (                             2 + 1)) );
      int b2 = ( ((c2 << 4) & (128 + 64 + 32 + 16                )) +
                 ((c3 >> 2) & (                     8 + 4 + 2 + 1)) );
      int b3 = ( ((c3 << 6) & (128 + 64                          )) +
                 ((c4 >> 0) & (           32 + 16 + 8 + 4 + 2 + 1)) );


      /* */                  { os << (unsigned char)(b1); ++decodedBytes; }
      if (appendedZeros < 2) { os << (unsigned char)(b2); ++decodedBytes; }
      if (appendedZeros < 1) { os << (unsigned char)(b3); ++decodedBytes; }

   }
   return decodedBytes;
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool base_startsWith(string line, string prefix)
{
   return line.substr(0, prefix.size()) == prefix;
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool base_endsWith(string line, string suffix)
{
   return line.size() > suffix.size() && line.substr(line.size() - suffix.size()) == suffix;
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_trim(string line, string whitespace)
{
   string::size_type pos1 = line.find_first_not_of(whitespace);
   if (pos1 == string::npos) return "";
   string::size_type pos2 = line.find_last_not_of(whitespace);
   return line.substr(pos1, (pos2 + 1) - pos1);
}

///////////////////////////////////////////////////////////////////////////////////////////////

string base_shorten(string line, int maxLen)
{
   if ((signed)line.size() <= maxLen)
   {
      return line;
   }

   const string dots = "...";
   const int endChars = 3;    // genau so viele werden gezeigt
   const int needed = (dots.size() + endChars);

   ASSERT(maxLen >= needed);
   return line.substr(0, maxLen - needed) + dots + line.substr(line.size() - endChars);
}

///////////////////////////////////////////////////////////////////////////////////////////////
