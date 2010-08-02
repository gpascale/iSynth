/*
 *  PSWeb.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PSWEB_H_
#define _PSWEB_H_

#include <string>
#include <common.h>
#include <LightweightXML.h>
#include <vector>
#include <Json.h>
#include <Functions.h>

class JsonParser;
class JsonInDict;

using namespace std;

struct WebCollection
{
	
	string	guid;
	string	name;
	string	ownerGuid;
	string	ownerName;
	string	description;
	string	thumbnailURL;
	string	jsonURL;
	string  date;
	int		synthiness;
	int		numPhotos;
	int		numViews;
    bool    isDeleted;	
	bool    expanded;
    
    // kept around only because some other code depends on these, probably should just remove
	C_GETTERSETTER(const string&, getGuid, setGuid, guid)	
	C_GETTERSETTER(const string&, getName, setName, name)
	C_GETTERSETTER(const string&, getOwnerGuid, setOwnerGuid, ownerGuid)
	C_GETTERSETTER(const string&, getOwnerName, setOwnerName, ownerName)
	C_GETTERSETTER(const string&, getDescription, setDescription, description)
	C_GETTERSETTER(const string&, getThumbnailURL, setThumbnailURL, thumbnailURL)
	C_GETTERSETTER(const string&, getJsonURL, setJsonURL, jsonURL)
	C_GETTERSETTER(const string&, getDate, setDate, date)
	C_GETTERSETTER(int, getSynthiness, setSynthiness, synthiness)
	C_GETTERSETTER(int, getNumPhotos, setNumPhotos, numPhotos)
	C_GETTERSETTER(int, getNumViews, setNumViews, numViews)
    C_GETTERSETTER(bool, getIsDeleted, setIsDeleted, isDeleted)
	
	WebCollection() : expanded(false) { }
	
};


enum GetCollectionsResults
{
	GC_ERROR,
	GC_NO_RESULTS,
	GC_OK
};


// encapsulates arguments needed by the AJAX wrappers, getCollectionsFrom*
struct GetCollectionsArgs
{	
	int				firstResult;
	int				maxResults;		
	NSString*		queryString;
	
	GetCollectionsArgs(NSString* s = @"", int f = 0, int m = 0) : 
		firstResult(f), maxResults(m)
	{
		queryString = [NSString stringWithString: s];
		[queryString retain];
	}
	
	GetCollectionsArgs(const GetCollectionsArgs& rhs) :
		firstResult(rhs.firstResult), maxResults(rhs.maxResults)
	{
		queryString = [NSString stringWithString: rhs.queryString];
		[queryString retain];
	}
	
	~GetCollectionsArgs()
	{
		[queryString release];
	}
};


class CollectionParseException : public Exception
{
public:
	CollectionParseException(const string& why = "Unknown CollectionParseException",
							 int line = -1) : 
		Exception(why, line)
	{
		//cerr << "throwing CollectionParseException at line " << line << ": " << why << endl;
	}
	
	CollectionParseException(int line) :
		Exception(line)
	{
		//cerr << " throwing CollectionParseException at line " << line << endl;
	}
	
	#define COLLECTIONPARSETHROW(why) throw CollectionParseException(why, __LINE__)
	
};


class PSWeb
{
	
public:
    
    // returns the most popular collections in ret
	
	static void  getCollectionsFromExplore(GetCollectionsArgs args,
										   OUT vector<WebCollection>& ret,
										   OUT int& totalFound);
	
    static int  getCollectionsFromExplore(const string& exploreType, 
                                          OUT vector<WebCollection>& ret,
                                          const int firstResult = 0,
                                          const int maxResults = 20);
	
	static void  getCollectionsFromUser(GetCollectionsArgs args,
										OUT vector<WebCollection>& ret,
										OUT int& totalFound);
	
	static void  getCollectionsFromSearch(GetCollectionsArgs args,
										  OUT vector<WebCollection>& ret,
										  OUT int& totalFound);
	
    // does a search, returning the reults in ret.
    // since the number of results found may be huge, firstResult and maxResults allow the
    // caller to specify a particular range to get back. On the first search, the caller will
    // use firstResult = 0, maxResults = n. To retrieve the next page of results, the caller
    // will call firstResult = n, maxResults = n
    // the return value is the number of remaining results past the last one returned
    static int  getCollectionsFromSearch(const string& query, 
                                         OUT vector<WebCollection>& ret,
                                         const int firstResult = 0,
                                         const int maxResults = 20);        
    
    static void getCollectionFromGUID(const string& guid, OUT WebCollection& ret);
    
    
    static bool scrapeCollectionFromXML(const char* xmlData, OUT WebCollection& ret);
    static bool scrapeCollectionFromXML(const string& xmlData, OUT WebCollection& ret);
    
    static int  scrapeCollectionsFromXML(const string& xmlData,
                                         const int maxCollections,
                                         OUT vector<WebCollection>& ret);
    
	// extract collections from XML, filling ret with the results
	static int  scrapeCollectionsFromXML(const char* xmlData,
                                         const int maxCollections,
                                         OUT vector<WebCollection>& ret);        
    
    // extract up to 'maxCollections' collections from Json data. 
    static int  scrapeCollectionsFromJSON(const JsonString& s,
                                         const int maxCollections,
                                         OUT vector<WebCollection>& ret);
	
	
    
    // parser is assumed to have already parsed the text
    static int  scrapeCollectionsFromJSON(const JsonParser& parser,
                                         const int maxCollections,
                                         OUT vector<WebCollection>& ret);
    
    
	// performs a synchronous HTTP GET request, returning the results in 'ret'
    static bool synchronousGET(const string& url, OUT ByteArray& ret);	
	// performs a synchronous HTTP POST request, returning the results in 'ret'
    static bool synchronousPOST(const string& url, const string& body, OUT ByteArray& ret);
	// performs an asynchronous HTTP GET request, calling 'callback' when finished
	static bool asynchronousGET(const string& URL, FunctorA<ByteArray>* callback);
	// performs an asynchronous HTTP POST request, calling 'callback' when finished
	static bool asynchronousPOST(const string& URL, FunctorA<ByteArray>* callback);
    
private:        
	
	// wrapper around the GetCollection function in the Photosynth Web Service
    static int  getCollections(const string& postBody, int maxResults, OUT vector<WebCollection>& ret);
	
	// fills in a WebCollection from its Json encoding
    static void fillWebCollection(const JsonInDict& dict, OUT WebCollection& ret);
	
	
	
};



#endif _PSWEB_H_
