#include "TopLayer.h"
#include <Engine.h>
#include <PSMaster.h>
#include <iostream>
#include <PSInputHandler.h>
#include <UI.h>
#include <PSUIManager.h>
#include <TextureManager.h>
#include <PSSGManager.h>
#include <PSNetwork.h>

TopLayer::TopLayer()
{
	// this is where most of the program initialization takes place,
	// a few more specific initializations happen in either
	// WindowsTopLayer or MacTopLayer (e.g. setting up the 3D API)
	
	// first, create a scene
    m_scenegraph = new SceneGraph();

    // create a TextureManager
    m_scenegraphManager = new PSSGManager(m_scenegraph);
    m_textureManager = new TextureManager(m_scenegraphManager);    
    m_scenegraphManager->setTextureManager(m_textureManager);    
	
    // initialize the networkiage
    m_network = new PSNetwork(m_scenegraphManager, m_textureManager);
    
    m_app = new PSMaster();	
	
    m_ui = new UI();
    m_uiManager = new PSUIManager(m_ui);
	
    m_app->init(m_scenegraphManager, m_uiManager, m_textureManager, m_network, this);	
    m_uiManager->initUI(m_app);
	
	
    // create a UI and a PSUIManager. Use the UI manager to initialize the UI        
        
    
	// tell the input handler who to delegate to (the app)
    m_inputHandler = new PSInputHandler();
    m_inputHandler->setMasterApp(m_app);
    m_inputHandler->setUI(m_ui);
    m_inputHandler->setUIManager(m_uiManager);

}


TopLayer::~TopLayer()
{
	SAFE_DELETE(m_app);
	//SAFE_DELETE(m_scene);
}
