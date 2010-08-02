/*
 *  ButtonActions.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/4/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <ButtonActions.h>
#include <Button.h>
#include <Popup.h>
#include <PSMaster.h>
#include <PSUIManager.h>
#include <PSGlobalState.h>
#include <Functions.h>

PSMaster*			s_app;
PSUIManager*        s_uiManager;
Popup*				s_loadYesNoPopup;


void ButtonActionsInit(PSMaster* app, PSUIManager* uiManager)
{
    s_app = app;
    s_uiManager = uiManager;
}


void doLoadPopupAction()
{
	s_app->queueEvent(EVENT_END_VIEWING_SYNTH);
	//popup->hide();
}


void settingsButtonAction()
{	
	s_app->queueEvent(EVENT_SETTINGS);
}


void openButtonAction()
{
	Popup* p = Popup::TwoButtonPopup("", "Leave this synth and go back to the menu?",
									 StaticFunctor(doLoadPopupAction), "Yes",
									 Functor::Identity(), "No");
	p->show();
}


void leftControlButtonAction()
{
	s_app->queueEvent(EVENT_GOTO_LEFTNEIGHBOR);
}


void rightControlButtonAction()
{
	s_app->queueEvent(EVENT_GOTO_RIGHTNEIGHBOR);
}


void upControlButtonAction()
{
	s_app->queueEvent(EVENT_GOTO_UPNEIGHBOR);
}


void downControlButtonAction()
{
	s_app->queueEvent(EVENT_GOTO_DOWNNEIGHBOR);
}


void inControlButtonAction()
{
	s_app->queueEvent(EVENT_GOTO_INNEIGHBOR);
}


void outControlButtonAction()
{
	s_app->queueEvent(EVENT_GOTO_OUTNEIGHBOR);
}




// slideshow button actions

void slideshowPlayButtonAction()
{
	s_app->queueEvent(EVENT_PLAY_SLIDESHOW);
}


void slideshowPauseButtonAction()
{
	s_app->queueEvent(EVENT_PAUSE_SLIDESHOW);
}


void slideshowStopButtonAction()
{
	s_app->queueEvent(EVENT_LEAVE_SLIDESHOW);
}


void slideshowNextButtonAction()
{
    s_app->queueEvent(EVENT_PAUSE_SLIDESHOW);
	s_app->queueEvent(EVENT_GOTO_NEXTIMAGE);
}


void slideshowPrevButtonAction()
{
	s_app->queueEvent(EVENT_PAUSE_SLIDESHOW);
	s_app->queueEvent(EVENT_GOTO_PREVIMAGE);
}




// mode-change button actions

void pointcloudToggleButtonAction()
{
    s_app->queueEvent(EVENT_TOGGLE_POINTCLOUD);
}


void slideshowToggleButtonAction()
{
	s_app->queueEvent(EVENT_ENTER_SLIDESHOW);
}


void screenOrientationToggleButtonAction()
{
	s_app->queueEvent(EVENT_ROTATE_SCREEN);
}


void accelerometerToggleButtonAction()
{

}


void cameraConeToggleButtonAction()
{
	s_app->queueEvent(EVENT_TOGGLE_CAMCONES);
}


void arcballToggleButtonAction()
{
	s_app->toggleArcballMode();	
}


void showMenuButtonAction()
{	
    s_uiManager->showMenu();
	//s_app->queueEvent(EVENT_ROTATE_LEFT, 0.0f);
}


void hideMenuButtonAction()
{
    s_uiManager->hideMenu();
	//s_app->queueEvent(EVENT_ROTATE_LEFT, 0.0f);
}



void showDescriptionLabel(Button* descriptionLabel)
{
	descriptionLabel->show(0.2f, 1.0f);
}


void hideDescriptionLabel(Button* descriptionLabel)
{
	descriptionLabel->hide(0.2f, 0.0f);
}


void arcballExitButtonAction()
{
	s_app->queueEvent(EVENT_LEAVE_ARCBALL);	
}


void
next3DGroupButtonAction()
{
	s_app->queueEvent(EVENT_NEXT_3DGROUP);
}


void
helpButtonAction()
{
	s_app->queueEvent(EVENT_HELP);
}





/*
void showRectsButtonAction()
{
	s_uiManager->enterRectsMode();
}


void hideRectsButtonAction()
{
	s_uiManager->leaveRectsMode();
}
*/


























