/*
 *  GenericCamera.cpp
 *  Phonosynth
 *
 *  Created by Greg on 10/27/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "GenericCamera.h"
#include <Drawer.h>
#include <System.h>

void
GenericCamera::setDrawer(Drawer* d)
{
    ASSERT(d != NULL);
    m_drawer = d;
}


void
GenericCamera::setLens(const float aspectRatio,
                       const float focalLength,
                       const float zNear,
                       const float zFar,
                       const float screenOrientation)
{
	
	const float viewW = System::getScreenAspect();
	const float viewH = 1.0f;
	
	const float amt = 1 - (fabs(fmod(screenOrientation, (const float) PI) - PIOVER2) / PIOVER2);
	const float xFix = viewH + (amt * (viewW - viewH));
	const float yFix = viewW - (amt * (viewW - viewH));
    	
	Matrix scaleMat;
    
	Matrix::ScaleMatrix(scaleMat, xFix, -yFix, 1.0f);
	
	
	m_aspectRatio = aspectRatio;
	m_focalLength = focalLength;
	m_zNear = zNear;
	m_zFar = zFar;
	float fovy = FOVY(aspectRatio, focalLength);
	
	//m_mutex.lock();
	Matrix::ProjectionMatrixPerspRH(m_projectionMatrix, fovy, aspectRatio, zNear, zFar);		
	m_projectionMatrix *= scaleMat;
	//m_mutex.unlock();
	
	if(m_drawer)
		m_drawer->setScreenRotation(radToDeg(0.0f));
    
}


/*
void
GenericCamera::adjustViewport(float screenOrientation) //0 = portrait, PI/2 = landscape
{
	const float orientation01 = (1.0f / (0.5f * PI)) * screenOrientation;
	
	static const float screenWidth = 320.0f;
	static const float screenHeight = 480.0f;
	static const float diff = 160.0f;
	
	const float width  = screenWidth + orientation01 * diff;
	const float height = screenHeight - orientation01 * diff;
	
	if(m_drawer)
		m_drawer->setViewport(0, 0, width, height);	
}

*/