#ifndef JsonInput_h
#define JsonInput_h

#include "Json.h"

#include <iostream>

using namespace std;

/// This defines the maximum nesting level for the JSON source.  Count the
/// '[' and '{' to get the nesting level.
enum { DefaultJsonRecursionLimit = 64 };

enum JsonType {
    JsonTypeString      =  1,
    JsonTypeInt         =  2,
    JsonTypeDouble      =  3,
    JsonTypeBool        =  4,
    JsonTypeNull        =  5,
    JsonTypeArray       =  8,
    JsonTypeDict        = 16,
    JsonTypeArrayString =  8 | (1<<8),
    JsonTypeArrayInt    =  8 | (2<<8),
    JsonTypeArrayDouble =  8 | (3<<8),
    JsonTypeArrayBool   =  8 | (4<<8),
    JsonTypeDictString  = 16 | (1<<8),
    JsonTypeDictInt     = 16 | (2<<8),
    JsonTypeDictDouble  = 16 | (3<<8),
    JsonTypeDictBool    = 16 | (4<<8),
    JsonTypeUnknown     = 0x7FFFFFFF
};

class JsonInAtom;
class JsonInAtomP;
class JsonInString;
class JsonInInt;
class JsonInDouble;
class JsonInBool;
class JsonInNull;
class JsonInArray;
class JsonInArrayString;
class JsonInArrayInt;
class JsonInArrayDouble;
class JsonInArrayBool;
class JsonInDict;
class JsonInDictString;
class JsonInDictInt;
class JsonInDictDouble;
class JsonInDictBool;

typedef DynArray<JsonInAtom *>             JsonArrayV;
typedef DynArray1<JsonString>              JsonArrayStringV;
typedef DynArray<int>                      JsonArrayIntV;
typedef DynArray<double>                   JsonArrayDoubleV;
typedef DynArray<int>                      JsonArrayBoolV;
typedef std::map<JsonString, JsonInAtom *> JsonDictV;
typedef std::map<JsonString, JsonString>   JsonDictStringV;
typedef std::map<JsonString, int>          JsonDictIntV;
typedef std::map<JsonString, double>       JsonDictDoubleV;
typedef std::map<JsonString, bool>         JsonDictBoolV;

/// We use this union approach so that all JsonElements can be the same size;
/// for those that require more extensive storage (string, array and
/// dictionary) we allocate and manage data_.p_ on the heap.
/// The alias_ field allows temporary JsonElements to be constructed on the
/// stack, e.g. for returning via the [] operator, without requiring any deep
/// copies.  The plain JsonInAtom, less any payload on the heap, is only of
/// order the same size as a DPoint3, so can be copied, returned by value etc.
/// with negligible cost.









class JsonInAtom {
  public:
    static int nInstances_;
  private:
    void nInc() {                          ++nInstances_; }
    void nDec() { b_assert(nInstances_>0); --nInstances_; }

  protected:
    JsonType type_;
    bool alias_; // if true, don't delete data_.p_ on destruct
    union {
        int          i_;
        double       d_;
        bool         b_;
        void       * p_;
        const void * a_; // cheat-- throw away constness for alias, which can't mutate anyway.
    } data_;

    void set_type(JsonType t) { type_ = t; }

  public:
    /// Create alias.
    JsonInAtom(const JsonInAtom & rhs)
      : type_(rhs.type_),
        alias_(true) 
	{
        data_ = rhs.data_;
        nInc();
    }
	


  protected:
    /// Protected because this constructor is typed, but no data are
    /// initialized.
    JsonInAtom(JsonType t, bool alias=false)
      : type_(t),
        alias_(alias) 
	{
        data_.p_=0;
        nInc();
    }

