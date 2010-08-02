/*
 *  GTask.h
 *  Phonosynth
 *
 *  Created by Greg on 1/5/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */


#ifndef _GTASK_H_
#define _GTASK_H_

class Functor;
class GThread;


class GTask
{
public:
	
	GTask();
	GTask(const Functor& work, const Functor& callback);
	
	~GTask();
	
	virtual void performTask();
	virtual void cancelTask(bool waitUntilDone);
	
	virtual bool isExecuting() const;

protected:
	
	void watchdogRoutine();
	
	GThread*		m_worker;
	GThread*		m_watchdog;
	
	Functor*		m_work;
	Functor*		m_callback;
	
};



#endif // _GTASK_H_
