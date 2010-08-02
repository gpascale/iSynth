//
//  OptionsViewController.h
//  Phonosynth
//
//  Created by Greg on 12/13/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import <UIKit/UIKit.h>

class MacTopLayer;

@interface OptionsViewController : UIViewController {
    MacTopLayer*                  m_topLayer;
	
	IBOutlet UISlider*			  m_transitionSpeedSlider;
	IBOutlet UISlider*			  m_slideshowSpeedSlider;
	IBOutlet UISwitch*			  m_autoLockSwitch;
	IBOutlet UISwitch*			  m_buttonDescriptionsSwitch;
	IBOutlet UISwitch*			  m_autorotationSwitch;
	IBOutlet UISegmentedControl*  m_colorDepthControl;
	IBOutlet UISegmentedControl*  m_numPeersControl;
	IBOutlet UISegmentedControl*  m_numThreadsControl;	
	
	IBOutlet UIImageView*		  m_backgroundView;
	
}

- (void) setTopLayer: (MacTopLayer*) topLayer;

- (void) restoreDefaults;

- (IBAction) slideshowSpeedChanged: (id) sender;
- (IBAction) transitionSpeedChanged: (id) sender;
- (IBAction) autoLockChanged: (id) sender;
- (IBAction) colorDepthChanged: (id) sender;
- (IBAction) buttonDescriptionsChanged: (id) sender;
- (IBAction) autorotationChanged: (id) sender;
- (IBAction) numNeighborsChanged: (id) sender;
- (IBAction) numThreadsChanged: (id) sender;

- (IBAction) restoreDefaultsButtonClicked;

- (IBAction) infoButtonClicked: (id) sender;

@end
