//
//  EAGLView.m
//  Phonosynth
//
//  Created by Greg Pascale on 6/10/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//


#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import "EAGLView.h"

#include "GMath.h"
#include "Engine.h"
#include <PSMaster.h>
#include <TopLayer.h>
#include <MacTopLayer.h>
#include <GLDrawer.h>
#include <iostream>
#include <PSInputHandler.h>
#include <ContextManager.h>

#define USE_DEPTH_BUFFER		0


// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animationTimer;

@end


@implementation EAGLView

@synthesize context;
@synthesize animationTimer;
@synthesize animationInterval;



// You must implement this
+ (Class)layerClass {
	return [CAEAGLLayer class];
}


//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id) initWithCoder: (NSCoder*) coder {		
	
	if (self = [super initWithCoder:coder]) {
		// Get the layer
		CAEAGLLayer *eaglLayer = (CAEAGLLayer *) self.layer;
		
		eaglLayer.opaque = YES;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], 
                                        kEAGLDrawablePropertyRetainedBacking, 
                                        kEAGLColorFormatRGB565, 
                                        kEAGLDrawablePropertyColorFormat, 
                                        nil];
                
		/*
		ContextManager::activateRenderContext();			
		 */		
		context = ContextManager::getRenderContext();
		ContextManager::activateRenderContext();
		[self destroyFramebuffers];
		[self createFramebuffers];
		
		self.multipleTouchEnabled = true;
		
		return self;
		
	}		
		
}

- (bool) createContext: (EAGLSharegroup*) grp
{
    shareGroup = grp;        
    return true;
}



- (void) beginOnscreen
{
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
}


- (void) beginOnscreen: (const Color4F*) clearColor
{	
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);	
	
	glClearColor(clearColor->x, 
				 clearColor->y, 
				 clearColor->z, 
				 clearColor->w);
    
	glClear(GL_COLOR_BUFFER_BIT);

}

- (void) beginOffscreen
{
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, offscreenFramebuffer);
}


- (void) beginOffscreen: (const Color4F*) clearColor
{	
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, offscreenFramebuffer);	
	
	glClearColor(clearColor->x, 
				 clearColor->y, 
				 clearColor->z, 
				 clearColor->w);
    
	glClear(GL_COLOR_BUFFER_BIT);

}


- (void) present
{ 
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer: GL_RENDERBUFFER_OES];
}


- (BOOL)createFramebuffers {	
	
	//ASSERT([NSThread currentThread] != [NSThread mainThread]);
	
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);
				
	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		printf("failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	
	//cerr << "FRAME: " << endl;
	//cerr << "X: " << self.frame.origin.x << endl;
	//cerr << "Y: " << self.frame.origin.y << endl;
	//cerr << "W: " << self.frame.size.width << endl;
	//cerr << "H: " << self.frame.size.height << endl;
	
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*) self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
	
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	glViewport(0, 0, backingWidth, backingHeight);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);	
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glPointSize(0.1f);
	
	glActiveTexture(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glActiveTexture(GL_TEXTURE1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glActiveTexture(GL_TEXTURE0);
	
	
	/* BEGIN OFFSCREEN FRAMEBUFFER SHIT */	
	
	glGenFramebuffersOES(1, &offscreenFramebuffer);		
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, offscreenFramebuffer);     
	glBindTexture(GL_TEXTURE_2D, OFFSCREEN_TEXTURE_ID); // use texture ID 1001 since regular texture IDs don't go that high
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, OFFSCREEN_TEXTURE_ID);		
	glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, OFFSCREEN_TEXTURE_ID, 0);		
	ASSERT(glGetError() == 0);
	ASSERT(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) == GL_FRAMEBUFFER_COMPLETE_OES);	
	 
	/* END OFFSCREEN FRAMEBUFFER SHIT */
		

	return YES;
}


- (void)destroyFramebuffers {
	
	//ASSERT([NSThread currentThread] != [NSThread mainThread]);
	
	glDeleteFramebuffersOES(1, &viewFramebuffer);
	viewFramebuffer = 0;
	glDeleteRenderbuffersOES(1, &viewRenderbuffer);
	viewRenderbuffer = 0;
	glDeleteFramebuffersOES(1, &offscreenFramebuffer);
	offscreenFramebuffer = 0;
}


- (void)dealloc {
	
	if ([EAGLContext currentContext] == context) {
		[EAGLContext setCurrentContext:nil];
	}
	
	[context release];	
	[super dealloc];
}


- (void) setInputHandler: (InputHandler*) ih
{
	inputHandler = ih;
}



- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{	    
    inputHandler->touchesBegan(touches, event, self);
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{	
    inputHandler->touchesMoved(touches, event, self);
}


- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
    inputHandler->touchesEnded(touches, event, self);
}

- (void) setTopLayer: (MacTopLayer*) topLayer
{
    m_topLayer = topLayer;
}

@end


#undef USE_DEPTH_BUFFER
#undef OFFSCREEN_TEXTURE_ID

