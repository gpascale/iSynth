//#include "Stable.h"
#include "JsonInput.h"
#include <iomanip>
#include <limits.h>
#include <math.h>

int JsonInAtom::nInstances_ = 0;
JsonInNull JsonNull;

JsonException::JsonException(int line, const JsonString & why)
  : line_(line), why_(why) {
    json_printf(ToJsonString("JsonException (%d): %s").c_str(), line, why.c_str());
}

JsonInAtom::~JsonInAtom() {
    nDec();
}

JsonInString::~JsonInString() {
    if (!is_alias()) {
        delete &(value());
    }
}

JsonInArray::~JsonInArray() {
    if (!is_alias()) {
        const size_t n = value().size();
        for (size_t i=0; i<n; ++i) {
            delete value()[i];
        }
        delete &(value());
    }
}

JsonInDict::~JsonInDict() {
    if (!is_alias()) {
        V::iterator it, it1 = value().end();
        for (it=value().begin(); it!=it1; ++it) {
            delete it->second;;
        }
        delete &(value());
    }
}

JsonInArrayString::~JsonInArrayString() { if (!is_alias()) { delete &(value()); } }
JsonInArrayInt   ::~JsonInArrayInt   () { if (!is_alias()) { delete &(value()); } }
JsonInArrayDouble::~JsonInArrayDouble() { if (!is_alias()) { delete &(value()); } }
JsonInArrayBool  ::~JsonInArrayBool  () { if (!is_alias()) { delete &(value()); } }

JsonInDictString::~JsonInDictString() { if (!is_alias()) { delete &(value()); } }
JsonInDictInt   ::~JsonInDictInt   () { if (!is_alias()) { delete &(value()); } }
JsonInDictDouble::~JsonInDictDouble() { if (!is_alias()) { delete &(value()); } }
JsonInDictBool  ::~JsonInDictBool  () { if (!is_alias()) { delete &(value()); } }

template <class T> inline static const T & get_ind(const DynArray<T> & arr, size_t i) {
    if (i>=arr.size()) {
        JsonThrow(ToJsonString("index out of bounds"));
    }
    return arr[i];
}

const JsonInAtom & JsonInArray      ::get(size_t i) const { return  *(get_ind(value(),i)); }
const JsonString & JsonInArrayString::get(size_t i) const { return    get_ind(value(),i) ; }
int                JsonInArrayInt   ::get(size_t i) const { return    get_ind(value(),i) ; }
double             JsonInArrayDouble::get(size_t i) const { return    get_ind(value(),i) ; }
bool               JsonInArrayBool  ::get(size_t i) const { return 0!=get_ind(value(),i) ; }

template <class T> inline static const T & find_key(const std::map<JsonString,T> & dict,
                                                    const JsonString & key) {
	
    typename std::map<JsonString, T>::const_iterator it = dict.find(key);
    if (it==dict.end()) {
        JsonThrow(key+ToJsonString(" not found"));
    }
    return it->second;
}

const JsonInAtom & JsonInDict      ::operator [] (const JsonString & key) const { return  *(find_key(value(),key)); }
const JsonString & JsonInDictString::operator [] (const JsonString & key) const { return    find_key(value(),key) ; }
int                JsonInDictInt   ::operator [] (const JsonString & key) const { return    find_key(value(),key) ; }
double             JsonInDictDouble::operator [] (const JsonString & key) const { return    find_key(value(),key) ; }
bool               JsonInDictBool  ::operator [] (const JsonString & key) const { return 0!=find_key(value(),key) ; }

inline bool is_white(JsonChar c) {
    return c==L' '  ||
           c==L'\t' ||
           c==L'\n' ||
           c==L'\r';
}

inline bool is_end_separator(JsonChar c) {
    return c==L' '  ||
           c==L'\t' ||
           c==L'\n' ||
           c==L'\r' ||
           c==L','  ||
           c==L']'  ||
           c==L'}';
}

inline bool is_hex(JsonChar c) {
    return (c>=L'0' && c<=L'9') ||
           (c>=L'A' && c<=L'F') ||
           (c>=L'a' && c<=L'f');
}

