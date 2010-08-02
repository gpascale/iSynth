//
//  OptionsViewController.m
//  Phonosynth
//
//  Created by Greg on 12/13/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import  "OptionsViewController.h"
#include <PSSettings.h>
#include <Functions.h>
#include <Popup.h>
#include <common.h>

@interface OptionsViewController (Private)
- (void) revertTo16BitColors: (UISegmentedControl*) segControl;
@end

@implementation OptionsViewController


- (void) viewDidLoad
{	
	// fill in starting values from settings
	[m_transitionSpeedSlider setValue: 5.5f - PSSettings::Settings().getTransitionSpeed()];
	[m_slideshowSpeedSlider setValue: 10.5f - PSSettings::Settings().getSlideshowSpeed()];
	[m_autoLockSwitch setOn: PSSettings::Settings().getAutoLock()];
	[m_buttonDescriptionsSwitch setOn: PSSettings::Settings().getButtonDescriptions()];
	[m_autorotationSwitch setOn: PSSettings::Settings().getAutorotation()];
	[m_colorDepthControl setSelectedSegmentIndex: PSSettings::Settings().getColorDepth() == 16 ? 0 : 1];
	
	const int numPeers = PSSettings::Settings().getNumPeers();
	switch (numPeers) {
		case 0:
			[m_numPeersControl setSelectedSegmentIndex: 0];
			break;
		case 3:
			[m_numPeersControl setSelectedSegmentIndex: 1];
			break;
		case 5:
			[m_numPeersControl setSelectedSegmentIndex: 2];
			break;
		case 10000:
			[m_numPeersControl setSelectedSegmentIndex: 3];
			break;
		default:
			ASSERT(0 && "WTF???");
			[m_numPeersControl setSelectedSegmentIndex: 1];
			break;
	}
	
	const int numThreads = PSSettings::Settings().getNumThreads();	
	ASSERT(numThreads >= 1 && numThreads <= 3);	
	[m_numThreadsControl setSelectedSegmentIndex: numThreads - 1];
	
	NSString* path = [[NSBundle mainBundle] pathForResource: @"Icon_Photosynth" ofType: @"png"];
	[m_backgroundView setImage: [UIImage imageWithContentsOfFile: path]];
}


- (void) viewWillDisappear: (BOOL) animated
{
	// write out the settings when the user is done configuring them
	PSSettings::Settings().writeSettings();
}


- (void) viewDidDisappear: (BOOL) animated
{
	[m_backgroundView setImage: nil];
	[self setView: nil];
}


- (void) setTopLayer: (MacTopLayer*) topLayer
{
    m_topLayer = topLayer;
}







- (IBAction) slideshowSpeedChanged: (id) sender 
{    
	UISlider* slider = (UISlider*) sender;
	GUI_PRINTF("slideshowSpeedChanged, to %f\n", slider.value);
	PSSettings::Settings().setSlideshowSpeed([slider maximumValue] + 0.5f - slider.value);
}



- (IBAction) transitionSpeedChanged: (id) sender
{    
	UISlider* slider = (UISlider*) sender;
	GUI_PRINTF("transitionSpeedChanged, to %f\n", slider.value);
	PSSettings::Settings().setTransitionSpeed([slider maximumValue] + 0.5f - slider.value);
}



- (IBAction) autoLockChanged: (id) sender
{    
	UISwitch* sw = (UISwitch*) sender;
	GUI_PRINTF("autoLockChanged, to %f\n", sw.on);
	PSSettings::Settings().setAutoLock((bool) sw.on);
}


void revertTo16BitColors(UISegmentedControl* segControl)
{
	[segControl setSelectedSegmentIndex: 0];
}


- (IBAction) colorDepthChanged: (id) sender
{
	UISegmentedControl* segControl = (UISegmentedControl*) sender;
	int newColorDepth = [segControl selectedSegmentIndex] == 0 ? 16 : 32;
	PSSettings::Settings().setColorDepth(newColorDepth);
	
	if(newColorDepth == 32 && [self.navigationController topViewController] == self){
		
		static const string warningStr = string("Using 32-bit colors will make photos look marginally nicer but will use much more memory") +
										string(" and cause iSynth to run more slowly. Are you sure?");
	
		Popup* popup = Popup::TwoButtonPopup("Warning", warningStr,
											 Functor::Identity(), "Yes",
											 StaticFunctorA<UISegmentedControl*>(&revertTo16BitColors, segControl), "No");
		popup->show();
		
	}
}


- (IBAction) buttonDescriptionsChanged: (id) sender
{
	UISwitch* sw = (UISwitch*) sender;
	PSSettings::Settings().setButtonDescriptions((bool) sw.on);
}


