//
//  SynthDetailViewController.m
//  Phonosynth
//
//  Created by Greg on 12/30/08.
//  Copyright 2008 Brown University. All rights reserved.
//


#import  "SynthDetailViewController.h"
#import  "MenuViewController.h"
#include <GTask.h>
#include <Functions.h>
#include <Popup.h>
#include <GTime.h>
#include <ObjCCommon.h>


@interface SynthDetailViewController (Private)
- (void) tryLoadSynth;
// asynchronously download all the details of the collection (most are missing in initial search)
- (void) expandCollectionAsynch;
- (void) downloadedCollection;
@end // interface SynthDetailViewController (Private)


@implementation SynthDetailViewController

WebCollection s_collection;

- (void) awakeFromNib
{
	m_tryLoadTimer = new GTimer(ObjCFunctor(self, @selector(tryLoadSynth), nil));
	[m_viewSynthButton setUserInteractionEnabled: YES]; 
}
	   



- (void) dealloc{
	if(m_downloadCollectionTask)
		m_downloadCollectionTask->cancelTask(true); // cancel the task before deleting it. 
	//SAFE_DELETE(m_downloadCollectionTask);
	m_tryLoadTimer->stop();
	//SAFE_DELETE(m_tryLoadTimer);
    [super dealloc];
}


- (void) setMenuViewController: (MenuViewController*) menuVC
{	
	m_menuViewController = menuVC;
}


- (void) viewDidLoad
{
	[self fillWithWebCollection: m_collection];
}


- (void) viewWillAppear: (BOOL) animated {    
	
	if(self.view == nil)
		[self loadView];
	
	if(m_downloadCollectionTask != NULL){
		[m_activityIndicator startAnimating];
		[m_activityIndicator fadeIn: 0.25f];
	}
	
	if(m_thumbnail != nil)
		[self setThumbnail: m_thumbnail];		
	
	[m_viewSynthButton setUserInteractionEnabled: YES];
	[m_moreSynthsByUserButton setUserInteractionEnabled: YES];
	
	m_delayedID = 0;
	
	[super viewWillAppear: animated];
}


- (void) viewWillDisappear: (BOOL) animated
{
	[m_viewSynthButton setUserInteractionEnabled: YES];
	[m_moreSynthsByUserButton setUserInteractionEnabled: YES];
	
	[m_activityIndicator fadeOut: 0.5f];
	[m_activityIndicator stopAnimating];
	
	if(m_downloadCollectionTask)
		m_downloadCollectionTask->cancelTask(true);
	
	if(m_delayedID)
		GTimer::cancelPerformFunctorRequest(m_delayedID);
	
	m_tryLoadTimer->stop();
	
	[super viewWillDisappear: animated];
}




- (void) viewDidDisappear: (BOOL) animated
{
	//[self setThumbnail: nil];
	//[self setView: nil];
	[m_descriptionView setText: nil];
	[self setThumbnail: nil];		
	
	[super viewDidDisappear: animated];
}



- (void)didReceiveMemoryWarning {
	[super didReceiveMemoryWarning];
}


- (void) expandCollectionAsynch
{
	StaticFunctorA2<const string&, WebCollection&> work(&PSWeb::getCollectionFromGUID, m_collection->guid, s_collection);
	m_downloadCollectionTask = new GTask(work, ObjCFunctor(self, @selector(downloadedCollection), nil));
	m_downloadCollectionTask->performTask();
	[m_activityIndicator startAnimating];
	[m_activityIndicator fadeIn: 0.25f];
}


- (void) downloadedCollection
{
	m_downloadCollectionTask->cancelTask(true);
	SAFE_DELETE_AND_CLEAR(m_downloadCollectionTask);
	
	if(s_collection.getName() != ""){
		[m_activityIndicator fadeOut: 0.5f];
		[m_activityIndicator stopAnimating];
		*m_collection = s_collection;
		[self fillWithWebCollection: m_collection];
		m_delayedID = 0;
	}
	else{ // try the download again if it failed	 
		m_delayedID = GTimer::performFunctorAfterDelay(ObjCFunctor(self, @selector(expandCollectionAsynch), nil), 5.0f);
	}
}


