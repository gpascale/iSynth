/*
 *  PSCameraMan.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/21/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "PSCameraMan.h"
#include "SpringyCamera.h"
#include "GMath.h"
#include "GregJsonProjector.h"
#include "PSTypes.h"

PSCameraMan::PSCameraMan(SpringyCamera* camera) :
    m_camera(camera),
    m_spring(NULL)
{ 
    if(m_camera != NULL)
        m_spring = m_camera->getSpring();
}


PSCameraMan::~PSCameraMan()
{

}


void
PSCameraMan::cleanup()
{
	m_camera->setLens(1.0f, 1.0f, 0.001f, 10000.0f, 0.0f);	

	// manually set arcball radius to 0. This fixes a bug where exiting a synth while in orbit mode
	// would cause the camera to "snap" into place next time you entered orbit mode because the
	// start and end radii were both 10
	m_camera->setArcballRadius(0.0f);
	
	m_spring->setTarget(m_camera);
}


void
PSCameraMan::setCamera(SpringyCamera* camera)
{
    m_camera = camera;
    ASSERT(camera != NULL);
    m_spring = camera->getSpring();
}


void
PSCameraMan::goToProjector(const JsonProjector& projector, float duration)
{   
    m_spring->setEndPosition(projector.getPosition());
    m_spring->setEndRotation(projector.getCorrectedViewMatrix().toQuaternion());
    m_spring->setEndFocalLength(projector.getFocalLength());
    m_spring->setEndZNear(projector.getZNear());
    m_spring->setEndZFar(projector.getZFar());
    
    m_spring->start(duration, m_spring->getFlags() | SPRING_POSITION | SPRING_ROTATION | SPRING_LENS_ALL);    
}


void
PSCameraMan::enterArcballMode(float arcballRadius, float duration)
{
	// hack for now
	arcballRadius = 10.0f;
	
    // Move the camera onto the arcball	
	const Vector3 camLookVec = m_camera->getLookVector().normalized();
    const Point3  arcballPosition = m_camera->getArcballCenter() - (arcballRadius * camLookVec);
	
	const Vector3 look = -(m_camera->getArcballCenter() - arcballPosition).xyz().normalized();
    const Vector3 side = look.cross(Vector3::UnitZ()).normalized();
    
	Matrix targetViewMatrix;		
	Matrix::ViewMatrix(targetViewMatrix, Vector3::Zeros(), look, Vector3::UnitZ());    
    
	m_spring->setStartArcballCenter(m_camera->getPosition());
	m_spring->setEndArcballCenter(Point3(0.0f, 0.0f, 0.0f, 1.0f));
    m_spring->setEndArcballRadius(arcballRadius);
	m_spring->setEndRotation(targetViewMatrix.toQuaternion());
	m_spring->setEndPosition(arcballPosition);
	m_spring->setEndFocalLength(1.0f);

    m_spring->start(duration, m_spring->getFlags() | SPRING_ARCBALL_ALL | SPRING_FOCAL_LENGTH);    
}


void
PSCameraMan::leaveArcballMode(float duration)
{
    m_spring->setEndArcballCenter(Point3(0.0f, 0.0f, 0.0f, 1.0f));
    m_spring->setEndArcballRadius(0.0f);
    m_spring->start(duration, SPRING_ARCBALL_CENTER | SPRING_ARCBALL_RADIUS);
}


void
PSCameraMan::setScreenOrientation(int orientation, float duration)
{   
    m_spring->setEndScreenOrientation(orientation * PIOVER2);    
    m_spring->start(1.5f, m_spring->getFlags() | SPRING_SCREEN_ORIENTATION);
}


void
PSCameraMan::updateCamera()
{
    if(m_spring->getActive())
        m_spring->update();
}





