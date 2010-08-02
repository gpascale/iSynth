
#include "CameraSpring.h"
#include <SpringyCamera.h>
#include <GTime.h>
#include <PSButtonManager.h>

CameraSpring::CameraSpring() :
    m_target(NULL),
    m_duration(1.0f),    
    m_active(false),
	m_flags(0x00)
{	
	
}

CameraSpring::~CameraSpring()
{
}



void
CameraSpring::setTarget(SpringyCamera *c)
{
	ASSERT(c != NULL);
	m_target = c;
    m_startRotation = m_curRotation = m_endRotation = c->getQuaternion();
    m_startPosition = m_curPosition = m_endPosition = c->getPosition();	
    m_startAspectRatio = m_curAspectRatio = m_endAspectRatio = c->getAspectRatio();
	m_startFocalLength = m_curFocalLength = m_endFocalLength = c->getFocalLength();	
	m_startZNear = m_curZNear = m_endZNear = c->getZNear();
	m_startZFar = m_curZFar = m_endZFar = c->getZFar();
	m_startArcballCenter = m_curArcballCenter = m_endArcballCenter = c->getArcballCenter();
    m_startArcballRadius = m_curArcballRadius = m_endArcballRadius = c->getArcballRadius();
	m_startScreenOrientation = m_curScreenOrientation = m_endScreenOrientation = 0.0f;
}



void
CameraSpring::start(float duration, int flags)
{
    // if the camera 
        
    if(m_active)
        stop(false);

    
    // update duration and start time
    m_duration  = duration;
    m_flags     = flags;
	m_startTime = GTime::Now();
    
    // set the camera in motion
	m_active = true;	        
    
	m_parent->onCameraSpringStart();
}

	

void
CameraSpring::stop(bool didFinish)
{
    m_active = false;
    m_duration = 0.0f;	
	
    if(didFinish){
        m_startPosition			 = m_endPosition;
        m_startRotation			 = m_endRotation;
        m_startAspectRatio		 = m_endAspectRatio;
        m_startFocalLength		 = m_endFocalLength;
        m_startZNear			 = m_endZNear;
        m_startZFar				 = m_endZFar;
        m_startScreenOrientation = m_endScreenOrientation;
        m_startArcballCenter     = m_endArcballCenter;
        m_startArcballRadius     = m_endArcballRadius;
    }
    else{
        m_startPosition          = m_curPosition;
        m_startRotation          = m_curRotation;
        m_startAspectRatio       = m_curAspectRatio;
        m_startFocalLength       = m_curFocalLength;
        m_startZNear             = m_curZNear;
        m_startZFar              = m_curZFar;
        m_startScreenOrientation = m_curScreenOrientation;
        m_startArcballCenter     = m_curArcballCenter;
        m_startArcballRadius     = m_curArcballRadius;        
    }

	m_flags = 0;
	
    // tell the client the spring has stopped
    if(didFinish)
        m_parent->onCameraSpringStop();
}


void
CameraSpring::update(const GTime &now)
{   
    if(!m_active) return;

    // grab the normalized time
	const float t01Fast = getTime01Fast(now);
    
	// Now, update all attributes whose flags are set according to normalized time
    
    // First, handle camera position, orientation parameters
    
    // arcball center...
	if(m_flags & SPRING_ARCBALL_CENTER){			
        getCurArcballCenter(m_curArcballCenter, t01Fast);
        m_target->setArcballCenter(m_curArcballCenter);
	}
    if(m_flags & SPRING_ARCBALL_RADIUS){			
        getCurArcballRadius(m_curArcballRadius, t01Fast);
        m_target->setArcballRadius(m_curArcballRadius);
	}
	
    // if we're in arcball mode, then that will determine both rotation and position
	if(m_flags & SPRING_ARCBALL_ROTATION){		
        // we handle arcball rotation by first determining the angle
		getCurRotation(m_curRotation, t01Fast);		
		m_target->setRotation(m_curRotation, true);
        
        // and then placing the camera at the appropriate point on the sphere of radius 'm_arcballRadius'
		const Matrix m = m_curRotation.toMatrix();
		const Point3 pos(-(m.getColumnVector3(2).normalized() * m_curArcballRadius), 1.0f);
		m_curPosition = m_endPosition = m_curArcballCenter + pos;
		m_target->setPosition(m_curArcballCenter + pos);		
	}
	else{
        // otherwise, we handle position and rotation independently
		if(m_flags & SPRING_POSITION){
			m_curPosition = (m_startPosition + (t01Fast * (m_endPosition - m_startPosition)));
			m_target->setPosition(m_curPosition);
		}        
		if(m_flags & SPRING_ROTATION){
			getCurRotation(m_curRotation, t01Fast);
			m_target->setRotation(m_curRotation, true);
		}
	}

	
    // Second, handle camera lens parameters    
	
	if(m_flags & SPRING_FOCAL_LENGTH)
		getCurFocalLength(m_curFocalLength, t01Fast);
	if(m_flags & SPRING_ZNEAR)
		getCurZNear(m_curZNear, t01Fast);
	if(m_flags & SPRING_ZFAR)
		getCurZFar(m_curZFar, t01Fast);								
	if(m_flags & SPRING_SCREEN_ORIENTATION){
		getCurScreenOrientation(m_curScreenOrientation, t01Fast);
	}
    
	// only bother updating the lens if we've changed at least one lens parameter
	if(m_flags & (SPRING_FOCAL_LENGTH | SPRING_SCREEN_ORIENTATION | SPRING_ZNEAR | SPRING_ZFAR)){
		m_target->setLens(1.0f, m_curFocalLength, m_curZNear, m_curZFar, m_curScreenOrientation);
	}


    // finally, stop the timer if the spring has been going long enough
    if(t01Fast >= 1.0f)
        stop(true);
}

