//
//  SynthDetailViewController.h
//  Phonosynth
//
//  Created by Greg on 12/30/08.
//  Copyright 2008 Brown University. All rights reserved.
//

#import  <UIKit/UIKit.h>
#import  <MenuViewController.h>
#import  <MenuViewCell.h>
#include <PSWeb.h>
#include <GTimer.h>
#include <UnitTest.h>


class GTask;

@interface SynthDetailViewController : UIViewController {

	IBOutlet UIImageView*		m_thumbnailView;
	IBOutlet UITextView*		m_descriptionView;
	
	IBOutlet UIButton*			m_viewSynthButton;
	IBOutlet UIButton*			m_moreSynthsByUserButton;

	IBOutlet UILabel*			m_nameLabel;
	IBOutlet UILabel*			m_numPhotosLabel;
	IBOutlet UILabel*			m_pctSynthyLabel;
	IBOutlet UILabel*			m_numViewsLabel;
	IBOutlet UILabel*			m_userLabel;
	IBOutlet UILabel*			m_dateLabel;

	IBOutlet UIActivityIndicatorView* m_activityIndicator;
	
	WebCollection*				m_collection;
	const UIImage*				m_thumbnail;
	
	GTask*						m_downloadCollectionTask;
	GTimer*						m_tryLoadTimer;
	
	MenuViewController*			m_menuViewController;
	
	PerformFunctorRequestID     m_delayedID;
	
	friend class LoadLotsOfSynths;
}

- (id) initWithCollection: (WebCollection*) collection;
- (id) initWithCollection: (WebCollection*) collection 
				  nibName: (NSString*) nibName 
				   bundle: (NSBundle*) bundle;
- (void) fillWithWebCollection: (const WebCollection*) c;
- (void) setMenuViewController: (MenuViewController*) menuVC;
- (const WebCollection*) getCollection;

- (void) setCollection: (const WebCollection*) c;
- (void) setThumbnail: (UIImage*) thumbnail;

- (IBAction) viewSynthButtonClicked;
- (IBAction) moreSynthsByUserButtonClicked;

@end