    /// Aliased types-- not safe long-term!  We use this trick to allow cheap
    /// temporaries, avoid deep copies, and avoid the virtual destructor, but
    /// note that these temporary shallow copies rely on the passed-in data's
    /// lifetime exceeding the lifetime of this JsonInAtom.
    JsonInAtom(const JsonString       & v) : type_(JsonTypeString     ), alias_(true) { data_.a_ = &v; nInc(); }
    JsonInAtom(const JsonArrayV       & v) : type_(JsonTypeArray      ), alias_(true) { data_.a_ = &v; nInc(); }
    JsonInAtom(const JsonArrayStringV & v) : type_(JsonTypeArrayString), alias_(true) { data_.a_ = &v; nInc(); }
    JsonInAtom(const JsonArrayIntV    & v) : type_(JsonTypeArrayInt   ), alias_(true) { data_.a_ = &v; nInc(); }
    JsonInAtom(const JsonArrayDoubleV & v) : type_(JsonTypeArrayDouble), alias_(true) { data_.a_ = &v; nInc(); }
    //JsonInAtom(const JsonArrayBoolV   & v) : type_(JsonTypeArrayBool  ), alias_(true) { data_.a_ = &v; nInc(); }
    JsonInAtom(const JsonDictV        & v) : type_(JsonTypeDict       ), alias_(true) { data_.a_ = &v; nInc(); }
    JsonInAtom(const JsonDictStringV  & v) : type_(JsonTypeDictString ), alias_(true) { data_.a_ = &v; nInc(); }
    JsonInAtom(const JsonDictIntV     & v) : type_(JsonTypeDictInt    ), alias_(true) { data_.a_ = &v; nInc(); }
    JsonInAtom(const JsonDictDoubleV  & v) : type_(JsonTypeDictDouble ), alias_(true) { data_.a_ = &v; nInc(); }
    JsonInAtom(const JsonDictBoolV    & v) : type_(JsonTypeDictBool   ), alias_(true) { data_.a_ = &v; nInc(); }

  public:
    JsonInAtom() : type_(JsonTypeUnknown), alias_(false) { data_.p_=0; nInc(); }

    /// Trivial descendant constructors-- don't require destruction, and just
    /// as efficient to copy by value.
    JsonInAtom(int    i) : type_(JsonTypeInt   ), alias_(false) {               data_.i_ = i; nInc(); }
    JsonInAtom(double d) : type_(JsonTypeDouble), alias_(false) {               data_.d_ = d; nInc(); }
    JsonInAtom(bool   b) : type_(JsonTypeBool  ), alias_(false) {               data_.b_ = b; nInc(); }
    JsonInAtom(void * z) : type_(JsonTypeNull  ), alias_(false) { b_assert(!z); data_.p_ = 0; nInc(); }

    JsonType get_type() const { return type_; }
    bool is_alias() const { return alias_; }
    virtual ~JsonInAtom();

    /// Calling these conversion and indexing operators on the wrong type of
    /// element is a logic error (as per the b_assert()).
    inline size_t size() const;
    inline JsonInAtom operator [] (size_t i) const;
    inline JsonInAtom operator () (size_t i) const { return operator[](size_t(i)); }
    inline JsonInAtom operator [] (int    i) const { return operator[](size_t(i)); }
    inline JsonInAtom operator () (int    i) const { return operator[](size_t(i)); }
    inline JsonInAtom operator [] (const JsonString & key) const;
    inline JsonInAtom operator () (const JsonString & key) const { return operator[](key); }
    inline JsonInAtom operator () (const JsonChar * key) const { return operator[](JsonString(key)); }

#ifndef JSON_USE_CHAR
    inline JsonInAtom operator () (const char * key) const;
#endif // JSON_USE_CHAR

    operator const JsonString & () const { JsonVerify(type_==JsonTypeString); return *((const JsonString *)(data_.p_)); }
    operator int                () const { JsonVerify(type_==JsonTypeInt   ); return data_.i_; }
    operator bool               () const { JsonVerify(type_==JsonTypeBool  ); return data_.b_; }
    operator double             () const {
        double v;
        if      (type_==JsonTypeDouble) { v = data_.d_; }
        else if (type_==JsonTypeInt   ) { v = data_.i_; }
        else { JsonThrow(ToJsonString("can't convert to double")); }
        return v;
    }

    JsonStringW as_wide() const { return DecodeJsonStringToW(operator const JsonString & ()); }

  protected:
    void ChkT(JsonType t) const { if (type_!=t) { JsonThrow(ToJsonString("wrong JsonInAtom type")); } }

