//
//  MenuSearchBar.m
//  Phonosynth
//
//  Created by Greg on 12/5/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import  "MenuSearchBar.h"
#import  "ObjCCommon.h"
#include <common.h>
#include <iostream>
#import  <QuartzCore/QuartzCore.h>

@implementation MenuSearchBar


- (id)initWithCoder: (NSCoder*) coder
{			
	if (self = [super initWithCoder: coder]){
		[self setDelegate: self];
		[self setAutocorrectionType: UITextAutocorrectionTypeNo]; // turn of spellchecking
		return self;
	}
	ASSERT(0 && "failed to init MenuSearchBar");
	return nil;
}


- (void) searchBarTextDidBeginEditing: (UISearchBar*) searchBar
{
}

- (void) searchBarTextDidEndEditing: (UISearchBar*) searchBar
{
	[m_menuViewController revertTabBarSelection];
    [m_menuViewController hideSearchBar];
}


- (void) searchBarSearchButtonClicked: (UISearchBar*) searchBar
{	
	NSString* populateString;
	
	NSString* searchQuery = [[self text] stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceCharacterSet]];
	if([[searchQuery lowercaseString] hasPrefix: @"user:"]){		
		NSString* user = [[searchQuery substringFromIndex: 5] stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceCharacterSet]];
		populateString = [@"USER:" stringByAppendingString: user];
	}
	else if([[searchQuery lowercaseString] hasPrefix: @"u:"]){		
		NSString* user = [[searchQuery substringFromIndex: 2] stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceCharacterSet]];
		populateString = [@"USER:" stringByAppendingString: user];
	}
	else{
		populateString = [@"SEARCH:" stringByAppendingString: [self text]];				
	}
	
	[m_menuViewController populateMenu: populateString
						 updateTabBars: YES];
	[self fadeOut: 1.0f];    
}


- (void) searchBarCancelButtonClicked: (UISearchBar*) searchBar
{
	[m_menuViewController revertTabBarSelection];
	[m_menuViewController hideSearchBar];
}


#ifdef DEBUG
- (void) searchBar: (UISearchBar *) searchBar textDidChange: (NSString *) searchText
{
	//cerr << "Search bar reads: |" << CSTRING(searchText) << "|" << endl;
}
#endif


- (void) fadeIn: (float) duration
{
    CATransition *animation = [CATransition animation];
	[animation setDelegate:self];	
    [animation setSubtype: kCATransitionFade];	
	[animation setDuration:duration];
	[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];	
	[[self layer] addAnimation:animation forKey:@"fade"];
    [self setHidden: NO];
    
    [self becomeFirstResponder];
}


- (void) fadeOut: (float) duration
{
    CATransition *animation = [CATransition animation];
	[animation setDelegate:self];	
    [animation setSubtype: kCATransitionFade];	
	[animation setDuration:duration];
	[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];	
	[[self layer] addAnimation:animation forKey:@"fade"];
    [self setHidden: YES];
    
    [self resignFirstResponder];
}
    





@end
