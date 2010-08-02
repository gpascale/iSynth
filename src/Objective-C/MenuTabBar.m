//
//  MenuTabBar.m
//  Phonosynth
//
//  Created by Greg on 12/5/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import  "MenuTabBar.h"
#include <common.h>
#include <iostream>

@implementation MenuTabBar

- (id)initWithCoder: (NSCoder*) coder
{			
	if (self = [super initWithCoder: coder]){
		[self setDelegate: self];
		m_lastSelectedItem = nil;
		m_curSelectedItem = self.selectedItem;
		return self;		
	}	
}

- (void) tabBar: (UITabBar *) tabBar didSelectItem: (UITabBarItem *) item
{
	// if the already selected button is pressed, ignore it (unless it is the search button)
	if(item == m_curSelectedItem && item != m_searchButton)
		return;
	
	m_lastSelectedItem = m_curSelectedItem;
	m_curSelectedItem = item;
	
    if(item == m_searchButton){		
		[m_menuViewController hideMostViewedSelector];
		[m_menuViewController showSearchBar];
    }        
    else if(item == m_recentlySynthedButton){
		[m_menuViewController hideMostViewedSelector];
        [m_menuViewController populateMenu: @"EXPLORE:RECENT"];
    }
    else if(item == m_mostViewedButton){
		[m_menuViewController showMostViewedSelector];
		// rather than call populate with a certain string, we just call this function,
		// which willc call populate automatically with the appropriate 
        [m_menuViewController mostViewedTypeChanged];
    }
    else if(item == m_niceAndSynthyButton){
		[m_menuViewController hideMostViewedSelector];
        [m_menuViewController populateMenu: @"EXPLORE:NICE_AND_SYNTHY"];
    }
}


- (void) revertToLastSelectedItem
{
	[self setSelectedItem: m_lastSelectedItem];
	m_curSelectedItem = m_lastSelectedItem;
}


- (void) setSelectedIndex: (int) index
{
	ASSERT(index < (int) [[self items] count]);
	
	UITabBarItem* item;
	
	if(index == -1)
		item = nil;
	else
		item = [[self items] objectAtIndex: index];

	[self setSelectedItem: item];
	m_lastSelectedItem = m_curSelectedItem;
	m_curSelectedItem = item;
}

@end
