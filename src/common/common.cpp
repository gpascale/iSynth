/*
 *  common.cpp
 *  Phonosynth
 *
 *  Created by Greg on 2/13/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */


#include <common.h>

// a function that exists simpy to stop code execution in debug mode. Sometimes this is a little easier than
// setting and unsetting lots of breakpoints all over the place
void doBreakpoint(const string& message)
{
	//cerr << "Tripped a breakpoint" << endl;
	if(message != "")
		//cerr << message << endl;
	int x = 4;    // just do something to break on
}