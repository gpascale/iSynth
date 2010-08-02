//
//  AboutViewController.m
//  Phonosynth
//
//  Created by Greg on 2/18/09.
//  Copyright 2009 Brown University. All rights reserved.
//

#import  "AboutViewController.h"
#include <iostream>
#include <common.h>
#include <PhonosynthAppDelegate.h>
#include <Popup.h>



@implementation CopyrightViewController


		
- (void) didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; 
}


- (void) viewDidDisappear: (BOOL) animated
{
	//[self setView: nil];
	[super viewDidDisappear: animated];
}


- (void) viewWillAppear: (BOOL) animated
{
	[super viewWillAppear: animated];
}


- (void) loadView
{
	//cerr << "Loading view, nib file is " << CSTRING([self nibName]) << endl;
	[super loadView];
}


- (void) viewDidLoad
{
	[super viewDidLoad];
}


- (IBAction) launchWebsite
{
	UIApplication* app = [PhonosynthAppDelegate getUIApplication];
	
	NSURL* url = [[NSURL alloc] initWithString: @"http://cs.brown.edu/people/gpascale/iSynth"];
	
	Popup* p = Popup::TwoButtonPopup("", "Launching the website will exit iSynth. Are you sure?",
									 ObjCFunctor(app, @selector(openURL:), url), "Yes",
									 Functor::Identity(), "No");
	p->show();
	
}

	 
	
	
@end // CopyrightViewController



@implementation HelpViewController



- (void) didReceiveMemoryWarning {
	[super didReceiveMemoryWarning]; 
}


- (void) loadView
{
	[self setView: [[UIWebView alloc] init]];
}


- (void) viewDidLoad
{
	NSURL* url = [NSURL URLWithString: @"http://www.cs.brown.edu/people/gpascale/iSynth/help.html"];
	NSURLRequest* request = [NSURLRequest requestWithURL: url];
	[self.view loadRequest: request];
	[super viewDidLoad];
}


- (void) viewWillAppear: (BOOL) animated
{
	[super viewWillAppear: animated];
}


- (void) viewDidDisappear: (BOOL) animated
{
	[super viewDidDisappear: animated];
}

@end // HelpViewController











@implementation AboutViewController


- (void) awakeFromNib
{
	[self setDelegate: self];
}


- (void) tabBarController: (UITabBarController *) tabBarController 
  didSelectViewController: (UIViewController *) viewController
{
	if(viewController == m_helpViewController){
		[[self navigationItem] setTitle: @"Help"];
	}
	else{
		[[self navigationItem] setTitle: @"About"];
	}
}



/*
// UIWebViewDelegate methods

- (void) webViewDidStartLoad: (UIWebView *) webView
{
	//cerr << "WebView finished loading" << endl;
}


- (void) webViewDidFinishLoad: (UIWebView *) webView
{
	//cerr << "WebView finished loading" << endl;
}


- (void) webView: (UIWebView *) webView didFailLoadWithError: (NSError *) error
{
	//cerr << "WebView failed to load" << endl;
	//cerr << "Error was " << CSTRING([error localizedDescription]) << endl;
}
*/



@end
