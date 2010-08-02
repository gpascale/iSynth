/*
 *  ButtonGroup.cpp
 *  Phonosynth
 *
 *  Created by Greg on 10/30/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "ButtonGroup.h"
#include "Button.h"
#include "ButtonLayout.h"
#include <System.h>

ButtonGroup::ButtonGroup(int groupID) :
    m_groupID(groupID),
    m_active(false),
    m_layout(NULL)
{
    m_buttons.clear();
}


ButtonGroup::ButtonGroup(int groupID, const vector<Button*>& buttons) :
    m_groupID(groupID),
    m_buttons(buttons),
    m_layout(NULL)
{
    
}


ButtonGroup::ButtonGroup(const ButtonGroup& rhs) :
    m_groupID(rhs.m_groupID),
    m_buttons(rhs.m_buttons),
    m_active(rhs.m_active),
    m_layout(rhs.m_layout)
{
    //cout << "size is " << m_buttons.size() << endl;
}



ButtonGroup::~ButtonGroup()
{
    
}


bool
ButtonGroup::addButton(Button* button, bool deepCopy)
{
	Button* toAdd = (deepCopy) ? new Button(*button) : button;
    const int nButtons = (const int) m_buttons.size();
    const int buttonID = toAdd->getID();
    FOR(i,0,nButtons){
		if(m_buttons[i]->getID() == buttonID)
            return false;
    }    
    m_buttons.push_back(toAdd);
    return true;
}


bool
ButtonGroup::addButton(const Button& button)
{
    
}
    
    
bool
ButtonGroup::removeButton(const int buttonID)
{
    const int nButtons = (const int) m_buttons.size();
    FOR(i,0,nButtons){
        if(m_buttons[i]->getID() == buttonID){
            SAFE_DELETE(m_buttons[i]);
            m_buttons.erase(m_buttons.begin() + i);
            return true;
        }            
    }
    return false;
}


Button*
ButtonGroup::getButton(const int buttonID)
{
    const Button* ret = ((const ButtonGroup*) this)->getButton(buttonID);
    return const_cast<Button*>(ret);
}


const Button*
ButtonGroup::getButton(const int buttonID) const
{
    const int nButtons = (const int) m_buttons.size();
    FOR(i,0,nButtons){
        if(m_buttons[i]->getID() == buttonID){
            return m_buttons[i];
        }            
    }
    return NULL;
}


Button&
ButtonGroup::getButton(const int buttonID, bool& valid)
{
    const Button& ret = ((const ButtonGroup*) this)->getButton(buttonID, valid);
    return const_cast<Button&>(ret);
}


const Button&
ButtonGroup::getButton(const int buttonID, bool& valid) const
{
    const int nButtons = (const int) m_buttons.size();
    FOR(i,0,nButtons){
        if(m_buttons[i]->getID() == buttonID){
            valid = true;
            return *m_buttons[i];
        }
    }
    valid = false;
}


vector<Button*>&
ButtonGroup::getButtons()
{
    return m_buttons;
}


const vector<Button*>&
ButtonGroup::getButtons() const
{
    return m_buttons;
}


void
ButtonGroup::show(float duration)
{
    //activate();
    fade(1.0f, duration);  
}


void
ButtonGroup::hide(float duration)
{
    //deactivate();
    fade(0.0f, duration);
}


void
ButtonGroup::fade(float toOpacity, float duration)
{
    const int nButtons = (const int) m_buttons.size();
    FOR(i,0,nButtons){
        m_buttons[i]->setOpacity(toOpacity, duration);
    } 
}


void
ButtonGroup::screenRotated(float newWidth, float newHeight)
{
    if(m_layout != NULL){
        m_layout->apply(m_buttons, newWidth, newHeight);    // simply reapply the layout with the new width and height
    }
    else{
        const int   nButtons = (const int) m_buttons.size();                       
        FOR(i,0,nButtons)
            m_buttons[i]->screenRotated(newWidth, newHeight);
    }
}


void
ButtonGroup::applyLayout(float screenWidth, float screenHeight)
{
    m_layout->apply(m_buttons, screenWidth, screenHeight);
}


Button*
ButtonGroup::handleClickDown(int x, int y)
{
    const int nButtons = (const int) m_buttons.size();
    FOR(i,0,nButtons)
        if(m_buttons[i]->active())
			if(m_buttons[i]->clickHitTest(x, y)){
			   m_buttons[i]->respondToClickDown();
			   return m_buttons[i];
			}
    return NULL;
}


void
ButtonGroup::handleClickUp(int x, int y)
{
	// don't think i need this since UI should maintain the actively held down button
	return;
	/*
    const int nButtons = (const int) m_buttons.size();
    FOR(i,0,nButtons)
	if(m_buttons[i]->active())
		if(m_buttons[i]->clickHitTest(x, y)){
			m_buttons[i]->respondToClickUp(x, y);
			return m_buttons[i];
		}
    return NULL;
	 */
}


void
ButtonGroup::render(Drawer& drawer)
{
    const int nButtons = (const int) m_buttons.size();
    FOR(i,0,nButtons){
        m_buttons[i]->getSpring().update();
        m_buttons[i]->draw(drawer);
    }        
}


