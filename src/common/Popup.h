/*
 *  Popup.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/15/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _POPUP_H_
#define _POPUP_H_

#import  <UIKit/UIKit.h>
#include <vector>
#include <string>
#include <Functions.h>

using namespace std;

class Popup;

typedef void(*Action)(Popup*);

@interface PopupDelegate : NSObject
{
	Popup* m_parent;
}

- (void) alertView: (UIAlertView*) view clickedButtonAtIndex: (NSInteger) index;

- (PopupDelegate*) initWithParent: (Popup*) parent;

@end




class Popup
{

public:
	
	~Popup();

	static
	Popup* OneButtonPopup(const string& title, const string& body,
						  const Functor& action, const string& label);
	
	static
	Popup* TwoButtonPopup(const string& title, const string& body,
						  const Functor& action1, const string& label1,
						  const Functor& action2, const string& label2);
	
	static
	Popup* ManyButtonPopup(const string& title, const string& body,
						   const vector<Functor*>& actions,
						   const vector<string>& labels);

	
	UIAlertView*		getAlertView() { return m_alertView; }
	vector<Functor*>&	getActions()   { return m_actions;   }

	
	void				onButtonClick(int buttonIndex);
	
	void				show(int orientation = 0) const;
	void				hide() const { [m_alertView dismiss]; }
	
	void				setTitle(const string& title);
	void				setBody(const string& body);	
	
private:
	
	
	Popup(const string& title = "", const string& body = "");	
	
	vector<Functor*>	m_actions;
	UIAlertView*		m_alertView;
		
};

#endif _POPUP_H_
