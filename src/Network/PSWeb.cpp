/*
 *  PSWeb.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "PSWeb.h"
#include "LightweightXML.h"
#include <JsonInput.h>
#include <vector>
#include <OneShotDownloader.h>
#include <GThread.h>
#include <GTime.h>


void
PSWeb::getCollectionFromGUID(const string& collectionGuid, 
                             OUT WebCollection& ret)
{
	ret = WebCollection();
	
	const string url = "http://photosynth.net/photosynthws/photosynthservice.asmx/GetCollection?collectionId=" 
                       + collectionGuid;
    
    ByteArray data(NULL, 0);
    DownloadStatus status;
	
	OneShotDownloader downloader;	
	downloader.asynchronousGET(url, &data, &status);
	
	const float maxWait = 15.0f;
	const GTime start = GTime::Now();
	
	while(status == DS_DOWNLOADING){
		if(GThread::isCurThreadCancelled() || GTime::Now() - start > maxWait){
			downloader.cancel();
			GThread::exit();
		}
		GThread::sleep(0.1f);
	}
	
	// only parse the collection if the download was successful
	if(status == DS_SUCCEEDED){
		string s((const char*) (const unsigned char*) data, data.length());
		//cerr << "data is " << s << endl;
		scrapeCollectionFromXML(s, ret);
		ret.expanded = true;
	}
	
	data.release(DESTRUCT_FREE);
}


void
PSWeb::getCollectionsFromExplore(GetCollectionsArgs args,
								 OUT vector<WebCollection>& ret,
								 OUT int& totalFound)
{	
	ostringstream oss;
	NSString* s = [args.queryString stringByReplacingOccurrencesOfString: @"EXPLORE:"
															  withString: @""];
	
    if ([s isEqualToString: @"NICE_AND_SYNTHY"]){
        oss << "collectionId=&cmd=RetrieveNiceAndSynthySynths&text=";
		// search for a random group of synths
		oss << args.maxResults << "," << randInt(0, 969);
	}
    else if ([s isEqualToString: @"RECENT"]){
        oss << "collectionId=&cmd=RetrieveRecentSynths&text=";
		oss << args.maxResults << "," << args.firstResult;
	}
	else if ([s isEqualToString: @"MOST_VIEWED_LAST7"]){
        oss << "collectionId=&cmd=RetrieveMostViewedSynths&text=";
		oss << args.maxResults << "," << args.firstResult << ",Last7Days";
	}
	else if ([s isEqualToString: @"MOST_VIEWED_LAST30"]){
        oss << "collectionId=&cmd=RetrieveMostViewedSynths&text=";
		oss << args.maxResults << "," << args.firstResult << ",Last30Days";
	}
	else if ([s isEqualToString: @"MOST_VIEWED_ALLTIME"]){
        oss << "collectionId=&cmd=RetrieveMostViewedSynths&text=";
		oss << args.maxResults << "," << args.firstResult << ",AllTime";
	}
	else{
		ASSERT(0 && "invalid explore string");
		totalFound = 0;
		return;
	}
	
	
	totalFound = getCollections(oss.str(), args.maxResults, ret);
}


void
PSWeb::getCollectionsFromUser(GetCollectionsArgs args,
							  OUT vector<WebCollection>& ret,
							  OUT int& totalFound)
{	
	ostringstream oss;
	NSString* s = [args.queryString stringByReplacingOccurrencesOfString: @"USER:"
															  withString: @""];
	
    oss << "collectionId=&cmd=RetrieveProfileSynths&text=" << args.maxResults << ","
														   << args.firstResult << ","
														   << CSTRING(s);
	
	totalFound = getCollections(oss.str(), args.maxResults, ret);
	
	// since profile explore doesn't tell us how many results there actually are, increment totalFound
	// by one to indicate to the caller that there are still more results out there
	if(totalFound == args.maxResults)
		totalFound++;
	
	// manually set the user field in the returned collections since it doesn't seem to be filled in
	// by default
	FOR(i,0,ret.size())
		ret[i].ownerName = CSTRING(s);
	
	
}


void
PSWeb::getCollectionsFromSearch(GetCollectionsArgs args,
								OUT vector<WebCollection>& ret,
								OUT int& totalFound)
{	
	ostringstream oss;
	NSString* s = [args.queryString stringByReplacingOccurrencesOfString: @"SEARCH:"
															  withString: @""];
	
    oss << "collectionId=&cmd=Search&text=" << args.maxResults << "," 
											<< args.firstResult << ","
											<< CSTRING(s);
	
	totalFound = getCollections(oss.str(), args.maxResults, ret);	
}


int
PSWeb::getCollectionsFromSearch(const string& searchQuery,
                                OUT vector<WebCollection>& ret,
                                const int firstResult,
                                const int maxResults)
{
    ostringstream oss;
    oss << "collectionId=&cmd=Search&text=" << maxResults << "," << firstResult << "," << searchQuery;    
    return getCollections(oss.str(), maxResults, ret);
}


int
PSWeb::getCollections(const string& postBody, int maxResults, OUT vector<WebCollection>& ret){
	
    ByteArray data(NULL, 0);
	DownloadStatus status;
	
	OneShotDownloader* downloader = new OneShotDownloader();
	
	downloader->asynchronousPOST("http://photosynth.net/photosynthhandler.ashx", postBody, &data, &status);
	
	const float maxWait = 20.0f;
	const GTime start = GTime::Now();
	
	while(status == DS_DOWNLOADING){
		if(GThread::isCurThreadCancelled() || GTime::Now() - start > maxWait){
			downloader->cancel();
			ret.clear();
			return -1;
		}
		GThread::sleep(0.03f);
	}
	
    // if the download failed, return an empty vector	
	if(status == DS_FAILED || data.data() == NULL){
		ret.clear();
        return -1;
	}
    
    JsonString s((const char*) (const unsigned char*) data, data.length());
	data.release(DESTRUCT_FREE);
	
    return scrapeCollectionsFromJSON(s, maxResults, ret);
}


int
PSWeb::scrapeCollectionsFromJSON(const JsonString& s,
                                 const int maxCollections,
                                 OUT vector<WebCollection>& ret)
{
    JsonParser parser;
	int numCharsParsed;
	
	// try to parse the result. It's possible that it won't work though, especially if there's
	// something wrong with the internet connection and the string returned is some error bullshit
	
	printf("s = %s\n", s.c_str());
	
	try{
		numCharsParsed = parser.parse(s);
	}
	catch(JsonException e){
		return -1;	
	}
	
	
    if(numCharsParsed < s.length())
        return -1;
    
    return scrapeCollectionsFromJSON(parser, maxCollections, ret);
}


int
PSWeb::scrapeCollectionsFromJSON(const JsonParser& parser,
                                 const int maxCollections,
                                 OUT vector<WebCollection>& ret)
{
	try{		
		// exploring returns a JsonInDict which contains the array of collections while getting all
		// the synths for a user simply returns the array. Pain in the ass		
		const JsonInAtom& root = parser.root();	
		const JsonInArray& collections = (root.get_type() == JsonTypeDict && root.ToDict().has_key("Collections")) ?
										 (root("Collections").ToArray()) :
										 (root.ToArray());
		
		// if the returned results specify how many actual results there are, use it. Otherwise, the
		// number of results is simply the size of the returned array
		const int totalCollections = (root.get_type() == JsonTypeDict && root.ToDict().has_key("TotalResults")) ?
								     ((const int) root("TotalResults")) :
									 ((const int) collections.size());
		
		const int numCollections = min(totalCollections, maxCollections);
		
		ret.clear();
		ret.resize(numCollections);
		
		FOR(i,0,numCollections){
			const JsonInDict& dict = collections[i].ToDict();
			fillWebCollection(dict, ret[i]);
		}
    
		return min(totalCollections, 200);
	}
	
	catch(JsonException e){
		return 0;
	}
}



/*
 Keys:
CollectionUrl, Description, FavoriteCount, Id, ImageCount, Images, IsDeleted, Name, 
OwnerFriendlyName, OwnerUserGuid, SynthinessScore, ThumbnailUrl, UploadDate, Viewings
*/

