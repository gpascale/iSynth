/*
 *  PSInputHandler.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/28/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "PSInputHandler.h"
#include "PSTypes.h"
#include "PSMaster.h"
#include "PSSettings.h"
#include "PSGlobalState.h"
#import  "UI.h"
#import  <ObjCCommon.h>

// TODO: Make these 3 functions do all the work rather than throwing the touches into a 
// vector and delegating to the other ones


PSInputHandler::PSInputHandler(UIView* view) :
	InputHandler(view),
	m_autorotate(true),
	m_rectsActive(false)
{
	setAccelerometerReadFrequency(5);
}


void
PSInputHandler::cleanup()
{
	m_numFingersDown = 0;
	m_activeButton   = 0;	
}


void
PSInputHandler::onTouchesBegan(NSSet* touches, UIEvent* event, UIView* view)
{
	int nt = 0;
	vector<Touch> touchVec;
	
	const int numTaps = [[touches anyObject] tapCount];
	
	for (UITouch *touch in touches){
        CGPoint curLoc  = [touch transformedLocationInView: view];
        CGPoint prevLoc = [touch transformedPreviousLocationInView: view];
        touchVec.push_back(Touch(curLoc.x, curLoc.y, prevLoc.x, prevLoc.y, numTaps));
        nt++;
    }	
        
    onTouchesStarted(touchVec);
}

        
void
PSInputHandler::onTouchesMoved(NSSet* touches, UIEvent* event, UIView* view)
{

    int nt = 0;
    vector<Touch> touchVec;
    
    for (UITouch *touch in touches){
        CGPoint curLoc  = [touch transformedLocationInView: view];
        CGPoint prevLoc = [touch transformedPreviousLocationInView: view];
        touchVec.push_back(Touch(curLoc.x, curLoc.y, prevLoc.x, prevLoc.y));
        nt++;
    }	
    
    onTouchesMoved(touchVec);
}


void
PSInputHandler::onTouchesEnded(NSSet* touches, UIEvent* event, UIView* view)
{  
    int nt = 0;
    vector<Touch> touchVec;
    
    for (UITouch *touch in touches){
        CGPoint curLoc  = [touch transformedLocationInView: view];
        CGPoint prevLoc = [touch transformedPreviousLocationInView: view]; 
        touchVec.push_back(Touch(curLoc.x, curLoc.y, prevLoc.x, prevLoc.y));
        nt++;
    }
    
    onTouchesEnded(touchVec);
}


void
PSInputHandler::onReadAccelerometer(UIAcceleration* accel)
{
	if(!PSSettings::Settings().getAutorotation())
		return;
		
	if(accel.x < -0.85f){
		// rotate to landscape
		m_app->rotateToOrientation(SCREEN_ORIENTATION_LANDSCAPE);
	}
	else if(accel.x > 0.85f){
		// rotate to upside-down landscape
		m_app->rotateToOrientation(SCREEN_ORIENTATION_LANDSCAPE_REV);
	}
	else if(accel.y < -0.85f){
		// rotate to portrait
		m_app->rotateToOrientation(SCREEN_ORIENTATION_PORTRAIT);
	}
	else if(accel.y > 0.85f){
		// rotate to upside-down portrait
		m_app->rotateToOrientation(SCREEN_ORIENTATION_PORTRAIT_REV);
	}
}


void
PSInputHandler::setAutorotation(bool b)
{
	m_autorotate = b;
}


void
PSInputHandler::onTouchesStarted(const vector<Touch> &touches)
{	
	
	const PhonosynthMode mode = m_app->getMode();
	
	if(mode == MODE_NORMAL){
		
		// if there is one finger down, try to select a QRect
		if(m_numFingersDown == 1 && PSGlobalState::getRectsMode()){
			ASSERT(touches.size() == 1);
			const int touchX = touches[0].getCurX();
			const int touchY = touches[0].getCurY();
			
			m_app->selectQRect(touchX, touchY);
			m_rectsActive = true;
			
		}
		// otherwise, cancel the QRect selection
		else if (PSGlobalState::getRectsMode()){
			m_app->selectQRect(-1, -1);
			m_rectsActive = false;						
		}
		else{
			BREAK;
		}		
	}
				
	else if(mode == MODE_ARCBALL){
		FOR(i,0,touches.size()){
			
			const int touchX = touches[i].getCurX();
			const int touchY = touches[i].getCurY();				
					
			if(touches[i].getTapCount() > 1){				
				if(m_app->selectProjector(touchX, touchY)){
					return;
				}
			}
		}
	}
}


void
PSInputHandler::onTouchesMoved(const vector<Touch> &touches)
{
	
	ASSERT(touches.size() >= 0);
	if(touches.size() > 2) return;
		
	if(touches.size() == 1)
		handleSingleTouchMove(touches[0]);	
	else
		handleDoubleTouchMove(touches[0], touches[1]);
    
}


void
PSInputHandler::handleSingleTouchMove(const Touch &touch)
{			
	
	int dx = touch.getCurX() - touch.getPrevX();
	int dy = touch.getCurY() - touch.getPrevY();
	
	const float speed = 0.0025f;		
	
	m_compositeDrag.first += dx;
	m_compositeDrag.second += dy;	
	
	const PhonosynthMode mode = m_app->getMode();
	
	if(abs(m_compositeDrag.first) + abs(m_compositeDrag.second) > 10){
		
		// don't handle a single finger movement if there are two fingers down
		if(mode == MODE_NORMAL && m_numFingersDown < 2 && PSGlobalState::getRectsMode()){
			if(m_rectsActive)
				m_app->selectQRect(touch.getCurX(), touch.getCurY());
		}
        
		else{
			const float xAmt = abs(m_compositeDrag.first)  * speed;
			const float yAmt = abs(m_compositeDrag.second) * speed;		
			
			if(m_compositeDrag.first > 0){
				m_app->queueEvent(EVENT_ROTATE_LEFT, EventParam(xAmt));
			}
			else if(m_compositeDrag.first < 0){
				m_app->queueEvent(EVENT_ROTATE_RIGHT, EventParam(xAmt));
			}
			if(m_compositeDrag.second > 0)
				m_app->queueEvent(EVENT_ROTATE_UP, EventParam(yAmt));
			else if(m_compositeDrag.second < 0)	
				m_app->queueEvent(EVENT_ROTATE_DOWN, EventParam(yAmt));								
		}
		
		m_compositeDrag = make_pair(0, 0);
		
	}			
}


MultiTouchType
PSInputHandler::getMultiTouchType(const Touch &touch1,
								const Touch &touch2){	
	
	Vector3 touch1Dir(touch1.getCurX() - touch1.getPrevX(), 
					  touch1.getCurY() - touch1.getPrevY(),
					  0.0f);
	
	Vector3 touch2Dir(touch2.getCurX() - touch2.getPrevX(), 
					  touch2.getCurY() - touch2.getPrevY(),
					  0.0f);
	
	Vector3 axis12(touch2.getCurX() - touch1.getCurX(),
				   touch2.getCurY() - touch1.getCurY(),
				   0.0f);
	
	touch1Dir.normalize();
	touch2Dir.normalize();
	axis12.normalize();
    
	const bool oneTowardsTwo = touch1Dir.dot(axis12) > 0.5f;
	const bool twoTowardsOne = touch2Dir.dot(axis12) < 0.5f;
	
	if(oneTowardsTwo && twoTowardsOne)
		return MULTITOUCHTYPE_PINCH;
	else if (!oneTowardsTwo && !twoTowardsOne)
		return MULTITOUCHTYPE_SPREAD;
	
	return MULTITOUCHTYPE_DRAG;
	
}



Vector3
PSInputHandler::getMultiTouchDragVector(const Touch &touch1,
									  const Touch &touch2)
{
	Vector3 touch1Dir(touch1.getCurX() - touch1.getPrevX(), 
					  touch1.getCurY() - touch1.getPrevY(),
					  0.0f);
	
	Vector3 touch2Dir(touch2.getCurX() - touch2.getPrevX(),
					  touch2.getCurY() - touch2.getPrevY(),
					  0.0f);
	
	return 0.5f * (touch1Dir + touch2Dir);
}


float
PSInputHandler::getMagnitude(const Touch &touch1,
						     const Touch &touch2)
{
	Vector3 touch1Dir(touch1.getCurX() - touch1.getPrevX(), 
					  touch1.getCurY() - touch1.getPrevY(),
					  0.0f);
	
	Vector3 touch2Dir(touch2.getCurX() - touch2.getPrevX(), 
					  touch2.getCurY() - touch2.getPrevY(),
					  0.0f);
	
	return 0.5f * (touch1Dir.magnitude() + touch2Dir.magnitude());
}


void
PSInputHandler::handleDoubleTouchMove(const Touch &touch1, const Touch &touch2)
{
	////cout << "handleDoubleTouchMove" << endl;	
	
	m_compositeDrag = make_pair(0, 0);
	
	MultiTouchType mtType = getMultiTouchType(touch1, touch2);
	
	const float moveSpeed = 0.005f;
	const float zoomSpeed = 0.0075f;
	
	switch(mtType){
		case MULTITOUCHTYPE_DRAG:
		{
			Vector3 dragVec = getMultiTouchDragVector(touch1, touch2);			
			int dx = dragVec.x + 0.5f; //round up
			int dy = dragVec.y + 0.5f;					
			if(dx > 0)
				m_app->queueEvent(EVENT_MOVE_RIGHT, EventParam(dragVec.x * moveSpeed));
			else if (dx < 0)
				m_app->queueEvent(EVENT_MOVE_LEFT, EventParam((-dragVec.x) * moveSpeed));
			
			if(dy > 0)
				m_app->queueEvent(EVENT_MOVE_DOWN, EventParam(dragVec.y * moveSpeed));
			else if (dy < 0)
				m_app->queueEvent(EVENT_MOVE_UP, EventParam((-dragVec.y) * moveSpeed));
			
			break;
		}
		case MULTITOUCHTYPE_PINCH:
		{
			float amt = getMagnitude(touch1, touch2);
			m_app->queueEvent(EVENT_ZOOM_OUT, EventParam(amt * zoomSpeed));
			break;
		}
		case MULTITOUCHTYPE_SPREAD:
		{			
			float amt = getMagnitude(touch1, touch2);			
			m_app->queueEvent(EVENT_ZOOM_IN, EventParam(amt * zoomSpeed));
			break;
		}
	}		
}


void
PSInputHandler::onTouchesEnded(const vector<Touch> &touches)
{	
	FOR(i,0,touches.size()){
		if(touches.size() == 1 && m_rectsActive){
			if(m_app->selectQRect(touches[i].getCurX(), touches[i].getCurY(), true)){
				m_rectsActive = false;
				return;			
			}
		}
		m_ui->handleClickUp(touches[i].getCurX(), touches[i].getCurY());
	}
	m_rectsActive = false;
}







/*
 
 AccelerometerMotionType
 PSInputHandler::getAccelerometerMotionType(const Vector3 &accel)
 {
 if(max(fabs(accel.x), fabs(accel.y)) < 0.2f) return MOTION_NONE;
 
 if(fabs(accel.x) > fabs(accel.y))
 return accel.x > 0.0f ? MOTION_TILT_FORWARDS : MOTION_TILT_BACKWARDS;
 
 return accel.y > 0.0f ? MOTION_TILT_LEFT : MOTION_TILT_RIGHT;
 }
 
 void
 PSInputHandler::phoneTilted(const Vector3& tilt)
 {
 if(!PSSettings::Settings().getUseAccelerometer())
 return;
 
 AccelerometerMotionType motionType = getAccelerometerMotionType(tilt);		
 
 if(m_ticksSinceLastMotion++ > 4)
 switch(motionType){
 case MOTION_TILT_LEFT:
 m_app->queueEvent(EVENT_GOTO_LEFTNEIGHBOR);
 m_ticksSinceLastMotion = 0;
 break;
 case MOTION_TILT_RIGHT:
 m_app->queueEvent(EVENT_GOTO_RIGHTNEIGHBOR);
 m_ticksSinceLastMotion = 0;
 break;
 case MOTION_TILT_FORWARDS:
 m_app->queueEvent(EVENT_GOTO_INNEIGHBOR);
 m_ticksSinceLastMotion = 0;
 break;
 case MOTION_TILT_BACKWARDS:
 m_app->queueEvent(EVENT_GOTO_OUTNEIGHBOR);
 m_ticksSinceLastMotion = 0;
 break;
 }
 
 }
 */