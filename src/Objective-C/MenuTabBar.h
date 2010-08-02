//
//  MenuTabBar.h
//  Phonosynth
//
//  Created by Greg on 12/5/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <MenuViewController.h>

@interface MenuTabBar : UITabBar <UITabBarDelegate> {

    IBOutlet UITabBarItem* m_searchButton;
    IBOutlet UITabBarItem* m_recentlySynthedButton;
    IBOutlet UITabBarItem* m_mostViewedButton;
    IBOutlet UITabBarItem* m_niceAndSynthyButton;
    
    IBOutlet UISearchBar*  m_searchBar;
    
    IBOutlet MenuViewController* m_menuViewController;
    
    IBOutlet MainView*     m_mainView;
	
	UITabBarItem*		   m_lastSelectedItem;
	UITabBarItem*		   m_curSelectedItem;
}

- (void) setSelectedIndex: (int) index;

// delegate method, since a MenuTabBar is its own delegate, ignores the tabBar parameter
- (void) tabBar: (UITabBar *) tabBar didSelectItem: (UITabBarItem *) item;

@end
