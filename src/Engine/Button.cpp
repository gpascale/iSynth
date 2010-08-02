/*
 *  Button.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 6/27/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Button.h"
#include <System.h>
#include <PSSettings.h>


Button::Button(int buttonID, int width, int height, ButtonLocationInfo locInfo) :
    m_buttonID(buttonID),
    m_locationInfo(locInfo),
    m_active(true),
    m_ghettoQuad(),
	m_downAction(NULL),
	m_upAction(NULL),
	m_leaveAction(NULL),
	m_pressAction(NULL)
{		
    m_ghettoQuad.setSize(width, height);
	m_ghettoQuad.setOpacity(0.0f);    
    relocate();	
	initSpring();
}


Button::Button(const Button &b) :
	m_buttonID(b.m_buttonID),
    m_locationInfo(b.m_locationInfo),
    m_active(b.m_active),
	m_ghettoQuad(b.m_ghettoQuad),    
	m_downAction(b.m_downAction == NULL ? NULL : b.m_downAction->copy()),
	m_upAction(b.m_upAction == NULL ? NULL : b.m_upAction->copy()),
	m_leaveAction(b.m_leaveAction == NULL ? NULL : b.m_leaveAction->copy()),
	m_pressAction(b.m_pressAction == NULL ? NULL : b.m_pressAction->copy())
{
	initSpring();
}


Button::~Button()
{
	SAFE_DELETE(m_downAction);
	SAFE_DELETE(m_upAction);
	SAFE_DELETE(m_leaveAction);
	SAFE_DELETE(m_pressAction);
}


Button*
Button::ButtonWithImage(const string& texturePath, int buttonID, 
						int width, int height, 
						ButtonLocationInfo locInfo)
{
	Button* ret = new Button(buttonID, width, height, locInfo);	
	ret->m_ghettoQuad.setTextureWithImage(texturePath);
	return ret;
}


Button*
Button::ButtonWithText(const string& text, int buttonID, 
					   int width, int height, 
					   ButtonLocationInfo locInfo,
					   const Color4F& textColor,
					   const Color4F& backgroundColor)
{
	Button* ret = new Button(buttonID, width, height, locInfo);	
	ret->m_ghettoQuad.setTextureWithText(text, width, height, textColor, backgroundColor);
	return ret;
}


Button*
Button::ButtonWithText(const string& text, int buttonID, 
					   int width, int height, 
					   const Color4F& textColor,
					   const Color4F& backgroundColor)
{
	const ButtonLocationInfo locInfo(BUTTON_LOC_UPPERLEFT, 0.0f, 0.0f);
	Button* ret = new Button(buttonID, width, height, locInfo);	
	ret->m_ghettoQuad.setTextureWithText(text, width, height, textColor, backgroundColor);
	return ret;
}


void
Button::relocate()
{
    relocate(System::getScreenWidth(), System::getScreenHeight());
}


void
Button::relocate(int screenX, int screenY)
{
    int x, y;
    computePhysicalLocation(screenX, screenY,
                            m_ghettoQuad.getWidth(), m_ghettoQuad.getHeight(),
                            x, y);
    m_ghettoQuad.setLocation(x, y);
}


void
Button::screenRotated(float newWidth, float newHeight)
{
    relocate(newWidth, newHeight);
}


void
Button::setColor(const Color4UC& cAll)
{
    m_ghettoQuad.setColor(cAll);
}


void
Button::setColor(const Color4UC& c1, const Color4UC& c2, const Color4UC& c3, const Color4UC& c4)
{
    m_ghettoQuad.setColor(c1, c2, c3, c4);
}


void
Button::computePhysicalLocation(int screenW, int screenH,
                                int buttonW, int buttonH,
                                OUT int &x, OUT int &y) const
{
	switch(m_locationInfo.type)
	{
		case BUTTON_LOC_UPPERLEFT:
			x = 0;
			y = 0;
			break;
		case BUTTON_LOC_UPPERMIDDLE:
			x = screenW / 2 - buttonW / 2;
			y = 0;
			break;			
		case BUTTON_LOC_UPPERRIGHT:
			x = screenW - buttonW;
			y = 0;
			break;
		case BUTTON_LOC_RIGHT:
			x = screenW - buttonW;
			y = screenH / 2 - buttonH / 2;
			break;
		case BUTTON_LOC_LOWERRIGHT:
			x = screenW - buttonW;
			y = screenH - buttonH;
			break;
		case BUTTON_LOC_LOWERMIDDLE:
			x = screenW / 2 - buttonW / 2;
			y = screenH - buttonH;
			break;
		case BUTTON_LOC_LOWERLEFT:
			x = 0;
			y = screenH - buttonH;
			break;
		case BUTTON_LOC_LEFT:
			x = 0;
			y = screenH / 2 - buttonH / 2;
			break;
		case BUTTON_LOC_CENTER:
			x = screenW / 2 - buttonW / 2;
			y = screenH / 2 - buttonH / 2;
			break;
		case BUTTON_LOC_EXPLICIT:	//middle of screen
			x = m_locationInfo.x;
            y = m_locationInfo.y;
	}
    
	    
    if(m_locationInfo.type != BUTTON_LOC_EXPLICIT){
        x += 0.5f * buttonW;
        y += 0.5f * buttonH;
        x += buttonW * m_locationInfo.x;
        y += buttonH * m_locationInfo.y;
    }

}


void
Button::setLocation(ButtonLocationInfo loc)
{
    m_locationInfo = loc;
    relocate();
}


void
Button::setRotation(float angle)
{
	m_ghettoQuad.setRotation(angle);
}


void
Button::initSpring()
{
	m_spring.setTarget(&m_ghettoQuad);
	m_spring.setToOpacity(1.0f, 2.0f);
}


void
Button::show(float duration, float opacity)
{ 
    activate();
	setOpacity(opacity, duration);
}
	

void
Button::hide(float duration, float opacity)
{
    deactivate();
	setOpacity(opacity, duration);
}


void
Button::respondToClickDown()
{
	if(m_active && m_downAction && PSSettings::Settings().getButtonDescriptions())
		m_downAction->call();
}


void
Button::respondToClickLeave()
{
	if(m_active && m_leaveAction)
		m_leaveAction->call();
}

void
Button::respondToClickUp()
{
	if(m_active && m_upAction)
		m_upAction->call();
}


void
Button::respondToPress()
{
	if(m_active && m_pressAction)
		m_pressAction->call();
}


void
Button::getPhysicalLocation(int &x, int &y)
{
    m_ghettoQuad.getLocation(x, y);
}
	

	
	



