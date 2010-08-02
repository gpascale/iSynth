#ifndef JsonImage_h
#define JsonImage_h

#include "JsonInput.h"
#include "JsonOutput.h"
#include "MiniCommon/LivePtr.h"

class JsonSynthOutput;
class JsonCollection;
class JsonProjector;

class JsonImage : public RefpObj, public LiveObj {
    JsonCollection * collection_;
    int collectionIndex_;
    double width_, height_;

    JsonStringW hash_;
    JsonStringW path_;

    Refp<JsonOutAtom> license_url_;
    Refp<JsonOutAtom> author_;
    Refp<JsonOutAtom> attribution_url_;
    Refp<JsonOutAtom> caption_;

  public:
    Refp<JsonOutDict> properties_; // extensible properties

    JsonImage(JsonCollection * collection,
              int collectionIndex)
      : collection_     (collection),
        collectionIndex_(collectionIndex),
        width_ (1.0),
        height_(1.0) {}

    int GetIndex() const { return collectionIndex_; }
    JsonCollection * GetCollection() const { return collection_; }

    void SetHash(const wchar_t * hash) { hash_ = ToJsonStringW(hash); }
    void SetHash(const char    * hash) { hash_ = ToJsonStringW(hash); }
    const JsonStringW & GetHash() const { return hash_; }

    void SetPath(const wchar_t * path) { path_ = ToJsonStringW(path); }
    void SetPath(const char    * path) { path_ = ToJsonStringW(path); }
    const JsonStringW & GetPath() const { return path_; }    

    void SetSize(double w, double h) { width_=w; height_=h; }
    void GetSize(double * w, double * h) const { *w=width_; *h=height_; }

    void SetLicenseUrl    (const char * s);
    void SetAuthor        (const char * s);
    void SetAttributionUrl(const char * s);
    void SetCaption       (const char * s);

    void SetLicenseUrl    (const wchar_t * s);
    void SetAuthor        (const wchar_t * s);
    void SetAttributionUrl(const wchar_t * s);
    void SetCaption       (const wchar_t * s);

    JsonStringW GetLicenseUrl    () const;
    JsonStringW GetAuthor        () const;
    JsonStringW GetAttributionUrl() const;
    JsonStringW GetCaption       () const;

    void Parse(const JsonInDict & jIm);

    // by Greg
    void printInfo();
};



#endif // JsonImage_h
