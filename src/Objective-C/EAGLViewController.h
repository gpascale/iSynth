//
//  EAGLViewController.h
//  Phonosynth
//
//  Created by Greg on 12/13/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import  <UIKit/UIKit.h>
#import  <OpenGLES/EAGL.h>
#import  <OpenGLES/ES1/gl.h>
#import  <OpenGLES/ES1/glext.h>
#import  "EAGLView.h"
#include <Color.h>

class MacTopLayer;
class InputHandler;

@interface EAGLViewController : UIViewController {

    // The pixel dimensions of the backbuffer
	int                   m_width;
	int                   m_height;
	
    EAGLSharegroup*       m_shareGroup;
	EAGLContext*          m_context;
	
	// OpenGL names for the renderbuffer and framebuffers used to render to this view
	unsigned int		  m_renderBuffer, m_frameBuffer;		
	
	InputHandler*		  m_inputHandler;	
    MacTopLayer*          m_topLayer;    
    
    IBOutlet EAGLView*    m_glView;

}


- (void) setTopLayer: (MacTopLayer*) topLayer;
- (void) setInputHandler: (InputHandler*) ih;
- (void) begin: (const Color4F*) clearColor;
- (void) begin;
- (void) present;

- (bool)      initializeGL: (EAGLSharegroup*) sharegroup;
- (EAGLView*) getUnderlyingView;

 
@end