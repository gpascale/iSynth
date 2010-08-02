/*
 *  RenderThread.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/22/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "RenderThread.h"
#include <PSSGManager.h>
#include <PSUIManager.h>
#include <PSCameraMan.h>
#include <PSGlobalState.h>
#include <ContextManager.h>
#include <GenericCamera.h>
#import  <UIKit/UIKit.h>

#include <Functions.h>


RenderThread::RenderThread(PSSGManager* sgm, PSUIManager* uimgr, Drawer* d, 
                           GenericCamera* camera, PSCameraMan* cameraMan) :
    GThread(),
    m_scenegraphManager(sgm),
    m_uiManager(uimgr),
    m_drawer(d),
    m_camera(camera),
    m_cameraMan(cameraMan),
	m_renderSceneState(RS_NORMAL),
	m_useOffscreenBuffer(false),
	m_paused(false)
{

}


RenderThread::~RenderThread()
{
	
}


void
RenderThread::setUseOffscreenBuffer(bool b)
{
	if(b)
		m_makeOffscreenBuffer = b;			
	else{
		m_makeOffscreenBuffer = m_useOffscreenBuffer = false;		
		m_scenegraphManager->showPointcloud();
		m_scenegraphManager->invalidateScenegraph();
	}
}


void
RenderThread::run()
{
	[NSThread setThreadPriority: 1.0];
	
    ContextManager::activateRenderContext();		
	
	m_drawer->setMultitextureType(MULTITEXTURE_INTERPOLATE);
	
	// No idea why, but this makes an INCREDIBLE difference in the framerate (for the better)
	
	/*
	MemberFunctor<Drawer> f(m_drawer, &Drawer::regenBuffers);
	GThread::performFunctorOnMainThread(f);
	*/
	 
	m_drawer->regenBuffers();
	
    while(1){
		
		ContextManager::activateRenderContext();

		// first, check to see if the render thread has been cancelled. If so, stop rendering immediately
		if(isCancelled())
			break;

		if(m_paused){			
			sleep(1.0f);			
			continue;
		}
		
		NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		

		if(m_makeOffscreenBuffer){
			
			m_useOffscreenBuffer = true;
			m_makeOffscreenBuffer = false;
			
			// make only the pointcloud visible
			bool qVis  = m_scenegraphManager->hideQuads();
			bool ccVis = m_scenegraphManager->hideCameraCones();				
			bool qrVis = m_scenegraphManager->hideQRects();
						 m_scenegraphManager->showPointcloud();
			
			m_scenegraphManager->invalidateScenegraph();
			
			// turn the density up to full for this one drawing
			const float oldDensity = PSGlobalState::getPointcloudDensity();
			PSGlobalState::setPointcloudDensity(1.0f);
			
			// draw to the offscreen framebuffer
			m_drawer->beginOffscreen(Color4F::Black());
			m_scenegraphManager->render(*m_drawer);
			
			// reset the scenegraph
			if(qVis)
				m_scenegraphManager->showQuads();
			if(ccVis)
				m_scenegraphManager->showCameraCones();
			if(qrVis)
				m_scenegraphManager->showQRects();
			m_scenegraphManager->hidePointcloud();
			m_scenegraphManager->invalidateScenegraph();
			
			// turn the pointcloud density back down
			PSGlobalState::setPointcloudDensity(oldDensity);
							
		}			
		
		if(m_useOffscreenBuffer){
			m_drawer->beginOnscreen();
			m_drawer->renderOffscreenBuffer();
		}
		else
			m_drawer->beginOnscreen(Color4F::Black());
		
		m_cameraMan->updateCamera();
		m_scenegraphManager->render(*m_drawer);				
		m_uiManager->render(*m_drawer);
		m_drawer->end();
			
		
		const float diff = GTime::Now() - m_lastRender;
		if(diff < 0.033f)
			sleep(0.033f - diff);
		m_lastRender.update();

		[pool release];
    }	
	
	//m_drawer->destroyBuffers();
	
	ContextManager::releaseCurrentContext();
}


void
RenderThread::pause()
{
	m_paused = true;
}


void
RenderThread::resume()
{
    m_paused = false;  
}


void
RenderThread::stopRendering()
{	
	m_renderSceneState = RS_ONCE;
}


void
RenderThread::startRendering()
{
	PSGlobalState::setPointcloudDensity(0.1f);
	m_renderSceneState = RS_NORMAL;
}


