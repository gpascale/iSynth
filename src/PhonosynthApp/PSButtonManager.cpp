/*
 *  PSButtonManager.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/4/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "PSButtonManager.h"
#include <Button.h>
#include <Drawer.h>

#if 0

void
PSButtonManager::addButton(const Button &b, 
								   ButtonType type, 
								   const string& tag)
{ 
	printf("addButton %d\n", type);
	
	if(m_buttonVectors.find(type) == m_buttonVectors.end())
		m_buttonVectors[type] = vector<Button*>();
	
	Button* newButton = new Button(b);
	m_buttonVectors[type].push_back(newButton);
	if(tag != "")
		m_tags[tag] = newButton;
	
}


void
PSButtonManager::addButton(const Button &b, 
								   ButtonType type, 
								   ButtonCircle* circle,
								   const string& tag)
{
    
	printf("addButton %d\n", type);
	
	if(m_buttonVectors.find(type) == m_buttonVectors.end())
		m_buttonVectors[type] = vector<Button*>();
	
	Button* newButton = new Button(b);
	m_buttonVectors[type].push_back(newButton);
	circle->addButton(newButton);
			
	if(tag != "")
		m_tags[tag] = newButton;
	
}



void
PSButtonManager::handleButtonPress(ButtonType type, int clickX, int clickY)
{
	
	ButtonVectors::iterator iter;
	if((iter = m_buttonVectors.find(type)) == m_buttonVectors.end())
		return;
	
	FOR(i,0,iter->second.size()){		
		// only allow one button to actually fire. had a problem where a button's
		// action made another button later in the vector active, which would
		// then fire as well.
		if(iter->second[i]->reactIfClicked(clickX, clickY))
			break; 
	}

}



const vector<Button*>& 
PSButtonManager::getButtonVector(ButtonType type)
{
	ButtonVectors::iterator iter;
	ASSERT((iter = m_buttonVectors.find(type)) != m_buttonVectors.end());
	return iter->second;
}



void
PSButtonManager::hideButtons(ButtonType type)
{
	
	ButtonVectors::iterator iter;
	if((iter = m_buttonVectors.find(type)) == m_buttonVectors.end()){
		//cout << "no buttons of type " << type << endl;
		return;
	}
	
	FOR(i,0,iter->second.size())
		iter->second[i]->hide();	

}


void
PSButtonManager::showButtons(ButtonType type)
{
	
	ButtonVectors::iterator iter;
	if((iter = m_buttonVectors.find(type)) == m_buttonVectors.end()){
		//cout << "no buttons of type " << type << endl;
		return;
	}
	
	FOR(i,0,iter->second.size())
		iter->second[i]->show();
	
}


void
PSButtonManager::showTaggedButton(const string& tag)
{
	map<string, Button*>::const_iterator iter;
	if((iter = m_tags.find(tag)) != m_tags.end()){
		iter->second->show();
	}
	else{
		//cout << "no button with tag " << tag << endl;
		exit(1);
	}
}


void
PSButtonManager::hideTaggedButton(const string& tag)
{
	map<string, Button*>::const_iterator iter;
	if((iter = m_tags.find(tag)) != m_tags.end()){
		iter->second->hide();
	}
	else{
		//cout << "no button with tag " << tag << endl;
		exit(1);
	}
	
}


Button*
PSButtonManager::getButtonByTag(const string& tag)
{
	map<string, Button*>::const_iterator iter;
	if((iter = m_tags.find(tag)) != m_tags.end()){
		return iter->second;
	}		
	return NULL;
}



void
PSButtonManager::updateTaggedButton(const string& tag, float opacity, float duration)
{
	Button *b;
	if((b = getButtonByTag(tag)))
		b->setOpacity(opacity, duration);
	else{
		//cout << "button with tag " << tag << " does not exist!" << endl;
		exit(1);
	}
}



void 
PSButtonManager::adjustForScreenRotation(float screenRotation)
{
	FOR(i,0,m_buttonCircles.size()){
		m_buttonCircles[i]->adjustForScreenRotation(screenRotation);
	}
}


#endif // 0





