/*
 *  SpringyCamera.h
 *  Phonosynth
 *
 *  Created by Greg on 10/27/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _SPRINGYCAMERA_H_
#define _SPRINGYCAMERA_H_

#include "GenericCamera.h"
#include "CameraSpring.h"

class SpringyCamera : public GenericCamera
{
    
public:
    
    SpringyCamera();
    ~SpringyCamera();
    
    void                setSpring(const CameraSpring* spring){ m_spring = spring; }
    CameraSpring*       getSpring() { return m_spring; }
    
    void                rotateLeft(float amt);
	void                rotateRight(float amt);
	void                rotateUp(float amt);
	void                rotateDown(float amt);
	void                zoomIn(float amt);
	void                zoomOut(float amt);
    
    void                rotateLeftArcball(float amt);
    void                rotateRightArcball(float amt);
    void                rotateUpArcball(float amt);
    void                rotateDownArcball(float amt);
    
    void                moveForwardArcball(float amt);
    void                moveBackwardArcball(float amt);
    void                moveLeftArcball(float amt);
    void                moveRightArcball(float amt);
    void                moveUpArcball(float amt);
    void                moveDownArcball(float amt);
    
protected:    
    
    void                doSpringyArcballRotate(const Matrix& newRotation);
    void                doSpringyRotate(const Matrix &newOrientation);
    void                doSpringyZoom(float newFocalLength);
    void                doSpringyArcballMove(const Point3& newArcballCenter,
                                             float newArcballRadius);
    
    CameraSpring*       m_spring;
};

#include "SpringyCamera.hpp"

#endif // _SPRINGYCAMERA_H_