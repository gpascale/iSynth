//
//  ViewMoreCell.h
//  Phonosynth
//
//  Created by Greg on 12/14/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface ViewMoreCell : UITableViewCell {

    IBOutlet UILabel* m_mainLabel;
    IBOutlet UILabel* m_subLabel;
    
}

- (void) setMainLabelText: (NSString*) text;
- (void) setSubLabelText: (NSString*) text;

- (void) setMainLabelNum: (int) n;
- (void) setSubLabelNum: (int) n;

@end
