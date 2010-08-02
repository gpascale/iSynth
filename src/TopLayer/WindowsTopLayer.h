#pragma once

#include "TopLayer.h"
#include <CFramework.h>

class D3DApp;
class Drawer;

class WindowsTopLayer : public TopLayer
{
public:
	                    WindowsTopLayer();
                        ~WindowsTopLayer();

	void				run();	
    void                update();
	Drawer*				getDrawer(){ return m_drawer; }    

private:	

	D3DApp				*m_d3dApp;
	CFramework			*m_framework;

};