  public:
    /// 'Safe' downcasting (with JsonException on type violation).
    const JsonInArray       & ToArray      () const { ChkT(JsonTypeArray      ); return *((const JsonInArray       *)this); }
    const JsonInArrayString & ToArrayString() const { ChkT(JsonTypeArrayString); return *((const JsonInArrayString *)this); }
    const JsonInArrayInt    & ToArrayInt   () const { ChkT(JsonTypeArrayInt   ); return *((const JsonInArrayInt    *)this); }
    const JsonInArrayDouble & ToArrayDouble() const { ChkT(JsonTypeArrayDouble); return *((const JsonInArrayDouble *)this); }
    const JsonInArrayBool   & ToArrayBool  () const { ChkT(JsonTypeArrayBool  ); return *((const JsonInArrayBool   *)this); }

    const JsonInDict       & ToDict      () const { ChkT(JsonTypeDict      ); return *((const JsonInDict       *)this); }
    const JsonInDictString & ToDictString() const { ChkT(JsonTypeDictString); return *((const JsonInDictString *)this); }
    const JsonInDictInt    & ToDictInt   () const { ChkT(JsonTypeDictInt   ); return *((const JsonInDictInt    *)this); }
    const JsonInDictDouble & ToDictDouble() const { ChkT(JsonTypeDictDouble); return *((const JsonInDictDouble *)this); }
    const JsonInDictBool   & ToDictBool  () const { ChkT(JsonTypeDictBool  ); return *((const JsonInDictBool   *)this); }

    /// All nulls are the same.
    bool operator == (const JsonInNull & rhs) const { return type_==JsonTypeNull; }

    /// Comparison syntactic sugar.
    bool operator == (const JsonChar   * rhs) const { return JsonString(*this)==rhs; }
    bool operator == (const JsonString & rhs) const { return JsonString(*this)==rhs; }
    bool operator == (int                rhs) const { return        int(*this)==rhs; }
    bool operator == (double             rhs) const { return     double(*this)==rhs; }
    bool operator == (bool               rhs) const { return       bool(*this)==rhs; }

#ifdef JSON_SEADRAGON_TYPES
    /// Convenience conversions to complex types.  Will throw on failure.
    inline operator DPoint     () const;
    inline operator DPoint3    () const;
    inline operator DPoint4    () const;
    inline operator DMatrix4   () const;
    inline operator DQuaternion() const;
#endif // JSON_SEADRAGON_TYPES
};

/// Throws on parse error.  @s is the input, and @i is the starting position
/// in @s.  @i is returned incremented (but not beyond the end of @s).
/// Premature ending of @s is an exception, but the parsing may end with
/// @i < @s.size()-1.  @aliases, if nonzero, points to a JsonString,JsonString
/// map that translates variable names (dict keys).  In other words, if a key
/// matches a key in @aliases, the parsed tree will be the corresponding
/// @aliases value.  @specialize, if true, allows the construction of more
/// optimized arrays and dicts for homogeneous primitive types.
JsonInAtom * json_parse(const JsonString & s, size_t & i,
                        const JsonDictStringV * aliases = 0,
                        bool specialize=true,
                        int recursionLimit=DefaultJsonRecursionLimit);

/// For exception safety and to avoid memory leaks-- temporary holder of
/// pointer to JsonInAtom, until we're ready to commit the pointer to a more
/// permanent home.  If this gets destroyed before commit(), the pointer is
/// deleted.  Remember it's safe to delete 0.
class JsonInAtomP {
    JsonInAtom * p_;
  public:
    JsonInAtomP() : p_(0) {}
    JsonInAtomP(JsonInAtom * p) : p_(p) {}
    ~JsonInAtomP() { delete p_; }
    JsonInAtom * commit() { JsonInAtom * p=p_; p_=0; return p; }
    operator JsonInAtom * () const { b_assert(p_); return p_; }
    operator bool () const { return !!p_; }
    JsonInAtom * operator -> () const { b_assert(p_); return p_; }
    JsonInAtomP & operator = (JsonInAtom * p) { delete p_; p_=p; return *this; }
    JsonInAtomP & operator = (JsonInAtomP & rhs) { delete p_; p_ = rhs.commit(); return *this; }
};

