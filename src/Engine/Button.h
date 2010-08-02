/*
 *  Button.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 6/27/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BUTTON_H_
#define _BUTTON_H_

enum ButtonLocationType
{
	BUTTON_LOC_UPPERLEFT   = 0,
	BUTTON_LOC_UPPERMIDDLE = 1,
	BUTTON_LOC_UPPERRIGHT  = 2,
	BUTTON_LOC_RIGHT	   = 3,
	BUTTON_LOC_LOWERRIGHT  = 4,
	BUTTON_LOC_LOWERMIDDLE = 5,
	BUTTON_LOC_LOWERLEFT   = 6,
	BUTTON_LOC_LEFT		   = 7,
	BUTTON_LOC_CENTER	   = 8,
	BUTTON_LOC_EXPLICIT	   = 9
};

// A ButtonLocationInfo completely describes how to place a button a little bit more robustly than
// a simple x and y coordinate. The use of ButtonLocationType allows a button's location to be
// defined relative to the screen on which it is placed. If type is BUTTON_LOC_EXPLICIT, then
// x and y are to be interpreted as the button's explicit coordinates. If type if one of the 
// relative locations, then x and y are interpreted as offsets from that location in terms
// of the button's width and height respectively
struct ButtonLocationInfo
{
    ButtonLocationType type;
    float              x;
    float              y;
    
    ButtonLocationInfo(ButtonLocationType _type, float _x = 0.0f, float _y = 0.0f) :
        type(_type), x(_x), y(_y) 
    { }    
};


	
#include <Drawer.h>
#include <OpacitySpring.h>
#include <GhettoQuad.h>
#include <ctime>
#include <GTime.h>
#include <Functions.h>
#include <PSTypes.h>

class Button : AdjustableOpacity
{

public:

	// factory constructors
	
	static Button* ButtonWithImage(const string& texturePath = "", int buttonID = -1, 
								   int width = 50, int height = 50,
								   ButtonLocationInfo loc = ButtonLocationInfo(BUTTON_LOC_UPPERLEFT, 0.0f, 0.0f));
	
	static Button* ButtonWithText(const string& text = "", int buttonID = -1, 
								  int width = 50, int height = 50,								  
								  ButtonLocationInfo loc = ButtonLocationInfo(BUTTON_LOC_UPPERLEFT, 0.0f, 0.0f),
								  const Color4F& textColor = Color4F::White(),
								  const Color4F& backgroundColor = Color4F::None());
	
	static Button* ButtonWithText(const string& text = "", int buttonID = -1, 
								  int width = 50, int height = 50,
								  const Color4F& textColor = Color4F::White(),
								  const Color4F& backgroundColor = Color4F::None());
				
	Button(const Button &b);
    
    ~Button();
    
    // relocates the button 
    void relocate();
    void relocate(int screenW, int screenH);    
    void screenRotated(float newWidth, float newHeight);
    	
					
	void    setColor(const Color4UC& cAll);
    void    setColor(const Color4UC& c1, const Color4UC& c2, const Color4UC& c3, const Color4UC& c4);
	void	setOpacity(float opacity);
	void	setOpacity(float opacity, float duration);
	float	getOpacity() const	   { return m_ghettoQuad.getOpacity(); }	
	int		getWidth()		const  { return m_ghettoQuad.getWidth();   }
	int		getHeight()		const  { return m_ghettoQuad.getHeight();  }
	int     getID()         const  { return m_buttonID;  }    
    bool    active()        const  { return m_active;    }
    void    activate()             { m_active = true;    }
    void    deactivate()           { m_active = false;   }
    
	const GhettoQuad& getGhettoQuad() const { return m_ghettoQuad; }
	
	// test to see if a click falls within the button's bounds
	bool	clickHitTest(int clickX, int clickY) const;
	

	// can customize a Button either by passing it function pointers or subclassing and overriding the
	// respondToClick* functions
	
	// setPressAction should be used to set the button's actual action. The up and down actions are
	// intended for small effects such as displaying and hiding a label when the user puts a finger
	// down on, or picks a finger up from a button
	
	// called when a click/touch comes up inside the button
	void    setClickUpAction(const Functor& action)    { m_upAction = action.copy();    }
	// called when a click/touch moves outside of the button
	void	setClickLeaveAction(const Functor& action) { m_leaveAction = action.copy(); }
	// called when a click/touch comes down inside the button
	void    setClickDownAction(const Functor& action)  { m_downAction = action.copy();  }
	// called when a click/touch comes down and then up inside the button
	void    setPressAction(const Functor& action)	   { m_pressAction = action.copy(); }
	
	void    respondToClickDown();	
	void    respondToClickLeave();	
	void	respondToClickUp();		
	void	respondToPress();		
	
	OpacitySpring& getSpring(){ return m_spring; }
	
	void    setLocation(ButtonLocationInfo loc);
	void	setRotation(float angle);
	
	void	getPhysicalLocation(int &x, int &y);
				
	void	draw(Drawer& d);
    	
	bool	handlePress(const int x, const int y);

	void	show(float duration = 0.0f, float opacity = 1.0f);
    void    hide(float duration = 0.0f, float opacity = 0.0f);
    
    
private:
	
    
	// constructor
	Button(int buttonID = -1, 
           int width = 50, int height = 50,
           ButtonLocationInfo loc = ButtonLocationInfo(BUTTON_LOC_UPPERLEFT, 0.0f, 0.0f));
	

    void computePhysicalLocation(int screenW, int screenH, 
                                 int buttonW, int buttonH,
                                 OUT int& x, OUT int& y) const;
        
    
	void initSpring();	
    	
	int             m_buttonID;
    
	bool			m_active;
	
	ButtonLocationInfo	m_locationInfo;
	OpacitySpring	m_spring;
	
	GhettoQuad		m_ghettoQuad;
	
	Functor*		m_downAction;
	Functor*		m_upAction;
	Functor*		m_leaveAction;
	Functor*		m_pressAction;

};


inline
void
Button::setOpacity(float opacity)
{
	m_spring.setToOpacity(opacity, 0.0f);
}


inline
void
Button::setOpacity(float opacity, float duration)
{
	m_spring.setToOpacity(opacity, duration);
}
	

inline
void
Button::draw(Drawer& d)
{	
	m_ghettoQuad.draw(d);
}
	

inline
bool
Button::clickHitTest(int clickX, int clickY) const
{
	return m_ghettoQuad.clickIntersects(clickX, clickY);
}


#endif