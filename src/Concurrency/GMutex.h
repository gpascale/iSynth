/*
 *  GMutex.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/22/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

/*
 * A very simple platform-independent mutex. Implemented as a wrapper around NSLock for Cocoa, not yet 
 * implemented for Windows
 */

#ifndef _GMUTEX_H_
#define _GMUTEX_H_

@class MainThreadLockHelper;

class GMutex
{

public:
	
	GMutex();
	~GMutex();
	    
	void lock();                // lock the mutex
    bool trylock();             // tries to lock the mutex, returning true if successful. Doesn't block
    void safeLock();            // throws an assertion failure it the calling thread already holds the mutex        
    void lockOnMainThread();    // sends the main thread a request to lock the mutex
    
	void unlock();              // unlock the mutex
    bool safeUnlock();          // unlock the mutex if we are the current holder, unlike safeLock, this
                                // function won't throw an assertion on failure because it does not 
                                // necessarily indicate bad code
    
    // returns whether or not the calling thread holds the mutex
    inline bool currentThreadIsHolder() const;
    
private:
	
#ifdef IPHONE
    NSThread*               m_holder;       // pointer to the lock's current holding thread
	NSLock*                 m_nsLock;       // underlying NSLock (Cocoa implementation)
    MainThreadLockHelper*   m_lockHelper;   // helper structure for locking on main thread
#endif

#ifdef WINDOWS
	// Windows Mutex structure	
#endif
	
};


#ifdef IPHONE

inline bool
GMutex::currentThreadIsHolder() const
{
    return m_holder == [NSThread currentThread];
}

@interface MainThreadLockHelper : NSObject
{
    GMutex* m_mutex;
}

- (id)   initWithGMutex: (GMutex*) mutex;
- (void) lock;

@end // interface MainThreadLockHelper

#endif

#endif _GMUTEX_H_

