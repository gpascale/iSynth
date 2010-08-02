//
//  ObjCCommon.m
//  Phonosynth
//
//  Created by Greg on 12/28/08.
//  Copyright 2008 Brown University. All rights reserved.
//


#import  "ObjCCommon.h"
#include <common.h>

@implementation UIView (Common)

+ (UIViewAnimationTransition) randomUIViewAnimationTransition
{
    static const UIViewAnimationTransition animations[4] = {UIViewAnimationTransitionFlipFromLeft,
        UIViewAnimationTransitionFlipFromRight,
        UIViewAnimationTransitionCurlUp,
    UIViewAnimationTransitionCurlDown};
    return animations[randInt(0, 3)];
}    


+ (UIViewAnimationCurve) randomUIViewAnimationCurve
{
    static const UIViewAnimationCurve curves[4] = {UIViewAnimationCurveEaseInOut,
        UIViewAnimationCurveEaseIn,
        UIViewAnimationCurveEaseOut,
    UIViewAnimationCurveLinear};
    return curves[randInt(0, 3)];
} 

+ (NSString*) randomCAAnimation
{
    static const NSString* animations[4] = {kCATransitionFade,
											kCATransitionMoveIn,
											kCATransitionPush,
											kCATransitionReveal};
    return animations[randInt(0, 3)];    
}

+ (NSString*) randomCAAnimationCurve
{
    static const NSString* curves[4] = {kCAMediaTimingFunctionLinear,
										kCAMediaTimingFunctionEaseIn,
										kCAMediaTimingFunctionEaseOut,
										kCAMediaTimingFunctionEaseInEaseOut};
    return curves[randInt(0, 3)];    
}


- (void) addSubview: (UIView*) view
      withAnimation: (NSString*) animation  
     animationCurve: (NSString*) animationCurve
           duration: (float) duration
{		
    CAAnimation* caAnim = [CATransition animation];
    [caAnim setDelegate: self];
    [caAnim setType: kCATransitionFade];	
	[caAnim setDuration: duration];
	[caAnim setTimingFunction: [CAMediaTimingFunction functionWithName: animationCurve]];
	[[self layer] addAnimation: caAnim forKey: @"options view fade in"];
    [self addSubview: view];
}


- (bool) removeSubview: (UIView*) view
         withAnimation: (NSString*) animation  
        animationCurve: (NSString*) animationCurve
              duration: (float) duration
{
    // make sure the parent view actually contains the view to remove
    bool hasViewAsChild = false;    
    const NSArray* subviews = [self subviews];
    FOR(i,0,[subviews count]){
        if([subviews objectAtIndex: i] == view){
            hasViewAsChild = true;
            break;
        }
    }
    
    // if it does, remove the subview from its superview
    if(hasViewAsChild){    
        CAAnimation* caAnim = [CATransition animation];
        [caAnim setDelegate: self];
        [caAnim setType: animation];	
        [caAnim setDuration: duration];
        [caAnim setTimingFunction: [CAMediaTimingFunction functionWithName: animationCurve]];
        [[self layer] addAnimation: caAnim forKey: @"options view fade in"];
        [view removeFromSuperview];
    }
    
    return hasViewAsChild;
}


- (void) fadeIn: (float) duration
{
	CATransition *animation = [CATransition animation];
	[animation setDelegate:self];	
    [animation setSubtype: kCATransitionFade];	
	[animation setDuration:duration];
	[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];	
	[[self layer] addAnimation:animation forKey:@"FadeInAnimation"];
    [self setHidden: NO];
}


- (void) fadeOut: (float) duration
{
	CATransition *animation = [CATransition animation];
	[animation setDelegate:self];	
    [animation setSubtype: kCATransitionFade];	
	[animation setDuration:duration];
	[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];	
	[[self layer] addAnimation:animation forKey:@"FadeOutAnimation"];
    [self setHidden: YES];
}


// use the tag to store my own orientation information. What a clever hacker I am
- (void) setOrientation: (int) newOrientation
{
    ASSERT(newOrientation >= 0 && newOrientation <= 3);
    self.tag = newOrientation;
}


- (int) getOrientation
{
    return self.tag;
}


@end // implementation UIView (Common)





@implementation UINavigationController (Common)


- (void) pushViewController: (UIViewController*) viewController
              withAnimation: (UIViewAnimationTransition) animation
             animationCurve: (UIViewAnimationCurve) curve
                   duration: (float) duration
{
	[self pushViewController: viewController
			   withAnimation: animation
			  animationCurve: curve
					duration: duration
					   cache: NO];
}


- (void) pushViewController: (UIViewController*) viewController
              withAnimation: (UIViewAnimationTransition) animation
             animationCurve: (UIViewAnimationCurve) curve
                   duration: (float) duration
					  cache: (BOOL) cache
{
    /* configure the animation block */
    [UIView beginAnimations: nil context: NULL];
    [UIView setAnimationTransition: animation
                           forView: [self view]
                             cache: cache];
    [UIView setAnimationCurve: curve];
    [UIView setAnimationDuration: duration];
    
    /* push the new view controller and fire the animation */
    [self pushViewController: viewController
                    animated: NO];
    [UIView commitAnimations];
}


- (bool) popToViewController: (UIViewController*) viewController
			   withAnimation: (UIViewAnimationTransition) animation
			  animationCurve: (UIViewAnimationCurve) curve
					duration: (float) duration
{
	return [self popToViewController: viewController
					   withAnimation: animation
					  animationCurve: curve
							duration: duration
							   cache: NO];
}


- (bool) popToViewController: (UIViewController*) viewController
               withAnimation: (UIViewAnimationTransition) animation
              animationCurve: (UIViewAnimationCurve) curve
                    duration: (float) duration
					   cache: (BOOL) cache
{    
    /* configure the animation block */
    [UIView beginAnimations: nil context: NULL];
    [UIView setAnimationTransition: animation
                           forView: [self view]
                             cache: cache];
    [UIView setAnimationCurve: curve];
    [UIView setAnimationDuration: duration];
    
    /* push the new view controller and fire the animation */
    [self popToViewController: viewController animated: NO];
    [UIView commitAnimations];    
}              

@end // implementation UINavigationController (Common)




@implementation UITouch (Common)

void transform(CGPoint& p, UIView* view)
{
    switch([view getOrientation])
    {
        case 0:
            break;
        case 1:
            swap(p.x, p.y);
            p.y = view.bounds.size.width - p.y;
            break;
        case 2:
            p.x = view.bounds.size.width - p.x;
            p.y = view.bounds.size.height - p.y;
            break;
        case 3:
            swap(p.x, p.y);
			p.x = view.bounds.size.height - p.x;
            break;
        default:
            printf("UIView has invalid orientation, not between 0 and 3!");
            exit(1);		
    }
}

- (CGPoint) transformedLocationInView: (UIView*) view
{
    CGPoint ret = [self locationInView: view];
    transform(ret, view);
    return ret;
}

- (CGPoint) transformedPreviousLocationInView: (UIView*) view
{
    CGPoint ret = [self previousLocationInView: view];
    transform(ret, view);
    return ret;
}

@end // implementation UITouch (Common)



@implementation NSArray (Common)

- (id) randomElement
{
	return [self objectAtIndex: randInt(0, [self count] - 1)];
}


- (void) printS
{
	const int n = [self count];
	
	//cerr << "{";
	//FOR(i,0,n-1)
		//cerr << CSTRING([self objectAtIndex: i]) << ", ";
	//if(n > 0)
		//cerr << CSTRING([self objectAtIndex: n - 1]);
	//cerr << "}";
}
	
@end