static inline void eat_white_nothrow(const JsonString & s, size_t & i) {
    while (i < s.size()) {
        if (is_white(s[i])) {
            ++i;
            continue;
        }
        return;
    }
}

static inline void eat_white(const JsonString & s, size_t & i) {
    while (i < s.size()) {
        if (is_white(s[i])) {
            ++i;
            continue;
        }
        return;
    }
    JsonThrow(ToJsonString("stream ends prematurely"));
}

inline static bool compare_end(const JsonString & s, size_t & i, const JsonString & sub) {
    size_t k = i;
    const size_t n = sub.size();
    for (size_t j=0; j<n; ++j) {
        if (sub[j]!=s[k++]) {
            return true;
        }
    }
    if (is_end_separator(s[k])) {
        i = k;
        return false;
    }
    return true;
}

inline static bool is_initial_numeric(JsonChar c) {
    return (c>=L'0' && c<=L'9') || c==L'-'
                                || c==L'+'
                                || c==L'.';
}

inline static JsonInAtom * try_number(const JsonString & s, size_t & i) {
    JsonInAtom * e = 0;
    bool ok = false;
    const size_t maxChars=64, maxDigits=32, maxDigitsE=4;
    bool inFrac = false, inExp = false;
    int  intDigits[maxDigits ],  nIntDigits=0, intSign=0,
        fracDigits[maxDigits ], nFracDigits=0,
         expDigits[maxDigitsE],  nExpDigits=0, expSign=0;
    size_t j=0;
    for ( ; j<maxChars; ++j) {
        size_t k = i+j;
        if (k>=s.size()) {
            ok = (nIntDigits+nFracDigits)>0 && (!inExp || nExpDigits>0);
            break;
        }
        JsonChar c = s[k];
        if (c>=L'0' && c<=L'9') {
            const int d = c-L'0';
            if (inExp) {
                if (nExpDigits < maxDigitsE) {
                    expDigits[nExpDigits++] = d;
                } else { // out of exp digits-- parse failure.
                    break;
                }
            } else if (inFrac) {
                if (nFracDigits < maxDigits) {
                    fracDigits[nFracDigits++] = d;
                } else { // out of frac digits-- parse failure.
                    break;
                }
            } else {
                if (nIntDigits < maxDigits) {
                    intDigits[nIntDigits++] = d;
                } else { // out of int digits-- parse failure.
                    break;
                }
            }
        } else if (is_white(c)) {
            ok = (nIntDigits+nFracDigits)>0 && (!inExp || nExpDigits>0);
            break;
        } else if (c==L'-' || c==L'+') {
            int sign = c==L'-' ? -1 : 1;
            if (inFrac) { // we shouldn't see a sign in the frac part-- parse failure.
                break;
            } else if (!inExp) {
                if (!nIntDigits && !intSign) {
                    intSign = sign;
                } else { // int sign already assigned or after first int digit-- parse failure.
                    break;
                }
            } else { // inExp
                if (!nExpDigits && !expSign) {
                    expSign = sign;
                } else { // exp sign already assigned or after first exp digit-- parse failure.
                    break;
                }
            }
        } else if (c==L'.') {
            if (inExp) { // there should be no dots in the exp part-- parse failure.
                break;
            } else if (!inFrac) {
                inFrac = true;
            } else { // already supposedly in frac part-- parse failure.
                break;
            }
        } else if (c==L'e' || c==L'E') {
            inFrac = false;
            if (!inExp) {
                inExp = true;
            } else { // inExp-- repeated E? parse failure.
                break;
            }
        } else { // character not in ToJsonString("0123456789-+eE."
            ok = (nIntDigits+nFracDigits)>0 && (!inExp || nExpDigits>0);
            break;
        }
    }
    if (ok) {
        const long long kMax = (1LL << 52);
        long long k = 0;
        long long m = 1;
        while (nIntDigits) {
            const long long d = intDigits[--nIntDigits];
            k += m * d;
            if (k >= kMax) {
                ok = false;
                break;
            }
            m *= 10;
        }

        if (!inFrac && !inExp) { // there was no decimal or E-- it's an int
            if (intSign<0) {
                k = -k;
            }
            if (ok && k>=LONG_MIN && k<=LONG_MAX) {
                e = new JsonInInt(int(k));
                i += j;
            }
        } else { // either inFrac, or inExp-- it's a double
            double mantissa = 0.;
            {
                double m = 0.1;
                for (int fj=0; fj<nFracDigits; ++fj) {
                    mantissa += m * fracDigits[fj];
                    m *= 0.1;
                }
                mantissa += k;
                if (intSign<0) {
                    mantissa = -mantissa;
                }
            }

            int exponent = 0;
            {
                int m = 1;
                while (nExpDigits) {
                    const int d = expDigits[--nExpDigits];
                    exponent += m * d;
                    m *= 10;
                }
                if (expSign<0) {
                    exponent = -exponent;
                }
            }

            const double f = mantissa * pow(10.0, exponent);
            e = new JsonInDouble(f);
            i += j;
        }
    }
    return e;
}

