//
//  URLConnectionDelegate.h
//  Phonosynth
//
//  Created by Greg Pascale on 7/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#ifndef _URLCONNECTIONDELEGATE_H_
#define _URLCONNECTIONDELEGATE_H_

#import <UIKit/UIKit.h>

#include <map>
#include <GMutex.h>

class MacDataFetcher;

using namespace std;

typedef map<NSURLConnection*, unsigned char*> ConnectionDataMap;
typedef map<NSURLConnection*, int			> ConnectionOffsetMap;
typedef map<NSURLConnection*, int			> ConnectionAllocMap;

@interface URLConnectionDelegate : NSObject
{
	
	int						m_length;
	MacDataFetcher*			m_parent;
	
	
	ConnectionDataMap		m_data;
	ConnectionOffsetMap		m_offsets;
	ConnectionAllocMap		m_alloced;
	
	// all of these functions are called by lots of downloading threads
	GMutex					m_mutex;
}

// constructors

- (URLConnectionDelegate*) init;
- (URLConnectionDelegate*) initWithParent: (MacDataFetcher*) parent;

- (void) dealloc;

- (void) setParent: (MacDataFetcher*) parent;

- (void) cleanup;

// NSURLConnection delegate methods

- (void) connection: (NSURLConnection*) connection 
	 didReceiveData: (NSData*) data;

- (void) connectionDidFinishLoading: (NSURLConnection*) connection;

- (void) connection: (NSURLConnection*) connection
   didFailWithError: (NSError*) error;

- (void) connection: (NSURLConnection*) connection 
 didReceiveResponse: (NSURLResponse*) response;


- (NSCachedURLResponse*) connection: (NSURLConnection*) connection
				  willCacheResponse: (NSCachedURLResponse*) cachedResponse;


@end


#endif