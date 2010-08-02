//
//  main.m
//  Phonosynth
//
//  Created by Greg Pascale on 6/10/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import  <UIKit/UIKit.h>

NSAutoreleasePool* g_mainPool;

int main(int argc, char *argv[]) {
	g_mainPool = [[NSAutoreleasePool alloc] init];
	srand(time(NULL));
	int retVal = UIApplicationMain(argc, argv, nil, nil);            
	[g_mainPool release];
	return retVal;
}