class JsonInString : public JsonInAtom {
  public:
    typedef JsonString V;
    typedef JsonInAtom E;
    typedef JsonInString T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    ~JsonInString();
    JsonInString(const V & v = ToJsonString("")) : E(JsonTypeString) { data_.p_ = new V(v); }
    JsonInString(const T & rhs) : E(rhs) { data_.p_ = new V(rhs.value()); }
    T & operator = (const T & rhs) { value() = rhs.value(); return *this; }
    operator const V & () const { return value(); }
    operator       V & ()       { return value(); }

    /// Advances @i on success; throws an exception on failure.  Expects the
    /// first character @s[@i] to be double-quote.  Handles escape sequences
    /// as per the JSON spec.
    void parse(const JsonString & s, size_t & i);
};

class JsonInInt : public JsonInAtom {
  public:
    typedef int V;
    typedef JsonInAtom E;
    typedef JsonInInt T;

  protected:
    int   value() const { return data_.i_; }
    int & value()       { return data_.i_; }

  public:
    JsonInInt(V v = 0) : E(JsonTypeInt) { value() = v; }
    JsonInInt(const T & rhs) : E(rhs) { value() = rhs.value(); }
    T & operator = (const T & rhs) { value() = rhs.value(); return *this; }
    operator V   () const { return value(); }
    operator V & ()       { return value(); }
};

class JsonInDouble : public JsonInAtom {
  public:
    typedef double V;
    typedef JsonInAtom E;
    typedef JsonInDouble T;

  protected:
    double   value() const { return data_.d_; }
    double & value()       { return data_.d_; }

  public:
    JsonInDouble(V v = 0.) : E(JsonTypeDouble) { value() = v; }
    JsonInDouble(const T & rhs) : E(rhs) { value() = rhs.value(); }
    T & operator = (const T & rhs) { value() = rhs.value(); return *this; }
    operator V   () const { return value(); }
    operator V & ()       { return value(); }
};

class JsonInBool : public JsonInAtom {
  public:
    typedef bool V;
    typedef JsonInAtom E;
    typedef JsonInBool T;

  protected:
    bool   value() const { return data_.b_; }
    bool & value()       { return data_.b_; }

  public:
    JsonInBool(V v = false) : E(JsonTypeBool) { value() = v; }
    JsonInBool(const T & rhs) : E(rhs) { value() = rhs.value(); }
    T & operator = (const T & rhs) { value() = rhs.value(); return *this; }
    operator V   () const { return value(); }
    operator V & ()       { return value(); }
};

class JsonInNull : public JsonInAtom {
  public:
    typedef void * V;
    typedef JsonInAtom E;
    typedef JsonInNull T;
  public:
    JsonInNull() : E(JsonTypeNull) {}
    JsonInNull(const T & rhs) : E(rhs) {}
    T & operator = (const T & rhs) { return *this; }
    operator V() const { return 0; }
};

/// Convenience for == comparison with any other JsonInAtom (equivalent to
/// get_type()==JsonTypeNull).
extern JsonInNull JsonNull;

/// Design decision: array bounds checking with the [] operator works the same
/// as DynArray bounds checking-- it's a b_assert error, and may be disabled
/// in release for optimal performance.  In other words, it's a programming
/// error, not a catchable runtime exception, to use the [] operator without
/// first checking the .size() of the JsonInArray.  This contrasts with
/// JsonInDict and friends, for which the [] operator can be used m_maxConcurrentConnectionly
/// within a try/catch block-- at runtime a safe JsonException will be thrown
/// on out-of-bounds.
/// If you want 'safe' [] behavior for JsonInArray and friends, use
/// .get() instead, which checks bounds at runtime and throws a JsonException
/// on out-of-bounds.
class JsonInArray : public JsonInAtom {
  public:
    typedef JsonArrayV V;
    typedef JsonInAtom E;
    typedef JsonInArray T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    JsonInArray() : E(JsonTypeArray) { data_.p_ = new V(); }
    ~JsonInArray();
    size_t size() const { return value().size(); }
    const E & get(size_t i) const;
    const E & operator [] (size_t i) const { return *(value()[i]); }

