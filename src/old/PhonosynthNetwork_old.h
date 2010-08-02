/*
 *  PSNetwork.h
 *  Phonosynth
 *
 *  Created by Greg on 9/27/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include <vector>
#include <MacDataFetcher.h>

class ImageReceiver : public DataClient
{
    virtual void onRequestComplete(const Request &r, 
								   unsigned char* data, 
								   int len) { }	
	virtual void onRequestFailed(const Request &r) { }
	virtual bool getNextRequest(OUT Request &r) { }
};

class PointcloudReceiver : public DataClient
{
    virtual void onRequestComplete(const Request &r, 
								   unsigned char* data, 
								   int len) { }	
	virtual void onRequestFailed(const Request &r) { }
	virtual bool getNextRequest(OUT Request &r) { }
};

struct PrioritizedRequest
{
    Request         request;
    unsigned        requestID;
    unsigned        priority;
    
    PrioritizedRequest(const unsigned rID, const string& url, unsigned pri = 0) :
        requestID(rID),
        priority(pri),
        request("dummy", url)
    { }
};

class PSNetwork
{

public:
    
    PSNetwork();
    
    void begin();
    
    void addImageRequest(const unsigned requestID, const string& url);
    void addPointcloudRequest(const unsigned requestID, const string& url);
    
    void setConcurrentDownloads(const int nImages, const int nPCs);
    
    // reprioritizes image download order
    void reprioritize(const map<unsigned int, int>& priorities);
    
    void reset();
    
private:
    
    vector<PrioritizedRequest>  m_imageRequests;
    vector<PrioritizedRequest>  m_pointcloudRequests;
        
    DataFetcher*                m_imageFetcher;
    DataFetcher*                m_pointcloudFetcher;
    
    ImageReceiver               m_imageReceiver;
    PointcloudReceiver          m_pointcloudReceiver;
};