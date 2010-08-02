#ifndef JsonOutput_h
#define JsonOutput_h

#include "Json.h"

class JsonOutBase;
class JsonOutAtom;
class JsonOutArray;
class JsonOutDict;

enum JsonOutType {
    JsonOutTypeAtom  = 1,
    JsonOutTypeArray = 2,
    JsonOutTypeDict  = 3
};

class JsonOutBase : public RefpObj {
    JsonOutType type_;
  protected:
    bool trivial_; /// meaning, should always serialize on a single line
  public:
    static int nInstances_;
    JsonOutBase(JsonOutType type, bool trivial = false)
      : type_(type),
        trivial_(trivial) {
        ++nInstances_;
    }
    JsonOutBase(const JsonOutBase & rhs)
      : type_(rhs.type_),
        trivial_(rhs.trivial_) {
        ++nInstances_;
    }
    bool is_atom()  const { return type_==JsonOutTypeAtom; }
    bool is_array() const { return type_==JsonOutTypeArray; }
    bool is_dict()  const { return type_==JsonOutTypeDict; }
    bool trivial()  const { return trivial_; }
    virtual ~JsonOutBase() { b_assert(nInstances_>0); --nInstances_; }
    virtual void serialize(JsonOStream & out, int indentLevel = 0) const = 0;
};

class JsonOutAtom : public JsonOutBase {
    JsonString item_;
    void set_item(const JsonString & rhs) { item_ = rhs; }
    void set_item(const char * rhs) { set_item(ToJsonString(rhs)); }
  public:
    void set(const JsonString & rhs) { JsonString q(ToJsonString("\"")); set_item(q+rhs+q); }
    void set(const JsonChar   * rhs) { set(JsonString(rhs)); }
    void set(int                rhs);
    void set(double             rhs);
    void set(bool               rhs) { set_item(rhs ? "true" : "false"); }
    JsonOutAtom() : JsonOutBase(JsonOutTypeAtom, true) {}
    JsonOutAtom(const JsonOutAtom & rhs) : JsonOutBase(rhs), item_(rhs.item_) {}
    JsonOutAtom(const JsonString  & rhs) : JsonOutBase(JsonOutTypeAtom, true) { set(rhs); }
    JsonOutAtom(const JsonChar    * rhs) : JsonOutBase(JsonOutTypeAtom, true) { set(rhs); }
    JsonOutAtom(bool rhs) : JsonOutBase(JsonOutTypeAtom, true) { set(rhs); }
    JsonOutAtom & operator = (const JsonOutAtom & rhs) { set_item(rhs.item_); return *this; }
    JsonOutAtom & operator = (const JsonString  & rhs) { set(rhs); return *this; }
    JsonOutAtom & operator = (bool rhs) { set(rhs); return *this; }
    template <class T> JsonOutAtom(const T & rhs) : JsonOutBase(JsonOutTypeAtom, true) { set(rhs); }
    template <class T> JsonOutAtom & operator = (const T & rhs) { set(rhs); return *this; }
    virtual void serialize(JsonOStream & out, int indentLevel = 0) const;
    bool operator == (const JsonOutAtom & rhs) const { return item_==rhs.item_; }
    bool operator != (const JsonOutAtom & rhs) const { return item_!=rhs.item_; }
    JsonStringW as_wide() const;
};

class JsonOutArray : public JsonOutBase {
    bool multiline_;
    DynArray1<Refp<JsonOutBase> > items_;

    void add_item(JsonOutBase * item) { b_assert(item); *(items_.add()) = item; }

  public:
    ~JsonOutArray();
    JsonOutArray(const JsonOutArray & rhs);
    JsonOutArray(bool isMultiline = false)
      : JsonOutBase(JsonOutTypeArray, !isMultiline),
        multiline_(isMultiline) {}
    void singleline() { multiline_ = false; trivial_ = true ; }
    void multiline()  { multiline_ = true ; trivial_ = false; }
    void add(JsonOutBase  * o);
    void add(JsonOutAtom  * o);
    void add(JsonOutArray * o);
    void add(JsonOutDict  * o);
    void add(const Refp<JsonOutAtom > & o) { add((JsonOutAtom  *)o); }
    void add(const Refp<JsonOutBase > & o) { add((JsonOutBase  *)o); }
    void add(const Refp<JsonOutArray> & o) { add((JsonOutArray *)o); }
    void add(const Refp<JsonOutDict > & o) { add((JsonOutDict  *)o); }
    template <class T> void add(const T & o) { add_item(new JsonOutAtom(o)); }
    size_t size() const { return items_.size(); }
    virtual void serialize(JsonOStream & out, int indentLevel = 0) const;

#ifdef JSON_SEADRAGON_TYPES
    // a few convenience methods
    JsonOutArray(const DPoint   & o); void add(const DPoint   & o);
    JsonOutArray(const DPoint3  & o); void add(const DPoint3  & o);
    JsonOutArray(const DPoint4  & o); void add(const DPoint4  & o);
    JsonOutArray(const DMatrix4 & o); void add(const DMatrix4 & o);
#endif // JSON_SEADRAGON_TYPES
};