    /// Advances @i on success, and fills in this element array.
    /// Note that @s[@i] should equal L'['-- that is, there should be no
    /// leading whitespace.  (Otherwise, how did you know it was an array?)
    /// If @specialize, then on success, if the type of all elements is the
    /// same, parse() will create the appropriate specialized array type and
    /// will return a pointer to the new instance.  (In this case, the generic
    /// array *this should be deleted and replaced in the parse tree with the
    /// returned specialized array.)  If the types are mixed, the return will
    /// be 0.  A parse error will cause a JsonException to be raised.
    /// See json_parse for a description of @aliases, @specialize and
    /// @recursionLimit.
    E * parse(const JsonString & s, size_t & i,
              const JsonDictStringV * aliases = 0,
              bool specialize = true,
              int recursionLimit = DefaultJsonRecursionLimit);
};

class JsonInDict : public JsonInAtom {
  public:
    typedef JsonDictV V;
    typedef JsonInAtom E;
    typedef JsonInDict T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    JsonInDict() : E(JsonTypeDict) { data_.p_ = new V(); }
    ~JsonInDict();
    size_t size() const { return value().size(); }
    const E & operator [] (const JsonString & key) const;

    /// Returns the complete list of keys (e.g. for iteration).
    void keys(JsonArrayStringV & ks) const;
    bool has_key(const JsonString & k) const { return value().find(k) != value().end(); }

    /// Returns 0 if the key @k doesn't exist in this dictionary; otherwise
    /// like operator [] (but returning pointer instead of reference).
    /// Doesn't throw.
    const E * if_key(const JsonString & k) const;

    /// Advances @i on success, and fills in this json 'object'.
    /// Note that @s[@i] should equal L'{'-- that is, there should be no
    /// leading whitespace.  (Otherwise, how did you know it was a dict?)
    /// If @specialize, then on success, if the type of all elements is the
    /// same, parse() will create the appropriate specialized dict type and
    /// will return a pointer to the new instance.  (In this case, the generic
    /// dict *this should be deleted and replaced in the parse tree with the
    /// returned specialized array.)  If the types are mixed, the return will
    /// be 0.  A parse error will cause a JsonException to be raised.
    /// See json_parse for a description of @aliases, @specialize and
    /// @recursionLimit.
    E * parse(const JsonString & s, size_t & i,
              const JsonDictStringV * aliases = 0,
              bool specialize = true,
              int recursionLimit = DefaultJsonRecursionLimit);
};

class JsonInArrayString : public JsonInAtom {
    typedef JsonArrayStringV V;
    typedef JsonInAtom E;
    typedef JsonInArrayString T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    ~JsonInArrayString();
    JsonInArrayString() : E(JsonTypeArrayString) { data_.p_ = new V(); }
    size_t size() const { return value().size(); }
    void append(const JsonString & v) { value().append(v); }
    const JsonString & operator [] (size_t i) const { return value()[i]; }
    const JsonString & get(size_t i) const;
};

class JsonInArrayInt : public JsonInAtom {
    typedef JsonArrayIntV V;
    typedef JsonInAtom E;
    typedef JsonInArrayInt T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    ~JsonInArrayInt();
    JsonInArrayInt() : E(JsonTypeArrayInt) { data_.p_ = new V(); }
    size_t size() const { return value().size(); }
    void append(int v) { value().append(v); }
    int operator [] (size_t i) const { return value()[i]; }
    int get(size_t i) const;
};

class JsonInArrayDouble : public JsonInAtom {
    typedef JsonArrayDoubleV V;
    typedef JsonInAtom E;
    typedef JsonInArrayDouble T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    ~JsonInArrayDouble();
    JsonInArrayDouble() : E(JsonTypeArrayDouble) { data_.p_ = new V(); }
    size_t size() const { return value().size(); }
    void append(double v) { value().append(v); }
    double operator [] (size_t i) const { return value()[i]; }
    double get(size_t i) const;
};

class JsonInArrayBool : public JsonInAtom {
    typedef JsonArrayBoolV V;
    typedef JsonInAtom E;
    typedef JsonInArrayBool T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    ~JsonInArrayBool();
    JsonInArrayBool() : E(JsonTypeArrayBool) { data_.p_ = new V(); }
    size_t size() const { return value().size(); }
    void append(bool v) { value().append(v); }
    bool operator [] (size_t i) const { return 0!=(value()[i]); }
    bool get(size_t i) const;
};

