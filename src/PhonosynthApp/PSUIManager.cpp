/*
 *  PSUIManager.cpp
 *  Phonosynth
 *
 *  Created by Greg on 10/31/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "PSUIManager.h"
#include <UI.h>
#include "PSMaster.h"
#include <PSTypes.h>
#include <PSInputHandler.h>
#include <PSSGManager.h>
#include <PSGlobalState.h>
#include <ButtonLayout.h>
#include <vector>
#include <System.h>
#include <ButtonActions.h>

const char* IMG_LOGO				= "Icon_Photosynth.png";
const char* IMG_OPEN				= "icon_load.png";
const char* IMG_SETTINGS			= "icon_settings.png";
const char* IMG_SLIDESHOW			= "icon_slideShowMode_off.png";
const char* IMG_LEFTARROW			= "neighbor_left.bmp";
const char* IMG_RIGHTARROW			= "neighbor_right.bmp";
const char* IMG_UPARROW             = "neighbor_up.bmp";
const char* IMG_DOWNARROW			= "neighbor_down.bmp";
const char* IMG_INARROW             = "neighbor_in.bmp";
const char* IMG_OUTARROW			= "neighbor_out.bmp";
const char* IMG_CONFIDENTIAL		= "confidential_logo_viewer.bmp";
const char* IMG_POINTCLOUD			= "icon_pointCloud_off.png";
const char* IMG_ARCBALL             = "icon_arcBall_off.png";
const char* IMG_SLIDESHOW_PLAY		= "icon_play.png";
const char* IMG_SLIDESHOW_PAUSE     = "icon_pause.png";
const char* IMG_SLIDESHOW_STOP		= "icon_stop.png";
const char* IMG_SHOW_BUTTONS		= "icon_menu_off.png";
const char* IMG_HIDE_BUTTONS		= "icon_menu_on.png";
const char* IMG_TOGGLE_IMAGES		= "icon_pointCloud_off.png";
const char* IMG_TOGGLE_CONES		= "icon_cone_off.png";
const char* IMG_LANDSCAPE			= "icon_landscape.png";
const char* IMG_PORTRAIT			= "icon_portrait.png";
const char* IMG_SHOW_RECTS			= "icon_cone_off.png";
const char* IMG_HIDE_RECTS			= "icon_cone_on.png";
const char* IMG_HELP				= "icon_help.png";
const char* IMG_ARCBALL_EXIT		= "icon_arcball_exit.png";
const char* IMG_SLIDESHOW_NEXT      = "icon_next.png";
const char* IMG_SLIDESHOW_PREV		= "icon_prev.png";
const char* IMG_NEXT_3D_GROUP		= "icon_next_3d_group.png";

PSUIManager::PSUIManager(UI* ui) :
    m_ui(ui),
    m_mode(MODE_NONE),
    m_menuVisible(false),
	m_rectsMode(false)
{    
	screenRotated(0.0f);
}


PSUIManager::~PSUIManager()
{
}


void
PSUIManager::cleanup()
{
	// reset state to default
	modeChanged(MODE_NORMAL);	
	// unrotate the screen in case it has been rotated
	screenRotated(0.0f);
	
	leaveRectsMode();
}


void
PSUIManager::initUI(PSMaster* master)
{
    ButtonGroup& normalControlButtons    = *m_ui->addButtonGroup(BUTTONS_NORMAL_CONTROL);	
    ButtonGroup& arcballControlButtons   = *m_ui->addButtonGroup(BUTTONS_ARCBALL_CONTROL);	
	ButtonGroup& slideshowControlButtons = *m_ui->addButtonGroup(BUTTONS_SLIDESHOW_CONTROL);
	
	ButtonGroup& normalMenuButtons       = *m_ui->addButtonGroup(BUTTONS_NORMAL_MENU);
    ButtonGroup& arcballMenuButtons      = *m_ui->addButtonGroup(BUTTONS_ARCBALL_MENU);	    
    ButtonGroup& slideshowMenuButtons    = *m_ui->addButtonGroup(BUTTONS_SLIDESHOW_MENU);
	
	ButtonGroup& otherButtons            = *m_ui->addButtonGroup(BUTTONS_OTHER);
	
	ButtonGroup& descriptionLabels       = *m_ui->addButtonGroup(BUTTONS_DESCRIPTION_LABELS);
	
   
	
    
    // create each button
	
	/*
	Button* logoButton = Button::ButtonWithImage(IMG_LOGO, ID_LOGO, 40, 40, 
												 ButtonLocationInfo(BUTTON_LOC_UPPERLEFT));		
	 */
	
    Button* leftButton = Button::ButtonWithImage(IMG_LEFTARROW, ID_LEFTARROW, 22, 64, 
												 ButtonLocationInfo(BUTTON_LOC_LEFT));
    Button* rightButton = Button::ButtonWithImage(IMG_RIGHTARROW, ID_RIGHTARROW, 22, 64, 
												  ButtonLocationInfo(BUTTON_LOC_RIGHT));
    Button* upButton = Button::ButtonWithImage(IMG_UPARROW, ID_UPARROW, 64, 22, 
											   ButtonLocationInfo(BUTTON_LOC_UPPERMIDDLE));
    Button* downButton = Button::ButtonWithImage(IMG_DOWNARROW, ID_DOWNARROW, 64, 22, 
												ButtonLocationInfo(BUTTON_LOC_LOWERMIDDLE));
    Button* inButton = Button::ButtonWithImage(IMG_INARROW, ID_INARROW, 53, 34, 
											   ButtonLocationInfo(BUTTON_LOC_LOWERMIDDLE, 1.0f, -1.0f));
    Button* outButton = Button::ButtonWithImage(IMG_OUTARROW, ID_OUTARROW, 53, 34, 
												ButtonLocationInfo(BUTTON_LOC_LOWERMIDDLE, -1.0f, -1.0f));
    Button* slideshowPlayButton = Button::ButtonWithImage(IMG_SLIDESHOW_PLAY, ID_SLIDESHOW_PLAY, 55, 55, 
														  ButtonLocationInfo(BUTTON_LOC_LOWERMIDDLE, -1.1f, -0.1f));
    Button* slideshowPauseButton = Button::ButtonWithImage(IMG_SLIDESHOW_PAUSE, ID_SLIDESHOW_PAUSE, 55, 55, 
														   ButtonLocationInfo(BUTTON_LOC_LOWERMIDDLE, 0.0f, -0.1f));
   	Button* slideshowStopButton = Button::ButtonWithImage(IMG_SLIDESHOW_STOP, ID_SLIDESHOW_STOP, 55, 55, 
														  ButtonLocationInfo(BUTTON_LOC_LOWERMIDDLE, 1.1f, -0.1f));
	Button* slideshowNextButton = Button::ButtonWithImage(IMG_SLIDESHOW_NEXT, ID_SLIDESHOW_NEXT, 55, 55, 
														  ButtonLocationInfo(BUTTON_LOC_LOWERRIGHT, -0.2f, -0.1f));
	Button* slideshowPrevButton = Button::ButtonWithImage(IMG_SLIDESHOW_PREV, ID_SLIDESHOW_PREV, 55, 55, 
														  ButtonLocationInfo(BUTTON_LOC_LOWERLEFT, 0.2f, -0.1f));
	
	
    Button* pointcloudToggleButton = Button::ButtonWithImage(IMG_POINTCLOUD, ID_POINTCLOUD, 55, 55);
    Button* slideshowToggleButton = Button::ButtonWithImage(IMG_SLIDESHOW, ID_SLIDESHOW, 55, 55);
    Button* cameraConeToggleButton = Button::ButtonWithImage(IMG_TOGGLE_CONES, ID_TOGGLE_CONES, 55, 55);
    Button* arcballToggleButton = Button::ButtonWithImage(IMG_ARCBALL, ID_ARCBALL, 55, 55);
	Button* screenOrientationToggleButton = Button::ButtonWithImage(IMG_PORTRAIT, ID_PORTRAIT, 55, 55);
    Button* openButton = Button::ButtonWithImage(IMG_OPEN, ID_OPEN, 55, 55);
    Button* settingsButton = Button::ButtonWithImage(IMG_SETTINGS, ID_SETTINGS, 55, 55);
	Button* helpButton = Button::ButtonWithImage(IMG_HELP, ID_HELP, 55, 55);
	
    Button* showMenuButton = Button::ButtonWithImage(IMG_SHOW_BUTTONS, ID_SHOW_BUTTONS, 32, 32, 
													 ButtonLocationInfo(BUTTON_LOC_UPPERRIGHT));
    Button* hideMenuButton = Button::ButtonWithImage(IMG_HIDE_BUTTONS, ID_HIDE_BUTTONS, 32, 32, 													 
													 ButtonLocationInfo(BUTTON_LOC_UPPERRIGHT));
	
	Button* showRectsButton = Button::ButtonWithImage(IMG_SHOW_RECTS, ID_SHOW_RECTS, 32, 32, 													 
													  ButtonLocationInfo(BUTTON_LOC_UPPERLEFT));
	
	Button* hideRectsButton = Button::ButtonWithImage(IMG_HIDE_RECTS, ID_HIDE_RECTS, 32, 32, 													 
													  ButtonLocationInfo(BUTTON_LOC_UPPERLEFT));
		
	Button* arcballExitButton = Button::ButtonWithImage(IMG_ARCBALL_EXIT, ID_ARCBALL_EXIT, 32, 32, 													 
													    ButtonLocationInfo(BUTTON_LOC_UPPERLEFT));
	
	Button* next3DGroupButton = Button::ButtonWithImage(IMG_NEXT_3D_GROUP, ID_NEXT_3D_GROUP, 32, 32,
														ButtonLocationInfo(BUTTON_LOC_UPPERLEFT));
	
	
	
	// configure button labels
    		
	Button* pointcloudToggleLabel		 = Button::ButtonWithText("View Pointcloud", ID_POINTCLOUD, 150, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
    Button* slideshowToggleLabel		 = Button::ButtonWithText("Slideshow Mode", ID_SLIDESHOW, 180, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
    Button* cameraConeToggleLabel		 = Button::ButtonWithText("Show/Hide Cameras ", ID_TOGGLE_CONES, 200, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
    Button* arcballToggleLabel			 = Button::ButtonWithText("Orbit Mode", ID_ARCBALL, 120, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
	Button* screenOrientationToggleLabel = Button::ButtonWithText("Rotate Screen", ID_PORTRAIT, 140, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
    Button* openLabel					 = Button::ButtonWithText("Back to Menu", ID_OPEN, 130, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
    Button* settingsLabel				 = Button::ButtonWithText("Settings", ID_SETTINGS, 90, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));		
	Button* slideshowPlayLabel			 = Button::ButtonWithText("Play Slideshow", ID_SLIDESHOW_PLAY, 160, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
    Button* slideshowPauseLabel			 = Button::ButtonWithText("Pause Slideshow", ID_SLIDESHOW_PAUSE, 170, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
    Button* slideshowStopLabel			 = Button::ButtonWithText("Leave Slideshow Mode", ID_SLIDESHOW_STOP, 200, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
	
	Button* showMenuLabel				 = Button::ButtonWithText("Show Menu", ID_SHOW_BUTTONS, 120, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
	
	Button* hideMenuLabel				 = Button::ButtonWithText("Hide Menu", ID_HIDE_BUTTONS, 120, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
	
	Button* showRectsLabel				 = Button::ButtonWithText("Show Photo Outlines", ID_SHOW_RECTS, 200, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
	
	Button* hideRectsLabel				 = Button::ButtonWithText("Hide Photo Outlines", ID_HIDE_RECTS, 200, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
	 	
	Button* helpLabel					 = Button::ButtonWithText("Show Help Screen", ID_HELP, 200, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));

	Button* arcballExitLabel			= Button::ButtonWithText("Leave Orbit Mode", ID_ARCBALL_EXIT, 200, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));

	Button* slideshowNextLabel			= Button::ButtonWithText("Next Image", ID_SLIDESHOW_NEXT, 130, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));

	Button* slideshowPrevLabel			= Button::ButtonWithText("Previous Image", ID_SLIDESHOW_PREV, 150, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));

	Button* next3DGroupLabel			= Button::ButtonWithText("Next 3D Group", ID_NEXT_3D_GROUP, 150, 30,
																  ButtonLocationInfo(BUTTON_LOC_CENTER, 0.0f, -3.7f),
																  Color4F::White(),
																  Color4F(0.0f, 0.0f, 0.0f, 0.0f));
	
	
	
    // set button colors
#define GREEN Color4UC(0, 200, 0)        
    leftButton->setColor(GREEN);     // control buttons are all green
    rightButton->setColor(GREEN);
    upButton->setColor(GREEN);
	downButton->setColor(GREEN);
    inButton->setColor(GREEN);
    outButton->setColor(GREEN);
#undef GREEN
    
#define WHITE4 Color4UC(205, 205, 205), Color4UC(255, 255, 255), Color4UC(255, 255, 255), Color4UC(205, 205, 205) 
#define GOLD4 Color4UC(25, 25, 25), Color4UC(255, 255, 127), Color4UC(255, 255, 127), Color4UC(25, 25, 25)
    pointcloudToggleButton->setColor(WHITE4);         // menu buttons are tinted gold (colored per vertex)
    slideshowToggleButton->setColor(WHITE4);
    cameraConeToggleButton->setColor(WHITE4);
    arcballToggleButton->setColor(WHITE4);
    screenOrientationToggleButton->setColor(WHITE4);
    openButton->setColor(WHITE4);
    settingsButton->setColor(WHITE4);
	helpButton->setColor(WHITE4);
#undef GOLD4
    	
	// associate each button with the appropriate action
    leftButton->setPressAction(StaticFunctor(&leftControlButtonAction));
    rightButton->setPressAction(StaticFunctor(&rightControlButtonAction));
    upButton->setPressAction(StaticFunctor(&upControlButtonAction));
    downButton->setPressAction(StaticFunctor(&downControlButtonAction));
    inButton->setPressAction(StaticFunctor(&inControlButtonAction));
    outButton->setPressAction(StaticFunctor(&outControlButtonAction));
    slideshowPlayButton->setPressAction(StaticFunctor(&slideshowPlayButtonAction));
    slideshowPauseButton->setPressAction(StaticFunctor(&slideshowPauseButtonAction));
    slideshowStopButton->setPressAction(StaticFunctor(&slideshowStopButtonAction));
    pointcloudToggleButton->setPressAction(StaticFunctor(&pointcloudToggleButtonAction));
    slideshowToggleButton->setPressAction(StaticFunctor(&slideshowToggleButtonAction));
    cameraConeToggleButton->setPressAction(StaticFunctor(&cameraConeToggleButtonAction));
    arcballToggleButton->setPressAction(StaticFunctor(&arcballToggleButtonAction));
    screenOrientationToggleButton->setPressAction(StaticFunctor(&screenOrientationToggleButtonAction));	
    openButton->setPressAction(StaticFunctor(&openButtonAction));
    settingsButton->setPressAction(StaticFunctor(&settingsButtonAction));	
    showMenuButton->setPressAction(StaticFunctor(&showMenuButtonAction));
    hideMenuButton->setPressAction(StaticFunctor(&hideMenuButtonAction));	
	
	/*
	showRectsButton->setPressAction(StaticFunctor(&showRectsButtonAction));
	hideRectsButton->setPressAction(StaticFunctor(&hideRectsButtonAction));
	 */
		
	arcballExitButton->setPressAction(StaticFunctor(&arcballExitButtonAction));
	helpButton->setPressAction(StaticFunctor(&helpButtonAction));
	slideshowNextButton->setPressAction(StaticFunctor(&slideshowNextButtonAction));
	slideshowPrevButton->setPressAction(StaticFunctor(&slideshowPrevButtonAction));
	next3DGroupButton->setPressAction(StaticFunctor(&next3DGroupButtonAction));
    
	typedef MemberFunctorA2<Button, float, float> ShowFunctor;
	typedef MemberFunctorA2<Button, float, float> HideFunctor;
	
#define SETACTIONS(button, target)													\
	(button)->setClickDownAction(ShowFunctor(target, &Button::show, 0.2f, 1.0f));	 \
	(button)->setClickUpAction(ShowFunctor(target, &Button::hide, 0.2f, 0.0f));		  \
	(button)->setClickLeaveAction(ShowFunctor(target, &Button::hide, 0.2f, 0.0f));		   
	
	SETACTIONS(pointcloudToggleButton, pointcloudToggleLabel);
	SETACTIONS(arcballToggleButton, arcballToggleLabel);
	SETACTIONS(slideshowToggleButton, slideshowToggleLabel);
	SETACTIONS(cameraConeToggleButton, cameraConeToggleLabel);
	SETACTIONS(openButton, openLabel);
	SETACTIONS(settingsButton, settingsLabel);
	SETACTIONS(screenOrientationToggleButton, screenOrientationToggleLabel);
	SETACTIONS(slideshowPlayButton, slideshowPlayLabel);
	SETACTIONS(slideshowPauseButton, slideshowPauseLabel);
	SETACTIONS(slideshowStopButton, slideshowStopLabel);
	SETACTIONS(showMenuButton, showMenuLabel);
	SETACTIONS(hideMenuButton, hideMenuLabel);
	SETACTIONS(showRectsButton, showRectsLabel);
	SETACTIONS(hideRectsButton, hideRectsLabel);
	SETACTIONS(helpButton, helpLabel);
	SETACTIONS(arcballExitButton, arcballExitLabel);
	//SETACTIONS(slideshowNextButton, slideshowNextLabel);
	//SETACTIONS(slideshowPrevButton, slideshowPrevLabel);
	SETACTIONS(next3DGroupButton, next3DGroupLabel);
	
	/*
	SETACTIONS(inButton, inLabel);
	SETACTIONS(outButton, outLabel);
	SETACTIONS(leftButton, leftLabel);
	SETACTIONS(rightButton, rightLabel);
	SETACTIONS(upButton, upLabel);
	SETACTIONS(downButton, downLabel);
	SETACTIONS(showMenuButton, showMenuLabel);
	SETACTIONS(hideMenuButton, hideMenuLabel);
	 */
	
#undef SETACTIONS
	
    // navigation buttons			
	normalControlButtons.addButton(leftButton);
	normalControlButtons.addButton(rightButton);
    normalControlButtons.addButton(upButton);
    normalControlButtons.addButton(downButton);
    normalControlButtons.addButton(inButton);
    normalControlButtons.addButton(outButton);
	normalControlButtons.addButton(next3DGroupButton);
    
	// slideshow buttons
    slideshowControlButtons.addButton(slideshowPlayButton);
    slideshowControlButtons.addButton(slideshowPauseButton);
    slideshowControlButtons.addButton(slideshowStopButton);
	slideshowControlButtons.addButton(slideshowNextButton);
	slideshowControlButtons.addButton(slideshowPrevButton);
    
    // normal mode menu buttons	
    normalMenuButtons.addButton(pointcloudToggleButton);
    normalMenuButtons.addButton(openButton);
    normalMenuButtons.addButton(slideshowToggleButton);
    normalMenuButtons.addButton(arcballToggleButton);
    normalMenuButtons.addButton(screenOrientationToggleButton);
    normalMenuButtons.addButton(settingsButton);
	normalMenuButtons.addButton(helpButton);
	
    
    // slideshow mode menu buttons
    slideshowMenuButtons.addButton(pointcloudToggleButton);
	slideshowMenuButtons.addButton(openButton);
    slideshowMenuButtons.addButton(screenOrientationToggleButton);    
    slideshowMenuButtons.addButton(settingsButton);
	slideshowMenuButtons.addButton(helpButton);

    
	// arcball mode menu buttons
    arcballMenuButtons.addButton(cameraConeToggleButton);
	arcballMenuButtons.addButton(openButton);
    arcballMenuButtons.addButton(screenOrientationToggleButton);    
    arcballMenuButtons.addButton(settingsButton);
	arcballMenuButtons.addButton(helpButton);
	
	arcballControlButtons.addButton(arcballExitButton);
    
	// We want to assign buttons references to the labels here (see below) so we don't
	// want to deep-copy them when adding to button groups
	descriptionLabels.addButton(pointcloudToggleLabel, false);
	descriptionLabels.addButton(slideshowToggleLabel, false);
	descriptionLabels.addButton(cameraConeToggleLabel, false);
	descriptionLabels.addButton(arcballToggleLabel, false);
	descriptionLabels.addButton(screenOrientationToggleLabel, false);
	descriptionLabels.addButton(openLabel, false);
	descriptionLabels.addButton(settingsLabel, false);
	descriptionLabels.addButton(slideshowPlayLabel, false);
	descriptionLabels.addButton(slideshowPauseLabel, false);
	descriptionLabels.addButton(slideshowStopLabel, false);
	descriptionLabels.addButton(showMenuLabel, false);
	descriptionLabels.addButton(hideMenuLabel, false);
	descriptionLabels.addButton(showRectsLabel, false);
	descriptionLabels.addButton(hideRectsLabel, false);
	descriptionLabels.addButton(helpLabel, false);
	descriptionLabels.addButton(arcballExitLabel, false);
	descriptionLabels.addButton(slideshowNextLabel, false);
	descriptionLabels.addButton(slideshowPrevLabel, false);
	descriptionLabels.addButton(next3DGroupLabel, false);
	
	/*
	descriptionLabels.addButton(inLabel, false);
	descriptionLabels.addButton(outLabel, false);
	descriptionLabels.addButton(leftLabel, false);
	descriptionLabels.addButton(rightLabel, false);
	descriptionLabels.addButton(upLabel, false);
	descriptionLabels.addButton(downLabel, false);
	descriptionLabels.addButton(showMenuLabel, false);	
	descriptionLabels.addButton(hideMenuLabel, false);
	 */
	
	
    // miscellaneous buttons
	//otherButtons.addButton(logoButton, false);
    otherButtons.addButton(showMenuButton);
    otherButtons.addButton(hideMenuButton);
	//otherButtons.addButton(showRectsButton);
	//otherButtons.addButton(hideRectsButton);
	
	PSGlobalState::setRectsMode(true);
    	
    
    const float screenW      = System::getScreenWidth();
    const float screenH      = System::getScreenHeight();
    
    ButtonCircle* normalCirc = new ButtonCircle(0.5f, 0.5f, 70.0f, 0.0f, -1, false);
    ButtonCircle* arcballCirc = new ButtonCircle(0.5f, 0.5f, 60.0f, 0.0f, -1, false);
    ButtonCircle* slideshowCirc = new ButtonCircle(0.5f, 0.5f, 60.0f, 0.0f, -1, false);

    normalMenuButtons.setLayout(normalCirc);
    arcballMenuButtons.setLayout(arcballCirc);
    slideshowMenuButtons.setLayout(slideshowCirc);
    
    normalMenuButtons.applyLayout(screenW, screenH);
    arcballMenuButtons.applyLayout(screenW, screenH);
    slideshowMenuButtons.applyLayout(screenW, screenH);    
	
	descriptionLabels.hide();
	descriptionLabels.activate();	
	
    otherButtons.show(1.0f);
	otherButtons.getButton(ID_HIDE_BUTTONS)->hide();
	otherButtons.getButton(ID_SHOW_BUTTONS)->show();
    
	
	// the buttons are deep copied when added to a button group, so free them here
	SAFE_DELETE(leftButton);
    SAFE_DELETE(rightButton);
	SAFE_DELETE(upButton);
    SAFE_DELETE(downButton);
	SAFE_DELETE(inButton);
    SAFE_DELETE(outButton);
    SAFE_DELETE(slideshowPlayButton);
    SAFE_DELETE(slideshowPauseButton);
    SAFE_DELETE(slideshowStopButton);
    SAFE_DELETE(pointcloudToggleButton);
    SAFE_DELETE(slideshowToggleButton);
    SAFE_DELETE(cameraConeToggleButton);
    SAFE_DELETE(arcballToggleButton);
    SAFE_DELETE(screenOrientationToggleButton);
    SAFE_DELETE(openButton);
    SAFE_DELETE(settingsButton);
    SAFE_DELETE(showMenuButton);
    SAFE_DELETE(hideMenuButton);
	SAFE_DELETE(arcballExitButton);
	SAFE_DELETE(helpButton);
	SAFE_DELETE(slideshowNextButton);
	SAFE_DELETE(slideshowPrevButton);
	SAFE_DELETE(next3DGroupButton);
	
	
    // finally, initialize the button actions so buttons can work
    ButtonActionsInit(master, this);
}


void
PSUIManager::modeChanged(const PhonosynthMode newMode)
{            
    
    bool showSomething = true, hideSomething = true;
    ButtonGroupID toHide;
    ButtonGroupID toShow;
    
    switch(m_mode)
    {
        case MODE_NORMAL: 
            toHide = BUTTONS_NORMAL_CONTROL;
            break;
        case MODE_ARCBALL: 
            toHide = BUTTONS_ARCBALL_CONTROL;
            break;
        case MODE_SLIDESHOW_PLAYING:
		{
			ButtonGroup* scb = m_ui->getButtonGroup(BUTTONS_SLIDESHOW_CONTROL); ASSERT(scb);
			Button* b = scb->getButton(ID_SLIDESHOW_PLAY); ASSERT(b);
			b->setColor(Color4UC::White());
			toHide = BUTTONS_SLIDESHOW_CONTROL;			
            break;
		}
		case MODE_SLIDESHOW_PAUSED:
		{
			ButtonGroup* scb = m_ui->getButtonGroup(BUTTONS_SLIDESHOW_CONTROL); ASSERT(scb);
			Button* b = scb->getButton(ID_SLIDESHOW_PAUSE); ASSERT(b);
			b->setColor(Color4UC::White());
            toHide = BUTTONS_SLIDESHOW_CONTROL;			
            break;
		}
        default:
            hideSomething = false;
            break;
    }
    
    switch(newMode)
    {
        case MODE_NORMAL: 
            toShow = BUTTONS_NORMAL_CONTROL;
            break;
        case MODE_ARCBALL:
            toShow = BUTTONS_ARCBALL_CONTROL;
            break;
        case MODE_SLIDESHOW_PLAYING:
		{
			ButtonGroup* scb = m_ui->getButtonGroup(BUTTONS_SLIDESHOW_CONTROL); ASSERT(scb);
			Button* b = scb->getButton(ID_SLIDESHOW_PLAY); ASSERT(b);
			b->setColor(Color4UC::Green());
			toShow = BUTTONS_SLIDESHOW_CONTROL;
            break;
		}
		case MODE_SLIDESHOW_PAUSED:
		{
			ButtonGroup* scb = m_ui->getButtonGroup(BUTTONS_SLIDESHOW_CONTROL); ASSERT(scb);
			Button* b = scb->getButton(ID_SLIDESHOW_PAUSE); ASSERT(b);
			b->setColor(Color4UC::Green());
            toShow = BUTTONS_SLIDESHOW_CONTROL;
            break;
		}
        default:
            showSomething = false;
            break;
    }
    
    ButtonGroup* buttons;
    
    if(hideSomething){
        buttons = m_ui->getButtonGroup(toHide);
        if(buttons != NULL){
			buttons->deactivate();
            buttons->hide(0.5f);
		}
        else
            ASSERT(0 && "invalid button group");
    }
     
    if(showSomething){
        buttons = m_ui->getButtonGroup(toShow);
		buttons->activate();
        if(buttons != NULL)
            buttons->show(1.0f);
        else
            ASSERT(0 && "invalid button group");
    }
    
    hideMenu();
    m_mode = newMode; // update the current mode
}


void
PSUIManager::showMenu()
{
 //   if(m_menuVisible)
   //     return;
    toggleMenuVisibility(true);
	
    //m_menuVisible = true;
}


void
PSUIManager::hideMenu()
{
   // if(!m_menuVisible)
     //   return;
    toggleMenuVisibility(false);
	
 //   m_menuVisible = false;
}


void
PSUIManager::enterRectsMode()
{
	return; // currently not doing a "rects mode"
	
	ButtonGroup* grp = m_ui->getButtonGroup(BUTTONS_OTHER); ASSERT(grp);
	Button* show = grp->getButton(ID_SHOW_RECTS); ASSERT(show);
	Button* hide = grp->getButton(ID_HIDE_RECTS); ASSERT(hide);
	
	show->hide(0.5f);
	hide->show(0.5f);
	
	PSGlobalState::setRectsMode(true);
}


void
PSUIManager::leaveRectsMode()
{
	return; // currently not doing a "rects mode"
	
	ButtonGroup* grp = m_ui->getButtonGroup(BUTTONS_OTHER); ASSERT(grp);
	Button* show = grp->getButton(ID_SHOW_RECTS); ASSERT(show);
	Button* hide = grp->getButton(ID_HIDE_RECTS); ASSERT(hide);
	
	show->show(0.5f);
	hide->hide(0.5f);
	
	PSGlobalState::setRectsMode(false);
}


void
PSUIManager::fadeOutControls()
{
    switch(m_mode){
        case MODE_NORMAL:
		{
            ButtonGroup* controlsGrp = m_ui->getButtonGroup(BUTTONS_NORMAL_CONTROL);
            ASSERT(controlsGrp != NULL);
            controlsGrp->hide(1.5f);                                
            break;
        }
        case MODE_SLIDESHOW_PLAYING: case MODE_SLIDESHOW_PAUSED:
		{
            ButtonGroup* controlsGrp = m_ui->getButtonGroup(BUTTONS_SLIDESHOW_CONTROL);
            ASSERT(controlsGrp != NULL);
            controlsGrp->hide(1.5f);
            break;
        }
		case MODE_ARCBALL:
		{
			ButtonGroup* controlsGrp = m_ui->getButtonGroup(BUTTONS_ARCBALL_CONTROL);
			ASSERT(controlsGrp != NULL);
			controlsGrp->hide(1.5f);
			break;
		}
		
    }    
    ButtonGroup* otherGrp = m_ui->getButtonGroup(BUTTONS_OTHER);
	
	hideMenu();
	
    ASSERT(otherGrp);
    otherGrp->hide(1.5f);

}


void
PSUIManager::fadeInControls(unsigned char neighborMask)
{
    ButtonGroup* otherGrp = m_ui->getButtonGroup(BUTTONS_OTHER);
    ASSERT(otherGrp);
	
	Button* showMenuButton = otherGrp->getButton(ID_SHOW_BUTTONS);
	//Button* logoButton     = otherGrp->getButton(ID_LOGO);
	
	otherGrp->activate();
	showMenuButton->show(0.5f);

	
	/*
	if(PSGlobalState::getRectsMode()){
		Button* hideRectsButton = otherGrp->getButton(ID_HIDE_RECTS);
		hideRectsButton->show(0.5f);
	}
	else{
		Button* showRectsButton = otherGrp->getButton(ID_SHOW_RECTS);
		showRectsButton->show(0.5f);
	}
	 */
	 
		
    ButtonGroup* controlsGrp;
    
    switch(m_mode){
        case MODE_NORMAL:
            controlsGrp = m_ui->getButtonGroup(BUTTONS_NORMAL_CONTROL);
            ASSERT(controlsGrp);
            break;
        case MODE_ARCBALL:
            controlsGrp = m_ui->getButtonGroup(BUTTONS_ARCBALL_CONTROL);
            ASSERT(controlsGrp);
            break;
        case MODE_SLIDESHOW_PAUSED: case MODE_SLIDESHOW_PLAYING:
            controlsGrp = m_ui->getButtonGroup(BUTTONS_SLIDESHOW_CONTROL);
            ASSERT(controlsGrp);
            break;
    }
    
    if(m_mode == MODE_NORMAL){
        controlsGrp->activate();
		controlsGrp->show(0.5f);
        updateControlButtons(neighborMask);
    }
    else{
        controlsGrp->show(0.5f);
    }
        
}


void
PSUIManager::toggleMenuVisibility(bool makeVisible)
{
    vector<ButtonGroup*> buttons;
    
    switch(m_mode)
    {
        case MODE_NORMAL:
            buttons.push_back(m_ui->getButtonGroup(BUTTONS_NORMAL_MENU));
            break;
        case MODE_ARCBALL:
            buttons.push_back(m_ui->getButtonGroup(BUTTONS_ARCBALL_MENU));
            break;
        case MODE_SLIDESHOW_PAUSED: case MODE_SLIDESHOW_PLAYING:
            buttons.push_back(m_ui->getButtonGroup(BUTTONS_SLIDESHOW_MENU));
            break;
        default:
            return; // nothing to do
    }
    
    ButtonGroup* logoButtons = m_ui->getButtonGroup(BUTTONS_OTHER);
    ASSERT(logoButtons);
    Button* showMenuButton = logoButtons->getButton(ID_SHOW_BUTTONS);
    Button* hideMenuButton = logoButtons->getButton(ID_HIDE_BUTTONS);
    ASSERT(showMenuButton && hideMenuButton);
    
    if(makeVisible){
		FOR(i,0,buttons.size()){
			buttons[i]->activate();
			buttons[i]->show(1.0f);
		}
        hideMenuButton->show(1.0f);
        showMenuButton->hide(1.0f);
    }
    else{
        FOR(i,0,buttons.size()){
			buttons[i]->deactivate();
			buttons[i]->hide(1.0f);
		}
        hideMenuButton->hide(1.0f);
        showMenuButton->show(1.0f);
    }        
}




void
PSUIManager::updateControlButtons(bool hasLeft, bool hasRight, bool hasUp,
                                  bool hasDown, bool hasIn, bool hasOut)
{
    unsigned char neighborMask = 0;
    neighborMask |= ((unsigned char) hasLeft << 0);
    neighborMask |= ((unsigned char) hasRight << 1);
    neighborMask |= ((unsigned char) hasUp << 2);
    neighborMask |= ((unsigned char) hasDown << 3);
    neighborMask |= ((unsigned char) hasIn << 4);
    neighborMask |= ((unsigned char) hasOut << 5);
    updateControlButtons(neighborMask);       
}


void
PSUIManager::updateControlButtons(bool hasNeighbor[6])
{
    unsigned char neighborMask = 0;
    FOR(i,0,6) 
        neighborMask |= ((unsigned char) hasNeighbor[i] << i);
    updateControlButtons(neighborMask);
}


void
PSUIManager::updateControlButtons(unsigned char neighborMask)
{
    static const ButtonID ids[6] = {ID_LEFTARROW, ID_RIGHTARROW, ID_UPARROW, 
                                    ID_DOWNARROW, ID_INARROW, ID_OUTARROW};    
    Button* curButton;
    
    ButtonGroup* buttons = m_ui->getButtonGroup(BUTTONS_NORMAL_CONTROL);
    ASSERT(buttons && "no such group BUTTONS_NORMAL_CONTROL");
    
    FOR(i,0,6){
		curButton = buttons->getButton(ids[i]);
        ASSERT(curButton != NULL && "no button");
        if(neighborMask & (1 << i))
            curButton->setOpacity(1.0f, 1.0f);
        else
            curButton->setOpacity(0.1f, 1.0f);
    }
}


void
PSUIManager::screenRotated(float newRotation)
{
    m_ui->screenRotated(newRotation);
}


void
PSUIManager::render(Drawer& d)
{
    m_ui->render(d);
}