- (IBAction) autorotationChanged: (id) sender
{
	UISwitch* sw = (UISwitch*) sender;
	PSSettings::Settings().setAutorotation((bool) sw.on);
}


- (IBAction) numNeighborsChanged: (id) sender
{
	static const int n[4] = {0, 3, 5, 10000};
	
	UISegmentedControl* segControl = (UISegmentedControl*) sender;
	PSSettings::Settings().setNumPeers(n[[segControl selectedSegmentIndex]]);
}


void revertTo1Thread(UISegmentedControl* segControl)
{
	[segControl setSelectedSegmentIndex: 0];
}


- (IBAction) numThreadsChanged: (id) sender
{
	int oldVal = PSSettings::Settings().getNumThreads();
	
	UISegmentedControl* segControl = (UISegmentedControl*) sender;
	PSSettings::Settings().setNumThreads([segControl selectedSegmentIndex] + 1);
	
	if(oldVal == 1 && PSSettings::Settings().getNumThreads() != 1){
		
		static const string warningStr = string("Setting this value to anything greater than one will make photos sharpen faster, but will also cause iSynth to run much less smoothly. Are you sure?");
		
		Popup* popup = Popup::TwoButtonPopup("Warning", warningStr,
											 Functor::Identity(), "Yes",
											 StaticFunctorA<UISegmentedControl*>(&revertTo1Thread, segControl), "No");
		popup->show();
		
	}
}
	

- (IBAction) restoreDefaultsButtonClicked
{
	Popup* p = Popup::TwoButtonPopup("", "Are you sure you want to restore the default settings?",
									 ObjCFunctor(self, @selector(restoreDefaults), nil), "Yes", 
									 Functor::Identity(), "No");
	p->show();
}


- (IBAction) infoButtonClicked: (id) sender
{
	string msg;
	
	UIButton* b = (UIButton*) sender;
	switch (b.tag)
	{
		case 0:  // transition speead
			msg = "Adjusts the speed with which you move from photo to photo.";
			break;
		case 1:
			msg = "Adjusts the speed of the slideshow when it is playing.";
			break;
		case 2:
			msg = "Toggles whether or not the device will automatically lock itself after a period of inactivity. iSynth uses significantly less battery power when the device is locked.";
			break;
		case 3:
			msg = "When on, you can hold down any button in the viewer to reveal a description of what it does. If you hold a button down for longer than a moment, it will not count as a press when you lift your finger.";
			break;
		case 4:
			msg = "Turn on to enable or off to disable automatic rotation of the screen when the device is tilted. You can still rotate the display using a button in the (i) menu in the viewer.";
			break;
		case 5:
			msg = "Adjusts color quality of photos. 32-bit color looks nicer but uses more memory and will cause iSynth to run more slowly and consume battery power more quickly.";
			break;
		case 6:
			msg = "Adjusts the maximum number of photos that appear at once. Turn to 0 to see only the currently focused photo. U (for Unlimited) will show all of the neighboring photos. The fewer photos shown, the more smoothly iSynth will run.";
			break;
		case 7:
			msg = "Adjusts the number of concurrent photo downloads iSynth will perform while you view a synth. Setting this to 2 or 3 will cause photos to sharpen faster, but will make the viewing experience much less smooth. In particular, you may notice shakes or jitters when moving from photo to photo.";
	}
	
	Popup* p = Popup::OneButtonPopup("", msg, Functor::Identity(), "Ok");
	p->show();	
}


- (void) restoreDefaults
{
	[m_transitionSpeedSlider setValue: 4.0f];
	[self transitionSpeedChanged: m_transitionSpeedSlider];
	
	[m_slideshowSpeedSlider setValue: 7.5f];
	[self slideshowSpeedChanged: m_slideshowSpeedSlider];
	
	[m_autoLockSwitch setOn: YES];
	[self autoLockChanged: m_autoLockSwitch];
	
	[m_buttonDescriptionsSwitch setOn: YES];
	[self buttonDescriptionsChanged: m_buttonDescriptionsSwitch];
	
	[m_autorotationSwitch setOn: YES];
	[self autorotationChanged: m_autorotationSwitch];
	
	[m_colorDepthControl setSelectedSegmentIndex: 0];
	[self colorDepthChanged: m_colorDepthControl];
	
	[m_numPeersControl setSelectedSegmentIndex: 1];
	[self numNeighborsChanged: m_numPeersControl];
	
	[m_numThreadsControl setSelectedSegmentIndex: 0];
	[self numThreadsChanged: m_numThreadsControl];
	
	PSSettings::Settings().writeSettings();
}


@end
