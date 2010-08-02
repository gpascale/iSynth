//
//  EAGLView.h
//  Phonosynth
//
//  Created by Greg Pascale on 6/10/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OptionsView.h>
#include <GMath.h>
#include <PSSettings.h>
#include <Color.h>

class InputHandler;
class MacTopLayer;

#define OFFSCREEN_TEXTURE_ID	1001

@interface EAGLView : UIView {
	
	
@private
	/* The pixel dimensions of the backbuffer */
	GLint				  backingWidth;
	GLint				  backingHeight;
	
    EAGLSharegroup*       shareGroup;
	EAGLContext*          context;
	
	/* OpenGL names for the renderbuffer and framebuffers used to render to this view */
	GLuint				  viewRenderbuffer, viewFramebuffer;
	GLuint				  offscreenFramebuffer;
	
	/* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
	GLuint				  depthRenderbuffer;
	
	NSTimer				  *animationTimer;
	NSTimeInterval		  animationInterval;
	
	InputHandler*		  inputHandler;
	
	IBOutlet OptionsView* optionsView;	
    MacTopLayer*          m_topLayer;
}

@property NSTimeInterval animationInterval;

- (BOOL) createFramebuffers;
- (void) destroyFramebuffers;

- (bool) createContext: (EAGLSharegroup*) grp;
- (void) setTopLayer: (MacTopLayer*) topLayer;
- (void) setInputHandler: (InputHandler*) ih;

- (void) beginOnscreen: (const Color4F*) clearColor;
- (void) beginOnscreen;

- (void) beginOffscreen: (const Color4F*) clearColor;
- (void) beginOffscreen;

- (void) present;


@end
