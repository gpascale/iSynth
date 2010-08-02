//
//  EAGLViewController.m
//  Phonosynth
//
//  Created by Greg on 12/13/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import  "EAGLViewController.h"
#import  <OpenGLES/EAGL.h>
#import  <OpenGLES/ES1/gl.h>
#import  <OpenGLES/ES1/glext.h>
#include <ContextManager.h>
#include <GMath.h>
#include <PSSettings.h>
#include <Color.h>
#include <PSInputHandler.h>
#include <MacTopLayer.h>
#include <vector>


@interface EAGLView ()

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

@implementation EAGLViewController

/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc
{
    [super dealloc];
}


- (void) viewWillAppear: (BOOL) animated
{
    [super viewWillAppear: animated];
    [[self navigationController] setNavigationBarHidden: YES animated: YES];
}


- (void) viewDidDisappear: (BOOL) animated
{
	m_inputHandler->cleanup();
}


- (void) setTopLayer: (MacTopLayer*) topLayer
{
    m_topLayer = topLayer;
}


- (void) setInputHandler: (InputHandler*) ih
{
	//cout << "setInputHandler" << endl;
	m_inputHandler = ih;
}


- (EAGLView*) getUnderlyingView
{
    return m_glView;
}


- (bool) initializeGL: (EAGLSharegroup*) sharegroup
{
    m_shareGroup = sharegroup;        
    return true;
}

// You must implement this
+ (Class)layerClass {
	return [CAEAGLLayer class];
}



- (void) awakeFromNib
{	

}


- (bool) createContext: (EAGLSharegroup*) grp
{
    m_shareGroup = grp;        
    return true;
}


- (void) begin
{
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_frameBuffer);		
}


- (void) begin: (const Color4F*) clearColor
{		
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_frameBuffer);		
	glClearColor(clearColor->x, 
				 clearColor->y, 
				 clearColor->z, 
				 clearColor->w);    
	glClear(GL_COLOR_BUFFER_BIT);
}


- (void) present
{
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_renderBuffer);	
	[m_context presentRenderbuffer:GL_RENDERBUFFER_OES];
}



@end
