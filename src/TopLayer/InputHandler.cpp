#include <PSInputHandler.h>
#include <PSMaster.h>
#include <Button.h>
#include <PSButtonManager.h>
#include <PSSettings.h>
#include <PSGlobalState.h>
#include "ButtonLayout.h"
#include "ButtonGroup.h"
#include <UI.h>
#include <PSUIManager.h>
#include <PSTypes.h>
#include <AccelerometerDelegate.h>

//#include <windows.h>

  
using namespace std;




#ifdef IPHONE

#include <ObjCCommon.h>

InputHandler::InputHandler(UIView* view) :
	m_view(view),
    m_numFingersDown(0),
	m_activeButton(NULL)
{	
    m_compositeDrag = make_pair(0, 0);
	m_accelerometerDelegate = [[AccelerometerDelegate alloc] initWithInputHandler: this];
	[[UIAccelerometer sharedAccelerometer] setDelegate: m_accelerometerDelegate];
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval: 1.0];
    m_app = NULL;
}


int
InputHandler::numFingersDown() const
{
	return 0;
}


void
InputHandler::touchesBegan(NSSet* touches, UIEvent* event, UIView* view)
{				
	if(view != m_view)
		return;
	
	m_numFingersDown += [touches count];
	
	if(!PSGlobalState::getUIActive()){
		m_app->fadeInUI();
		return;
	}
	else{
		m_app->resetInactivityTimer();
	}
	
	m_clickDownTime.update();
	
	UITouch* touch = [touches anyObject];
	CGPoint loc = [touch transformedLocationInView: view];		
	
	if(m_numFingersDown == 1){
		m_activeButton = m_ui->handleClickDown(loc.x, loc.y);
		if(m_activeButton)
			return;
	}
	else
		m_activeButton = NULL;

	onTouchesBegan(touches, event, view);
}


void
InputHandler::touchesMoved(NSSet* touches, UIEvent* event, UIView* view)
{		
	if(!PSGlobalState::getUIActive()){
		m_app->fadeInUI();
		return;
	}
	else{
		m_app->resetInactivityTimer();
	}
	
	// check to see if a touch was moved out from inside a button and handle it if so
	if(m_activeButton && [touches count] == 1){
		UITouch* touch = [touches anyObject];
		CGPoint loc = [touch transformedLocationInView: view];
		if(!m_activeButton->clickHitTest(loc.x, loc.y)){
			m_activeButton->respondToClickLeave();
			m_activeButton = NULL;
		}
	}
	
	// give onTouchesMoved a chance to do something more specific
	onTouchesMoved(touches, event, view);
}


void
InputHandler::touchesEnded(NSSet* touches, UIEvent* event, UIView* view)
{	
	
	if(m_activeButton){
		m_activeButton->respondToClickUp();
		//cerr << GTime::Now() - m_clickDownTime << endl;
		if(GTime::Now() - m_clickDownTime < 0.5f)
			m_activeButton->respondToPress();
		
		m_numFingersDown -= [touches count];
		return;
	}
		
	// keep track of the number of fingers down
	m_numFingersDown -= [touches count];
	// give onTouchesBegan a chance to do something more specific
	onTouchesEnded(touches, event, view);
}


// important to include this so that m_numFingersDown stays correct
void
InputHandler::touchesCancelled(NSSet* touches, UIEvent* event, UIView* view)
{	
	m_activeButton = NULL;
	m_numFingersDown -= [touches count];
}


void
InputHandler::readAccelerometer(UIAcceleration* accel)
{
	onReadAccelerometer(accel);
}


void
InputHandler::setAccelerometerReadFrequency(int readsPerSec)
{
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval: 1.0 / readsPerSec];
}

#endif






#ifdef WINDOWS

InputHandler::InputHandler() :
	m_lastX(-1),
	m_lastY(-1),
	m_numFingersDown(0),
	m_activeButton(NULL)
{	
    
    m_app = NULL;
    memset(m_keyMask, 0x00, sizeof(m_keyMask));
    m_mouseMask[MOUSE_BUTTON_LEFT] = false;
    m_mouseMask[MOUSE_BUTTON_RIGHT] = false;			    
	
}


void
InputHandler::keyUp(int key)
{
    m_keyMask[key] = false;
}



void
InputHandler::mouseDown(MouseButton button, int x, int y)
{
    m_mouseMask[button] = true;
    if(button == MOUSE_BUTTON_LEFT){
        m_lastX = x;
        m_lastY = y;
    }
}



void
InputHandler::mouseUp(MouseButton button, int x, int y)
{
    m_mouseMask[button] = false;
    if(button == MOUSE_BUTTON_LEFT){
        m_lastX = -1;
        m_lastY = -1;
    }
}



void
InputHandler::mouseMove(int x, int y)
{
    if(!m_mouseMask[MOUSE_BUTTON_LEFT]) return;
    
    if(m_lastX == -1){
        m_lastX = x;
        m_lastY = y;
        return;
    }
    
    const static float speed = 0.004f;
    
    if(x > m_lastX) m_app->queueEvent(EVENT_ROTATE_RIGHT);
    else if(m_lastX > x) m_app->queueEvent(EVENT_ROTATE_LEFT);
    if(y > m_lastY) m_app->queueEvent(EVENT_ROTATE_DOWN);
    else if(m_lastY > y) m_app->queueEvent(EVENT_ROTATE_UP);
    
    m_lastX = x;
    m_lastY = y;
}


void
InputHandler::keyDown(int key)
{    
    m_keyMask[key] = true;
    
    ASSERT(m_app);
    
    //cout << "Input handler detected that " << (char) key << " was pressed" << endl;
    
    switch(key){
        case 'W':
            m_app->queueEvent(EVENT_MOVE_FORWARD);
            break;
        case 'S':
            m_app->queueEvent(EVENT_MOVE_BACKWARD);
            break;
        case 'A':
            m_app->queueEvent(EVENT_MOVE_LEFT);
            break;
        case 'D':
            m_app->queueEvent(EVENT_MOVE_RIGHT);
            break;
        case 'Q':
            m_app->queueEvent(EVENT_MOVE_UP);
            break;
        case 'E':
            m_app->queueEvent(EVENT_MOVE_DOWN);
            break;
        case '2':
            //m_app->enter2DMode(1000);
            break;
        case '3':
            //m_app->enter3DMode(1000);
            break;
        case '4':
            //m_app->enter3DMode(200, false);
            break;
        case 'R':
            //m_app->toggleAutoRotate();
            break;
		case 'G':
			//m_app->doAnimation();
		    break;       
			/*
             case VK_LEFT:
             m_app->queueEvent(EVENT_GOTO_LEFTNEIGHBOR);
             break;
             case VK_RIGHT:
             m_app->queueEvent(EVENT_GOTO_RIGHTNEIGHBOR);
             break;
             case VK_UP:
             m_app->queueEvent(EVENT_GOTO_UPNEIGHBOR);
             break;
             case VK_DOWN:
             m_app->queueEvent(EVENT_GOTO_DOWNNEIGHBOR);
             break;
			 */
        case 'N':
            //m_app->toggleCurrentCoordSystem();
            break;
			/*
             case 'T':
             m_app->takeTour();
             break;
			 */
        case '7':
            m_app->queueEvent(EVENT_GOTO_RANDOMNEIGHBOR);
            break;
        case 'C':
            m_app->printCameraInfo();
            break;
    }
}

#endif