JsonInAtom * json_parse(const JsonString & s, size_t & i,
                        const JsonDictStringV * aliases,
                        bool specialize,
                        int recursionLimit) {
    if (recursionLimit <= 0) {
        JsonThrow(ToJsonString("exceeded recursion limit in element parse"));
    }
    if (i >= s.size()) {
        JsonThrow(ToJsonString("empty parse input"));
    }
    JsonInAtomP e;

    eat_white(s,i);
    const JsonChar c = s[i];
    if (is_initial_numeric(c)) {
        e = try_number(s,i);
        if (!e) {
            // an initial number, but not parseable as a number?-- parse failure.
            JsonThrow(ToJsonString("bad number"));
        }
    } else if (c==L'"') {
        JsonInString * es = new JsonInString();
        JsonInAtomP ep = es;
        es->parse(s,i);
        e = ep;
    } else if (c==L'[') {
        JsonInArray * ea = new JsonInArray();
        JsonInAtomP ep = ea;
        if (JsonInAtom * specialized = ea->parse(s, i, aliases, specialize, recursionLimit-1)) {
            e = specialized;
        } else {
            e = ep;
        }
    } else if (c==L'{') {
        JsonInDict * ed = new JsonInDict();
        JsonInAtomP ep = ed;
        if (JsonInAtom * specialized = ed->parse(s, i, aliases, specialize, recursionLimit-1)) {
            e = specialized;
        } else {
            e = ep;
        }
    } else if (!compare_end(s,i,ToJsonString("true"))) {
        e = new JsonInBool(true);
    } else if (!compare_end(s,i,ToJsonString("false"))) {
        e = new JsonInBool(false);
    } else if (!compare_end(s,i,ToJsonString("null"))) {
        e = new JsonInNull();
    } else {
        //json_printf(ToJsonString("? : %s", s.substr(i).c_str());
        JsonThrow(ToJsonString("unexpected token"));
    }
    b_assert(!!e);
    return e.commit();
}

void JsonInString::parse(const JsonString & s, size_t & i) {
    const size_t n = s.size();
    if (n < i+2) {
file://localhost/Users/greg/code/Phonosynth/src/Classes/JSON/JsonInput.cpp
        JsonThrow(ToJsonString("string too short"));
    }
    size_t j = i;
    if (s[j++]!=L'"') {
        JsonThrow(ToJsonString("string (or object key?) doesn't begin with doublequote"));
    }
    while (j < n) {
        JsonChar c = s[j];
        if (c==L'\\') {
            ++j;
            if (j >= n) {
                JsonThrow(ToJsonString("missing string escape sequence"));
            }
            c = s[j];
            if (c==L'"'  ||
                c==L'\\' ||
                c==L'/'  ||
                c==L'b'  ||
                c==L'f'  ||
                c==L'n'  ||
                c==L'r'  ||
                c==L't') {
                ++j;
            } else if (c==L'u') {
                if (j+4 >= n) {
                    JsonThrow(ToJsonString("\\u followed by <4 characters"));
                }
                if (!is_hex(s[++j]) ||
                    !is_hex(s[++j]) ||
                    !is_hex(s[++j]) ||
                    !is_hex(s[++j])) {
                    JsonThrow(ToJsonString("\\u followed by non-hex characters"));
                }
                // Didn't write this code, but this is definitely a bug. Was failing to parse
                // search results containing unicode characters
                // j += 4;   <------ bug
            } else {
                JsonThrow(ToJsonString("unrecognized escape sequence"));
            }
            if (j >= n) {
                JsonThrow(ToJsonString("truncated string escape sequence"));
            }
        } else if (c==L'"') {
            value() = s.substr(i+1,j-i-1);
            i = j+1;
            return;
        } else {
            ++j;
        }
    }
    JsonThrow(ToJsonString("string missing end-quote"));
}

