//
//  MenuViewController.m
//  Phonosynth
//
//  Created by Greg on 12/12/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import  "PhonosynthAppDelegate.h"
#import  "MenuViewController.h"
#import  "MenuViewCell.h"
#import  "ViewMoreCell.h"
#import  "MenuSearchBar.h"
#import  "MenuTabBar.h"
#include <algorithm>
#include <MacTopLayer.h>
#import  <ObjCCommon.h>
#include <GTask.h>
#include <SynthDetailViewController.h>
#include <Popup.h>

#include <Functions.h>

// specific downloader for asynchronously downloading thumbnails
class ThumbnailDownloader : public ContinuousDownloader<MenuViewCell*>
{    
    MenuViewController* m_parent;
    
public:
    
    ThumbnailDownloader(MenuViewController* parent) : m_parent(parent) { }        
    void downloadSucceeded(MenuViewCell* requestID, PtrWithLen<unsigned char> data)
    {
        NSData* nsdata = [NSData dataWithBytesNoCopy: (unsigned char*) data
											  length: data.length()
										freeWhenDone: YES];
        UIImage* img = [[UIImage alloc] initWithData: nsdata];
        
        // notify the MenuViewController a thumbnail has been downloaded
        [m_parent downloadedThumbnail: img forCell: requestID];
		
    }
    
    void
    downloadFailed(MenuViewCell* requestID)
    {
        //cerr << "thumbnail download failure" << endl;
    }
};


// for private functions

@implementation MenuViewController



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.


- (id) initWithCoder: (NSCoder*) coder
{
	//cerr << "initWithCoder" << endl;
	if (self = [super initWithCoder: coder]){
		return self;
	}
	ASSERT(0 && "Failed to initialize MenuViewController");
	return nil;	
}


- (void) awakeFromNib
{
    m_thumbnails = vector<UIImage*>();
	m_collections = vector<WebCollection>();
    m_thumbnailDownloader = new ThumbnailDownloader(self);
    m_topLayer = [m_appDelegate getTopLayer];
    m_moreToView = 0;
    m_downloadCollectionsTask = NULL;	
	m_requestNum = 0;		
    [m_defaultThumbnail retain]; // keep one copy of the default thumbnail around
	m_contentMutex.unlock();
	m_taskMutex.unlock();
	
	NSString* path = [[NSBundle mainBundle] pathForResource: @"Icon_Photosynth" ofType: @"png"];
	m_defaultThumbnail = [[UIImage alloc] initWithContentsOfFile: path];
	
	[m_synthDetailViewController setMenuViewController: self];
}



- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void) viewWillAppear: (BOOL) animated
{
    [super viewWillAppear: animated];

	NSIndexPath* selectedPath = [m_tableView indexPathForSelectedRow];
	if(selectedPath != nil){
		[m_tableView deselectRowAtIndexPath: selectedPath animated: YES];
	}			
	
	// restart the thumbnail downloader
	if(m_thumbnailDownloader->numRequests() == 0){
		FOR(i,0,m_collections.size()){
			if(m_thumbnails[i] == nil)
				m_thumbnailDownloader->addRequest(i, m_collections[i].thumbnailURL);
		}
	}
	
	m_thumbnailDownloader->start();
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}



- (void)dealloc {
	[m_defaultThumbnail release];
    [super dealloc];    
}


- (void) setTopLayer: (MacTopLayer*) topLayer
{
    m_topLayer = topLayer;
}


- (void) showEAGLView
{
	[self hideSearchBar];
    [m_navController popToRootViewControllerAnimated: NO];    
    [m_navController pushViewController: m_eaglViewController
                          withAnimation: [UIView randomUIViewAnimationTransition]
                         animationCurve: [UIView randomUIViewAnimationCurve]
                               duration: 1.0f
								  cache: NO]; // if set to YES, we'll briefly see garbage from the last synth or program to run
}


