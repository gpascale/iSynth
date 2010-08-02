//#include "Stable.h"
#include "JsonOutput.h"

int JsonOutBase::nInstances_ = 0;

void JsonOutAtom::set(int rhs) {
    JsonOStringStream s;
    s<<rhs;
    set_item(s.str());
}

void JsonOutAtom::set(double rhs) {
    JsonOStringStream oss;
    oss<<rhs;
    JsonString s = oss.str();
    // ensure no weird characters, e.g. NaN, 1.#ind, etc.
    if (JsonString::npos != s.find_first_not_of(ToJsonString("0123456789.eE+-"))) {
        JsonThrow(ToJsonString("bad double in json output"));
    }
    set_item(s);
}

JsonStringW JsonOutAtom::as_wide() const {
    JsonStringW result;
    if (item_.size()>=2) {
        if (item_[0] == JsonChar('"')) {
            if (item_[item_.size()-1] != JsonChar('"')) {
                JsonThrow(ToJsonString("unbalanced quotes"));
            }
            result = DecodeJsonStringToW(item_.substr(1,item_.size()-2));
        }
    } else {
        result = ToJsonStringW(item_.c_str());
    }
    return result;
}

JsonOutArray::JsonOutArray(const JsonOutArray & rhs)
  : JsonOutBase(rhs),
    multiline_(rhs.multiline_),
    items_(rhs.items_.size(), true) {
    const size_t n = rhs.items_.size();
    for (size_t i=0; i<n; ++i) {
        const JsonOutBase * r = rhs.items_[i];
        Refp<JsonOutBase> & p = items_[i];
        if      (r->is_atom ()) { p = new JsonOutAtom (*((const JsonOutAtom  *)r)); }
        else if (r->is_array()) { p = new JsonOutArray(*((const JsonOutArray *)r)); }
        else if (r->is_dict ()) { p = new JsonOutDict (*((const JsonOutDict  *)r)); }
        else { b_assert(false); } // corrupt or unknown type?
    }
}

JsonOutDict::JsonOutDict(const JsonOutDict & rhs)
  : JsonOutBase(rhs),
    multiline_(rhs.multiline_) {
    Dict::const_iterator it, it1 = rhs.dict_.end();
    for (it = rhs.dict_.begin(); it!=it1; ++it) {
        const JsonString & key = it->first;
        const JsonOutBase * r = it->second;
        Refp<JsonOutBase> p;
        if      (r->is_atom ()) { p = new JsonOutAtom (*((const JsonOutAtom  *)r)); }
        else if (r->is_array()) { p = new JsonOutArray(*((const JsonOutArray *)r)); }
        else if (r->is_dict ()) { p = new JsonOutDict (*((const JsonOutDict  *)r)); }
        else { b_assert(false); } // corrupt or unknown type?
        bool inserted = dict_.insert(Dict::value_type(key, p)).second;
        b_assert(inserted); // duplicate key?
    }
}

static void indent(JsonOStream & out, int indentLevel) {
    if (indentLevel >= 0) {
        out << ToJsonString("\n");
        for (int i=0; i<indentLevel; ++i) {
            out << ToJsonString("  ");
        }
    }
}

void JsonOutAtom::serialize(JsonOStream & out, int indentLevel) const {
    indent(out, indentLevel);
    out << item_;
}

JsonOutArray::~JsonOutArray() {}

void JsonOutArray::add(JsonOutBase  * o) { add_item(o); }
void JsonOutArray::add(JsonOutAtom  * o) { add_item(o); }
void JsonOutArray::add(JsonOutArray * o) { add_item(o); }
void JsonOutArray::add(JsonOutDict  * o) { add_item(o); }

#ifdef JSON_SEADRAGON_TYPES

JsonOutArray::JsonOutArray(const DPoint & o)
  : JsonOutBase(JsonOutTypeArray, true),
    multiline_(false) {
    add(o.x);
    add(o.y);
}

JsonOutArray::JsonOutArray(const DPoint3 & o)
  : JsonOutBase(JsonOutTypeArray, true),
    multiline_(false) {
    add(o.x);
    add(o.y);
    add(o.z);
}

JsonOutArray::JsonOutArray(const DPoint4 & o)
  : JsonOutBase(JsonOutTypeArray, true),
    multiline_(false) {
    add(o.x);
    add(o.y);
    add(o.z);
    add(o.w);
}

JsonOutArray::JsonOutArray(const DMatrix4 & o)
  : JsonOutBase(JsonOutTypeArray, false),
    multiline_(true) {
    for (int i=0; i<4; ++i) {
        add(o[i]);
    }
}

void JsonOutArray::add(const DPoint   & o) { add_item(new JsonOutArray(o)); }
void JsonOutArray::add(const DPoint3  & o) { add_item(new JsonOutArray(o)); }
void JsonOutArray::add(const DPoint4  & o) { add_item(new JsonOutArray(o)); }
void JsonOutArray::add(const DMatrix4 & o) { add_item(new JsonOutArray(o)); }

