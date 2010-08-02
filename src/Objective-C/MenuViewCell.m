//
//  MenuViewCell.m
//  Phonosynth
//
//  Created by Greg Pascale on 8/16/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import  "MenuViewCell.h"
#include <common.h>
#include <PSWeb.h>


@implementation MenuViewCell




- (void) setThumbnail: (UIImage*) img
{	
	ASSERT(img != nil);
	m_thumbnailView.image = img;
}


- (void) setName: (NSString*) name
{
	[m_nameLabel setText: name];
}


- (void) setSynthiness: (int) synthiness
{		
	const string s = i2s(synthiness) + "% synthy";
	[m_synthinessLabel setText: NSSTRING(s.c_str())];
}


- (void) setNumPhotos: (int) numPhotos
{
	const string s = i2s(numPhotos) + " photos";
	[m_numPhotosLabel setText: NSSTRING(s.c_str())];	
}


- (void) setAuthor: (NSString*) author
{
	[m_authorLabel setText: author];
}


- (void) setNumViews: (int) numViews
{
	const string s = i2s(numViews) + " views";
	[m_numViewsLabel setText: NSSTRING(s.c_str())];
}


- (void) fillWithWebCollection: (const WebCollection*) c
{	
	[self setName: NSSTRING(c->getName().c_str())];
	[self setAuthor: NSSTRING(c->getOwnerName().c_str())];
	[self setSynthiness: c->getSynthiness()];
	[self setNumViews: c->getNumViews()];
	[self setNumPhotos: c->getNumPhotos()];
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated {

	[super setSelected:selected animated:animated];

	// Configure the view for the selected state
}


- (void)dealloc {
	[m_thumbnailView release];
	[m_synthinessLabel release];
	[m_numPhotosLabel release];
	[m_authorLabel release];
	[m_numViewsLabel release];
	[super dealloc];
}


@end
