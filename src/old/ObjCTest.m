//
//  ObjCTest.m
//  Phonosynth
//
//  Created by Greg Pascale on 6/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "ObjCTest.h"

#if 0

@implementation ObjCTest

-(void) init: (EAGLView*) view
{
	m_view = view;
}


-(void) draw
{	
	m_topLayer->update();
}


-(void) run
{
	m_topLayer = new MacTopLayer();
	m_topLayer->setEAGLView(m_view);
	m_topLayer->run();
}


@end

#endif 0