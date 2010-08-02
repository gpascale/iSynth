/*
 *  GTask.cpp
 *  Phonosynth
 *
 *  Created by Greg on 1/5/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#include "GTask.h"
#include "GThread.h"
#include <Functions.h>
#include <common.h>



GTask::GTask() :
	m_worker(NULL),
	m_watchdog(NULL),
	m_work(NULL),
	m_callback(NULL)
{
	
}


GTask::GTask(const Functor& work, const Functor& callback) :
	m_worker(NULL),
	m_watchdog(NULL)
{
	m_work = work.copy();
	m_callback = callback.copy();
}


GTask::~GTask()
{
	//cerr << "GTask::~GTask()" << endl;
	SAFE_DELETE(m_worker);
	SAFE_DELETE(m_watchdog);
	SAFE_DELETE(m_work);
	SAFE_DELETE(m_callback);
}


void
GTask::performTask()
{
	//cerr << "GTask::performTask" << endl;
	
	ASSERT(m_work && m_callback);
	
	m_worker = new GThread(*m_work);
	m_worker->start();
	
	MemberFunctor<GTask> watchdogFunctor(this, &GTask::watchdogRoutine);
	m_watchdog = new GThread(watchdogFunctor);
	m_watchdog->start();
}


void
GTask::cancelTask(bool waitUntilDone)
{		
	//cerr << "GTask::cancelTask" << endl;
	
	m_watchdog->cancel();
	m_worker->cancel();	
	
	// if specified, wait until both threads have actually exited before returning
	if(waitUntilDone)
		
		while(m_worker->isRunning() || m_watchdog->isRunning())
			GThread::sleep(0.05f);
}


bool
GTask::isExecuting() const
{
	return m_worker->isRunning();
}


void 
GTask::watchdogRoutine()
{
	
	sleep(1.0f); // this is a cheap hack to "make sure" the watchdog doesn't run until the worker has started
	
	ASSERT(m_worker);
	
	while(1){
		if(m_worker->isCancelled())
			GThread::exit();		
		if(m_worker->isRunning()){
			GThread::sleep(0.05f);
		}
		else{
			GThread::performFunctorOnMainThread(*m_callback);
			break;
		}
	}
}