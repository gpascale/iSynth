#ifdef WINDOWS

#ifndef _D3DAPP_H_
#define _D3DAPP_H_

#include <CFramework.h>

class TopLayer;
class InputHandler;

class D3DApp : public CBaseApp
{
public:
	D3DApp();
	~D3DApp();

	void			setTopLayer(TopLayer *topLayer){ m_topLayer = topLayer; }
    void            setInputHandler(InputHandler *handler){ m_inputHandler = handler; }
	BOOL			initialize();

	// overrides
	void			onRenderFrame(LPDIRECT3DDEVICE9 device);    
    void            onKeyDown(KeyEvent e);
    void            onKeyUp(KeyEvent e);
    void            onMouseDown(MouseEvent e);
    void            onMouseUp(MouseEvent e);
    void            onMouseMove(MouseEvent e);
    void            onResetDevice(LPDIRECT3DDEVICE9 device);

private:

	TopLayer		*m_topLayer;
    InputHandler    *m_inputHandler;    

};

#endif

#endif