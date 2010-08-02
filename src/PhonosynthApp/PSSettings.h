/*
 *  PSSettings.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/13/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

// A simple structure for maintaining the current settings. There should only
// be one of these, hence the private constructor. Clients use Settings()
// to access the one copy of PSSettings for the whole program

#ifndef _PSSETTINGS_H_
#define _PSSETTINGS_H_


class PSMaster;

class PSSettings
{

public:
	
	static PSSettings& Settings();
	
	float         getTransitionSpeed()		    const { return m_transitionSpeed;          }
	float         getSlideshowSpeed()			const { return m_slideshowSpeed;		   }
	int			  getColorDepth()			    const { return m_colorDepth;			   }
	bool		  getButtonDescriptions()       const { return m_buttonDescriptions;	   }
	bool          getAutorotation()				const { return m_autorotation;		       }
	bool		  getAutoLock()				    const { return m_autoLock;				   }
	int			  getNumPeers()					const { return m_numPeers;				   }
	int			  getNumThreads()				const { return m_numThreads;			   }
	
	void setTransitionSpeed(float f)		  { m_transitionSpeed = f;		    }
	void setSlideshowSpeed(float f);
	void setColorDepth(int n)				  { m_colorDepth = n;			    }
	void setButtonDescriptions(bool b)		  { m_buttonDescriptions = b;		}
	void setAutorotation(bool b)		      { m_autorotation = b;			    }
	void setAutoLock(bool b);
	void setNumPeers(int n)					  { m_numPeers = n;					}
	void setNumThreads(int n);

	void readSettings();		// reads settings from the application's settings bundle	
	void writeSettings();		// writes settings to the applications settings bundle
	
	// some settings require sending a message to app (e.g. changing multivew type).
    void setApp(PSMaster* app) { m_app = app; }
    
private:
	
	float				m_transitionSpeed;			// in seconds
	float				m_slideshowSpeed;			// in seconds	
	bool				m_autoLock;
	int					m_colorDepth;
	bool				m_buttonDescriptions;
	bool				m_autorotation;
	int					m_numPeers;
	int					m_numThreads;
    PSMaster*			m_app;
	
	// nobody should be creating one of these	
	PSSettings();
	PSSettings(const PSSettings& rhs);
	~PSSettings();    	
    
};

inline
PSSettings&
PSSettings::Settings()
{
	static PSSettings settings;
	return settings;
}

#endif 