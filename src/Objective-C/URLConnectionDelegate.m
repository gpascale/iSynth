//
//  URLConnectionDelegate.m
//  Phonosynth
//
//  Created by Greg Pascale on 7/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "URLConnectionDelegate.h"

#include "MacDataFetcher.h"
#include <iostream>
#include <common.h>

#include <GTime.h>

@implementation URLConnectionDelegate

int numResponses;
int bytesAllocated;
int failCount;
int reallocCount;

- (URLConnectionDelegate*) init
{
	bytesAllocated = 0;
	numResponses = 0;
	failCount = 0;
	reallocCount = 0;
	m_parent = nil;
	m_data.clear();
	m_offsets.clear();
	m_alloced.clear();
	return self;
}


- (URLConnectionDelegate*) initWithParent: (MacDataFetcher*) parent
{
	m_parent = parent;
	m_data.clear();
	m_offsets.clear();
	m_alloced.clear();
	return self;
}


- (void) dealloc
{
	[super dealloc];
	// shouldn't be anything to get rid of
}


- (void) setParent: (MacDataFetcher*) parent
{
	m_parent = parent;
}


- (void) cleanup
{
	ConnectionDataMap::iterator iter, beg = m_data.begin(), end = m_data.end();
	for(iter = beg; iter != end; iter++){
		SAFE_FREE(iter->second);
	}
	
	m_data.clear();
	m_offsets.clear();
	m_alloced.clear();
}	


- (void) connection: (NSURLConnection*) connection 
	 didReceiveData: (NSData*) data

{

	const GTime startTime;
	
	ASSERT([NSThread currentThread] == [NSThread mainThread]);		
	ASSERT(m_data.find(connection) != m_data.end());
	ASSERT(m_offsets.find(connection) != m_offsets.end());
	
	      int &lenSoFar = m_offsets[connection];
	const int lenToDL   = [data length];
	
	// if there isn't enough memory allocated to store all the data
	// (expected length was wrong), fix that shit yo
	
	if(lenSoFar + lenToDL > m_alloced[connection]){
		reallocCount++;
		printf("whoops\n");
		unsigned char* newBuffer = (unsigned char*) malloc(lenSoFar + lenToDL);
		memcpy(newBuffer, m_data[connection], lenSoFar);
		free(m_data[connection]);
		m_data[connection] = newBuffer;
	}
	
	// append the newly received data to the buffer
	memcpy(m_data[connection] + lenSoFar, [data bytes], lenToDL);
	
	lenSoFar += lenToDL;
	//printf("offset now = %d\n", m_offsets[connection]);		
	 
}



- (void) connectionDidFinishLoading: (NSURLConnection*) connection
{
	
	m_parent->downloadComplete(connection, 
							   m_data[connection], 
							   m_offsets[connection]);
	
	ConnectionDataMap::iterator   d_iter = m_data.find(connection);
	ConnectionOffsetMap::iterator o_iter = m_offsets.find(connection);
	ConnectionAllocMap::iterator  a_iter = m_alloced.find(connection);
		
	ASSERT(d_iter != m_data.end());
	ASSERT(o_iter != m_offsets.end());
	ASSERT(a_iter != m_alloced.end());
		
	[[NSURLCache sharedURLCache] removeAllCachedResponses];
	
	m_data.erase(d_iter);
	m_offsets.erase(o_iter);		
	m_alloced.erase(a_iter);
	
}
 



- (void) connection: (NSURLConnection*) connection 
   didFailWithError: (NSError*) error
{
	
	ASSERT([NSThread currentThread] == [NSThread mainThread]);
	
	printf("********Download %d Failed*********\n", connection);
	printf("Error was: %s\n", [[error description] UTF8String]);

	ConnectionDataMap::iterator d_iter   = m_data.find(connection);
	ConnectionOffsetMap::iterator o_iter = m_offsets.find(connection);	
	
	// perform necessary cleanup
	if (d_iter != m_data.end()){
		SAFE_FREE(m_data[connection]);
		//cerr << "freed connection " << connection << endl;
		m_data.erase(d_iter);
	}
	if (o_iter != m_offsets.end()){
		m_offsets.erase(o_iter);	
	}		
	
	m_parent->downloadFailed(connection);
	
}


- (void) connection: (NSURLConnection*) connection 
didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge
{
    ASSERT("Auth challenge" && 0);
}


- (void) connection: (NSURLConnection*) connection 
 didReceiveResponse: (NSURLResponse*) response
{
	ASSERT(m_data.find(connection) == m_data.end());
	ASSERT([NSThread currentThread] == [NSThread mainThread]);
    
	const int expLen      =  [response expectedContentLength];
	m_data[connection]	  =  (unsigned char*) malloc (expLen * sizeof(unsigned char));
	bytesAllocated		  += expLen * sizeof(unsigned char);
	m_alloced[connection] =  expLen;
	m_offsets[connection] =  0; 
}


// This should be documented better
- (NSCachedURLResponse*) connection: (NSURLConnection*) connection
				  willCacheResponse: (NSCachedURLResponse*) cachedResponse	
{
	return nil;
}
	

@end