template <class A> inline static
void copy_specialized_array(JsonInAtomP & specialized,
                            const JsonArrayV & src) {
    A * p = new A();
    const size_t n = src.size();
    for (size_t i=0; i<n; ++i) {
        p->append(*(src[i]));
    }
    specialized = p;
}

JsonInAtom * JsonInArray::parse(const JsonString & s, size_t & i,
                                const JsonDictStringV * aliases,
                                bool specialize,
                                int recursionLimit) {
    if (recursionLimit <= 0) {
        JsonThrow(ToJsonString("exceeded recursion limit in array parse"));
    }
    const size_t n = s.size();
    if (n-i < 2) {
        JsonThrow(ToJsonString("truncated array"));
    }
    JsonChar c = s[i];
    if (c!=L'[') {
        JsonThrow(ToJsonString("array doesn't begin with ["));
    }
    JsonType eType = JsonTypeUnknown;
    size_t j = i+1;
    while (j<n) {
        eat_white(s,j);
        c = s[j];
        if (c==L']') {
            break; // commit
        } else {
            if (value().size()) {
                if (c!=L',') {
                    JsonThrow(ToJsonString("missing comma in array"));
                }
                ++j;
                eat_white(s,j);
                c = s[j];
                if (c==L']') {
                    JsonThrow(ToJsonString("array ends after comma"));
                }
            }
            if (JsonInAtom * e = json_parse(s, j, aliases, specialize, recursionLimit-1)) {
                if (value().size()) {
                    if (eType != e->get_type()) {
                        eType = JsonTypeUnknown;
                    }
                } else { // array empty
                    eType = e->get_type();
                }
                value().append(e);
            } else {
                JsonThrow(ToJsonString("bad element in array"));
            }
        }
    }
    if (j>=n || s[j]!=L']') {
        JsonThrow(ToJsonString("array ends prematurely"));
    }
    ++j;
    i = j;
    JsonInAtomP specialized;
    if (specialize) {
        if      (eType == JsonTypeString) { copy_specialized_array<JsonInArrayString>(specialized, value()); }
        else if (eType == JsonTypeInt   ) { copy_specialized_array<JsonInArrayInt   >(specialized, value()); }
        else if (eType == JsonTypeDouble) { copy_specialized_array<JsonInArrayDouble>(specialized, value()); }
        else if (eType == JsonTypeBool  ) { copy_specialized_array<JsonInArrayBool  >(specialized, value()); }
    }
    return specialized.commit();
}

template <class V, class T> inline
void insert_specialized(V & dict, const JsonString & key, const T & t) {
    std::pair<typename V::iterator, bool> p = dict.insert(typename V::value_type(key, t));
    if (!p.second) {
        JsonThrow(ToJsonString("duplicate key in object"));
    }
}

void JsonInDictString::insert(const JsonString & k, const JsonString & t) { insert_specialized(value(), k, t); }
void JsonInDictInt   ::insert(const JsonString & k,       int          t) { insert_specialized(value(), k, t); }
void JsonInDictDouble::insert(const JsonString & k,       double       t) { insert_specialized(value(), k, t); }
void JsonInDictBool  ::insert(const JsonString & k,       bool         t) { insert_specialized(value(), k, t); }