class JsonOutDict : public JsonOutBase {
    bool multiline_;
    typedef std::map<JsonString, Refp<JsonOutBase> > Dict;
    Dict dict_;

    void add_item(const JsonString & name, JsonOutBase * value);

  public:
    ~JsonOutDict();
    JsonOutDict(const JsonOutDict & rhs);
    JsonOutDict(bool isMultiline = true)
      : JsonOutBase(JsonOutTypeDict, !isMultiline),
        multiline_(isMultiline) {}
    void singleline() { multiline_ = false; }
    void multiline()  { multiline_ = true; }
    void add(const JsonString & name, JsonOutBase  * value);
    void add(const JsonString & name, JsonOutAtom  * value);
    void add(const JsonString & name, JsonOutArray * value) { add(name, (JsonOutBase *)value); }
    void add(const JsonString & name, JsonOutDict  * value) { add(name, (JsonOutBase *)value); }
    void add(const JsonString & name, const Refp<JsonOutBase >   & value) { add(name, (JsonOutBase  *)value) ; }
    void add(const JsonString & name, const Refp<JsonOutAtom >   & value) { add(name, (JsonOutAtom  *)value) ; }
    void add(const JsonString & name, const Refp<JsonOutArray>   & value) { add(name, (JsonOutArray *)value) ; }
    void add(const JsonString & name, const Refp<JsonOutDict >   & value) { add(name, (JsonOutDict  *)value) ; }
    void add(const JsonString & name, const JsonString           & value) { add(name, new JsonOutAtom(value)); }
    void add(const JsonString & name, const JsonChar             * value) { add(name,     JsonString (value)); }
    template <class T> void add(const JsonString & name, const T & value) { add(name, new JsonOutAtom(value)); }
#ifndef JSON_USE_CHAR
    void add(const JsonString & name, const char           * value) { add(name, ToJsonString(value)); }
    void add(const char * name, JsonOutBase                * value) { add(ToJsonString(name), value); }
    void add(const char * name, JsonOutAtom                * value) { add(ToJsonString(name), value); }
    void add(const char * name, JsonOutArray               * value) { add(ToJsonString(name), value); }
    void add(const char * name, JsonOutDict                * value) { add(ToJsonString(name), value); }
    void add(const char * name, const Refp<JsonOutBase >   & value) { add(ToJsonString(name), value); }
    void add(const char * name, const Refp<JsonOutAtom >   & value) { add(ToJsonString(name), value); }
    void add(const char * name, const Refp<JsonOutArray>   & value) { add(ToJsonString(name), value); }
    void add(const char * name, const Refp<JsonOutDict >   & value) { add(ToJsonString(name), value); }
    void add(const char * name, const JsonString           & value) { add(ToJsonString(name), value); }
    void add(const char * name, const JsonChar             * value) { add(ToJsonString(name), value); }
    void add(const char * name, const char                 * value) { add(ToJsonString(name), value); }
    template <class T> void add(const char * name, const T & value) { add(ToJsonString(name), value); }
#endif // JSON_USE_CHAR

#ifdef JSON_SEADRAGON_TYPES
    // a few convenience methods
    void add(const JsonString & name, const DPoint   & value);
    void add(const JsonString & name, const DPoint3  & value);
    void add(const JsonString & name, const DPoint4  & value);
    void add(const JsonString & name, const DMatrix4 & value);
#endif // JSON_SEADRAGON_TYPES

    void zipper_in(const JsonOutBase * rhs);
    virtual void serialize(JsonOStream & out, int indentLevel = 0) const;
    size_t size() const { return dict_.size(); }
};

#endif // JsonOutput_h
