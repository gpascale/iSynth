//
//  AccelerometerDelegate.h
//  Phonosynth
//
//  Created by Greg Pascale on 8/7/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

class InputHandler;

@interface AccelerometerDelegate : NSObject <UIAccelerometerDelegate> 
{
	InputHandler* m_inputHandler;	
}

- (AccelerometerDelegate*) init;
- (AccelerometerDelegate*) initWithInputHandler: (InputHandler*) inputHandler;

- (void) accelerometer: (UIAccelerometer*) 
		 didAccelerate: (UIAcceleration*) accceleration;

@end
