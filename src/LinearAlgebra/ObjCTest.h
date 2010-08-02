//
//  ObjCTest.h
//  Phonosynth
//
//  Created by Greg Pascale on 6/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <EAGLView.h>
#include <MacTopLayer.h>

@interface ObjCTest : NSObject {

	MacTopLayer *m_topLayer;
	EAGLView *m_view;
	
}

- (void) init: (EAGLView*) view;
- (void) draw;
- (void) run;

@end