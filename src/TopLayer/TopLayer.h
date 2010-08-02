#ifndef _TOPLAYER_H_
#define _TOPLAYER_H_

#include <string>

class SceneGraph;
class GenericCamera;
class Drawer;
class PSMaster;
class InputHandler;
class TextureManager;
class UI;
class PSUIManager;
class PSSGManager;
class PSNetwork;

using namespace std;

class TopLayer
{

public:
	
                        TopLayer();
    virtual             ~TopLayer();

    virtual void		run(){ }
    virtual void		update(){ }	// redraw the scene	

    virtual void        showOptionsMenu()    = 0;
	virtual void		showHelpScreen()     = 0;
    virtual void        endViewingSynth()    = 0;
	virtual void		enterLowPowerState() = 0;
	virtual void		leaveLowPowerState() = 0;
	virtual void		setAutoLock(bool b)	 = 0;
	
	// to display to user what is going on
    virtual void        screenRotated(int newRotation) = 0;
	virtual void		updateLoadStatus(const string& statusMsg) = 0;	
    
    
	virtual void		cleanup() = 0;
	
    //TODO: remove this super hack
    
    Drawer* getDrawer() { return m_drawer; }
	
protected:

	bool				  m_needsUpdate;
	Drawer*               m_drawer;
    GenericCamera*        m_camera;
	SceneGraph*           m_scenegraph;
	PSMaster*             m_app;
    InputHandler*         m_inputHandler;
    TextureManager*       m_textureManager;
    PSSGManager*          m_scenegraphManager;
    PSUIManager*          m_uiManager;
    UI*                   m_ui;
    PSNetwork*            m_network;	
    
};

#endif