class JsonInDictString : public JsonInAtom {
  public:
    typedef JsonDictStringV V;
    typedef JsonInAtom E;
    typedef JsonInDictString T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    ~JsonInDictString();
    JsonInDictString() : E(JsonTypeDictString) { data_.p_ = new V(); }
    size_t size() const { return value().size(); }
    void insert(const JsonString & key, const JsonString & s);
    const JsonString & operator [] (const JsonString & key) const;
};

class JsonInDictInt : public JsonInAtom {
  public:
    typedef JsonDictIntV V;
    typedef JsonInAtom E;
    typedef JsonInDictInt T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    ~JsonInDictInt();
    JsonInDictInt() : E(JsonTypeDictInt) { data_.p_ = new V(); }
    size_t size() const { return value().size(); }
    void insert(const JsonString & key, int i);
    int operator [] (const JsonString & key) const;
};

class JsonInDictDouble : public JsonInAtom {
  public:
    typedef JsonDictDoubleV V;
    typedef JsonInAtom E;
    typedef JsonInDictDouble T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    ~JsonInDictDouble();
    JsonInDictDouble() : E(JsonTypeDictDouble) { data_.p_ = new V(); }
    size_t size() const { return value().size(); }
    void insert(const JsonString & key, double d);
    double operator [] (const JsonString & key) const;
};

class JsonInDictBool : public JsonInAtom {
  public:
    typedef JsonDictBoolV V;
    typedef JsonInAtom E;
    typedef JsonInDictBool T;

  protected:
    const V & value() const { return *((V *)data_.p_); }
          V & value()       { return *((V *)data_.p_); }

  public:
    ~JsonInDictBool();
    JsonInDictBool() : E(JsonTypeDictBool) { data_.p_ = new V(); }
    size_t size() const { return value().size(); }
    void insert(const JsonString & key, bool b);
    bool operator [] (const JsonString & key) const;
};

inline size_t JsonInAtom::size() const {
    if      (type_ == JsonTypeArray      ) { return ((const JsonInArray       *)this)->size(); }
    else if (type_ == JsonTypeDict       ) { return ((const JsonInDict        *)this)->size(); }
    else if (type_ == JsonTypeArrayString) { return ((const JsonInArrayString *)this)->size(); }
    else if (type_ == JsonTypeArrayInt   ) { return ((const JsonInArrayInt    *)this)->size(); }
    else if (type_ == JsonTypeArrayDouble) { return ((const JsonInArrayDouble *)this)->size(); }
    else if (type_ == JsonTypeDictString ) { return ((const JsonInDictString  *)this)->size(); }
    else if (type_ == JsonTypeDictInt    ) { return ((const JsonInDictInt     *)this)->size(); }
    else if (type_ == JsonTypeDictDouble ) { return ((const JsonInDictDouble  *)this)->size(); }
    else if (type_ == JsonTypeArrayBool  ) { return ((const JsonInArrayBool   *)this)->size(); }
    else if (type_ == JsonTypeDictBool   ) { return ((const JsonInDictBool    *)this)->size(); }
    JsonThrow(ToJsonString("JsonInAtom::size() undefined"));
    return 0;
}

inline JsonInAtom JsonInAtom::operator [] (size_t i) const {
    if      (type_ == JsonTypeArray      ) { return ((const JsonInArray       *)this)->operator[](i); }
    else if (type_ == JsonTypeArrayString) { return ((const JsonInArrayString *)this)->operator[](i); }
    else if (type_ == JsonTypeArrayInt   ) { return ((const JsonInArrayInt    *)this)->operator[](i); }
    else if (type_ == JsonTypeArrayDouble) { return ((const JsonInArrayDouble *)this)->operator[](i); }
    else if (type_ == JsonTypeArrayBool  ) { return ((const JsonInArrayBool   *)this)->operator[](i); }
    JsonThrow(ToJsonString("JsonInAtom::[size_t] undefined"));
    return *this;
}

