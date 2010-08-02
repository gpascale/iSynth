/*
 *  ButtonLayout.h
 *  Phonosynth
 *
 *  Created by Greg on 10/30/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _BUTTONLAYOUT_H_
#define _BUTTONLAYOUT_H_

#include <vector>
#include <common.h>

using namespace std;

class Button;

class ButtonLayout
{
    
public:
    virtual void apply(vector<Button*>& buttons, float screenWidth, float screenHeight) = 0;
};


class Button;

using namespace std;

class ButtonCircle : public ButtonLayout
{
    
public:
    
    ButtonCircle(float xCenter = 0.5f, float yCenter = 0.5f, 
                 int radius = 0, int minAngle = 0, int span = -1,
				 bool rotateButtons = false);
    
    // the center of a button circle is defined on a normalized scale so it is independent
    // of screen dimensions
    void	setCenter(float x, float y) { m_centerX = x; m_centerY = y; }
    void	setRadius(int r) { m_radius = r; }
    void	setMinAngle(int a) { m_minAngle = a; }
    void	setSpan(int s) { m_span = s; }
    
    virtual void    apply(vector<Button*>& buttons, float screenWidth, float screenHeight);
    
protected:
    
    float	m_centerX;
    float	m_centerY;
    int		m_radius;
    int		m_minAngle;
    int		m_span;
	bool	m_rotateButtons;
    
};


class OffsetButtonCircle : public ButtonCircle
{
public:
	
	OffsetButtonCircle(float xOffset, float yOffset,
					   float xCenter = 0.5f, float yCenter = 0.5f, 
					   int radius = 0, int minAngle = 0, int span = -1,
					   bool rotateButtons = false);
	
	void    apply(vector<Button*>& buttons, float screenWidth, float screenHeight);
	
private:
	
	float		m_xOffset;
	float		m_yOffset;
		
};

#endif // _BUTTONLAYOUT_H_