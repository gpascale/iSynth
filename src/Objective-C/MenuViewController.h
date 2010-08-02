//
//  MenuViewController.h
//  Phonosynth
//
//  Created by Greg on 12/12/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#ifndef _MENUVIEWCONTROLLER_H_
#define _MENUVIEWCONTROLLER_H_

#import  <UIKit/UIKit.h>
#include <vector>
#include <ContinuousDownloader.h>
#include <ViewControllers.h>
#include <PSWeb.h>
#include <GMutex.h>

@class PhonosynthAppDelegate;
@class MenuViewCell;
class  MacTopLayer;
class  ThumbnailDownloader;
class  WebCollection;
class  GTask;
@class MenuSearchBar;
@class MenuTabBar;
@class SynthDetailViewController;

enum PopulateMenuType
{
    SHOW_POPULAR = 0,
    SHOW_FEATURED = 1,
    SHOW_SYNTHY = 2
};

struct CellInfo
{
    string   name;
    string   jsonURL;
    string   thumbnailURL;
    string   author;
    string   guid;
    int      synthiness;
    int      numPhotos;
    int      numViews;
    UIImage* thumbnail;
    
    CellInfo() : thumbnail(nil) {
        //cout << "thumbnail == nil = " << (thumbnail == nil) << endl;
    }
    
    CellInfo(const CellInfo& rhs) :
        name(rhs.name),
        jsonURL(rhs.jsonURL),
        thumbnailURL(rhs.thumbnailURL),
        author(rhs.author),
        guid(rhs.guid),
        synthiness(rhs.synthiness),
        numPhotos(rhs.numPhotos),
        numViews(rhs.numViews),
        thumbnail(rhs.thumbnail)
    { }
    
};
    
@interface MenuViewController : UIViewController <UITableViewDelegate, UITableViewDataSource> 
{
    IBOutlet PhonosynthAppDelegate*      m_appDelegate;
    IBOutlet UINavigationController*     m_navController;
            
	IBOutlet SynthDetailViewController*  m_synthDetailViewController;	
	IBOutlet LoadingViewController*      m_loadingViewController;
    IBOutlet EAGLViewController*         m_eaglViewController;
    IBOutlet OptionsViewController*      m_optionsViewController;
    IBOutlet AboutViewController*        m_aboutViewController;	
    
    IBOutlet MenuSearchBar*              m_searchBar;
	IBOutlet MenuTabBar*				 m_tabBar;
    IBOutlet UITableView*                m_tableView;
	
	IBOutlet UIActivityIndicatorView*	 m_activityIndicator;
	IBOutlet UILabel*					 m_activityLabel;
	
	IBOutlet UISegmentedControl*		 m_mostViewedSelector;
	
	IBOutlet UITextView*				 m_searchNote;
    
    MacTopLayer*                         m_topLayer;
    
    // Menu-specific functionality
	vector<WebCollection>				 m_collections;
	vector<UIImage*>					 m_thumbnails;	
	
    ThumbnailDownloader*                 m_thumbnailDownloader;
    
    UIImage*                             m_defaultThumbnail;    
    int                                  m_moreToView;
	
	GetCollectionsArgs					 m_lastArgs;
	GTask*								 m_downloadCollectionsTask;
	
	// the underlying collections are accessed from different threads (because they are
	// downloaded asynchronously) so we need to make sure to keep it safe
	GMutex								 m_contentMutex;
	GMutex								 m_taskMutex;
	
	int									 m_requestNum;
	
	NSIndexPath*						 m_selectedIndexPath;
}

// all view controllers in Phonosynth need to know about TopLayer. It's how they are linked
- (void) setTopLayer: (MacTopLayer*) topLayer;

/* 
 MenuViewController acts as something of a control center for switching between the various views
 of the program. These functions are called by TopLayer when it is time to switch to another view
 */
- (void)     showEAGLView;
- (void)     showLoadingView;
- (void)	 showOptionsViewAsSubview;
- (void)	 showSynthDetailViewWithCollection: (const WebCollection&) c
								     thumbnail: (UIImage*) thumbnail;
- (void)	 showOptionsView: (BOOL) animateNavBar;
- (void)	 showAboutView:   (BOOL) animateNavBar;
- (IBAction) showOptionsView;
- (IBAction) showAboutView;

- (void)     hideEAGLView;
- (void)     hideLoadingView;
- (void)	 hideSynthDetailView;
- (void)	 hideOptionsView: (BOOL) animateNavBar;
- (void)	 hideAboutView:   (BOOL) animateNavBar;
- (IBAction) hideOptionsView;
- (IBAction) hideAboutView;

- (void)     showMostViewedSelector;
- (void)	 hideMostViewedSelector;
- (IBAction) mostViewedTypeChanged;

- (void)	 revertTabBarSelection;
- (void)     setTabBarSelection: (int) selectedIndex;
- (void)     clearTabBarSelection;

- (void) clearMenuAndStop;

- (void) loadAndViewSynth: (const WebCollection*) collection;

// type UIViewController* for now
- (LoadingViewController*) getLoadingViewController;
- (OptionsViewController*) getOptionsViewController;
- (EAGLViewController*)    getEAGLViewController;
- (SynthDetailViewController*) getSynthDetailViewController;

- (void)     showSearchBar;
- (void)     hideSearchBar;
- (void)	 showSearchNote;
- (void)	 hideSearchNote;

- (NSString*) getLastPopulateString;

/*******************************/
/* Menu-specific functionality */
/*******************************/


- (void) populateMenu: (NSString*) populateString;

- (void) populateMenu: (NSString*) populateString
		updateTabBars: (BOOL) updateTabBars;

- (void) populateMenu: (NSString*) populateString
          firstResult: (int) firstResult
           maxResults: (int) maxResults;


- (void) populateMenuFromXMLFile: (NSString*) path;

- (void) populateMenuFromCollections: (const vector<WebCollection>&) collections
                            firstRow: (int) firstRow;

// populate the menu with the collections in the given file
- (void) populateMenu: (NSString*) collectionsFile;
// clear the menu and release all associated resources
- (void) clearMenu;
// randomize the order of the cells in the menu
- (void) randomizeOrder;
// asynchronously download thumbnail images
- (void) downloadThumbnails;

// callback for menu data downloads
- (void) downloadedCollections;
// callback for thumbnail downloads
- (void) downloadedThumbnail: (UIImage*) image 
                     forCell: (MenuViewCell*) cell;


/*******************************/
/* UITableViewDelegate methods */
/*******************************/

- (void)      tableView: (UITableView*) tableView
didSelectRowAtIndexPath: (NSIndexPath*) indexPath;

- (CGFloat)	  tableView: (UITableView*) tableView
heightForRowAtIndexPath: (NSIndexPath*) indexPath;


/*********************************/
/* UITableViewDataSource methods */
/*********************************/

- (UITableViewCell*) tableView: (UITableView*) tableView
		 cellForRowAtIndexPath: (NSIndexPath*) indexPath;

- (NSInteger) numberOfSectionsInTableView: (UITableView*) tableView;

- (NSInteger) tableView: (UITableView*) tableView
  numberOfRowsInSection: (NSInteger) section;


@end





#endif // _MENUVIEWCONTROLLER_H_