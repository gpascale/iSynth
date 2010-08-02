/*
 *  ContinuousDownloader.h
 *  Phonosynth
 *
 *  Created by Greg on 9/28/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _CONTINUOUSDOWNLOADER_H_
#define _CONTINUOUSDOWNLOADER_H_

#include <vector>
#include <map>
#include <MacDataFetcher.h>

using namespace std;

template<typename T>
struct PrioritizedRequest
{
    T               requestID;
    int             priority;
    Request         request;
    
    PrioritizedRequest<T>(const T rID, const Request& r, int pri = 0) :
        requestID(rID),
        request(r),
        priority(pri)
    { }
    
    bool operator < (const PrioritizedRequest& rhs) const { return priority < rhs.priority; }
};


template<typename T>
class ContinuousDownloader : public DataClient
{
    
public:
    
    ContinuousDownloader(const int nConcurrentConnections = 5);
    ~ContinuousDownloader();
    
    // simple settings
    void setNumConcurrentDownloads(const int n);

    // main functionality
    void reprioritize(const map<T, int>& priorities);

    void addRequest(const T requestID, const string& url, const int pri = 0);
    void start();
    void stop(bool cancelCurrentDownloads = true);
    void reset();
    
	int  numRequests() const { return (int) m_requests.size(); }
	
    // overridden DataClient functions
    virtual void onRequestComplete(const Request& r, ByteArray data);
    virtual void onRequestFailed(const Request& r);
    virtual bool getNextRequest(OUT Request& r);
    
protected:    
    // overridable callbacks that fires when a download completes successfully or unsuccessfully
    virtual void downloadSucceeded(const T requestID, ByteArray data) = 0;
    virtual void downloadFailed(const T requestID) = 0;
    
private:
        
    vector<PrioritizedRequest<T> >  m_requests;
    DataFetcher*                    m_fetcher;
    vector<Response>                m_responses;
    
    map<T, int>                m_idToRequestMap;
    map<int, T>                m_requestToIDMap;
	
	int								m_nextID;
    
};

#include "ContinuousDownloader.hpp"

#endif //_CONTINUOUSDOWNLOADER_H_