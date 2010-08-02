

#import "OptionsView.h"

#import "MainView.h"
#import "EAGLView.h"
#import "LoadingView.h"

#include <common.h>
#include <PSSettings.h>
#import <QuartzCore/QuartzCore.h>

#include <MacTopLayer.h>

@implementation OptionsView





- (IBAction) slideshowSpeedChanged: (id) sender 
{    
	UISlider* slider = (UISlider*) sender;
	GUI_PRINTF("slideshowSpeedChanged, to %f\n", slider.value);
	PSSettings::Settings().setSlideshowSpeed(5.5f - slider.value);
}



- (IBAction) transitionSpeedChanged: (id) sender
{    
	UISlider* slider = (UISlider*) sender;
	GUI_PRINTF("transitionSpeedChanged, to %f\n", slider.value);
	PSSettings::Settings().setTransitionSpeed(5.5f - slider.value);
}



- (IBAction) pointcloudSimplificationChanged: (id) sender
{    
	UISlider* slider = (UISlider*) sender;
	GUI_PRINTF("transitionSpeedChanged, to %f\n", slider.value);
	PSSettings::Settings().setPointcoloudSimplification(0.5f - slider.value);
}


- (IBAction) freeRotationChanged: (id) sender
{
	UISwitch* sw = (UISwitch*) sender;
	PSSettings::Settings().setFreeRotation(sw.on ? 1 : 0);	
}


- (IBAction) multiViewTypeChanged: (id) sender
{
	UISegmentedControl* segControl = (UISegmentedControl*) sender;
	MultiViewType type = [segControl selectedSegmentIndex] == 0 ? 
						 MULTIVIEW_PEERS : 
						 MULTIVIEW_TRAIL;
	PSSettings::Settings().setMultiViewType(type);
}


- (IBAction) autorotationChanged: (id) sender
{
	UISwitch* sw = (UISwitch*) sender;
	PSSettings::Settings().setAutorotation(sw.on ? 1 : 0);
}


- (IBAction) doneButtonClicked: (id) sender
{

}


- (void) setTopLayer: (MacTopLayer*) topLayer
{
    m_topLayer = topLayer;
}



@end
