


#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

@class MainView;
@class EAGLView;
@class LoadingView;

class MacTopLayer;

@interface OptionsView : UIView {	
    MacTopLayer*            m_topLayer;
}



- (void)     setTopLayer: (MacTopLayer*) topLayer;

- (IBAction) slideshowSpeedChanged: (id) sender;
- (IBAction) transitionSpeedChanged: (id) sender;
- (IBAction) pointcloudSimplificationChanged: (id) sender;
- (IBAction) freeRotationChanged: (id) sender;
- (IBAction) autorotationChanged: (id) sender;
- (IBAction) multiViewTypeChanged: (id) sender;
- (IBAction) doneButtonClicked: (id) sender;


@end
