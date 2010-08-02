/*
 *  ObjCCommon.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 12/24/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _OBJCCOMMON_H_
#define _OBJCCOMMON_H_


#import  <QuartzCore/QuartzCore.h>
#import  <UIKit/UIKit.h>


// This category extends UIView to include some helpful functions for adding 
// and removing subviews using various animations

@interface UIView (Common)


+ (UIViewAnimationTransition) randomUIViewAnimationTransition;
+ (UIViewAnimationCurve)      randomUIViewAnimationCurve;
+ (NSString*)                 randomCAAnimation;
+ (NSString*)                 randomCAAnimationCurve;

// adds 'view' as a subview of this UIView using the specified animation information
- (void) addSubview: (UIView*) view
      withAnimation: (NSString*) animation
     animationCurve: (NSString*) animationCurve
           duration: (float) duration;

// removes 'view'from this UIView using the specified animation information. First checks
// to see if 'view' actually is a subview. If not, silently returns false
- (bool) removeSubview: (UIView*) view
         withAnimation: (NSString*) animation
        animationCurve: (NSString*) animationCurve
              duration: (float) duration;

- (void) setOrientation: (int) newOrientation;
- (int)  getOrientation;

// encapsulates the Core Animation garbage involved in simple fade-in/fade-out animations
- (void) fadeIn:  (float) duration;
- (void) fadeOut: (float) duration;

@end // interface UIView (Common)






// This category extends UINavigationController to include some helpful functions
// for pushing and popping ViewControllers with custom animations

@interface UINavigationController (Common)

- (void) pushViewController: (UIViewController*) viewController
              withAnimation: (UIViewAnimationTransition) animation
             animationCurve: (UIViewAnimationCurve) curve
                   duration: (float) duration;

- (void) pushViewController: (UIViewController*) viewController
              withAnimation: (UIViewAnimationTransition) animation
             animationCurve: (UIViewAnimationCurve) curve
                   duration: (float) duration
					  cache: (BOOL) cache;



- (bool) popToViewController: (UIViewController*) viewController
               withAnimation: (UIViewAnimationTransition) animation
              animationCurve: (UIViewAnimationCurve) curve
                    duration: (float) duration;

- (bool) popToViewController: (UIViewController*) viewController
               withAnimation: (UIViewAnimationTransition) animation
              animationCurve: (UIViewAnimationCurve) curve
                    duration: (float) duration
					   cache: (BOOL) cache;

@end // interface UINavigationController (Common)




// This category extends UITouch, adding a couple functions for retrieving touch locations 
// pretransformed according to the screen's orientation. These rely on the use of the view's
// tag parameter to store the orientation, so if that isn't done, they won't work

@interface UITouch (Common)

// returns the location of the touch according to the given screen rotation
// For simplicity, 0 = portrait, 1 = lanscape, ...
- (CGPoint) transformedLocationInView: (UIView*) view;

// same thing for previous touch location
- (CGPoint) transformedPreviousLocationInView: (UIView*) view;

@end // interface UITouch (Common)



@interface NSThread (Common)

- (void) setAutoreleasePool: (NSAutoreleasePool*) pool;

@end // interface NSThread (Common)


@interface NSArray (Common)

- (id)   randomElement;
- (void) printS;

@end // interface NSArray (Common)


#endif // _OBJCCOMMON_H_