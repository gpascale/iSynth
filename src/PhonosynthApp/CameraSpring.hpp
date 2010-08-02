/*
 *  CameraSpring.hpp
 *  Phonosynth
 *
 *  Created by Greg on 10/26/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */


// returns the current spring time normalized to a 0-1 scale (faster than linear)
inline float
CameraSpring::getTime01Fast(const GTime &now) const
{
	// could be optimized
    const float tf = min(max(now - m_startTime, 0.0f), m_duration);	
    return 0.5f * (sin((pow(tf / m_duration, 0.38f) - 0.5) * PI) + 1.0f);
}

// returns the current spring time normalized to a 0-1 scale (slower than linear)
inline float
CameraSpring::getTime01Slow(const GTime &now) const
{
	// could be optimized
    const float tf = min(max(now - m_startTime, 0.0f), m_duration);	
    return 0.5f * (sin((pow(tf / m_duration, 1.8f) - 0.5) * PI) + 1.0f);
}		



inline void
CameraSpring::getCurRotation(OUT Quaternion &ret, float t01) const
{ 
    Quaternion::SlerpQuaternion(t01, m_startRotation, m_endRotation, ret, 0);
}


inline void
CameraSpring::getCurScreenOrientation(OUT float& ret, float t01) const
{
    ret = m_startScreenOrientation + t01 * (m_endScreenOrientation - m_startScreenOrientation);
}


inline void
CameraSpring::getCurPosition(OUT Point3 &ret, float t01) const
{ 
    ret = m_startPosition + t01 * (m_endPosition - m_startPosition);	
}


inline void
CameraSpring::getCurArcballCenter(OUT Point3 &ret, float t01) const
{
    ret = m_startArcballCenter + t01 * (m_endArcballCenter - m_startArcballCenter);
}


inline void
CameraSpring::getCurArcballRadius(OUT float &ret, float t01) const
{
    ret = m_startArcballRadius + t01 * (m_endArcballRadius - m_startArcballRadius);
}


inline void
CameraSpring::getCurFocalLength(OUT float &ret, float t01) const
{ 
    ret = m_startFocalLength + t01 * (m_endFocalLength - m_startFocalLength);
}


inline void
CameraSpring::getCurZNear(OUT float &ret, float t01) const
{ 
    ret = m_startZNear + t01 * (m_endZNear - m_startZNear);
}


inline void 
CameraSpring::getCurZFar(OUT float &ret, float t01) const
{ 
    ret = m_startZFar + t01 * (m_endZFar - m_startZFar);
}	


inline void
CameraSpring::update()
{
    update(GTime::Now());
}
