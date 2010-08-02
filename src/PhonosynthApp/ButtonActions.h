/*
 *  ButtonActions.h
 *  Phonosynth
 *
 *  Created by Greg on 12/28/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */


#ifndef _BUTTON_ACTIONS_H_
#define _BUTTON_ACTIONS_H_

class Button;
class Popup;
class PSMaster;
class PSUIManager;

// initialization routine that must be run before buttons are used
extern void ButtonActionsInit(PSMaster* app, PSUIManager* uiManager);


// popup actions
extern void doLoadPopupAction();

// control button actions
extern void	leftControlButtonAction();
extern void	rightControlButtonAction();
extern void	upControlButtonAction();
extern void	downControlButtonAction();
extern void	inControlButtonAction();
extern void	outControlButtonAction();

// slideshow button actions
extern void	slideshowPlayButtonAction();
extern void	slideshowPauseButtonAction();
extern void	slideshowStopButtonAction();
extern void slideshowNextButtonAction();
extern void slideshowPrevButtonAction();

// menu button actions
extern void	openButtonAction();
extern void	settingsButtonAction();	
extern void	pointcloudToggleButtonAction();
extern void	slideshowToggleButtonAction();
extern void	accelerometerToggleButtonAction();
extern void	cameraConeToggleButtonAction();	
extern void	arcballToggleButtonAction();
extern void	screenOrientationToggleButtonAction();
extern void helpButtonAction();

// miscellaneous button actions
extern void showMenuButtonAction();
extern void hideMenuButtonAction();
extern void arcballExitButtonAction();
extern void next3DGroupButtonAction();

/*
// show/hide rects
extern void showRectsButtonAction();
extern void hideRectsButtonAction();
*/

#endif // _BUTTON_ACTIONS_H_