inline JsonInAtom JsonInAtom::operator [] (const JsonString & key) const {
    if      (type_ == JsonTypeDict      ) { return ((const JsonInDict       *)this)->operator[](key); }
    else if (type_ == JsonTypeDictString) { return ((const JsonInDictString *)this)->operator[](key); }
    else if (type_ == JsonTypeDictInt   ) { return ((const JsonInDictInt    *)this)->operator[](key); }
    else if (type_ == JsonTypeDictDouble) { return ((const JsonInDictDouble *)this)->operator[](key); }
    else if (type_ == JsonTypeDictBool  ) { return ((const JsonInDictBool   *)this)->operator[](key); }
    JsonThrow(ToJsonString("JsonInAtom::[JsonString] undefined"));
    return *this;
}

#ifndef JSON_USE_CHAR
inline JsonInAtom JsonInAtom::operator () (const char * key) const {
    return operator[](ToJsonString(key));
}
#endif // JSON_USE_CHAR

template <class A> inline void chk_size(const A & a, size_t n) {
    if (a.size()!=n) {
        JsonThrow(ToJsonString("wrong array or object size"));
    }
}

#ifdef JSON_SEADRAGON_TYPES

inline JsonInAtom::operator DPoint() const {
    const JsonInArrayDouble & a = ToArrayDouble();
    chk_size(a,2);
    return DPoint(a[0],a[1]);
}

inline JsonInAtom::operator DPoint3() const {
    const JsonInArrayDouble & a = ToArrayDouble();
    chk_size(a,3);
    return DPoint3(a[0],a[1],a[2]);
}

inline JsonInAtom::operator DPoint4() const {
    const JsonInArrayDouble & a = ToArrayDouble();
    chk_size(a,4);
    return DPoint4(a[0],a[1],a[2],a[3]);
}

inline JsonInAtom::operator DQuaternion() const {
    const JsonInArrayDouble & a = ToArrayDouble();
    chk_size(a,4);
    return DQuaternion(a[0],a[1],a[2],a[3]);
}

inline JsonInAtom::operator DMatrix4() const {
    const JsonInArray & a = ToArray();
    chk_size(a,4);
    DMatrix4 M;
    for (int i=0; i<4; ++i) {
        const JsonInArrayDouble & r = a[i].ToArrayDouble();
        chk_size(r,4);
        M.v[i].set(r[0],r[1],r[2],r[3]);
    }
    return M;
}

#endif // JSON_SEADRAGON_TYPES

class JsonParser {
    JsonInAtomP root_;

  public:
    /// Returns number of characters parsed.  Eats any whitespace at the end
    /// of the stream, so if the return is less than s.size(), there are extra
    /// characters at the end.
    size_t parse(const JsonString & s,
                 const JsonDictStringV * aliases = 0,
                 bool specialize = true,
                 int recursionLimit=DefaultJsonRecursionLimit);

    JsonParser() {}
	
    JsonParser(const JsonString & s,
               const JsonDictStringV * aliases = 0,
               bool specialize = true,
               int recursionLimit=DefaultJsonRecursionLimit) {
        size_t n = parse(s, aliases, specialize, recursionLimit);
        if (n < s.size()) {
            JsonThrow(ToJsonString("extra characters at end of stream"));
        }
    }
	
	~JsonParser()
	{
	
	}
	
    const JsonInAtom & root() const { b_assert(root_); return *root_; }
    operator const JsonInAtom & () const { return root(); }

    size_t size() const { return root_->size(); }
    JsonInAtom operator [] (size_t i) const { return root_->operator[](i); }
    JsonInAtom operator () (size_t i) const { return root_->operator()(i); }
    JsonInAtom operator [] (int    i) const { return root_->operator[](i); }
    JsonInAtom operator () (int    i) const { return root_->operator()(i); }
    JsonInAtom operator [] (const JsonString & key) const { return root_->operator[](key); }
    JsonInAtom operator () (const JsonString & key) const { return root_->operator()(key); }
    JsonInAtom operator () (const JsonChar   * key) const { return root_->operator()(key); }
	
#ifndef JSON_USE_CHAR
    JsonInAtom operator () (const char       * key) const { return root_->operator()(key); }
#endif // JSON_USE_CHAR

    operator const JsonString & () const { return root_->operator const JsonString & (); }
    operator int                () const { return root_->operator int   (); }
    operator double             () const { return root_->operator double(); }
    operator bool               () const { return root_->operator bool  (); }
};

#endif // JsonInput_h
