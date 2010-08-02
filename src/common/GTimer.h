/*
 *  GTimer.h
 *  Phonosynth
 *
 *  Created by Greg on 9/14/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

/* 
 * A platform independent timer class. Implemented for Cocoa as a wrapper around NSTimer. Not yet implemented
 * for Windows. The class structure follows a similar pattern as GThread/GThreadHelper/Threaded in terms
 * of having to jump through a few different hoops to overcome Objective-C's inability to integrate with
 * C++ templates. Just as member functions which we want to use for GThreads must be declared in the Threaded
 * interface, C/C++ functions must be declared in TimerClient if we wish to call them using a GTimer. 
 */

#ifndef _GTIMER_H_
#define _GTIMER_H_

#import  <UIKit/UIKit.h>
#include <GTime.h>
#include <set>

class Functor;
class Predicate;

using namespace std;


// As explained above, a somewhat hacky interface that allows us to call C++ functions using GTimers 
// (somewhat) cleanly
class TimerClient
{
public:
    virtual void update() { }
    virtual void loadATexture() { }
    virtual void loadSynthThreadCheckComplete() { }
};


// Similar to GThreadHelper, this class allows for the use of NSTimer as the underlying structure for
// the Cocoa implementation of GTimer
@interface GTimerHelper : NSObject
{
	Functor*	m_functor;
}
- (id) initWithFunctor: (const Functor&) functor;
- (void) setFunctor: (const Functor&) functor;
- (void) onTimerFire;
@end 


@interface OneShotTimerHelper : NSObject
{
	Functor*	m_functor;
	Predicate*	m_predicate;
	float	    m_frequency;
	float		m_maxWaitTime;
	NSTimer*	m_timer;
	GTime		m_invocationTime;
}

- (id) initWithFunctor: (const Functor&) functor
			  fireTime: (const GTime&) time;

- (id) initWithFunctor: (const Functor&) functor
			 predicate: (const Predicate&) pred
			 frequency: (float) freq
		   maxWaitTime: (float) maxWait;

- (void) onTimerFire;

- (void) onTimerFirePredicate;

@end


// GTimer itself is a pretty simple platform-independent Timer class. Cocoa implementation uses NSTimer 
// at its core and, as described above, there is some baggage involved in making that work

typedef unsigned int PerformFunctorRequestID;

class GTimer
{

public:
    
#ifdef IPHONE
    GTimer(const Functor& functor, float ticksPerSec = 1.0f);
    void setFunction(id selector);
#endif
    
    void  start();
    void  stop();
    void  setTicksPerSec(float ticksPerSec);
    void  setFunctor(const Functor& f) { [m_helper setFunctor: f]; }
	float runningTime() { return GTime::Now() - m_startTime; }
    
    bool isRunning() const { return m_isRunning; }		
	
	// performs the specified functor after 'delay' seconds. Returns an identifier which can
	// be used to cancel the request by calling cancelPerformFunctorRequest
	static PerformFunctorRequestID performFunctorAfterDelay(const Functor& functor, float delay);
	
	// performs the specified functor at the specified time. Returns an identifier which can
	// be used to cancel the request by calling cancelPerformFunctorRequest
	static PerformFunctorRequestID performFunctorAtTime(const Functor& functor, const GTime& time);

	// performs the specified functor when the predicate is true. checkFrequency specifies how often
	// to check the value of the predicate (in ticks per second)	
	static PerformFunctorRequestID performFunctorUponPredicate(const Functor& functor,
															   const Predicate& pred,
															   float checkFrequency = 1.0f,
															   float maxWaitTime = 60.0f);
	
	static void cancelPerformFunctorRequest(PerformFunctorRequestID requestID);		

private:

#ifdef IPHONE	
    NSTimer*             m_nsTimer;
    GTimerHelper*        m_helper;
#endif

	float                m_ticksPerSec;
    bool                 m_isRunning;
	GTime				 m_startTime;
	
};


#endif _GTIMER_H_

