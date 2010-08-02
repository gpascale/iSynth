/*
 *  Time.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/24/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "GTime.h"

#ifdef IPHONE

GTime::GTime()
{
	m_nsDate = [NSDate date];
	[m_nsDate retain];
}


GTime::GTime(const GTime& gt)
{
	m_nsDate = [gt.getNSDate() copy];
	//[m_nsDate retain];
}


GTime::GTime(NSDate* date, bool retainDate)
{
    m_nsDate = date;
    if(retainDate)
        [m_nsDate retain];
}


GTime::~GTime()
{
	[m_nsDate release];
}


const GTime&
GTime::Zero()
{
    static const GTime ret([NSDate distantPast]);
    return (const GTime&) ret;
}

#endif