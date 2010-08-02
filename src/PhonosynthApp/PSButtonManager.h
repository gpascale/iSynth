/*
 *  PSButtonManager.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/4/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PSButtonManager_H_
#define _PSButtonManager_H_

#include <vector>
#include <map>
#include <ButtonLayout.h>
class Button;
class Drawer;

using namespace std;

enum ButtonType
{
	BUTTON_GROUP_NORMAL_CONTROL,
	BUTTON_GROUP_NORMAL_MODAL,
	BUTTON_GROUP_ARCBALL_CONTROL,
	BUTTON_GROUP_ARCBALL_MODAL,
	BUTTON_GROUP_SLIDESHOW_CONTROL,
	BUTTON_GROUP_SLIDESHOW_MODAL,
	BUTTON_GROUP_LOGO
};

class PSButtonManager
{

	typedef map<int, vector<Button*> > ButtonVectors;
	
public:	
	
	PSButtonManager(){ }
	~PSButtonManager(){ }
	
	void	addButton(const Button &b, 
					  ButtonType type,
					  const string& tag = "");
	
	void	addButton(const Button &b, 
					  ButtonType type,
					  ButtonCircle* circle,
					  const string& tag = "");
	

	void	hideButtons(ButtonType type);
	void	showButtons(ButtonType type);	
	
	void	showTaggedButton(const string& tag);
	void	hideTaggedButton(const string& tag);
	
	void	updateTaggedButton(const string& tag, float opacity, float duration);
	void	handleButtonPress(ButtonType type, int clickX, int clickY);
	
	const	vector<Button*>&	getButtonVector(ButtonType type);
	
	void	recomputeButtonLocationInfos();		
	void	adjustForScreenRotation(float screenRotation);
	
	void	addButtonCircle(ButtonCircle* circle) {m_buttonCircles.push_back(circle); }
	Button* getButtonByTag(const string& tag);
	
private:
	
	vector<ButtonCircle*> m_buttonCircles;			
	map<int, vector<Button*> > m_buttonVectors;	
	map<string, Button*> m_tags;
	
};


#endif