- (void) showEAGLViewWithCollection: (const WebCollection*) c							
{	
	[m_navController setNavigationBarHidden: YES animated: YES];
	
	[self hideSearchBar];
	m_topLayer->beginViewingSynth(c->getName(), c->getJsonURL());
	
    [m_navController popToRootViewControllerAnimated: NO];    
    [m_navController pushViewController: m_eaglViewController
                          withAnimation: [UIView randomUIViewAnimationTransition]
                         animationCurve: [UIView randomUIViewAnimationCurve]
                               duration: 1.0f
								  cache: NO]; // if set to YES, we'll briefly see garbage from the last synth or program to run
}


- (void) showLoadingView
{

	[self hideSearchBar];
	
	[m_navController setNavigationBarHidden: YES animated: NO];
		
	m_thumbnailDownloader->stop();	
	
    [m_navController popToRootViewControllerAnimated: NO];
    [m_navController pushViewController: m_loadingViewController
                          withAnimation: [UIView randomUIViewAnimationTransition]
                         animationCurve: [UIView randomUIViewAnimationCurve]
                               duration: 1.0f
								  cache: YES]; 
}


- (void) showSynthDetailViewWithCollection: (const WebCollection&) collection
							 withThumbnail: (UIImage*) thumbnail
{
	[self hideSearchBar];
	
	[m_synthDetailViewController setCollection: &collection];	
	if(thumbnail != nil)
		[m_synthDetailViewController setThumbnail: thumbnail];
	else
		[m_synthDetailViewController setThumbnail: m_defaultThumbnail];
	
	
	[m_navController popToRootViewControllerAnimated: NO];
    [m_navController pushViewController: m_synthDetailViewController
                          withAnimation: [UIView randomUIViewAnimationTransition]
                         animationCurve: [UIView randomUIViewAnimationCurve]
                               duration: 1.0f
								  cache: YES];
}


- (void) showOptionsView: (BOOL) animateNavBar
{
	[self hideSearchBar];
	
	[m_navController setNavigationBarHidden: NO animated: animateNavBar];
	
	//[m_navController popToRootViewControllerAnimated: NO];
	[m_navController pushViewController: m_optionsViewController 
						  withAnimation: [UIView randomUIViewAnimationTransition]
						 animationCurve: [UIView randomUIViewAnimationCurve]
							   duration: 1.0f
								  cache: YES];
}


- (void) showAboutView: (BOOL) animateNavBar
{
	[self hideSearchBar];
	
	[m_navController setNavigationBarHidden: NO animated: animateNavBar];
	
    //[m_navController popToRootViewControllerAnimated: NO];
    [m_navController pushViewController: m_aboutViewController
                          withAnimation: [UIView randomUIViewAnimationTransition]
                         animationCurve: [UIView randomUIViewAnimationCurve]
                               duration: 1.0f
								  cache: YES];
}


- (IBAction) showOptionsView
{
	[self showOptionsView: NO];
}


- (IBAction) showAboutView
{
	[self showAboutView: NO];
}






- (void) hideEAGLView
{
	[m_navController setNavigationBarHidden: NO animated: NO];
	
    ASSERT([m_navController topViewController] == m_eaglViewController);    
    [m_navController popToViewController: self
                           withAnimation: [UIView randomUIViewAnimationTransition]
                          animationCurve: [UIView randomUIViewAnimationCurve]
                                duration: 1.0f
								   cache: YES];
}



- (void) hideLoadingView
{
	[m_navController setNavigationBarHidden: NO animated: NO];
	
    ASSERT([m_navController topViewController] == m_loadingViewController);    
    [m_navController popToViewController: self
                           withAnimation: [UIView randomUIViewAnimationTransition]
                          animationCurve: [UIView randomUIViewAnimationCurve]
                                duration: 1.0f
								   cache: YES];
}


- (void) hideSynthDetailView
{
	ASSERT([m_navController topViewController] == m_synthDetailViewController);
	[m_navController popToViewController: self
						   withAnimation: [UIView randomUIViewAnimationTransition]
						  animationCurve: [UIView randomUIViewAnimationCurve]
								duration: 1.0f
								   cache: YES];
}


