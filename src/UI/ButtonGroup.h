/*
 *  ButtonGroup.h
 *  Phonosynth
 *
 *  Created by Greg on 10/30/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */


#ifndef _BUTTONGROUP_H_
#define _BUTTONGROUP_H_

#include <vector>
#include "Button.h"

class ButtonLayout;

class ButtonGroup
{
public:

    ButtonGroup(int groupID);
    ButtonGroup(int groupID, const vector<Button*>& buttons);
    ButtonGroup(const ButtonGroup& rhs);
    ~ButtonGroup();
    
    bool addButton(Button* button, bool deepCopy = true);
    bool addButton(const Button& button);
    
    bool removeButton(const int buttonID);
    
    const Button& getButton(const int buttonID, bool& valid) const;
          Button& getButton(const int buttonID, bool& valid);
    
    const Button* getButton(const int buttonID) const;
          Button* getButton(const int buttonID);
    
    const vector<Button*>& getButtons() const;
          vector<Button*>& getButtons();
    
    void show(float duration = 0.0f);
    void hide(float duration = 0.0f);    
    void fade(float toOpacity, float duration = 1.0f);        
    
    bool active() const { return m_active;  }
    void activate()     { m_active = true;  }
    void deactivate()   { m_active = false; }
    
    int  getID() const { return m_groupID;              }
    int  size()  const { return (int) m_buttons.size(); }         
    
    void screenRotated(float newWidth, float newHeight);
    
    bool hasLayout() const { return m_layout != NULL; }
    void setLayout(ButtonLayout* layout) { m_layout = layout; }
    void applyLayout(float screenWidth, float screenHeight);
    
	Button* handleClickDown(int x, int y);
	void    handleClickUp(int x, int y);
    
    void render(Drawer& drawer);
    
private:
            
    int              m_groupID;
    vector<Button*>  m_buttons;    
    bool             m_active;    
    ButtonLayout*    m_layout;
};

#endif _BUTTONGROUP_H_