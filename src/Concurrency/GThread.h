/*
 *  Thread.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/22/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _THREAD_H_
#define _THREAD_H_

#include <Functions.h>

class  Threaded;
class  GThread;

/* 
 * This helper class is necessary in order to use NSThread as GThread's underlying thread library
 * It simply creates an NSThread which calls 'threadRoutine' which simply calls the parent GThread's
 * 'callMemberFunc' function. This is just a roundabout way of calling a C++ function using an NSThread
 */

@interface GThreadHelper : NSObject
{			
	GThread* m_parent;
}

- (id)   initWithParent: (GThread*) parent;
- (void) threadRoutine;

@end

@interface MainThreadHelper : NSObject {
	Functor* m_functor;
}
- (void) performFunctorOnMainThread: (const Functor&) functor;
@end



class GThread
{
	
public:
	    
	// constructors
	GThread();
	GThread(const Functor& functor);
	
	~GThread();
		
#ifdef IPHONE
    // explicitly set the underlying NSThread for a GThread. Not sure why I put this in
	// and don't feel like figuring it out, so it stays
	void			setNSThread(NSThread* nsThread) { m_nsThread = nsThread; }
#endif
    

	// main GThread interface
	void			setFunction(const Functor& f);
	void			start();            // start the GThread running
	void			cancel();           // tell a GThread to cancel itself
	bool			isRunning();        
	bool			isCancelled();
	bool			isFinished();
	
	// release the thread's autorelease pool. Need to do this every so often	
	void		releasePool();
    
    // by default, run will call the Objective-C, C or C++ function given to the thread. Alternatively,
	// GThread can be subclassed and the run method may be overridden.
	virtual void	run();  
    
	// static version of isCancelled(), refers to the current thread
	static bool		isCurThreadCancelled();
    // put the current thread to sleep for 'seconds' seconds	
    static void     sleep(const double seconds); 	
	// exit the current thread
	static void		exit();	
	
	
	// convenience function
	static void		performFunctorOnMainThread(const Functor& f);
	
private:
	
#ifdef IPHONE		
	
	GThreadHelper*      m_helper;               // explained above
	NSThread*			m_nsThread;             // the underlying thread
	NSAutoreleasePool*  m_autoreleasePool;      // Objective-C garbage
    
#endif // IPHONE
	
	Functor*			m_functor;

};



#endif _THREAD_H_