/*
 *  Mutex.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/22/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "GMutex.h"
#include <common.h>

#ifdef IPHONE

@implementation MainThreadLockHelper


- (id) initWithGMutex: (GMutex*) mutex
{
    if(self = [super init])
        m_mutex = mutex;
    else
        ASSERT(0 && "[super init] failed (MainThreadLockHelper)");
        
    return self;
}


- (void) lock
{
    m_mutex->lock();
}


@end // implementation MainThreadLockHelper

// Mac implementation
GMutex::GMutex() :
    m_holder(nil)
{
	m_nsLock = [[NSLock alloc] init];
    m_lockHelper = [[MainThreadLockHelper alloc] initWithGMutex: this];
}


GMutex::~GMutex()
{
	[m_nsLock release];
	[m_lockHelper release];
}


void
GMutex::lock()
{	
	[m_nsLock lock];
    m_holder = [NSThread currentThread];
}


bool
GMutex::trylock()
{
    if([m_nsLock tryLock]){
        m_holder = [NSThread currentThread];
        return true;
    }
    return false;
}


void
GMutex::safeLock()
{
    if(currentThreadIsHolder())
        ASSERT("Thread tried to lock a mutex it already holds. Check your code!" && 0);
    [m_nsLock lock];
    m_holder = [NSThread currentThread];
}


void
GMutex::lockOnMainThread()
{    
    [m_lockHelper performSelectorOnMainThread: @selector(lock)
                                   withObject: nil
                                waitUntilDone: NO];     
}



void
GMutex::unlock()
{
	[m_nsLock unlock];
}


bool
GMutex::safeUnlock()
{
    if(currentThreadIsHolder()){
        [m_nsLock unlock];
        return true;
    }
    return false;
}

#endif // IPHONE






#ifdef WINDOWS

// Windows implementation of Mutex ...
GMutex::GMutex()
{
	
}


GMutex::~GMutex()
{

}

void	
GMutex::lock()
{

}

void
GMutex::unlock()
{
	
}

#endif WINDOWS


