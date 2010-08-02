#ifndef Json_h
#define Json_h

#include <map>
#include <string>
#include <sstream>
#include "DynArray.h"
#include "Ref.h"

//#define JSON_SEADRAGON_TYPES

#ifdef JSON_SEADRAGON_TYPES
    #ifdef USE_MINI_COMMON
        #include "MiniCommon/Quaternion.h"
        #include "MiniCommon/Matrix.h"
    #else
    ///////INSERT HEADERS FOR !USE_MINI_COMMON CASE HERE///////
        using namespace Seadragon;
    #endif
#endif // JSON_SEADRAGON_TYPES

#define JSON_USE_CHAR

#ifdef JSON_USE_CHAR
typedef char JsonChar;
#else // !JSON_USE_CHAR
typedef wchar_t JsonChar;
#endif // JSON_USE_CHAR

typedef std::basic_string<char> JsonString8;
typedef std::basic_string<wchar_t> JsonStringW;
typedef std::basic_string<JsonChar> JsonString;
typedef std::basic_istream<JsonChar> JsonIStream;
typedef std::basic_ostream<JsonChar> JsonOStream;
typedef std::basic_istringstream<JsonChar> JsonIStringStream;
typedef std::basic_ostringstream<JsonChar> JsonOStringStream;

/// JsonException is used for safe runtime errors, which include: attempting
/// to index an associative array (JsonInDict*) element that doesn't
/// exist using [], attempting to index out-of-bounds on an array element
/// (JsonInArray*) using .get(), JSON parse errors, and exceeding the
/// maximum nesting depth.
class JsonException {
public:
    int line_;
    JsonString why_;
    JsonException(int line, const JsonString & why);
};

#define JsonThrow(XX) throw JsonException(__LINE__, (XX))
#define JsonVerify(XX) if (!(XX)) JsonThrow( ToJsonString( #XX ) )

inline int HexDigitToInt(JsonChar x);


template <class T> inline JsonStringW ToJsonStringW(const T * p) {
    b_assert(p);
    JsonStringW s;
    while (*p) {
        s += wchar_t(*p++);
    }
    return s;
}

template <class T> inline JsonString8 ToJsonString8(const T * p) {
    b_assert(p);
    JsonString8 s;
    while (*p) {
        s += char(*p++);
    }
    return s;
}


// added by Greg
inline
JsonString8 toJsonString8(const JsonStringW &jsw)
{
    JsonString8 ret(jsw.length(), '?');
    for(int i = 0; i < (int) jsw.length(); i++)
        ret[i] = (char) jsw[i];
    return ret;
}

inline
JsonStringW toJsonStringW(const JsonString8 &js8)
{
    JsonStringW ret(js8.length(), '?');
    for(int i = 0; i < (int) js8.length(); i++)
        ret[i] = js8[i];
    return ret;
}


#ifdef JSON_USE_CHAR

inline JsonString ToJsonString(const JsonChar * p) { return ToJsonString8(p); }
inline JsonString ToJsonString(const wchar_t  * p) { return ToJsonString8(p); }

#define json_printf b_printf

inline JsonString JsonUnicode(JsonChar c) {
    const static JsonString h(ToJsonString("0123456789abcdef"));
    return JsonString("\\u00") + h[int(c)>>4] + h[int(c)&0xf];
}

#else // !JSON_USE_CHAR

inline JsonString ToJsonString(const JsonChar * p) { return ToJsonStringW(p); }
inline JsonString ToJsonString(const char     * p) { return ToJsonStringW(p); }

#define json_printf b_wprintf
inline JsonString JsonUnicode(JsonChar c) {
    const static JsonString h(ToJsonString("0123456789abcdef"));
    return JsonString(ToJsonString("\\u"))
        + h[ int(c)>>12       ]
        + h[(int(c)>> 8) & 0xf]
        + h[(int(c)>> 4) & 0xf]
        + h[ int(c)      & 0xf];
}
#endif // JSON_USE_CHAR

