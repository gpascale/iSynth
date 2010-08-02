/*
 *  Popup.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/15/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Popup.h"
#include <common.h>

@implementation PopupDelegate

- (PopupDelegate*) initWithParent: (Popup*) parent
{ 
	m_parent = parent; 
	return self;
}

- (void) alertView: (UIAlertView*) view clickedButtonAtIndex: (NSInteger) index
{
	m_parent->onButtonClick(index);
}


- (void) alertView: (UIAlertView*) view didDismissWithButtonIndex: (NSInteger) index
{
	ASSERT(m_parent);
	SAFE_DELETE(m_parent);
}

@end


Popup::Popup(const string& title, const string& body)
{
	m_alertView = [[UIAlertView alloc] init];
	[m_alertView setTitle: NSSTRING(title.c_str())];
	[m_alertView setMessage: NSSTRING(body.c_str())];
	m_alertView.delegate = [[PopupDelegate alloc] initWithParent: this];
}


Popup::~Popup()
{
	[m_alertView.delegate release];
	[m_alertView release];
	FOR(i,0,m_actions.size())
		SAFE_DELETE(m_actions[i]);
}


void
Popup::setTitle(const string& title)
{
	[m_alertView setTitle: NSSTRING(title.c_str())];
}


void
Popup::setBody(const string& body)
{
	[m_alertView setMessage: NSSTRING(body.c_str())];	
}


Popup*
Popup::OneButtonPopup(const string& title, const string& body,
					  const Functor& action, const string& label)
{
		
	Popup*				ret = new Popup(title, body);	
	UIAlertView*		alertView = ret->getAlertView();
	vector<Functor*>&   actions = ret->getActions();
 
	[alertView addButtonWithTitle: NSSTRING(label.c_str())];
	[alertView dismissWithClickedButtonIndex: 0 animated: YES];
	 
	actions.push_back(action.copy());
	
	return ret;
}	 	 
	 
Popup*
Popup::TwoButtonPopup(const string& title, const string& body,
					  const Functor& action1, const string& label1,
					  const Functor& action2, const string& label2)
{
	Popup*				ret = new Popup(title, body);
	UIAlertView*		alertView = ret->getAlertView();
	vector<Functor*>&   actions = ret->getActions();
	 
	[alertView addButtonWithTitle: NSSTRING(label1.c_str())];
	[alertView addButtonWithTitle: NSSTRING(label2.c_str())];
	[alertView dismissWithClickedButtonIndex: 0 animated: YES];
	[alertView dismissWithClickedButtonIndex: 1 animated: YES];
	
	actions.push_back(action1.copy());
	actions.push_back(action2.copy());
	
	return ret;
}	 
	

void
Popup::show(int orientation) const
{
	[m_alertView show];
}


void	 
Popup::onButtonClick(int buttonIndex)
{
	m_actions[buttonIndex]->call();
}
	 

