//
//  AccelerometerDelegate.m
//  Phonosynth
//
//  Created by Greg Pascale on 8/7/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "AccelerometerDelegate.h"
#include <PSInputHandler.h>
#include <Vector3.h>

@implementation AccelerometerDelegate


- (AccelerometerDelegate*) init
{
	m_inputHandler = NULL;
	return self;
}


- (AccelerometerDelegate*) initWithInputHandler: (InputHandler*) inputHandler
{
	m_inputHandler = inputHandler;
	return self;
}


- (void) accelerometer: (UIAccelerometer*) accelerometer
		 didAccelerate: (UIAcceleration*) acceleration
{
	m_inputHandler->readAccelerometer(acceleration);		
}




@end
