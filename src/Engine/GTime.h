/*
 *  GTime.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/24/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GTIME_H_
#define _GTIME_H_

#include <common.h>

#ifdef IPHONE
	//#import <UIKit/UIKit.h>
	//#import<ctime>
#endif

class GTime
{

public:	
    
	GTime();	
	GTime(const GTime &gt);
#ifdef IPHONE
    GTime(NSDate* date, bool retainDate = false);
#endif
    
	~GTime();
		
    static       GTime  Now() { return GTime(); }
    static const GTime& Zero();

    
	GTime& operator =  (const GTime &t);
	bool   operator == (const GTime &t) const;
	bool   operator <  (const GTime &t) const;
	bool   operator <= (const GTime &t) const;
	bool   operator >  (const GTime &t) const;
	bool   operator >= (const GTime &t) const;
	float  operator -  (const GTime &t) const;
    GTime  operator +  (float secs)     const;
    GTime& operator += (float secs);


	static float secondsBetween(const GTime &t1, const GTime &t2);    
	void update();


#ifdef IPHONE
	NSDate* getNSDate() const { return m_nsDate; }
#endif

	
private:	
    
#ifdef IPHONE
	NSDate* m_nsDate;
#endif

};


#ifdef IPHONE



inline
void
GTime::update()
{
	[m_nsDate release];
	m_nsDate = [NSDate date];
	[m_nsDate retain];
}


inline
GTime&
GTime::operator = (const GTime &t)
{
	[m_nsDate release];
	m_nsDate = [t.getNSDate() copy];
	return *this;
}


inline 
float
GTime::operator - (const GTime &t) const
{
	//ASSERT( t <= *this );
	return (float) [m_nsDate timeIntervalSinceDate: t.getNSDate()];
}


inline
bool
GTime::operator == (const GTime &t) const
{
	return m_nsDate == t.getNSDate();
}


inline
bool
GTime::operator < (const GTime &t) const
{
	return ([m_nsDate compare: t.getNSDate()] == NSOrderedAscending);
}


inline
bool
GTime::operator <= (const GTime &t) const
{
	return operator < (t) || operator == (t);
}


inline
bool
GTime::operator > (const GTime &t) const
{
	return ([m_nsDate compare: t.getNSDate()] == NSOrderedDescending);
}


inline
bool
GTime::operator >= (const GTime &t) const
{
	return operator > (t) || operator == (t);
}


inline
GTime
GTime::operator + (float secs) const
{
    NSDate* newDate = [[NSDate alloc] initWithTimeInterval: secs
                                                 sinceDate: m_nsDate];
    
    return GTime(newDate, false); // don't retain the date, since we just alloc'ed it
}


inline
GTime&
GTime::operator += (float secs)
{
    [m_nsDate addTimeInterval: secs];
    return *this;
}

#endif // IPHONE


#endif // _GTIME_H_












