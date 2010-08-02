//
//  MenuViewCell.h
//  Phonosynth
//
//  Created by Greg Pascale on 8/16/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#include <string>

class WebCollection;

using namespace std;

@interface MenuViewCell : UITableViewCell {

	IBOutlet UIImageView*	m_thumbnailView;	
	IBOutlet UILabel*		m_numPhotosLabel;
	IBOutlet UILabel*		m_synthinessLabel;
	IBOutlet UILabel*		m_nameLabel;
	IBOutlet UILabel*		m_authorLabel;
	IBOutlet UILabel*		m_numViewsLabel;
	
}


- (void) setThumbnail:	  (UIImage*) img;
- (void) setName:		  (NSString*) name;
- (void) setSynthiness:   (int) synthiness;
- (void) setNumPhotos:	  (int) numPhotos;
- (void) setAuthor:		  (NSString*) author;
- (void) setNumViews:	  (int) numViews;

// convenience function to fill in the cell with the contents of a WebCollection
- (void) fillWithWebCollection: (const WebCollection*) c;


@end