void
PSWeb::fillWebCollection(const JsonInDict& dict, OUT WebCollection& ret)
{    
    const JsonInAtom* cur;
    
    if(cur = dict.if_key("CollectionUrl")){
        ret.jsonURL = DecodeJsonStringTo8(*cur); 
        const int n = ret.jsonURL.find(".dzc");
        if (n != string::npos)
            ret.jsonURL = ret.jsonURL.replace(n, 4, ".synth_files/0.json");
    }
    if(cur = dict.if_key("Description")){
        ret.ownerGuid = DecodeJsonStringTo8(*cur);
    }
    if(cur = dict.if_key("Id")){
        ret.guid = DecodeJsonStringTo8(*cur); 
    }    
    if(cur = dict.if_key("ImageCount")){
        ret.numPhotos = (int) *cur;
    }
    if(cur = dict.if_key("IsDeleted")){
        ret.isDeleted = (bool) *cur;
    }    
    if(cur = dict.if_key("Name")){
        ret.name = DecodeJsonStringTo8(*cur);
    }
    if(cur = dict.if_key("OwnerFriendlyName")){
        ret.ownerName = DecodeJsonStringTo8(*cur);
    }
    if(cur = dict.if_key("OwnerUserGuid")){
        ret.ownerGuid = DecodeJsonStringTo8(*cur);
    }
    if(cur = dict.if_key("SynthinessScore")){
        ret.synthiness = (int) *cur;
    }
    if(cur = dict.if_key("ThumbnailUrl")){
        ret.thumbnailURL = DecodeJsonStringTo8(*cur);
    }
    if(cur = dict.if_key("Viewings")){
        ret.numViews = (int) *cur;
    }
}


