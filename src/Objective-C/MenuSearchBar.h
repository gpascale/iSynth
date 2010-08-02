//
//  MenuSearchBar.h
//  Phonosynth
//
//  Created by Greg on 12/5/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MenuViewController.h>

@interface MenuSearchBar : UISearchBar <UISearchBarDelegate> {    
    
    IBOutlet MenuViewController* m_menuViewController;
    
}

- (void) searchBarTextDidBeginEditing: (UISearchBar*) searchBar;  // called when text starts editing
- (void) searchBarTextDidEndEditing: (UISearchBar*)   searchBar;  // called when text ends editing

- (void) searchBarSearchButtonClicked: (UISearchBar*) searchBar;  // called when keyboard search button pressed
- (void) searchBarCancelButtonClicked: (UISearchBar*) searchBar;  // called when cancel button pressed

- (void) fadeIn: (float) duration;
- (void) fadeOut: (float) duration;

@end
