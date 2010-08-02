/*
 *  PSSettings.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/13/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "PSSettings.h"
#include <PSMaster.h>
#include <PSGlobalState.h>

static const float MIN_TRANSITION_SPEED = 0.5f;
static const float MAX_TRANSITION_SPEED = 5.0f;

static const float MIN_SLIDESHOW_SPEED = 0.5f;
static const float MAX_SLIDESHOW_SPEED = 10.0f;

static const float MIN_POINTCLOUD_SIMP = 0.0f;

PSSettings::PSSettings()
{
	readSettings();
}


PSSettings::PSSettings(const PSSettings& rhs) :
	m_transitionSpeed( rhs.getTransitionSpeed() ),
	m_slideshowSpeed( rhs.getSlideshowSpeed() ),
	m_autoLock( rhs.getAutoLock() ),
	m_buttonDescriptions( rhs.getButtonDescriptions() ),
	m_autorotation( rhs.getAutorotation() ),
	m_colorDepth( rhs.getColorDepth() ),
	m_numPeers( rhs.getNumPeers() ),
	m_numThreads( rhs.getNumThreads() )
{
	
}



PSSettings::~PSSettings()
{
	
}


void registerDefaults()
{	
	
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	
	id K[8] = {@"transitionSpeed",
			   @"slideshowSpeed",
			   @"buttonDescriptions",
			   @"autoLock",
			   @"autoScreenRotation",
			   @"colorDepth",
			   @"numPeers",
			   @"numThreads"};
	
	id V[8] = {[NSNumber numberWithFloat: 4.0f],
			   [NSNumber numberWithFloat: 7.5f],
			   [NSNumber numberWithBool: YES],
			   [NSNumber numberWithBool: YES],
			   [NSNumber numberWithBool: YES],
			   [NSNumber numberWithInt: 16],
			   [NSNumber numberWithInt: 3],
			   [NSNumber numberWithInt: 1]};

	NSDictionary* d = [NSDictionary dictionaryWithObjects: V
												  forKeys: K
													count: 8];
		
	[defaults registerDefaults: d];

}

void
PSSettings::readSettings()
{			
	
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	
	// register defaults if it's the first run
	registerDefaults();
	
	//cerr << "raw transition speed is " << [defaults floatForKey: @"transitionSpeed"] << endl;
	m_transitionSpeed = MAX_TRANSITION_SPEED + 0.5f - [defaults floatForKey: @"transitionSpeed"];
	//cerr << "transition speed is " << m_transitionSpeed << endl;
	
	//cerr << "raw slideshow speed is " << [defaults floatForKey: @"slideshowSpeed"] << endl;
	m_slideshowSpeed = MAX_SLIDESHOW_SPEED + 0.5f - [defaults floatForKey: @"slideshowSpeed"];
	//cerr << "slideshow speed is " << m_slideshowSpeed << endl;
	
	m_autoLock = [defaults floatForKey: @"autoLock"];
	//cerr << "auto lock is " << m_autoLock << endl;
	
	m_buttonDescriptions = [defaults boolForKey: @"buttonDescriptions"];
	//cerr << "button descriptions is " << m_buttonDescriptions << endl;
	
	m_autorotation = [defaults boolForKey: @"autoScreenRotation"];
	//cerr << "autorotation is " << m_autorotation << endl;
	
	m_colorDepth = [defaults integerForKey: @"colorDepth"];
	//cerr << "color depth is " << m_colorDepth << endl;
	
	m_numPeers = [defaults integerForKey: @"numPeers"];
	//cerr << "num peers is " << m_numPeers << endl;
	
	m_numThreads = [defaults integerForKey: @"numThreads"];
	//cerr << "num threads is " << m_numThreads << endl;
	 
}


void
PSSettings::writeSettings()
{

	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	
	[defaults setFloat: MAX_TRANSITION_SPEED + 0.5f - m_transitionSpeed forKey: @"transitionSpeed"];
	//cerr << "set transition speed to " << [defaults floatForKey: @"transitionSpeed"] << endl;
	
	[defaults setFloat: MAX_SLIDESHOW_SPEED + 0.5f - m_slideshowSpeed forKey: @"slideshowSpeed"];
	//cerr << "set slideshow speed to " << [defaults floatForKey: @"slideshowSpeed"] << endl;
	
	[defaults setBool: m_autoLock forKey: @"autoLock"];
	//cerr << "set autoLock to " << [defaults boolForKey: @"autoLock"] << endl;
	
	[defaults setBool: m_buttonDescriptions forKey: @"buttonDescriptions"];
	//cerr << "set button descriptions to " << [defaults boolForKey: @"buttonDescriptions"] << endl;
	
	[defaults setBool: m_autorotation forKey: @"autoScreenRotation"];
	//cerr << "set autorotation to " << [defaults boolForKey: @"autoScreenRotation"] << endl;
	
	[defaults setInteger: m_colorDepth forKey: @"colorDepth"];
	//cerr << "set color depth to " << [defaults integerForKey: @"colorDepth"] << endl;
	
	[defaults setInteger: m_numPeers forKey: @"numPeers"];
	//cerr << "set num peers to " << [defaults integerForKey: @"numPeers"] << endl;
	
	[defaults setInteger: m_numThreads forKey: @"numThreads"];
	//cerr << "set num threads to " << [defaults integerForKey: @"numThreads"] << endl;

}


void
PSSettings::setSlideshowSpeed(float f)
{
	m_slideshowSpeed = f;
	m_app->slideshowSpeedChanged(f);
}


void
PSSettings::setNumThreads(int n)
{
	if(m_app)
		m_app->setNumNetworkThreads(n);
	m_numThreads = n;
}


void
PSSettings::setAutoLock(bool b)
{
	m_autoLock = b;
	m_app->setAutoLock(b);
}

