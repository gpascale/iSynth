//
//  LoadingViewController.h
//  Phonosynth
//
//  Created by Greg on 12/13/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import <UIKit/UIKit.h>

class MacTopLayer;

@interface LoadingViewController : UIViewController {

    IBOutlet UIActivityIndicatorView*	m_activityThingy;
    IBOutlet UIButton*					m_cancelButton;
    IBOutlet UITextView*				m_textView;
	IBOutlet UIImageView*				m_imageView;
    
    MacTopLayer*                        m_topLayer;
    
    NSTimer*							m_progressTimer;
}

- (void)            setTopLayer: (MacTopLayer*) topLayer;

- (void)			turnOnActivityThingy;
- (void)			turnOffActivityThingy;

- (void)			appendStatusText: (NSString*) s;
- (void)			setStatusText: (NSString*) s;

- (IBAction)		cancelLoading;

@property(nonatomic, retain) NSTimer* m_progressTimer;

@end