template <class A> inline static
JsonInAtom * make_specialized_dict(const JsonDictV & src) {
    A * p = new A();
    JsonInAtomP e = p;
    JsonDictV::const_iterator it, it1 = src.end();
    for (it=src.begin(); it!=it1; ++it) {
        p->insert(it->first, *(it->second));
    }
    return e.commit(); // if we don't get here, dispose of the new element
}

void JsonInDict::keys(JsonArrayStringV & ks) const {
    ks.resize(0);
    JsonDictV::const_iterator it, it1 = value().end();
    for (it=value().begin(); it!=it1; ++it) {
        *(ks.add()) = it->first;
    }
}

const JsonInAtom * JsonInDict::if_key(const JsonString & k) const {
    const E * result = 0;
    JsonDictV::const_iterator p = value().find(k);
    if (p != value().end()) {
        result = p->second;
    }
    return result;
}

JsonInAtom * JsonInDict::parse(const JsonString & s, size_t & i,
                               const JsonDictStringV * aliases,
                               bool specialize,
                               int recursionLimit) {
    if (recursionLimit <= 0) {
        JsonThrow(ToJsonString("exceeded recursion limit in object parse"));
    }
    const size_t n = s.size();
    if (n-i < 2) {
        JsonThrow(ToJsonString("truncated object"));
    }
    JsonChar c = s[i];
    if (c!=L'{') {
        JsonThrow(ToJsonString("object doesn't begin with {"));
    }
    JsonType eType = JsonTypeUnknown;
    size_t j = i+1;
    while (j<n) {
        eat_white(s,j);
        c = s[j];
        if (c==L'}') {
            break; // commit
        } else {
            if (value().size()) {
                if (c!=L',') {
                    JsonThrow(ToJsonString("missing comma in object"));
                }
                ++j;
                eat_white(s,j);
            }
            JsonInString key;
            key.parse(s,j); // may throw if this is an invalid string key
            eat_white(s,j);
            c = s[j];
            if (c!=':') {
                JsonThrow(ToJsonString("missing : in object"));
            }
            ++j;
            eat_white(s,j);
            if (JsonInAtomP e = json_parse(s, j, aliases, specialize, recursionLimit-1)) {
                if (value().size()) {
                    if (eType != e->get_type()) {
                        eType = JsonTypeUnknown;
                    }
                } else { // object empty
                    eType = e->get_type();
                }
                {
                    JsonString sKey = JsonString(key);
                    if (aliases) {
                        JsonDictStringV::const_iterator it = aliases->find(sKey);
                        if (it != aliases->end()) {
                            sKey = it->second;
                        }
                    }
                    std::pair<JsonDictV::iterator, bool> p =
                        value().insert(JsonDictV::value_type(sKey, e));
                    if (!p.second) {
                        JsonThrow(ToJsonString("duplicate key in object"));
                    }
                }
                e.commit(); // if we don't get here, dispose of the new element
            } else {
                JsonThrow(ToJsonString("bad value in object"));
            }
        }
    }
    if (j>=n || s[j]!=L'}') {
        JsonThrow(ToJsonString("object ends prematurely"));
    }
    ++j;
    i = j;
    JsonInAtomP specialized;
    if (specialize) {
        if      (eType == JsonTypeString) { specialized = make_specialized_dict<JsonInDictString>(value()); }
        else if (eType == JsonTypeInt   ) { specialized = make_specialized_dict<JsonInDictInt   >(value()); }
        else if (eType == JsonTypeDouble) { specialized = make_specialized_dict<JsonInDictDouble>(value()); }
        else if (eType == JsonTypeBool  ) { specialized = make_specialized_dict<JsonInDictBool  >(value()); }
    }
    return specialized.commit();
}

size_t JsonParser::parse(const JsonString & s,
                         const JsonDictStringV * aliases,
                         bool specialize,
                         int recursionLimit) {
    size_t i = 0;
    root_ = json_parse(s, i, aliases, specialize, recursionLimit);
    const size_t n = s.size();
    if (i<n) {
        eat_white_nothrow(s,i);
    }
    return i;
}