- (void) hideOptionsView: (BOOL) animateNavBar
{    
	[m_navController setNavigationBarHidden: YES animated: animateNavBar];
	
	[m_navController popViewControllerAnimated: NO];
	[m_navController pushViewController: m_eaglViewController
                          withAnimation: [UIView randomUIViewAnimationTransition]
                         animationCurve: [UIView randomUIViewAnimationCurve]
                               duration: 1.0f];
}


- (void) hideAboutView: (BOOL) animateNavBar
{
	[m_navController setNavigationBarHidden: YES animated: animateNavBar];
	
    ASSERT([m_navController topViewController] == m_aboutViewController);    
    [m_navController popToViewController: self
						   withAnimation: [UIView randomUIViewAnimationTransition]
						  animationCurve: [UIView randomUIViewAnimationCurve]
								duration: 1.0f
								   cache: YES];
}


- (IBAction) hideOptionsView
{
	[self hideOptionsView: YES];
}
	

- (IBAction) hideAboutView
{
	[self hideAboutView: YES];
}


- (void) setTabBarSelection: (int) selectedIndex
{
	[m_tabBar setSelectedIndex: selectedIndex];
}


- (void) clearTabBarSelection
{
	[m_tabBar setSelectedIndex: -1];
}


- (void) clearMenuAndStop
{
	if(m_downloadCollectionsTask){
		m_downloadCollectionsTask->cancelTask(true);
		SAFE_DELETE_AND_CLEAR(m_downloadCollectionsTask);
	}
	[self clearMenu];
}


-(void) revertTabBarSelection
{
	[m_tabBar revertToLastSelectedItem];
}


- (void) showMostViewedSelector
{
	if(!m_mostViewedSelector.hidden)
		return;
	
	[m_mostViewedSelector fadeIn: 1.0f];
	CGRect r = m_tableView.frame;
	r.origin.y += m_mostViewedSelector.frame.size.height;
	r.size.height -= m_mostViewedSelector.frame.size.height;
	[m_tableView setFrame: r];
}


- (void) hideMostViewedSelector
{
	if(m_mostViewedSelector.hidden)
		return;
	
	[m_mostViewedSelector fadeOut: 1.0f];
	CGRect r = m_tableView.frame;
	r.origin.y -= m_mostViewedSelector.frame.size.height;
	r.size.height += m_mostViewedSelector.frame.size.height;
	[m_tableView setFrame: r];
}


- (IBAction) mostViewedTypeChanged
{
	switch((const int) [m_mostViewedSelector selectedSegmentIndex])
	{
		case 0:
			[self populateMenu: @"EXPLORE:MOST_VIEWED_LAST7"];
			break;
		case 1:
			[self populateMenu: @"EXPLORE:MOST_VIEWED_LAST30"];
			break;
		case 2:
			[self populateMenu: @"EXPLORE:MOST_VIEWED_ALLTIME"];
			break;
	}
}


- (void) loadAndViewSynth: (const WebCollection*) collection
{
	m_topLayer->beginViewingSynth(collection->getName(), collection->getJsonURL());
	[self showLoadingView];
}


- (UIViewController*) getLoadingViewController
{
    return m_loadingViewController;
}


- (UIViewController*) getOptionsViewController
{
    return m_optionsViewController;
}


- (UIViewController*) getEAGLViewController
{
    return m_eaglViewController;
}


- (UIViewController*) getSynthDetailViewController
{
    return m_synthDetailViewController;
}


-(IBAction) showSearchBar
{		
    [m_searchBar fadeIn: 1.0f];
	[m_searchNote fadeIn: 1.0f];
}


-(IBAction) hideSearchBar
{
    [m_searchBar fadeOut: 1.0f];
	[m_searchNote fadeOut: 1.0f];
}


- (NSString*) getLastPopulateString
{
	return m_lastArgs.queryString;
}





// Menu-specific methods


