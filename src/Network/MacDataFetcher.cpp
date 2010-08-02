/*
 *  
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "MacDataFetcher.h"
#include <iostream>
using namespace std;

MacDataFetcher::MacDataFetcher(DataClient* client, int maxConcurrentConnections) : 	
	DataFetcher(client, maxConcurrentConnections),
	m_cancelled(false)
{
	m_client = client;
	m_receiver = [[URLConnectionDelegate alloc] initWithParent: this];
}


MacDataFetcher::~MacDataFetcher()
{
	[m_receiver release];
}



void
MacDataFetcher::beginFetchingData()
{
	while(m_numActiveConnections < m_maxConcurrentConnections && handleNextRequest());	
}



bool
MacDataFetcher::handleNextRequest()
{	
	m_lock.lock();
	
	if(m_cancelled){
		m_lock.unlock();
		return false;
	}
	
	bool ret;	
	Request r;
	
	// check to make sure we don't exceed the maximum number of connections
	if(m_numActiveConnections < m_maxConcurrentConnections){
		if(m_client->getNextRequest(r)){
			ret = true;
			handleRequest(r);
		}
		else
			ret = false;
	}
	else
		ret = false;
	
	m_lock.unlock();
	
	return ret;
}


void
MacDataFetcher::handleRequest(const Request &r)
{			
	//cerr << "handling request for url " << r.url << endl;
	
	if(m_cancelled)
		return;
	
	NSString* nss = [NSString stringWithUTF8String: r.url.c_str()];
	NSURL* nsurl = [NSURL URLWithString: nss];
	
	NSURLRequest* urlRequest;
	if(r.body == "")
		urlRequest = [NSURLRequest requestWithURL: nsurl
									  cachePolicy: NSURLRequestReloadIgnoringCacheData
								  timeoutInterval: 20.0f];
	else{
		NSMutableURLRequest* mRequest = [NSMutableURLRequest requestWithURL: nsurl
																cachePolicy: NSURLRequestReloadIgnoringCacheData
															timeoutInterval: 20.0f];
		NSData* httpBody = [NSData dataWithBytes: (const void*) r.body.c_str() 
										  length: r.body.length()];
		[mRequest addValue: NSSTRING(i2s(r.body.length()).c_str())
	   forHTTPHeaderField: @"Content-Length"];
		[mRequest setHTTPMethod: @"POST"];
		[mRequest setHTTPBody: httpBody];
		urlRequest = mRequest;
	}				
		
	m_numActiveConnections++;
	NSURLConnection* connection = [[NSURLConnection alloc] initWithRequest: urlRequest 
																  delegate: m_receiver
														  startImmediately: YES];
	 	
	ASSERT(connection != nil);
	m_connectionRequestMap[connection] = r;	

}



void
MacDataFetcher::downloadComplete(NSURLConnection* connection,
								 unsigned char* data,
								 int len)
{
	m_lock.lock();
	
	if(m_cancelled){
		m_lock.unlock();
		return;
	}
	
	[[NSURLCache sharedURLCache] removeAllCachedResponses];
	
	const Request &r = m_connectionRequestMap[connection];
	m_client->onRequestComplete(r, ByteArray(data, len));
	m_connectionRequestMap.erase(m_connectionRequestMap.find(connection));
	
	[connection release];		
	
	m_numActiveConnections--;
	
	m_lock.unlock();
	if(!m_cancelled)
		handleNextRequest();	
}


void
MacDataFetcher::downloadFailed(NSURLConnection* connection)
{
	//ASSERT([NSThread currentThread] == [NSThread mainThread]);
	
	m_lock.lock();
	
	if(m_cancelled){
		m_lock.unlock();
		return;
	}
	
	const Request &r = m_connectionRequestMap[connection];
	map<Request, int>::iterator iter = m_failCounts.find(r);

	//if(iter->second == m_connectionRequestMap.end())
		
	int &failCount = iter == m_failCounts.end() ? (m_failCounts[r] = 0) : iter->second;

	// try again if the request has failed less than 3 times, otherwise fuck it
	m_numActiveConnections--;
	
	m_lock.unlock();
	
	if(++failCount < 3)
		handleRequest(r);
	else
		m_client->onRequestFailed(r);
	
}



void
MacDataFetcher::reset(bool cancelCurrentDownloads)
{	
	m_lock.lock();			
	doReset(cancelCurrentDownloads);
	m_lock.unlock();
}


void
MacDataFetcher::cancel()
{	
	m_lock.lock();
	m_cancelled = true;
	doReset(true);
	m_lock.unlock();
}


void
MacDataFetcher::doReset(bool cancelCurrentDownloads)
{
	ConnectionRequestMap::iterator it;
	if(cancelCurrentDownloads){
		for(it = m_connectionRequestMap.begin(); it != m_connectionRequestMap.end(); it++){
			[it->first cancel];
			[it->first release];		
		}
	}
	
	[m_receiver cleanup];
	
	m_connectionRequestMap.clear();
	m_numActiveConnections = 0;
}
