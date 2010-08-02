//#include "Stable.h"
#include "JsonImage.h"
#include <iostream>

using namespace std;

void JsonImage::SetLicenseUrl    (const char * s) { license_url_     = new JsonOutAtom(EncodeJsonString(s)); }
void JsonImage::SetAuthor        (const char * s) { author_          = new JsonOutAtom(EncodeJsonString(s)); }
void JsonImage::SetAttributionUrl(const char * s) { attribution_url_ = new JsonOutAtom(EncodeJsonString(s)); }
void JsonImage::SetCaption       (const char * s) { caption_         = new JsonOutAtom(EncodeJsonString(s)); }

void JsonImage::SetLicenseUrl    (const wchar_t * s) { license_url_     = new JsonOutAtom(EncodeJsonString(s)); }
void JsonImage::SetAuthor        (const wchar_t * s) { author_          = new JsonOutAtom(EncodeJsonString(s)); }
void JsonImage::SetAttributionUrl(const wchar_t * s) { attribution_url_ = new JsonOutAtom(EncodeJsonString(s)); }
void JsonImage::SetCaption       (const wchar_t * s) { caption_         = new JsonOutAtom(EncodeJsonString(s)); }

void JsonImage::Parse(const JsonInDict & jIm) {
    
    if (const JsonInAtom * a = jIm.if_key("h")) { // image_hash
        JsonStringW s = a->as_wide();
        b_printf(" h(%s)", s.c_str());
        SetHash(s.c_str());
    }
    
    if (const JsonInAtom * a = jIm.if_key("u")) { // image_path
        JsonStringW s = a->as_wide();
        SetPath(s.c_str());
        b_printf(" (%s)", s.c_str());
    }
    
    if (const JsonInAtom * a = jIm.if_key("c")) { // license_url
        JsonStringW s = a->as_wide();
        SetLicenseUrl(s.c_str());
        b_printf(" (%s)", s.c_str());
    }
    
    if (const JsonInAtom * a = jIm.if_key("b")) { // author
        JsonStringW s = a->as_wide();
        SetAuthor(s.c_str());
        b_printf(" (%s)", s.c_str());
    }
    
    if (const JsonInAtom * a = jIm.if_key("s")) { // attribution_url
        JsonStringW s = a->as_wide();
        SetAttributionUrl(s.c_str());
        b_printf(" (%s)", s.c_str());
    }
    
    if (const JsonInAtom * a = jIm.if_key("g")) { // caption
        JsonStringW s = a->as_wide();
        SetCaption(s.c_str());
        b_printf(" (%s)", s.c_str());
    }
    
}


JsonStringW JsonImage::GetLicenseUrl() const {
    JsonStringW result;
    if (license_url_) {
        result = license_url_->as_wide();
    }
    return result;
}

JsonStringW JsonImage::GetAuthor() const {
    JsonStringW result;
    if (author_) {
        result = author_->as_wide();
    }
    return result;
}

JsonStringW JsonImage::GetAttributionUrl() const {
    JsonStringW result;
    if (attribution_url_) {
        result = attribution_url_->as_wide();
    }
    return result;
}

JsonStringW JsonImage::GetCaption() const {
    JsonStringW result;
    if (caption_) {
        result = caption_->as_wide();
    }
    return result;
}


void
JsonImage::printInfo()
{    
    return;
    //cout << "Image " << collectionIndex_ << ":" << endl;
    //wcout << "\tPath: " << path_ << endl;
    //wcout << "\tHash: " << hash_ << endl;
    //cout << "\tAuthor: " << author_ << endl;
    //cout << "\tCaption: " << caption_ << endl;
}