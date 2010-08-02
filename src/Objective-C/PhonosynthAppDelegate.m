//
//  PhonosynthAppDelegate.m
//  Phonosynth
//
//  Created by Greg Pascale on 6/10/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "PhonosynthAppDelegate.h"
#include <MacTopLayer.h>
#include <PSSettings.h>

UIApplication* s_uiApplication;

@implementation PhonosynthAppDelegate

- (void)applicationDidFinishLaunching: (UIApplication*) application {		
	m_application = application;
	s_uiApplication = application;
    [self configureApplication: application];
    [self runApplication: application];
}


- (MacTopLayer*) getTopLayer
{
    return m_topLayer;
}
	

- (void) configureApplication: (UIApplication*) application
{
    [application setStatusBarHidden: YES];
	//application.idleTimerDisabled = YES;
}


- (void) runApplication: (UIApplication*) application
{
    // create a new top layer which will initialize and begin the program
    [m_window addSubview: [m_navController view]];
    m_topLayer = new MacTopLayer(m_menuViewController, self);
}



- (void) applicationDidBecomeActive: (UIApplication*) application
{
	APP_PRINTF("Becoming active\n");
	m_topLayer->leaveLowPowerState();
}


- (void) applicationWillResignActive: (UIApplication*) application
{	
	// TODO: Go into a low power state by putting render thread to sleep
	APP_PRINTF("Resigning active\n");
	m_topLayer->enterLowPowerState();
}



- (void) applicationWillTerminate: (UIApplication*) application
{
	//cerr << "Application terminating, writing settings to settings bundle first" << endl;
	PSSettings::Settings().writeSettings();
	m_topLayer->writeDataToFile();
}


- (void) applicationDidReceiveMemoryWarning: (UIApplication*) application 
{ 
	//cerr << "memory warning!" << endl;
    m_topLayer->receivedMemoryWarning();
}


- (void) setAutoLock: (BOOL) b
{
	m_application.idleTimerDisabled = !b;
}


+ (UIApplication*) getUIApplication
{
	return s_uiApplication;
}


@end