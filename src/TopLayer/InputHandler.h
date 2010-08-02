#ifndef _INPUTHANDLER_H_
#define _INPUTHANDLER_H_

#include <map>
#include <common.h>
#include <vector>
#include <MiniCommon/QuickSort.h>
#include <GMath.h>
#include <GTime.h>

class  PSMaster;
class  Scene;
class  Button;
class  Popup;
class  UI;
class  PSUIManager;
@class AccelerometerDelegate;

using namespace std;






struct Touch
{		
	Touch(int curX = -1, int curY = -1, 
		  int prevX = -1, int prevY = -1,
		  int tapCount = 1) :
		m_curX(curX), m_curY(curY), 
		m_prevX(prevX), m_prevY(prevY),
		m_tapCount(tapCount)
	{ }
	
	inline void	setCurX(int x)	   { m_curX = x;     }
	inline void	setCurY(int y)	   { m_curY = y;	 }
	inline void	setPrevX(int x)	   { m_prevX = x;    }
	inline void	setPrevY(int y)	   { m_prevY = y;    }
	inline void	setTapCount(int n) { m_tapCount = n; }
	
	inline int	getCurX()     const { return m_curX;     }
	inline int	getCurY()     const { return m_curY;     }
	inline int	getPrevX()    const { return m_prevX;    }
	inline int	getPrevY()    const { return m_prevY;    }
	inline int  getTapCount() const { return m_tapCount; }
	
	
private:		
	
	UIView*				m_view;
	int					m_tapCount;
	bool				m_active;
	int					m_curX;
	int					m_curY;
	int					m_prevX;
	int					m_prevY;
	
};

class InputHandler
{

public:

    InputHandler(UIView* view = nil);
    virtual ~InputHandler(){}

	virtual void	cleanup() = 0;
	
    void            setUI(UI* ui)                        { m_ui = ui;               }            
    void            setMasterApp(PSMaster *app)          { m_app = app;             }
    void            setUIManager(PSUIManager* uiManager) { m_uiManager = uiManager; }

    // windows hooks
    
#ifdef WINDOWS 
    
    void            keyDown(int key) = 0;
    void            keyUp(int key) = 0;
    void            mouseDown(MouseButton button, int x, int y) = 0;
    void            mouseUp(MouseButton button, int x, int y) = 0;
    void            mouseMove(int x, int y) = 0;
    
#endif // WINDOWS

    // iphone hooks ...
#ifdef IPHONE
    
    /*
	void			touchesStarted(const vector<Touch> &touches);
	void			touchesMoved(const vector<Touch> &touches);
	void			touchesEnded(const vector<Touch> &touches);
	void			phoneTilted(const Vector3 &tilt);
     */
	
	void	setView(UIView* view) { m_view = view; }
	
	void	setAccelerometerReadFrequency(int readsPerSec);
    
    void    touchesBegan(NSSet* touches, UIEvent* event, UIView* view);
    void    touchesMoved(NSSet* touches, UIEvent* event, UIView* view);
    void    touchesEnded(NSSet* touches, UIEvent* event, UIView* view);
	void    touchesCancelled(NSSet* touches, UIEvent* event, UIView* view);
	void	readAccelerometer(UIAcceleration* accel);
	
	virtual void    onTouchesBegan(NSSet* touches, UIEvent* event, UIView* view) = 0;
    virtual void    onTouchesMoved(NSSet* touches, UIEvent* event, UIView* view) = 0;
    virtual void    onTouchesEnded(NSSet* touches, UIEvent* event, UIView* view) = 0;
	virtual void	onReadAccelerometer(UIAcceleration* accel)					  { }
    
#endif // IPHONE
	
	// control button actions
	
	
	
	
	// modal button actions		    
    
protected:	
	
	int								numFingersDown() const;

#ifdef WINDOWS		
    bool                            m_keyMask[256];
    std::map<MouseButton, bool>     m_mouseMask;
    int                             m_lastX;
    int                             m_lastY;
    
#endif // WINDOWS

#ifdef IPHONE
   	int								m_numFingersDown;    		
	pair<int, int>					m_compositeDrag;							
	Popup*							m_loadYesNoPopup;        
	AccelerometerDelegate*			m_accelerometerDelegate;
	UIView*							m_view;

#endif // IPHONE    
    		
	GTime							m_clickDownTime;
	Button*							m_activeButton;
    PSMaster*                       m_app;    
    PSUIManager*                    m_uiManager;
    UI*                             m_ui;
	
	
};


#endif // _INPUTHANDLER_H_