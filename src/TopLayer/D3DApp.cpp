#ifdef WINDOWS

#include "D3DApp.h"
#include <TopLayer.h>
#include <iostream>
#include <PSInputHandler.h>


using namespace std;

D3DApp::D3DApp()
{

}

D3DApp::~D3DApp()
{

}


BOOL
D3DApp::initialize()
{			
	return TRUE;
}



void
D3DApp::onResetDevice(LPDIRECT3DDEVICE9 device)
{    

    device->SetRenderState(D3DRS_ZENABLE,  FALSE); // Z-Buffer (Depth Buffer)
    device->SetRenderState(D3DRS_CULLMODE, FALSE); // Disable Backface Culling
    device->SetRenderState(D3DRS_LIGHTING, FALSE); // Disable Light

    
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    
    device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);          
    device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    
    device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
    device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, 
                                    D3DTTFF_PROJECTED | D3DTTFF_COUNT4);
    
}


void
D3DApp::onRenderFrame(LPDIRECT3DDEVICE9 device)
{	
	m_topLayer->update();
}


void
D3DApp::onKeyDown(KeyEvent e)
{
    m_inputHandler->keyDown(e.getKey());
}



void
D3DApp::onKeyUp(KeyEvent e)
{
    m_inputHandler->keyUp(e.getKey());
}



void
D3DApp::onMouseDown(MouseEvent e)
{
    if(e.getClickInfo() & MOUSE_LEFT)
        m_inputHandler->mouseDown(MOUSE_BUTTON_LEFT, e.getX(), e.getY());
    else
        m_inputHandler->mouseDown(MOUSE_BUTTON_RIGHT, e.getX(), e.getY());    
}



void
D3DApp::onMouseUp(MouseEvent e)
{    
    if(e.getClickInfo() & MOUSE_LEFT)
        m_inputHandler->mouseUp(MOUSE_BUTTON_LEFT, e.getX(), e.getY());
    else
        m_inputHandler->mouseUp(MOUSE_BUTTON_RIGHT, e.getX(), e.getY());
}



void
D3DApp::onMouseMove(MouseEvent e)
{    
    m_inputHandler->mouseMove(e.getX(), e.getY());
}

#endif