- (void) clearMenu
{    
	
	m_contentMutex.lock();	
	m_collections.clear();
	FOR(i,0,m_thumbnails.size()){
		//cerr << "releasing thumbnail " << m_thumbnails[i] << endl;
		[m_thumbnails[i] release];
	}
	m_thumbnails.clear();	
	m_contentMutex.unlock();
	
	m_moreToView = 0;
	
	[m_tableView reloadData];
}


- (void) populateMenu: (NSString*) populateString
{
    [self populateMenu: populateString
		 updateTabBars: NO];
}


- (void) populateMenu: (NSString*) populateString
		updateTabBars: (BOOL) updateTabBars
{
	[self populateMenu: populateString
		   firstResult: 0
			maxResults: 30];
	
		
	if(updateTabBars){
		
		[self hideMostViewedSelector];
		
		if([populateString hasPrefix: @"SEARCH"])
			[m_tabBar setSelectedIndex: 0];

		else if([populateString hasPrefix: @"USER"])
			[m_tabBar setSelectedIndex: -1];

		else if([populateString hasPrefix: @"EXPLORE"]){
			if([populateString rangeOfString: @"RECENT"].location != NSNotFound)
				[m_tabBar setSelectedIndex: 1];

			else if([populateString rangeOfString: @"NICE_AND_SYNTHY"].location != NSNotFound)
				[m_tabBar setSelectedIndex: 3];

			else if([populateString rangeOfString: @"MOST_VIEWED"].location != NSNotFound){
				
				[m_tabBar setSelectedIndex: 2];
				[self showMostViewedSelector];
		
				if([populateString rangeOfString: @"LAST7"].location != NSNotFound)
					[m_mostViewedSelector setSelectedSegmentIndex: 0];
				else if ([populateString rangeOfString: @"LAST30"].location != NSNotFound)
					[m_mostViewedSelector setSelectedSegmentIndex: 1];
				else if ([populateString rangeOfString: @"ALLTIME"].location != NSNotFound)
					[m_mostViewedSelector setSelectedSegmentIndex: 2];
			}	
		}		
	}
}


/* kind of hacky, these are declared globally so the callback function has access
   TODO: think of something cleaner but it's not a top priority
 */
vector<WebCollection> g_collections;
int					  g_totalFound;