bool
PSWeb::scrapeCollectionFromXML(const string& xmlData, OUT WebCollection& ret)
{
    return scrapeCollectionFromXML(xmlData.c_str(), ret);
}


bool
PSWeb::scrapeCollectionFromXML(const char* xmlData, OUT WebCollection& ret)
{
    vector<WebCollection> v;
	
	try{
		if(scrapeCollectionsFromXML(xmlData, 1, v) == 1){
			ret = v[0];
			return true;
		}
	}
	catch(CollectionParseException& cpe){

	}
    return false;
}


int
PSWeb::scrapeCollectionsFromXML(const string& xmlData, 
                                const int maxCollections,
                                OUT vector<WebCollection>& ret)
{
    return scrapeCollectionsFromXML(xmlData.c_str(), maxCollections, ret);
}


int
PSWeb::scrapeCollectionsFromXML(const char* xmlData, 
                                const int maxCollections,
                                OUT vector<WebCollection>& ret)
{

    XML::Parser parser;    
	parser.setXMLData(xmlData);
	XML::Parsed parsedXML;
	
	if(!parser.parse(parsedXML))
		return 0;
	
	//cerr << "xmlData = " << xmlData << endl;
	
	parsedXML.printParsedXMLTree();
	
	vector<const XML::Node*> collectionNodes;
	parsedXML.getAllNodesWithName("PhotosynthCollection", collectionNodes);
	
	int numCollectionsFound = (int) collectionNodes.size();

	
    const int nCollections = min(numCollectionsFound, maxCollections);
    
    ret.resize(nCollections, WebCollection());
	
	FOR(i,0,nCollections){
	
		const vector<XML::Node>& children = collectionNodes[i]->getChildren();
		WebCollection& cur = ret[i];
		
		string  nodeDataS;
		int		nodeDataI;
		
		FOR(j,0,children.size()){			
			if(children[j].getName() == "Id"){
				if(!(children[j].dataAsString(nodeDataS)))
					throw CollectionParseException();
				cur.setGuid(nodeDataS);
			}
			
			if(children[j].getName() == "Name"){
				if(!(children[j].dataAsString(nodeDataS)))
					throw CollectionParseException();
				cur.setName(nodeDataS);
			}
			
			if(children[j].getName() == "OwnerUserGuid"){
				if(!(children[j].dataAsString(nodeDataS)))
					throw CollectionParseException();
				cur.setOwnerGuid(nodeDataS);
			}
			
			if(children[j].getName() == "OwnerFriendlyName"){
				if(!(children[j].dataAsString(nodeDataS)))
					throw CollectionParseException();
				cur.setOwnerName(nodeDataS);
			}
			
			if(children[j].getName() == "Description"){
				if(!(children[j].dataAsString(nodeDataS)))
					throw CollectionParseException();
				cur.setDescription(nodeDataS);
			}
			
			if(children[j].getName() == "ThumbnailUrl"){
				if(!(children[j].dataAsString(nodeDataS)))
					throw CollectionParseException();
				cur.setThumbnailURL(nodeDataS);
			}
			
			if(children[j].getName() == "SynthinessScore"){
				if(!(children[j].dataAsInt(nodeDataI)))
					throw CollectionParseException();
				cur.setSynthiness(nodeDataI);
			}
			
			if(children[j].getName() == "ImageCount"){
				if(!(children[j].dataAsInt(nodeDataI)))
					throw CollectionParseException();
				cur.setNumPhotos(nodeDataI);
			}
			
			if(children[j].getName() == "Viewings"){
				if(!(children[j].dataAsInt(nodeDataI)))
					throw CollectionParseException();
				cur.setNumViews(nodeDataI);
			}
			
			if(children[j].getName() == "UploadDate"){
				if(!(children[j].dataAsString(nodeDataS)))
					throw CollectionParseException();
				int n = nodeDataS.find("T");
				if(n != string::npos)
					nodeDataS = nodeDataS.substr(0, n);
				cur.setDate(nodeDataS);
			}
			
			if(children[j].getName() == "CollectionUrl"){
				if(!(children[j].dataAsString(nodeDataS)))
					throw CollectionParseException();
				int n;
				if((n = nodeDataS.find(".dzc")) == string::npos)
					throw CollectionParseException();
				cur.setJsonURL(nodeDataS.replace(n, 4, ".synth_files/0.json"));
			}
		}        
	}	
	
	return nCollections;
	
}


