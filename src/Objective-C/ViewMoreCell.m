//
//  ViewMoreCell.m
//  Phonosynth
//
//  Created by Greg on 12/14/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import "ViewMoreCell.h"


@implementation ViewMoreCell

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier {
    if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier]) {
        // Initialization code
    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated {

    [super setSelected:selected animated:animated];
}


- (void)dealloc {
    [super dealloc];
}



- (void) setMainLabelText: (NSString*) text
{
    [m_mainLabel setText: text];
}


- (void) setSubLabelText: (NSString*) text
{
    [m_subLabel setText: text];
}


- (void) setMainLabelNum: (int) n
{
	if(n == -1)
		[m_mainLabel setText: @"More results"];
	else
		[m_mainLabel setText: [NSString stringWithFormat: @"%d more results", n]];
}


- (void) setSubLabelNum: (int) n
{
	if(n == -1)
		[m_subLabel setText: @"Tap to load more"];
	else
		[m_subLabel setText: [NSString stringWithFormat: @"Tap to load %d more", n]];
}

@end