- (void) populateMenu: (NSString*) populateString
          firstResult: (int) firstResult
           maxResults: (int) maxResults
{		
	m_topLayer->writeDataToFile(populateString);
	
	m_taskMutex.lock();
    if(m_downloadCollectionsTask){
		// if we're interrupting a task that's already going on, cancel it, and wait for it
		// to actually die before proceeding
		m_downloadCollectionsTask->cancelTask(true);
		SAFE_DELETE_AND_CLEAR(m_downloadCollectionsTask);
	}
	
	
    if(firstResult == 0){
        [self clearMenu];
        m_thumbnailDownloader->reset();
    }	
	
	// would have been simpler to use a C++ string but I'm trying to get more comfortable with the Obj C libraries
    populateString = [populateString stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceAndNewlineCharacterSet]];		
	
	m_lastArgs = GetCollectionsArgs(populateString, firstResult, maxResults);
	
	// functor for asynch downloading and parsing of collections
	typedef StaticFunctorA3<GetCollectionsArgs, vector<WebCollection>&, int&> GCFunctor;
	
	
    if ([populateString hasPrefix: @"SEARCH:"]){

		// Asynchronously download and parse the collections				
		GCFunctor   work(&PSWeb::getCollectionsFromSearch, m_lastArgs, g_collections, g_totalFound);
		ObjCFunctor callback(self, @selector(downloadedCollections), nil);
		m_downloadCollectionsTask = new GTask(work, callback);
		m_downloadCollectionsTask->performTask();
		
		NSString* searchQuery = [populateString stringByReplacingOccurrencesOfString: @"SEARCH:"
																		  withString: @""];
		[self.navigationItem setTitle: [@"Search: " stringByAppendingString: searchQuery]];
    }
	
    else if ([populateString hasPrefix: @"EXPLORE:"]){
		
		// Asynchronously download and parse the collections				
		GCFunctor   work(&PSWeb::getCollectionsFromExplore, m_lastArgs, g_collections, g_totalFound);
		ObjCFunctor callback(self, @selector(downloadedCollections), nil);
		m_downloadCollectionsTask = new GTask(work, callback);
		m_downloadCollectionsTask->performTask();
		
		NSString* exploreType = [populateString stringByReplacingOccurrencesOfString: @"EXPLORE:"
																		  withString: @""];
		
		if([exploreType isEqualToString: @"RECENT"])
			[self.navigationItem setTitle: @"Recently Synthed"];
		else if([exploreType isEqualToString: @"NICE_AND_SYNTHY"])
			[self.navigationItem setTitle: @"Nice and Synthy"];
		else if([exploreType isEqualToString: @"MOST_VIEWED_LAST7"])
			[self.navigationItem setTitle: @"Most Viewed"];
		else if([exploreType isEqualToString: @"MOST_VIEWED_LAST30"])
			[self.navigationItem setTitle: @"Most Viewed"];
		else if([exploreType isEqualToString: @"MOST_VIEWED_ALLTIME"])
			[self.navigationItem setTitle: @"Most Viewed"];
    }
	
	else if ([populateString hasPrefix: @"USER:"]){		
		
		// Asynchronously download and parse the collections				
		GCFunctor   work(&PSWeb::getCollectionsFromUser, m_lastArgs, g_collections, g_totalFound);
		ObjCFunctor callback(self, @selector(downloadedCollections), nil);
		m_downloadCollectionsTask = new GTask(work, callback);
		m_downloadCollectionsTask->performTask();
		
		NSString* user = [populateString stringByReplacingOccurrencesOfString: @"USER:"
																   withString: @""];
		
		[self.navigationItem setTitle: [@"User: " stringByAppendingString: user]];
		
    }
	
    else{
        printf("Invalid populate string: %s\n", CSTRING(populateString));
        ASSERT(0);
    }
		
	m_taskMutex.unlock();
	
	[m_activityLabel fadeIn: 0.25f];
	[m_activityIndicator fadeIn: 0.25f];
	[m_activityIndicator startAnimating];	
}


- (void) downloadedCollections 
{		
	
	const int firstResult = m_lastArgs.firstResult;
	const int maxResults  = m_lastArgs.maxResults;
	
	
	if(g_totalFound > 0){
		if([m_lastArgs.queryString hasPrefix: @"USER"]){
			if(g_totalFound > m_lastArgs.maxResults)
				m_moreToView = -1;
			else
				m_moreToView = 0;
		}
		else if([m_lastArgs.queryString hasPrefix: @"EXPLORE:RECENT"])
			m_moreToView = 0;  // only load a small number of recent synths
		else			
			m_moreToView = max(min(200, g_totalFound - (firstResult + maxResults)), 0);
		
        [self populateMenuFromCollections: g_collections
								 firstRow: firstResult];
    }
    else{
        m_moreToView = 0;				
		// g_totalFound < 0 indicates an error rather than just finding no results
		if(g_totalFound < 0){
			Popup* p = Popup::OneButtonPopup("", "Unable to populate menu. Make sure you have an active network connection.", 
											 Functor::Identity(), "Ok");
			p->show();
		}
		else{
			Popup* p;
			if([m_lastArgs.queryString hasPrefix: @"USER:"]){
				p = Popup::OneButtonPopup("", "User not found", 
										  Functor::Identity(), "Ok");
			}
			else{
				p = Popup::OneButtonPopup("", "No results found", 
										  Functor::Identity(), "Ok");
			}				
			p->show();		
		}
    }
	
	[m_activityLabel fadeOut: 1.0f];
	[m_activityIndicator fadeOut: 1.0f];
	[m_activityIndicator stopAnimating];
}


- (void) populateMenuFromXMLFile: (NSString*) path
{
    NSData* data = [NSData dataWithContentsOfFile: path];
    ASSERT(data != nil);
    
    vector<WebCollection> collections;		
		
    PSWeb::scrapeCollectionsFromXML([data bytes], [data length], collections);
    
    [self populateMenuFromCollections: collections 
                             firstRow: 0];
}


