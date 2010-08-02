//
//  AboutViewController.h
//  Phonosynth
//
//  Created by Greg on 2/18/09.
//  Copyright 2009 Brown University. All rights reserved.
//

#import <UIKit/UIKit.h>

using namespace std;


@interface CopyrightViewController : UIViewController
{
	
}

- (IBAction) launchWebsite;

@end


@interface HelpViewController : UIViewController
{
	
}

@end


@interface AboutViewController : UITabBarController <UITabBarControllerDelegate>
{

	IBOutlet CopyrightViewController* m_aboutViewController;
	IBOutlet HelpViewController*	  m_helpViewController;
	
	UIWebView*		m_helpView;
	UIView*			m_aboutView;
	UITextView*		m_textView;
}

- (void) reloadViews;


@end
