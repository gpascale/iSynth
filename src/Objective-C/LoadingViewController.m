//
//  LoadingViewController.m
//  Phonosynth
//
//  Created by Greg on 12/13/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import "LoadingViewController.h"



@implementation LoadingViewController



- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; 
}


- (void)dealloc {
    [super dealloc];
}


- (void) viewDidLoad
{
	NSString* path = [[NSBundle mainBundle] pathForResource: @"Icon_Photosynth" ofType: @"png"];
	[m_imageView setImage: [UIImage imageWithContentsOfFile: path]];
}


- (void) viewWillAppear: (BOOL) animated
{
    [super viewWillAppear: animated];
}


- (void) viewDidDisappear: (BOOL) animated
{
	[self setView: nil];
	[super viewDidDisappear: animated];
}


- (void) setTopLayer: (MacTopLayer*) topLayer
{
    m_topLayer = topLayer;
}


- (void) setStatusText: (NSString*) s
{
	if(self.view == nil)
		[self loadView];
	
	m_textView.text = s;	
}

- (void) appendStatusText: (NSString*) s
{
	if(self.view == nil)
		[self loadView];
	
	m_textView.text = [m_textView.text stringByAppendingString: s];	
	printf("appended %s\n", [s UTF8String]);
	NSRange range;
	range.location = [m_textView.text length] - 1;
	[m_textView scrollRangeToVisible: range];
}


- (void) turnOnActivityThingy
{
	if(self.view == nil)
		[self loadView];
	[m_activityThingy startAnimating];
}


- (void) turnOffActivityThingy
{	
	[m_activityThingy stopAnimating];
}


- (IBAction) cancelLoading
{
    [[self navigationController] popViewControllerAnimated: YES];
}

@end
