/*
 *  ButtonCircle.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/14/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "ButtonLayout.h"
#include <Button.h>

ButtonCircle::ButtonCircle(float centerX,
						   float centerY,
						   int radius,
						   int minAngle,
						   int span,
						   bool rotateButtons) :
	m_centerX(centerX),
	m_centerY(centerY),
	m_radius(radius),
	m_minAngle(minAngle),
	m_span(span),
	m_rotateButtons(rotateButtons)
{

}



void
ButtonCircle::apply(vector<Button*>& buttons, float screenWidth, float screenHeight)
{
	const int numButtons = (int) buttons.size();
    const int screenX = m_centerX * screenWidth;
    const int screenY = m_centerY * screenHeight;
    
	float span;
	
	if(m_span == -1)
		span = degToRad(360 * ((numButtons - 1.0f) / numButtons));
	else
		span = degToRad(m_span);
        
	
	const float step = span / (numButtons - 1);
	
	float curAng = degToRad(m_minAngle);
	
	GUI_DEBUG(printf("m_span = %d\n", m_span);
			  printf("span = %f\n", span);
			  printf("numButtons = %d\n", numButtons);
			  printf("step = %f\n", step););
	
	FOR(i,0,numButtons){
		const int x = screenX + m_radius * cos(curAng);
		const int y = screenY - m_radius * sin(curAng);		
		GUI_DEBUG(printf("angle: %f\n", curAng));
		GUI_DEBUG(printf("radius: %f\n", m_radius));
		GUI_DEBUG(printf("positions: %d %d\n", x, y));
        buttons[i]->setLocation(ButtonLocationInfo(BUTTON_LOC_EXPLICIT, x, y));
		buttons[i]->relocate();
		
		//cerr << "x y = " << x << " " << y << endl;
		
		if(m_rotateButtons){
			if(curAng <= PIOVER2) // quadrant 1
				buttons[i]->setRotation(curAng - PIOVER2);
			else if(curAng <= PI)
				buttons[i]->setRotation(curAng - PIOVER2);
			else if(curAng <= 3 * PIOVER2)
				buttons[i]->setRotation(curAng + PIOVER2);
			else
				buttons[i]->setRotation(curAng + PIOVER2);
			//cerr << "set rotation to " << curAng << endl;
		}
		
		curAng += step;
	}
}



OffsetButtonCircle::OffsetButtonCircle(float xOffset, float yOffset,
									   float xCenter, float yCenter, 
									   int radius, int minAngle, int span,
									   bool rotateButtons) :
	m_xOffset(xOffset),
	m_yOffset(yOffset),
	ButtonCircle(xCenter, yCenter, radius, minAngle, span, rotateButtons)
{
	
}



void
OffsetButtonCircle::apply(vector<Button*>& buttons, float screenWidth, float screenHeight)
{
	ButtonCircle::apply(buttons, screenWidth, screenHeight);
	FOR(i,0,buttons.size()){
		int px, py;
		float x, y;
		buttons[i]->getPhysicalLocation(px, py);
		
		x = px; y = py;
		x = (float) px / screenWidth;
		y = (float) py / screenHeight;
		
		if(x > m_centerX){
			if(y > m_centerY)
				buttons[i]->setLocation(ButtonLocationInfo(BUTTON_LOC_EXPLICIT, px + m_xOffset, py + m_yOffset));
			else
				buttons[i]->setLocation(ButtonLocationInfo(BUTTON_LOC_EXPLICIT, px + m_xOffset, py - m_yOffset));
		}
		else{
			if(y > m_centerY)
				buttons[i]->setLocation(ButtonLocationInfo(BUTTON_LOC_EXPLICIT, px - m_xOffset, py + m_yOffset));
			else
				buttons[i]->setLocation(ButtonLocationInfo(BUTTON_LOC_EXPLICIT, px - m_xOffset, py - m_yOffset));
		}
	}
}




