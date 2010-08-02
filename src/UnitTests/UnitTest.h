/*
 *  UnitTest.h
 *  Phonosynth
 *
 *  Created by Greg on 2/6/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#ifndef _UNITTEST_H_
#define _UNITTEST_H_

#import  <MenuViewController.h>
#include <GTimer.h>
#include <ButtonActions.h>


// super hackily implemented, but provides for some pretty sweet automated stress-testing
// TODO: Make this a nice and robust framework for future testing


class UnitTest
{
public:

	static void performTest();
	
private:
	
};

// Simply loads and quits many synths

class LoadLotsOfSynths : public UnitTest
{
public:
	
	static void performTest(MenuViewController* mvc){
		performTest(mvc, 0);
	}
	
	
	static void performTest(MenuViewController* mvc, int row)
	{
		//cerr << "UNIT TEST: " << row << endl;
		
		NSIndexPath* path = [NSIndexPath indexPathForRow: row
											   inSection: 0];
		[mvc tableView: nil	didSelectRowAtIndexPath: path];
		loadSynth(mvc, row);						
	}
	
	
	static void loadSynth(MenuViewController* mvc, int row)
	{	
		SynthDetailViewController* sdvc = [mvc getSynthDetailViewController];
		[sdvc viewSynthButtonClicked];
		
		GTimer::performFunctorUponPredicate(StaticFunctorA2<MenuViewController*, int>(leaveSynth, mvc, row),
											StaticPredicate1<MenuViewController*>(synthHasLoaded, mvc),
											1.0f,
											30.0f);
		
	}
	
	
	static bool synthHasLoaded(MenuViewController* mvc)
	{
		return [[mvc navigationController] topViewController] == [mvc getEAGLViewController];
	}
	
	
	static void leaveSynth(MenuViewController* mvc, int row)
	{
		GTimer::performFunctorAfterDelay(StaticFunctor(doLoadPopupAction), 2.0f);

		if(row < 58)
			GTimer::performFunctorAfterDelay(StaticFunctorA2<MenuViewController*, int>(performTest, mvc, row+1), 3.0f);
		else
			BREAK;
	}
	
	
private:								 	
	
};



// Loads many synths, running each in slideshow mode for 60 seconds

class LotsOfSlideshows
{
public:
	
	
	void performTest(MenuViewController* mvc){
		performTest(mvc, 0);
	}
	
	
	void performTest(MenuViewController* mvc, int row)
	{
		//cerr << "UNIT TEST: " << row << endl;
		
		NSIndexPath* path = [NSIndexPath indexPathForRow: row
											   inSection: 0];
		[mvc tableView: nil	didSelectRowAtIndexPath: path];
		GTimer::performFunctorAfterDelay(MemberFunctorA2<LotsOfSlideshows, MenuViewController*, int>(this, &LotsOfSlideshows::loadSynth, mvc, row), 2.0f);
	}
	
	
	void loadSynth(MenuViewController* mvc, int row)
	{	
		SynthDetailViewController* sdvc = [mvc getSynthDetailViewController];
		[sdvc viewSynthButtonClicked];
		
		MemberFunctorA2<LotsOfSlideshows, MenuViewController*, int> skipFunctor(this, &LotsOfSlideshows::performTest, mvc, row + 1);
		//m_skipID = GTimer::performFunctorAfterDelay(skipFunctor, 32);
		
		MemberFunctionRA<LotsOfSlideshows, bool, MenuViewController*> mf = &LotsOfSlideshows::synthHasLoaded;
		
		
		GTimer::performFunctorUponPredicate(MemberFunctorA2<LotsOfSlideshows, MenuViewController*, int>(this, &LotsOfSlideshows::doStuffAndLeaveSynth, mvc, row),
											MemberPredicate1<LotsOfSlideshows, MenuViewController*>(this, mf, mvc),
											1.0f,
											60.0f);
		
	}
	
	
	bool synthHasLoaded(MenuViewController* mvc)
	{		
		return [[mvc navigationController] topViewController] == [mvc getEAGLViewController];
	}
	
	
	void doStuffAndLeaveSynth(MenuViewController* mvc, int row)
	{		
	
		GTimer::cancelPerformFunctorRequest(m_skipID);
		
		slideshowToggleButtonAction();
		slideshowPlayButtonAction();
		
		if(row < 58){
			GTimer::performFunctorAfterDelay(StaticFunctor(doLoadPopupAction), 180.0f);
			GTimer::performFunctorAfterDelay(MemberFunctorA2<LotsOfSlideshows, MenuViewController*, int>(this, &LotsOfSlideshows::performTest, mvc, row+1), 182.0f);
		}
		else
			BREAK;
		
	}
	
	
private:
	
	PerformFunctorRequestID m_skipID;
	
};


#endif // _UNITTEST_H_


