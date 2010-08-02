/*
 *  SpringyCamera.cpp
 *  Phonosynth
 *
 *  Created by Greg on 10/27/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

inline
SpringyCamera::SpringyCamera() :
    m_spring(NULL)
{
}


inline
SpringyCamera::~SpringyCamera()
{
    SAFE_DELETE(m_spring);
}   


inline void
SpringyCamera::rotateLeft(float amt)
{
	Matrix oldOrientation = m_spring->getEndRotation().toMatrix();
	Matrix newOrientation;
    GenericCamera::rotateLeft(newOrientation, oldOrientation, amt, m_position);
	doSpringyRotate(newOrientation);
}

inline void
SpringyCamera::rotateRight(float amt)
{
	Matrix oldOrientation = m_spring->getEndRotation().toMatrix();
	Matrix newOrientation;
	GenericCamera::rotateRight(newOrientation, oldOrientation, amt, m_position);
	doSpringyRotate(newOrientation);
}

inline void
SpringyCamera::rotateUp(float amt)
{
	Matrix oldOrientation = m_spring->getEndRotation().toMatrix();
	Matrix newOrientation;
	GenericCamera::rotateUp(newOrientation, oldOrientation, amt, m_position);
	doSpringyRotate(newOrientation);
}

inline void
SpringyCamera::rotateDown(float amt)
{
	Matrix oldOrientation = m_spring->getEndRotation().toMatrix();
	Matrix newOrientation;
	GenericCamera::rotateDown(newOrientation, oldOrientation, amt, m_position);
	doSpringyRotate(newOrientation);
}


inline void
SpringyCamera::rotateLeftArcball(float amt)
{
	const Matrix oldRotation = m_spring->getEndRotation().toMatrix();
	Matrix newRotation;
    GenericCamera::rotateRight(newRotation, oldRotation, amt);
	doSpringyArcballRotate(newRotation);
}


inline void
SpringyCamera::rotateRightArcball(float amt)
{
	const Matrix oldRotation = m_spring->getEndRotation().toMatrix();
	Matrix newRotation;
	GenericCamera::rotateLeft(newRotation, oldRotation, amt);
	doSpringyArcballRotate(newRotation);
}


inline void
SpringyCamera::rotateUpArcball(float amt)
{
    const Vector3 look = getLookVector();
    if(look.z < -0.95f)
        return;
	const Matrix oldRotation = m_spring->getEndRotation().toMatrix();
	Matrix newRotation;
	GenericCamera::rotateDown(newRotation, oldRotation, amt);
	doSpringyArcballRotate(newRotation);
}


inline void
SpringyCamera::rotateDownArcball(float amt)
{
    const Vector3 look = getLookVector();
    if(look.z > 0.95f)
        return;
	const Matrix oldRotation = m_spring->getEndRotation().toMatrix();
	Matrix newRotation;
	GenericCamera::rotateUp(newRotation, oldRotation, amt);
	doSpringyArcballRotate(newRotation);
}


inline void
SpringyCamera::moveForwardArcball(float amt)
{
    const float oldArcballRadius = m_spring->getEndArcballRadius();
    const float newArcballRadius = max(oldArcballRadius - (5 * amt), 1.0f);
    doSpringyArcballMove(m_spring->getEndArcballCenter(), newArcballRadius);
}


inline void
SpringyCamera::moveBackwardArcball(float amt)
{
    const float oldArcballRadius = m_spring->getEndArcballRadius();
    const float newArcballRadius = min(oldArcballRadius + (5 * amt), 50.0f);
    doSpringyArcballMove(m_spring->getEndArcballCenter(), newArcballRadius);
}


inline void
SpringyCamera::moveLeftArcball(float amt)
{
    const Point3& oldArcballCenter = m_spring->getEndArcballCenter();
    const Point3& newArcballCenter = oldArcballCenter + ((5 * amt) * getSideVector());
    doSpringyArcballMove(newArcballCenter, m_spring->getEndArcballRadius());
}


inline void
SpringyCamera::moveRightArcball(float amt)
{
    const Point3& oldArcballCenter = m_spring->getEndArcballCenter();
    const Point3& newArcballCenter = oldArcballCenter - ((5 * amt) * getSideVector());
    doSpringyArcballMove(newArcballCenter, m_spring->getEndArcballRadius());    
}


inline void
SpringyCamera::moveUpArcball(float amt)
{
    const Point3& oldArcballCenter = m_spring->getEndArcballCenter();
    const Point3& newArcballCenter = oldArcballCenter + ((5 * amt) * getUpVector());
    doSpringyArcballMove(newArcballCenter, m_spring->getEndArcballRadius());    
}


inline void
SpringyCamera::moveDownArcball(float amt)
{
    const Point3& oldArcballCenter = m_spring->getEndArcballCenter();
    const Point3& newArcballCenter = oldArcballCenter - ((5 * amt) * getUpVector());
    doSpringyArcballMove(newArcballCenter, m_spring->getEndArcballRadius());
}


inline void
SpringyCamera::zoomIn(float amt)
{
	const float &efl = m_spring->getEndFocalLength();
	if(efl <= 1000.0f)			
		doSpringyZoom(efl * (1.0f + amt));
}

inline void
SpringyCamera::zoomOut(float amt)
{
	const float &efl = m_spring->getEndFocalLength();
	if(efl >= 0.001f)			
		doSpringyZoom(efl * (1.0f - amt));	
}


inline void
SpringyCamera::doSpringyRotate(const Matrix &newOrientation)
{    
    m_spring->setEndRotation(newOrientation.toQuaternion());
    m_spring->start(1.5f, m_spring->getFlags() | SPRING_ROTATION);
}


inline void
SpringyCamera::doSpringyArcballRotate(const Matrix& newRotation)
{
    m_spring->setEndRotation(newRotation.toQuaternion());
    m_spring->start(1.5f, m_spring->getFlags() | SPRING_ARCBALL_ROTATION);
}


inline void
SpringyCamera::doSpringyZoom(float newFocalLength)
{
    m_spring->setEndFocalLength(newFocalLength);
    m_spring->start(1.5f, m_spring->getFlags() | SPRING_FOCAL_LENGTH);
}


inline void
SpringyCamera::doSpringyArcballMove(const Point3&  newArcballCenter,
                                    float newArcballRadius)
{
    m_spring->setEndArcballRadius(newArcballRadius);
    m_spring->setEndArcballCenter(newArcballCenter);
    m_spring->start(1.5f, m_spring->getFlags() | SPRING_ARCBALL_CENTER | SPRING_ARCBALL_RADIUS | SPRING_ARCBALL_ROTATION);
}