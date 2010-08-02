/*
 *  DataFetcher.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DATAFETCHER_H_
#define _DATAFETCHER_H_

#include <common.h>
#include <set>
#include <GMutex.h>

struct Request
{
	
    unsigned    requestID;
	string		url;
	string		body;
		
	Request(const unsigned _requestID = 0, const string& _url = "", const string& _body = "") : 
        requestID(_requestID),
        url(_url),
		body(_body)
	{ }	
	
	bool operator < (const Request &rhs) const { return requestID < rhs.requestID; }
	
};


struct Response
{
    Request                     request;
    PtrWithLen<unsigned char>   data;
    
    Response(const Request& r = Request(), 
             const PtrWithLen<unsigned char> d = PtrWithLen<unsigned char>()) :
        request(r),
        data(d)
    { }
    
    bool operator < (const Response& rhs) const { return request < rhs.request; }
};


// interface which must be implemented to use a DataFetcher
class DataClient
{
public:
	// callback called when a request downloads successfully
	virtual void onRequestComplete(const Request &r, ByteArray data) = 0;	
	// callback called when a request fails to download
	virtual void onRequestFailed(const Request &r) = 0;
	// callback called when the DataFetcher needs another request to download
	virtual bool getNextRequest(OUT Request &r) = 0;
};
 


#include <vector>
using namespace std;

class DataFetcher
{

public:
		
	DataFetcher(DataClient* client, int numConcurrentConnections = 20);
	virtual ~DataFetcher();	
	
	void setMaxConcurrentConnections(int n)  { m_lock.lock(); m_maxConcurrentConnections = n; m_lock.unlock(); }
	int  getMaxConcurrentConnections() const { return m_maxConcurrentConnections; }			
	
	virtual void beginFetchingData()  = 0;		
	virtual void reset(bool cancelCurrentDownloads = true)  = 0;
	virtual void cancel() = 0;					

protected:
	
	DataClient*				m_client;

	
	// helpers
	virtual void handleRequest(const Request &r) = 0;
	
	// data members
	GMutex  m_lock;
	
	int		m_numActiveConnections;
	int		m_maxConcurrentConnections;

};



#endif