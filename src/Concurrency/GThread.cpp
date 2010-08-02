
/*
 *  Thread.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/22/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "GThread.h"
#include <common.h>

#ifdef IPHONE

@implementation GThreadHelper


- (id) initWithParent: (GThread*) parent
{
	if(self = [super init]){
		m_parent = parent;
		NSThread* thread = [[NSThread alloc] initWithTarget: self
												   selector: @selector(threadRoutine)
													 object: nil];
		m_parent->setNSThread(thread);
		return self;
	}
	ASSERT(0 && "Error in GThread initWithParent");
	return nil;
}


- (void) threadRoutine
{	
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	m_parent->run();
	[pool release];
}


@end // implementation GThreadHelper



@interface MainThreadHelper () 
- (void) performFunctorMainThread;
@end



@implementation MainThreadHelper


- (void) performFunctorMainThread
{
	m_functor->call();
}


- (void) performFunctorOnMainThread: (const Functor&) functor
{
	m_functor = functor.copy();
	[self performSelectorOnMainThread: @selector(performFunctorMainThread)
						   withObject: nil
						waitUntilDone: NO];
}

@end // implementation MainThreadHelper



// constructs a GThread with no target or function pointer. Subclasses that override run()
// will call this constructor
GThread::GThread() :
	m_functor(NULL)
{
    m_helper = [[GThreadHelper alloc] initWithParent: this];
}

// constructs a thread that calls a C++ member function

GThread::GThread(const Functor& functor)
{
	m_functor = functor.copy();
	m_helper = [[GThreadHelper alloc] initWithParent: this];
}


GThread::~GThread()
{
	SAFE_DELETE(m_functor);
}


void
GThread::setFunction(const Functor& f)
{
	ASSERT(!isRunning());
	m_functor = f.copy();
}


void
GThread::start()
{	
	[m_nsThread start];
}
				   

void
GThread::cancel()
{    
	[m_nsThread cancel];
}


bool
GThread::isRunning()
{
	return [m_nsThread isExecuting];
}


bool
GThread::isCancelled()
{
	return [m_nsThread isCancelled];
}


bool
GThread::isFinished()
{
	return [m_nsThread isFinished];
}


bool
GThread::isCurThreadCancelled()
{
	return [[NSThread currentThread] isCancelled];
}


void
GThread::sleep(const double seconds)
{
    [NSThread sleepForTimeInterval: seconds];
}


void
GThread::exit()
{
	[NSThread exit];
}


void
GThread::releasePool()
{
	
}


void
GThread::run()
{
	if(m_functor)
		m_functor->call();
	else
		ASSERT("GThread::run() called but m_functor is NULL" && 0);
}


void
GThread::performFunctorOnMainThread(const Functor& functor)
{	
	MainThreadHelper* mth = [[MainThreadHelper alloc] init];		
	[mth performFunctorOnMainThread: functor];
	[mth release];
}


#endif
				   
				   
						
				   
#ifdef WINDOWS


GThread::~GThread()
{

}

void
GThread::start()
{

}

void
GThread::cancel()
{

}

bool
GThread::isRunning()
{

}

bool
GThread::isCancelled()
{

}

#endif