inline int HexDigitToInt(JsonChar x) {
    if (x >= JsonChar('0') && x <= JsonChar('9')) {
        return int(x)-int('0');
    } else if (x >= JsonChar('a') && x <= JsonChar('f')) {
        return int(x)+10-int('a');
    } else if (x >= JsonChar('A') && x <= JsonChar('F')) {
        return int(x)+10-int('A');
    }
    JsonThrow(ToJsonString("invalid hex digit").c_str());
    return -1;
}

inline JsonChar UnicodeJson(JsonChar a, JsonChar b, JsonChar c, JsonChar d) {
    return (JsonChar)((HexDigitToInt(a) << 12)
                    | (HexDigitToInt(b) <<  8)
                    | (HexDigitToInt(c) <<  4)
                    | (HexDigitToInt(d)      ));
}

template <class T> inline JsonString EncodeJsonString_T(const T * p) {
    b_assert(p);
    JsonString s;
    while (*p) {
        if (*p <= 126) {
            if (     *p == '"' ) { s += ToJsonString("\\\""); }
            else if (*p == '\\') { s += ToJsonString("\\\\"); }
            else if (*p == '/' ) { s += ToJsonString("\\/" ); }
            else if (*p == '\b') { s += ToJsonString("\\b" ); }
            else if (*p == '\f') { s += ToJsonString("\\f" ); }
            else if (*p == '\n') { s += ToJsonString("\\n" ); }
            else if (*p == '\r') { s += ToJsonString("\\r" ); }
            else if (*p == '\t') { s += ToJsonString("\\t" ); }
            else if (*p  <  32 ) { s += ToJsonString(JsonUnicode(JsonChar(*p)).c_str()); }
            else                 { s +=                          JsonChar(*p);           }
        } else                   { s += ToJsonString(JsonUnicode(JsonChar(*p)).c_str()); }
        p++;
    }
    return s;
}

inline JsonString EncodeJsonString(const char * p) { return EncodeJsonString_T(p); }
inline JsonString EncodeJsonString(const wchar_t * p) { return EncodeJsonString_T(p); }

template <class T> inline std::basic_string<T> DecodeJsonString_T(const JsonString & s) {
    std::basic_string<T> d;
    int i=0, n=int(s.size());
    while (i<n) {
        T c = T(s[i]);
        if (c==T('\\')) {
            ++i;
            if (i>=n) {
                JsonThrow(ToJsonString("string truncated in escape sequence").c_str());
            }
            c = T(s[i]);
            if (c=='"' || c=='\\' || c=='/') {
                d += c;
            } else if (c=='u') {
                if (i+4 >= n) {
                    JsonThrow(ToJsonString("string truncated in unicode quartet").c_str());
                }
                JsonChar c0 = JsonChar(s[++i]),
                         c1 = JsonChar(s[++i]),
                         c2 = JsonChar(s[++i]),
                         c3 = JsonChar(s[++i]);
                d += T(UnicodeJson(c0,c1,c2,c3));
            } else {
                if      (c=='b') { d += T('\b'); }
                else if (c=='f') { d += T('\f'); }
                else if (c=='n') { d += T('\n'); }
                else if (c=='r') { d += T('\r'); }
                else if (c=='t') { d += T('\t'); }
                else {
                    JsonThrow(ToJsonString("unrecognized escape sequence").c_str());
                }
            }
        } else {
            d += c;
        }
        ++i;
    }
    return d;
}

inline JsonString8 DecodeJsonStringTo8(const JsonString & s) { return DecodeJsonString_T<char    >(s); }
inline JsonStringW DecodeJsonStringToW(const JsonString & s) { return DecodeJsonString_T<wchar_t >(s); }
inline JsonString  DecodeJsonString   (const JsonString & s) { return DecodeJsonString_T<JsonChar>(s); }

#endif // Json_h