#endif // JSON_SEADRAGON_TYPES

void JsonOutArray::serialize(JsonOStream & out, int indentLevel) const {
    const int n = int(items_.size());
    indent(out, indentLevel);
    out << ToJsonString("[");
    if (multiline_ && indentLevel>=0 && n>0) {
        for (int i=0; i<n; ++i) {
            if (i) out<<ToJsonString(",");
            items_[i]->serialize(out, indentLevel+1);
        }
        indent(out, indentLevel);
    } else {
        for (int i=0; i<n; ++i) {
            if (i) out<<ToJsonString(",");
            items_[i]->serialize(out, -1);
        }
    }
    out << ToJsonString("]");
}

JsonOutDict::~JsonOutDict() {}

void JsonOutDict::add(const JsonString & name, JsonOutBase * value) {
    Dict::value_type v(name, value);
    std::pair<Dict::iterator,bool> p = dict_.insert(v);
    if (!p.second) { // duplicate key
        b_assert(p.first->first == name);

        if (value->is_dict()) {
            const JsonOutDict * src = (const JsonOutDict *)value;
            if (p.first->second->is_dict()) {
                // (recursively) copy instead of moving--
                JsonOutDict * match = (JsonOutDict *)(&(*(p.first->second)));
                Dict::const_iterator it, it1 = src->dict_.end();
                for (it=src->dict_.begin(); it!=it1; ++it) {
                    match->add(it->first, it->second);
                }
            } else {
                JsonThrow(ToJsonString("attempt to zipper dict into non-dict"));
            }
        } else { // value isn't a dict-- OK if atomic and values in perfect agreement
            JsonOutBase * matchBase = &(*(p.first->second));
            if (value->is_atom() && matchBase->is_atom()) {
                const JsonOutAtom * valueAtom = (const JsonOutAtom *)value;
                const JsonOutAtom * matchAtom = (const JsonOutAtom *)matchBase;
                if (*matchAtom != *valueAtom) {
                    JsonThrow(ToJsonString("attempt to zipper nonequal atoms"));
                }
            } else {
                JsonThrow(ToJsonString("attempt to zipper mismatched types"));
            }
        }
    }
}

void JsonOutDict::add(const JsonString & name, JsonOutAtom * value) {
    b_assert(value);
    Dict::value_type v(name, value);
    std::pair<Dict::iterator,bool> p = dict_.insert(v);
    if (!p.second) { // duplicate key

        JsonOutBase * matchBase = &(*(p.first->second));
        if (matchBase->is_atom()) {
            const JsonOutAtom * matchAtom = (const JsonOutAtom *)matchBase;
            if (*matchAtom != *value) {
                JsonThrow(ToJsonString("attempt to insert duplicate key with non-equal value"));
            }
        } else {
            JsonThrow(ToJsonString("attempt to insert duplicate key with non-atomic and non-dict value"));
        }
    }
}

#ifdef JSON_SEADRAGON_TYPES
void JsonOutDict::add(const JsonString & name, const DPoint   & value) { add(name, new JsonOutArray(value)); }
void JsonOutDict::add(const JsonString & name, const DPoint3  & value) { add(name, new JsonOutArray(value)); }
void JsonOutDict::add(const JsonString & name, const DPoint4  & value) { add(name, new JsonOutArray(value)); }
void JsonOutDict::add(const JsonString & name, const DMatrix4 & value) { add(name, new JsonOutArray(value)); }
#endif // JSON_SEADRAGON_TYPES

void JsonOutDict::zipper_in(const JsonOutBase * rhs) {
    if (!(rhs->is_dict())) {
        JsonThrow(ToJsonString("attempt to zipper_in from non-dict"));
    }
    const JsonOutDict * src = (const JsonOutDict *)rhs;
    // (recursively) copy instead of moving--
    Dict::const_iterator it, it1 = src->dict_.end();
    for (it=src->dict_.begin(); it!=it1; ++it) {
        add(it->first, it->second);
    }
}

void JsonOutDict::serialize(JsonOStream & out, int indentLevel) const {
    indent(out, indentLevel);
    out << ToJsonString("{");
    bool first = true;
    Dict::const_iterator
        it  = dict_.begin(),
        it1 = dict_.end();
    if (multiline_ && indentLevel>=0 && it!=it1) {
        for ( ; it!=it1; ++it) {
            if (!first) {
                out<<ToJsonString(",");
            }
            first = false;
            indent(out, indentLevel+1);
            out << ToJsonString("\"") << it->first << ToJsonString("\": ");
            const JsonOutBase * item = it->second;
            const int childIndent = item->trivial() ? -1 : indentLevel+2;
            item->serialize(out, childIndent);
        }
        indent(out, indentLevel);
    } else {
        for (it=dict_.begin(); it!=it1; ++it) {
            if (!first) {
                out<<ToJsonString(",");
            }
            first = false;
            out << ToJsonString("\"") << it->first << ToJsonString("\":");
            it->second->serialize(out, -1);
        }
    }
    out << ToJsonString("}");
}