- (void) populateMenuFromCollections: (const vector<WebCollection>&) collections
                            firstRow: (int) firstRow
{

	// append the new collections	
	printf("pop begin\n");
	
	m_contentMutex.lock();
	m_collections.insert(m_collections.end(), collections.begin(), collections.end());
	m_thumbnails.resize(m_collections.size(), nil);
	m_contentMutex.unlock();
	
    const int nCollections = (const int) collections.size();    
    FOR(i, 0, nCollections)
        m_thumbnailDownloader->addRequest(firstRow + i, m_collections[firstRow + i].thumbnailURL);        
	
	[m_tableView reloadData];
	[self downloadThumbnails]; // begin asynchronous downloading of thumbnails	
	
	printf("pop end\n");
}


- (void) randomizeOrder
{
	random_shuffle(m_collections.begin(), m_collections.end());
}


- (void) downloadThumbnails
{
	// create space to hold the downloaded thumbnails
	
	// start the asynchronous downloader
	m_thumbnailDownloader->start();
}


// NYI, eventually, loading the menu data will happen asynchronously and this callback will be used
- (void) downloadedCollectionData: (NSData*) data
{

}


- (void) downloadedThumbnail: (UIImage*) image
                     forCell: (int) cellIdx
{	
	if(cellIdx < m_collections.size()){

		m_thumbnails[cellIdx] = image;   
		
		if([m_navController topViewController] == m_synthDetailViewController &&
		   [m_synthDetailViewController getCollection] == &m_collections[cellIdx])
		{		
			[m_synthDetailViewController setThumbnail: image];
		}
	}
	

	[m_tableView reloadData];

}



/*******************************/
/* UITableViewDelegate methods */
/*******************************/

- (void)	  tableView: (UITableView*) tableView
didSelectRowAtIndexPath: (NSIndexPath*) indexPath
{
	
	const int idx = indexPath.row;
    ASSERT(idx < m_collections.size() || (idx == m_collections.size() && m_moreToView != 0));
    
    if(idx < m_collections.size()){   // regular cell selection, load up the associated synth
        WebCollection& col = m_collections[idx];
        if(col.jsonURL == ""){
			[self showSynthDetailViewWithCollection: col
									  withThumbnail: m_thumbnails[idx]];
        }
        else{
			[self showSynthDetailViewWithCollection: col
									  withThumbnail: m_thumbnails[idx]];
		}
    }
    else{ // user clicked the bottom cell, load more search results	
		
		// throw out the request if we already have an active load task going on. The point of this is to 
		// ignore clicking the same "Load more" button twice		
		if (!m_downloadCollectionsTask->isExecuting())
			if([m_lastArgs.queryString hasPrefix: @"EXPLORE:NICE"]){
				[self populateMenu: m_lastArgs.queryString
					   firstResult: 0
						maxResults: 30];
			}
			else
				[self populateMenu: m_lastArgs.queryString
					   firstResult: m_collections.size()
						maxResults: (m_moreToView > 0) ? (min(m_moreToView, 30)) : 30];
    }
		
}


- (CGFloat)	  tableView: (UITableView*) tableView
heightForRowAtIndexPath: (NSIndexPath*) indexPath
{
	return 93;
}





/*********************************/
/* UITableViewDataSource methods */
/*********************************/

