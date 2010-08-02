/*
 *  GTimer.cpp
 *  Phonosynth
 *
 *  Created by Greg on 9/14/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "GTimer.h"
#include <Functions.h>
#include <common.h>

#ifdef IPHONE

@implementation GTimerHelper

- (id) initWithFunctor: (const Functor&) functor
{	
    if(self = [super init]){
		m_functor = functor.copy();
		return self;
    }
	ASSERT(0 && "OneShotTimer initWithFunctor failed");
	return nil;
}



- (void) setFunctor: (const Functor&) functor
{
	SAFE_DELETE(m_functor);
	m_functor = functor.copy();
}


- (void) onTimerFire 
{	
	m_functor->call();
}

- (void) dealloc
{
	SAFE_DELETE(m_functor);
	[super dealloc];		
}

@end // implementation GTimerHelper

set<PerformFunctorRequestID> s_pendingRequests;

@implementation OneShotTimerHelper

- (id) initWithFunctor: (const Functor&) functor
			  fireTime: (const GTime&) time
{	
	if(self = [super init]){
		s_pendingRequests.insert(reinterpret_cast<PerformFunctorRequestID>(self));
		m_functor = functor.copy();
		m_predicate = NULL;
		m_frequency = 0.0f;
		m_maxWaitTime = 0.0f;
		m_invocationTime = GTime::Now();
		m_timer = [[NSTimer alloc] initWithFireDate: time.getNSDate()
										   interval: 0.0f
											 target: self
										   selector: @selector(onTimerFire)
										   userInfo: nil
											repeats: NO];		
		[[NSRunLoop mainRunLoop] addTimer: m_timer
								  forMode: NSDefaultRunLoopMode];
		return self;
	}
	ASSERT(0 && "OneShotTimerHelper init failed");
	return nil;
}


- (id) initWithFunctor: (const Functor&) functor
			 predicate: (const Predicate&) pred
			 frequency: (float) freq
		   maxWaitTime: (float) maxWait
{
	if(self = [super init]){
		s_pendingRequests.insert(reinterpret_cast<PerformFunctorRequestID>(self));
		m_functor = functor.copy();
		m_predicate = pred.copy();
		m_frequency = 0.0f;
		m_maxWaitTime = maxWait;
		m_invocationTime = GTime::Now();
		m_timer = [NSTimer scheduledTimerWithTimeInterval: freq
												   target: self
												 selector: @selector(onTimerFirePredicate)
												 userInfo: nil
												  repeats: YES];
		[m_timer retain];
		
		return self;
	}
	ASSERT(0 && "OneShotTimerHelper init failed");
	return nil;
}



- (void) cancelRequest
{
	s_pendingRequests.erase(reinterpret_cast<PerformFunctorRequestID>(self));
	[m_timer invalidate];
	[self release];
}


- (void) onTimerFire
{
	s_pendingRequests.erase(reinterpret_cast<PerformFunctorRequestID>(self));
	m_functor->call();
	[self release];		// not sure about this one
}


- (void) onTimerFirePredicate
{		
	ASSERT(m_predicate);
	
	if(GTime::Now() - m_invocationTime >= m_maxWaitTime){
		[m_timer invalidate];
		[self release];
	}
	
	else if(m_predicate->evaluate()){
		s_pendingRequests.erase(reinterpret_cast<PerformFunctorRequestID>(self));
		[m_timer invalidate];
		m_functor->call();
		[self release];
	}
}


- (void) dealloc
{	
	[m_timer release];
	SAFE_DELETE(m_functor);
	SAFE_DELETE(m_predicate);
	[super dealloc];
}


@end // implementation OneShotTimerHelper

GTimer::GTimer(const Functor& functor, float ticksPerSec) :
    m_isRunning(false),
	m_ticksPerSec(ticksPerSec),
	m_startTime(GTime::Zero()),
	m_nsTimer(nil)
{
    m_helper = [[GTimerHelper alloc] initWithFunctor: functor];
}


void
GTimer::start()
{
	// this was a bug causing multiple timers to be started. Don't start the timer if it's
	// already going!
	if(m_isRunning)
		return;
	
	m_startTime = GTime::Now();
    m_nsTimer = [NSTimer scheduledTimerWithTimeInterval: (1.0f / m_ticksPerSec)
                                                 target: m_helper
                                               selector: @selector(onTimerFire)
                                               userInfo: nil
                                                repeats: YES];
    [m_nsTimer retain];
    m_isRunning = true;
}


void
GTimer::stop()
{
    if(m_nsTimer == nil)
        return;
    
    [m_nsTimer invalidate];
    [m_nsTimer release];
    m_nsTimer = nil;
    m_isRunning = false;
}


void
GTimer::setTicksPerSec(float ticksPerSec)
{
	if(ticksPerSec == m_ticksPerSec)
		return;
	
    m_ticksPerSec = ticksPerSec;
    if(m_isRunning){
        stop();
        start();
    }
}


PerformFunctorRequestID
GTimer::performFunctorAfterDelay(const Functor& functor, float delay)
{
	return performFunctorAtTime(functor, GTime::Now() + delay);	
}



PerformFunctorRequestID
GTimer::performFunctorAtTime(const Functor& functor, const GTime& time)
{
	OneShotTimerHelper* helper = [[OneShotTimerHelper alloc] initWithFunctor: functor
																	fireTime: time];
	
	ASSERT(helper);
	
	return (PerformFunctorRequestID) helper;
}


PerformFunctorRequestID
GTimer::performFunctorUponPredicate(const Functor& functor, const Predicate& pred,
									float checkFrequency, float maxWait)
{
	OneShotTimerHelper* helper = [[OneShotTimerHelper alloc] initWithFunctor: functor
																   predicate: pred
																   frequency: checkFrequency
																 maxWaitTime: maxWait];
	ASSERT(helper);
	
	return reinterpret_cast<PerformFunctorRequestID> (helper);
}


void
GTimer::cancelPerformFunctorRequest(PerformFunctorRequestID requestID)
{
	// check to make sure the request hasn't already fired or been cancelled
	if(s_pendingRequests.count(requestID) > 0){
		OneShotTimerHelper* helper = (OneShotTimerHelper*) requestID;
		[helper cancelRequest];
	}
}


#endif
    