bool
PSWeb::synchronousGET(const string& url, OUT ByteArray& ret)
{
    NSURL* nsurl = [NSURL URLWithString: NSSTRING(url.c_str())];
    NSURLRequest* request = [NSURLRequest requestWithURL: nsurl];
    
    NSURLResponse* response;
    NSError*       error;    
    
    NSData* data = [NSURLConnection sendSynchronousRequest: request 
                                         returningResponse: &response 
                                                     error: &error];
    
    if(error != nil){
        ret.setData(NULL);
        ret.setLength(0);
        return false;
    }
        
    // memcpy and return memory newed C++ style. Otherwise we don't have to deal with trying to delete
    // memory allocated by Objective-C from C++ code later
    const int len = [data length];
    unsigned char* p = new unsigned char[len];
	
    memcpy(p, [data bytes], len);
    ret.setData(p);
    ret.setLength(len);
	
    return true;    
}


/*void
PSWeb::asynchronousGET(const string& url), Functor<ByteArray> callback)
{
	OneShotDownloader downloader;
	
	
}*/


bool
PSWeb::synchronousPOST(const string& url, const string& body, OUT ByteArray& ret)
{
    NSURL* nsurl = [NSURL URLWithString: NSSTRING(url.c_str())];
	
	NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL: nsurl];
	
	NSURLResponse*	response;
	NSError*		error;
	
    const int bodyLen = (const int) body.length();
    
	NSData* httpBody = [NSData dataWithBytesNoCopy: (void*) body.c_str() 
											length: bodyLen
									  freeWhenDone: NO];
	
	[request addValue: NSSTRING(i2s(bodyLen).c_str())
   forHTTPHeaderField: @"Content-Length"];
	[request setHTTPMethod: @"POST"];
	[request setHTTPBody: httpBody];
	
	NSData* data = [NSURLConnection sendSynchronousRequest: request 
										 returningResponse: &response
													 error: &error];
        
    if(error != nil){
        ret.setData(NULL);
        ret.setLength(0);
        return false;
    }
    
    // memcpy and return memory newed C++ style. Otherwise we don't have to deal with trying to delete
    // memory allocated by Objective-C from C++ code later
    const int len = [data length];
    unsigned char* p = new unsigned char[len];
    memcpy(p, [data bytes], len);

    // need this?
    // [data release];
    
    //cout << "data = " << (const char*) p << endl;
    
    ret.setData(p);
    ret.setLength(len);
    
    [[NSURLCache sharedURLCache] removeAllCachedResponses];
    
    return true;
}

