//
//  PhonosynthAppDelegate.h
//  Phonosynth
//
//  Created by Greg Pascale on 6/10/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MenuViewController;
class  MacTopLayer;


@interface PhonosynthAppDelegate : NSObject <UIApplicationDelegate> 
{
	    
    IBOutlet UINavigationController* m_navController;
    IBOutlet UIWindow*               m_window;    
    
    // type UIViewController* for now
    IBOutlet UIViewController*       m_loadingViewController;
    IBOutlet UIViewController*       m_optionsViewController;
    IBOutlet UIViewController*       m_eaglViewController;
    IBOutlet MenuViewController*     m_menuViewController;
 
    MacTopLayer*                     m_topLayer;
    bool                             didDisplayPopup;
	
	UIApplication*					 m_application;
		
}

- (void) configureApplication: (UIApplication*) application;
- (void) runApplication: (UIApplication*) application;

- (void) setAutoLock: (BOOL) b;

- (MacTopLayer*) getTopLayer;

+ (UIApplication*) getUIApplication;

@end

