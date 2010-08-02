/*
 *  UI.h
 *  Phonosynth
 *
 *  Created by Greg on 10/29/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _UI_H_
#define _UI_H_

#include <vector>
#include "ButtonGroup.h"

class UI
{
public:
    
    UI(Drawer* drawer = NULL);
    ~UI();
    
    void setDrawer(Drawer* drawer) { m_drawer = drawer; }
    
    ButtonGroup* addButtonGroup(int groupID);
    
    const ButtonGroup* getButtonGroup(int groupID) const;
          ButtonGroup* getButtonGroup(int groupID);
    
    // TODO: Less crappy way of doing this? Don't return a reference?
    const ButtonGroup& getButtonGroup(int groupID, OUT bool& valid) const;
          ButtonGroup& getButtonGroup(int groupID, OUT bool& valid);
    
    void render(Drawer& d);
    void render(int buttonGroupID);
            
    void screenRotated(float rot); 
	
    Button* handleClickDown(int x, int y);
	void    handleClickUp(int x, int y);
    
    float m_screenRotation;
        
private:
    
    Matrix                   m_projectionMatrix;    // the matrix used to render the UI
        
    vector<ButtonGroup*>     m_buttonGroups;
    Drawer*                  m_drawer;
    
};

#endif _UI_H_