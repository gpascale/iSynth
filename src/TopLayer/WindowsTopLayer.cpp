#ifdef WINDOWS

#include "WindowsTopLayer.h"
#include <Engine.h>
#include <D3DApp.h>
#include <D3DDrawer.h>
#include <assert.h>
#include <iostream>
#include <PSMaster.h>

using namespace std;

WindowsTopLayer::WindowsTopLayer()
{
	m_drawer = new D3DDrawer();
	m_scene->setDrawer(m_drawer);
}

WindowsTopLayer::~WindowsTopLayer()
{
	// TODO: m_maxConcurrentConnection resources here once #define issues are resolved
}

long appThreadRoutine(void *v)
{
    ((PSMaster*) v)->run();
    return 0L;
}

void
WindowsTopLayer::run()
{
	m_d3dApp = new D3DApp();
	m_framework = new CFramework(m_d3dApp);
	m_d3dApp->setFramework(m_framework);		
	if (!m_d3dApp->initialize()){
		//cout << "Could Not Initialize D3DApp" << endl;
		exit(-1);
	}
	if (!m_framework->initialize("Phonosynth!", NULL, 1024, 768)){
		//cout << "Could Not Initialize Framework" << endl;
		exit(-1);
	}			

	m_d3dApp->setTopLayer( this );
    m_d3dApp->setInputHandler( m_inputHandler );
	
	//this is hacky, but oh well    
	((D3DDrawer*) m_drawer)->setD3DDevice( m_framework->getDevice() );					

    /* DO NOT DO THIS, just keeping the thread code for reference     
    HANDLE appThread = 
    CreateThread(NULL,
				 0,
				 (LPTHREAD_START_ROUTINE) &appThreadRoutine, 
				 (void*) m_app, 
				 0, 
				 NULL);
    */

    m_framework->run();

}

void
WindowsTopLayer::update()
{
    m_app->update();
}

#endif