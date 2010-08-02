/*
 *  MacDataFetcher.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifdef IPHONE


#ifndef _MACDATAFETCHER_H_
#define _MACDATAFETCHER_H_


#include "DataFetcher.h"

#include <vector>
#include <queue>

#include <GMutex.h>

#import  <UIKit/UIKit.h>
#import  "URLConnectionDelegate.h"

using namespace std;

class MacDataFetcher : public DataFetcher
{

	typedef map<NSURLConnection*, Request> ConnectionRequestMap;
	
public:
	
	MacDataFetcher(DataClient* client, int maxConcurrentConnections = 5);
	~MacDataFetcher();
	
	void beginFetchingData();
	void cancelAllDownloads();
	void reset(bool cancelCurrentDownloads = true);
	void cancel();
	
	// callbacks, called by URLConnectionDelegate when a download finishes/fails
	void downloadComplete(NSURLConnection* connection,
						  unsigned char* data,
						  int len);	
	
	void downloadFailed(NSURLConnection* connection);				
	
private:
	
	void doReset(bool cancelCurrentDownloads);
		
	void handleRequest(const Request &r);
	bool handleNextRequest();
	
	map<Request, int>			m_failCounts;	
	URLConnectionDelegate*		m_receiver;	
	ConnectionRequestMap		m_connectionRequestMap;	

	bool						m_cancelled;
	
};

#endif

#endif