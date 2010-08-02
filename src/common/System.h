/*
 *  System.h
 *  Phonosynth
 *
 *  Created by Greg on 12/24/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_


class System
{

public:
    
    inline static int   getScreenWidth()  { return get().m_screenWidth;   }
    inline static int   getScreenHeight() { return get().m_screenHeight;  }
	inline static float getScreenAspect() { return get().m_aspect;		  }
	
private:
    
    
    inline static System& get() { static System sys; return sys; }
    
#ifdef IPHONE    
    System() : 
		m_screenWidth(320),
		m_screenHeight(480),
		m_aspect((m_screenWidth + 0.0f) / m_screenHeight)
	{ }
#endif
    
    int   m_screenWidth;
    int   m_screenHeight;
    float m_aspect;

};

#endif _SYSTEM_H_