/*
 *  RenderThread.h
 *  Phonosynth
 *
 *  Created by Greg on 12/22/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _RENDERTHREAD_H_
#define _RENDERTHREAD_H_

#include <GThread.h>
#include <GMutex.h>
#include <GTime.h>

class PSSGManager;
class PSUIManager;
class Drawer;
class GenericCamera;
class PSCameraMan;

class RenderThread : public GThread
{

public:
    
    RenderThread(PSSGManager* sgm, PSUIManager* uimgr, Drawer* d, 
                 GenericCamera* camera, PSCameraMan* cameraMan);
    ~RenderThread();

    void run();
    void pause();
    void resume();

	void startRendering();
	void stopRendering();
	
	void setUseOffscreenBuffer(bool b);
	
private:
    	
	enum RenderSceneState
	{
		RS_NORMAL,
		RS_ONCE,
		RS_DONT
	};
	
	RenderSceneState  m_renderSceneState;
	
    GMutex            m_mutex;    
    PSSGManager*      m_scenegraphManager;
    PSUIManager*      m_uiManager;
    Drawer*           m_drawer;
    GenericCamera*    m_camera;
    PSCameraMan*      m_cameraMan;
	
    bool			  m_paused;
	bool			  m_renderedFullPointcloud;	
	bool			  m_makeOffscreenBuffer;
	bool			  m_useOffscreenBuffer;

	GTime			  m_lastRender;
	
};

#endif _RENDERTHREAD_H_