- (UITableViewCell*) tableView: (UITableView*) tableView
		 cellForRowAtIndexPath: (NSIndexPath*) indexPath
{			
	
	UITableViewCell* ret = nil;
    const int idx = indexPath.row;     
    
	// lock the collections mutex to make sure the downloading thread doesn't modify them while we are in here
	m_contentMutex.lock();
	
    ASSERT(idx < m_collections.size() || (idx == m_collections.size() && m_moreToView != 0));
    
    if(idx < m_collections.size()){
		MenuViewCell* mvc = nil;
        mvc = (MenuViewCell*) [tableView dequeueReusableCellWithIdentifier: @"MenuViewCell"];
        if(mvc == nil){        
            NSArray* objs = [[NSBundle mainBundle] loadNibNamed: @"MenuViewCell" owner: self options: nil];
            mvc = (MenuViewCell*) [objs objectAtIndex: 0];
        }

        [mvc fillWithWebCollection: &m_collections[idx]];
        
        if(idx & 1)
            [[mvc contentView] setBackgroundColor: [UIColor colorWithRed: 0.1f
                                                                   green: 0.2f
                                                                    blue: 0.1f
                                                                   alpha: 1.0f]];
        else
            [[mvc contentView] setBackgroundColor: [UIColor colorWithRed: 0.15f
                                                                   green: 0.4f
                                                                    blue: 0.15f	
                                                                   alpha: 1.0f]];
        
        if(m_thumbnails[idx] != nil)
            [mvc setThumbnail: m_thumbnails[idx]];
		else
			[mvc setThumbnail: m_defaultThumbnail];
		
		ret = mvc;
		
    }
    
    else{

        ViewMoreCell* vmc = (ViewMoreCell*) [tableView dequeueReusableCellWithIdentifier: @"ViewMoreCell"];
        if(vmc == nil){        
            NSArray* objs = [[NSBundle mainBundle] loadNibNamed: @"ViewMoreCell" owner: self options: nil];        
            vmc = (ViewMoreCell*) [objs objectAtIndex: 0];
        }        
        
		if([m_lastArgs.queryString hasPrefix: @"EXPLORE:NICE"]){
			[vmc setMainLabelText: @"Tap to refresh list"];
			[vmc setSubLabelText: @"30 new random synths"];
		}
		else{
			[vmc setMainLabelNum: m_moreToView];
			[vmc setSubLabelNum: min(m_moreToView, 30)];
		}

		ret = vmc;
    }
	
	// unlock the mutex now that we're done accessing the collections
	m_contentMutex.unlock();

	
	return ret;
}


- (NSInteger) numberOfSectionsInTableView: (UITableView*) tableView
{
	return 1;
}


- (NSInteger) tableView: (UITableView*) tableView
  numberOfRowsInSection: (NSInteger) section
{	
	if(section != 0)
		return 0;
	
	m_contentMutex.lock();
	NSInteger ret = (NSInteger) m_collections.size() + (m_moreToView != 0 ? 1 : 0);
	m_contentMutex.unlock();
	return ret;
}


#if 0  // Unit Tests


//#ifdef DEBUG
LotsOfSlideshows* slideshowTest = new LotsOfSlideshows();
if(idx == 59){
	slideshowTest->performTest(self);
	return;
}


 else if(idx == 0){
 NSString* path = [[NSBundle mainBundle] pathForResource: @"bigjson" ofType: @"json"];
 NSData* data = [[NSData alloc] initWithContentsOfFile: path];
 const char* s = (const char*) [data bytes];
 
 //cerr << "PARSE TEST:" << endl;
 
 JsonParser *J = new JsonParser();
 JsonSynthAliases* a = new JsonSynthAliases();
 //cerr << "before parse, " << JsonInAtom::nInstances_ << endl;
 int nParsed = J->parse(s, *a, false);
 //cerr << "after parse, " << JsonInAtom::nInstances_ << endl;
 
 //cerr << "deleting parser and aliases" << endl;
 
 delete J;
 delete a;
 
 //cerr << "now, " << JsonInAtom::nInstances_ << endl;
 
 [data release];
 
 return;
 }	
 else if(idx == 1)
 {		
 int* reg[200000];
 FOR(i,0,200000){
 reg[i] = (int*) malloc(sizeof(int));
 if((rand() % 1000) == 0){
 //cerr << "reg[" << i << "] = " << reg[i] << endl;
 //cerr << "*reg[" << i << "] = " << *reg[i] << endl;
 }
 }
 sleep(5.0f);
 
 
 FOR(i,0,200000)
 free(reg[i]);		
 
 return;
 }


#endif // 0


@end
