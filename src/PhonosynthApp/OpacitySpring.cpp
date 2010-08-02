

#include "OpacitySpring.h"
#include <GTime.h>
#include <Drawer.h>
#include <Functions.h>

OpacitySpring::OpacitySpring() : 
    m_target(NULL),
    m_fromOpacity(0.0f),
    m_currentOpacity(0.0f),
    m_toOpacity(0.0f),
    m_duration(0.0f),
    m_active(false),
	m_startTime(),
	m_endFunctor(NULL)
{
}




OpacitySpring::OpacitySpring(AdjustableOpacity *e) :
    m_target(e),
    m_fromOpacity(0.0f),
    m_currentOpacity(0.0f),
    m_toOpacity(0.0f),
    m_duration(0.0f),   
    m_active(false),
	m_startTime(),
	m_endFunctor(NULL)
{

}

OpacitySpring::~OpacitySpring()
{
	SAFE_DELETE(m_endFunctor);
}


void
OpacitySpring::setFromOpacity(float opacity)
{
	m_mutex.lock();
    m_fromOpacity = m_currentOpacity = opacity;
	m_mutex.unlock();
}


void
OpacitySpring::setToOpacity(float opacity, float duration)
{
	m_mutex.lock();
	
    ASSERT(opacity <= 1.0f && opacity >= 0.0f);
	if(duration == 0.0f)
		duration = 0.001f;
        
    m_fromOpacity = m_currentOpacity; // update the "from" opacity to the current opacity so we don't see a jump
    m_toOpacity   = opacity;          // update the "to" opacity so the spring will actually cause a change
    m_duration    = duration;         // set the spring's duration
    
    m_startTime   = GTime::Now();     // mark the time at which we started the spring
    m_active      = true;             // setting m_active to true, causes the spring to be "released from rest" if you will
	
	m_mutex.unlock();
}


void
OpacitySpring::update()
{
    const GTime& now = GTime::Now();
    update(now);
}


void
OpacitySpring::update(const GTime& now)
{    
	
	// try to obtain the lock. If we can't, just don't update. Since this
	// happens from the render thread, we don't want to wait for the lock.
	// TODO: Whether or not to wait should be a parameter
	
	if(m_active && m_target && m_mutex.trylock()){

		// compute time since the spring was activated
		const float tf = min(max(now - m_startTime, 0.0f), m_duration);
	
		// normalize time to the range [0, 1]
		float t01;
		if(m_fromOpacity > m_toOpacity)
			t01 = sqrt(tf / m_duration); 
		else
			t01 = SQR(tf / m_duration);
	
		// current opacity is simply a weighted average of to and from opacities 
		// using normalized time value as the weight
		m_currentOpacity = t01 * (m_toOpacity - m_fromOpacity) + m_fromOpacity;
    
		// apply opacity to the target				
		m_target->setOpacity(m_currentOpacity);

		// if the spring has run to or past its scheduled duration, stop it
		if(tf >= m_duration){
			m_active = false;
			if(m_endFunctor)
				m_endFunctor->call();
		}
		
		m_mutex.unlock();
	}
}


void
OpacitySpring::setEndFunctor(const Functor& f)
{
	m_mutex.lock();
	SAFE_DELETE(m_endFunctor);
	m_endFunctor = f.copy();
	m_mutex.unlock();
}



