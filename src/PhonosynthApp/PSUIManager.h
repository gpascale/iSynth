/*
 *  PSUIManager.h
 *  Phonosynth
 *
 *  Created by Greg on 10/31/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _PSUIManager_H_
#define _PSUIManager_H_

#include <PSTypes.h>

class Drawer;
class UI;
class PSMaster;
class PSSGManager;

class PSUIManager
{

public:
    
    PSUIManager(UI* ui = NULL);
    ~PSUIManager();
    
	void cleanup();
	
    void setUI(UI* ui) { m_ui = ui; }
    bool hasUI() const { return m_ui != NULL; }
	
	void setSGManager(PSSGManager* sgm) { m_sgManager = sgm; }
    
    // this is where the actual UI should be built (i.e. buttons added)
    void initUI(PSMaster* master);
    
    // convenient functions
    void modeChanged(const PhonosynthMode newMode);
    
    void showMenu();
    void hideMenu();
	
	void enterRectsMode();
	void leaveRectsMode();
    
    void fadeOutControls();
    void fadeInControls(unsigned char neighborMask);

    void updateControlButtons(bool hasLeft, bool hasRight, bool hasUp, 
                              bool hasDown, bool hasIn, bool hasOut);    
    void updateControlButtons(bool hasNeighbor[6]);
    void updateControlButtons(unsigned char neighborMask);    
    
    
    // clean this up    
    void screenRotated(float newRotation);
    void render(Drawer& d);
    
    
private:
    
    void toggleMenuVisibility(bool makeVisible);
    
    PhonosynthMode  m_mode;
    bool            m_menuVisible;    		
    UI*             m_ui;
	
	// TODO: do this better
	PSSGManager*	m_sgManager;
  
	bool			m_rectsMode;
	
};

#endif // _PSUIManager_H_