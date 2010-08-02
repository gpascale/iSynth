/*
 *  PSInputHandler.h
 *  Phonosynth
 *
 *  Created by Greg on 12/28/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */


#ifndef _PSINPUTHANDLER_H_
#define _PSINPUTHANDLER_H_

#include <InputHandler.h>

enum MultiTouchType
{
	MULTITOUCHTYPE_PINCH  = 1,
	MULTITOUCHTYPE_SPREAD = 2,
	MULTITOUCHTYPE_DRAG   = 3
};

class PSInputHandler : public InputHandler
{

public:
	
	PSInputHandler(UIView* view = nil);
	
	// InputHandler overrides
    void    onTouchesBegan(NSSet* touches, UIEvent* event, UIView* view);
    void    onTouchesMoved(NSSet* touches, UIEvent* event, UIView* view);
    void    onTouchesEnded(NSSet* touches, UIEvent* event, UIView* view);
	void	onReadAccelerometer(UIAcceleration* accel);
    
	void	setAutorotation(bool b);
	
private:    

    // TODO: make touchesBegan et al. do this stuff directly, get rid of these guys
    
    void				onTouchesStarted(const vector<Touch> &touches);
    void				onTouchesMoved(const vector<Touch> &touches);
    void				onTouchesEnded(const vector<Touch> &touches);    	    
    void				handleSingleTouchMove(const Touch &touch);	
	void				handleDoubleTouchMove(const Touch &touch1, 
											  const Touch &touch2);	
	MultiTouchType		getMultiTouchType(const Touch &t1,
										  const Touch &t2);	
	Vector3				getMultiTouchDragVector(const Touch &touch1,
												const Touch &touch2);	
	float				getMagnitude(const Touch &touch1,
									 const Touch &touch2);
	void				initializePopups();
    
	void				cleanup();
	
	bool				m_rectsActive;	
	bool				m_autorotate;
};

#endif






























// old stuff

#if 0

enum AccelerometerMotionType
{
	MOTION_TILT_FORWARDS,
	MOTION_TILT_BACKWARDS,
	MOTION_TILT_LEFT,
	MOTION_TILT_RIGHT,
	MOTION_NONE
};

enum MouseButton{
    MOUSE_BUTTON_LEFT = 1,
    MOUSE_BUTTON_RIGHT = 2
};


// InputHandler members

AccelerometerMotionType	getAccelerometerMotionType(const Vector3 &tiltVector);




#endif