/*
 *  UI.cpp
 *  Phonosynth
 *
 *  Created by Greg on 10/29/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "UI.h"
#include <System.h>

UI::UI(Drawer* drawer) :
    m_drawer(drawer),
    m_buttonGroups()
{
    Matrix::ProjectionMatrixOrthoRH(m_projectionMatrix,
                                    0.0f, System::getScreenWidth(),
                                    System::getScreenHeight(), 0.0f,
                                    -1.0f, 1.0f);
}


UI::~UI()
{
    
}


// TODO: Handle the case where the user adds the same group ID twice
ButtonGroup*
UI::addButtonGroup(int groupID)
{
    ButtonGroup* b = new ButtonGroup(groupID);
    m_buttonGroups.push_back(b);
    return m_buttonGroups.back();
}


const ButtonGroup*
UI::getButtonGroup(int groupID) const
{
    const int nButtons = (const int) m_buttonGroups.size();
    FOR(i,0,nButtons){
        if(m_buttonGroups[i]->getID() == groupID){
            return (const ButtonGroup*) m_buttonGroups[i];
        }
    }
    return NULL;
}


ButtonGroup*
UI::getButtonGroup(int groupID)
{
    const ButtonGroup* ret = ((const UI*) this)->getButtonGroup(groupID);
    return const_cast<ButtonGroup*>(ret);
}


const ButtonGroup&
UI::getButtonGroup(int groupID, OUT bool& valid) const
{
    const int nButtons = (const int) m_buttonGroups.size();
    FOR(i,0,nButtons){
        if(m_buttonGroups[i]->getID() == groupID){
            valid = true;
            return (const ButtonGroup&) *m_buttonGroups[i];
        }
    }
    valid = false;
}


ButtonGroup&
UI::getButtonGroup(int groupID, OUT bool& valid)
{
    const ButtonGroup& ret = ((const UI*) this)->getButtonGroup(groupID, valid);
    return const_cast<ButtonGroup&>(ret);
}


void
UI::screenRotated(float newRotation)
{
    const float screenW   = System::getScreenWidth();
    const float screenH   = System::getScreenHeight();
	
    const float newWidth  = screenW + fabs(sin(newRotation)) * (screenH - screenW);
    const float newHeight = screenH + fabs(sin(newRotation)) * (screenW - screenH); 
	
    Matrix::ProjectionMatrixOrthoRH(m_projectionMatrix, 
                                    0.0f, newWidth,     // x
                                    newHeight, 0.0f,    // flip y
                                    -1.0f, 1.0f);       // z
    
    const int nGroups = (const int) m_buttonGroups.size();
    FOR(i,0,nGroups){
        m_buttonGroups[i]->screenRotated(newWidth, newHeight);
    }
    m_screenRotation = newRotation;
}


Button*
UI::handleClickDown(int x, int y)
{
    const int nGroups = (const int) m_buttonGroups.size();    
	Button* ret = NULL;
    FOR(i,0,nGroups){		
		if(m_buttonGroups[i]->active())
			if(ret = m_buttonGroups[i]->handleClickDown(x, y))
				break;
	}
	return ret;
}

	
void
UI::handleClickUp(int x, int y)
{
	return;
}		
	

void
UI::render(Drawer& d)
{	
	d.disableTextureUnit(1);
	
    // configure matrices. Only one that really matters is projection matrix
    d.setWorldViewMatrix(Matrix::IdentityCRef(), Matrix::IdentityCRef());
    d.setTextureMatrix(Matrix::IdentityCRef());
    d.setProjectionMatrix(m_projectionMatrix);
    
    // configure basic state
    d.setDrawMode(DRAWMODE_TRIANGLESTRIP);
    d.enableTextureUnit(0);
    d.enableBlending();
    
    // draw all active buttons
    const int nGroups = (const int) m_buttonGroups.size();        
    FOR(i,0,nGroups)
        if(m_buttonGroups[i]->active())
            m_buttonGroups[i]->render(d);
}


