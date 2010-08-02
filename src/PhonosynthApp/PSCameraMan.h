/*
 *  PSCameraMan.h
 *  Phonosynth
 *
 *  Created by Greg on 12/21/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _PSCAMERAMAN_H_
#define _PSCAMERAMAN_H_

// PSCameraMan is a relatively simple class designed to abstract away the finer details
// of moving the camera around the world

class SpringyCamera;
class CameraSpring;
class JsonProjector;

class PSCameraMan
{
public:
    
    // constructors, destructors and such
    PSCameraMan(SpringyCamera* camera);    
    ~PSCameraMan();
    
    void cleanup();
	
    void setCamera(SpringyCamera* camera);        
    
    
    // camera manipulation functions
    void goToProjector(const JsonProjector& projector, float duration = 1.5f);    
    void enterArcballMode(float arcballRadius = 10.0f, float duration = 3.0f);
    void leaveArcballMode(float duration = 3.0f);
    
    void setScreenOrientation(int orientation, float duration = 1.5f);
    
    void updateCamera();
    
private:
    
    SpringyCamera* m_camera;
    CameraSpring*  m_spring;

};



#endif // _PSCAMERAMAN_H_