- (const WebCollection*) getCollection
{
	return m_collection;
}


- (void) fillWithWebCollection: (const WebCollection*) c
{
	[[self navigationItem] setTitle: NSSTRING(c->getName().c_str())];
	[m_nameLabel setText: NSSTRING(c->getName().c_str())];
	[m_numPhotosLabel setText: [NSString stringWithFormat: @"%d Photos", c->getNumPhotos()]];
	[m_pctSynthyLabel setText: [NSString stringWithFormat: @"%d%% Synthy", c->getSynthiness()]];
	[m_numViewsLabel setText: [NSString stringWithFormat: @"%d Views", c->getNumViews()]];
	[m_dateLabel setText: NSSTRING(c->getDate().c_str())];
	[m_userLabel setText: NSSTRING(c->getOwnerName().c_str())];
	[m_moreSynthsByUserButton setTitle: [NSString stringWithFormat: @"More by %s", c->getOwnerName().c_str()]
							  forState: UIControlStateNormal];
	[m_moreSynthsByUserButton setTitle: [NSString stringWithFormat: @"More by %s", c->getOwnerName().c_str()]
							  forState: UIControlStateSelected];
	[m_moreSynthsByUserButton setTitle: [NSString stringWithFormat: @"More by %s", c->getOwnerName().c_str()]
							  forState: UIControlStateHighlighted];

	if(c->getDescription() == "")		
		[m_descriptionView setText: @"No Description"];
	else
		[m_descriptionView setText: NSSTRING(c->getDescription().c_str())];

}


- (void) setCollection: (const WebCollection*) c
{
	m_collection = c;
	[self fillWithWebCollection: c];
	if(!c->expanded)
		[self expandCollectionAsynch];
}


- (void) setThumbnail: (UIImage*) thumbnail
{
	m_thumbnailView.image = thumbnail;
	m_thumbnail = thumbnail;
}


- (void) tryLoadSynth
{
	if(m_collection != NULL && m_collection->expanded){
		m_tryLoadTimer->stop();
		if(m_downloadCollectionTask)
			m_downloadCollectionTask->cancelTask(true);
		[m_menuViewController loadAndViewSynth: m_collection];
	}		
	else if(m_tryLoadTimer->runningTime() >= 5.0f){
		m_tryLoadTimer->stop();
		string errorMsg = "Couldn't load the synth. Check your network connection and try reselecting this synth from the menu";	
		Popup* p = Popup::OneButtonPopup("", errorMsg, Functor::Identity(), "Ok");
		p->show();
		[m_viewSynthButton setUserInteractionEnabled: YES];
	}
}


- (IBAction) viewSynthButtonClicked\
{
	//wait up to 5 seconds for the task to finish. If it doesn't the internet connection is probably bad
	//cerr << "collection guid is " << m_collection->guid << endl;
	m_tryLoadTimer->start();
	[m_viewSynthButton setUserInteractionEnabled: NO];
}


- (IBAction) moreSynthsByUserButtonClicked
{
	[m_moreSynthsByUserButton setUserInteractionEnabled: NO];
	[m_viewSynthButton setUserInteractionEnabled: NO];
	
	if(m_downloadCollectionTask && m_downloadCollectionTask->isExecuting())
		m_downloadCollectionTask->cancelTask(true);
	
	NSString* populateString = @"USER:";
	populateString = [populateString stringByAppendingString: NSSTRING(m_collection->ownerName.c_str())];
	[m_menuViewController populateMenu: populateString];
	[m_menuViewController hideSynthDetailView];
	[m_menuViewController hideMostViewedSelector];
	[m_menuViewController clearTabBarSelection];
}

@end
