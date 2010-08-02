/*
 *  PSGlobalState.h
 *  Phonosynth
 *
 *  Created by Greg on 12/27/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _PSGLOBALSTATE_H_
#define _PSGLOBALSTATE_H_

// There are a few random things that certain objects need to know. E.g. a Pointcloud needs to
// know if the camera is moving so it can render itself with more or less points. For cases like
// that, it's just easier to keep this singleton around to hold that information rather than
// try to force it unnaturally into the Scenegraph structure. Hooray for overengineering!

#include <PSTypes.h>
#include <Matrix.h>

class JsonCollection;
class JsonProjector;

class PSGlobalState
{

public: 
    
    static bool              setCameraMoving(bool b)                   { get().cameraMoving = b;           }
    static bool              getCameraMoving()                         { return get().cameraMoving;        }
             
    static void              setScreenOrientation(ScreenOrientation o) { get().screenOrientation = o;      }
    static ScreenOrientation getScreenOrientation()                    { return get().screenOrientation;   }
    
    static const string&     getCurrentSynthURL()                      { return get().currentSynthURL;     }
    static void              setCurrentSynthURL(const string& s)       { get().currentSynthURL = s;        }
	
	static JsonCollection*	 getCurrentCollection()					   { return get().currentCollection;   }
	static void				 setCurrentCollection(JsonCollection* c)   { get().currentCollection = c;      }
	
	static void				 setCurProjector(JsonProjector* p)		   { get().curProjector = p;		   }
	static JsonProjector*	 getCurProjector()						   { return get().curProjector;		   }
	
	static void				 setCurCoordSystem(int curCS)			   { get().curCoordSystem = curCS;	   }
	static int				 getCurCoordSystem()					   { return get().curCoordSystem;	   }
	
	static void				 setPointcloudDensity(float f)			   { get().pointcloudDensity = f;      }
	static float			 getPointcloudDensity()					   { return get().pointcloudDensity;   }
	
	static void				 setPointcloudActive(bool b)			   { get().pointcloudActive = b;	   }
	static bool				 getPointcloudActive()					   { return get().pointcloudActive;    }
	
	static void				 setUIActive(bool b)					   { get().uiActive = b;			   }
	static bool				 getUIActive()							   { return get().uiActive;			   }
	
	static void				 setRectsMode(bool b)					   { get().rectsMode = b;			   }
	static bool				 getRectsMode()							   { return get().rectsMode;		   }
	
	static void				 setPortalMatrix(int coordSystem, const Matrix& m)
	{
		if(coordSystem >= get().portalMatrices.size())
			get().portalMatrices.resize(coordSystem + 1, Matrix::IdentityCRef());
		get().portalMatrices[coordSystem] = m;
	}
	
	static const Matrix&	 getPortalMatrix(int coordSystem)
	{
		if(coordSystem >= get().portalMatrices.size())
			BREAK;
		ASSERT(coordSystem < get().portalMatrices.size());
		return get().portalMatrices[coordSystem];
	}
	
	static void	resetImagePriorities()
	{
		get().imagePriorities.assign(get().imagePriorities.size(), IPRIORITY_LOWEST);
	}
	
	static void	setImagePriority(int idx, ImagePriority pri)
	{
		get().imagePriorities[idx] = pri;
	}
	
	static const vector<ImagePriority>& getImagePriorities()
	{
		return get().imagePriorities;
	}
		
	
	static void cleanup(){
		setCameraMoving(false);
		setScreenOrientation(SCREEN_ORIENTATION_PORTRAIT);
		setCurrentSynthURL("");
		setCurrentCollection(NULL);
		setCurProjector(NULL);
		setCurCoordSystem(0);
		setPointcloudDensity(0.1f);
		setRectsMode(true);
		setUIActive(true);
		setPointcloudActive(true);
	}
    
    ScreenOrientation		screenOrientation;
    bool					cameraMoving;
    string					currentSynthURL;
	JsonCollection*			currentCollection;
	int						curCoordSystem;
	vector<ImagePriority>   imagePriorities;
	JsonProjector*			curProjector;
	vector<Matrix>			portalMatrices;
	float					pointcloudDensity;
	bool					rectsMode;
	bool					uiActive;
	bool					pointcloudActive;
    
private:
    
    PSGlobalState() :
		screenOrientation(SCREEN_ORIENTATION_PORTRAIT),
        cameraMoving(false),        
        currentSynthURL(""),
		currentCollection(NULL),
		curProjector(NULL),
		curCoordSystem(0),
		pointcloudDensity(0.1f),
		rectsMode(true),
		uiActive(true),
		pointcloudActive(true)
    { }
    
    ~PSGlobalState() { }
        
    static PSGlobalState& get()
    {
        static PSGlobalState ret;
        return ret;
    }                    

};


#if 0

class ImageCounter
{
public:
	static void inc() { get().m_n++; }
	static void dec() { get().m_n--; }
	
	static int getCount() { return get().m_n; }
	
	
private:
	
	static ImageCounter& get() { static ImageCounter ret; return ret; }	
	ImageCounter() : m_n(0) { }

	int m_n;
};
		

class TextureCounter
{
public:
	static void inc() { get().m_n++; }
	static void dec() { get().m_n--; }
	
	static int getCount() { return get().m_n; }
	
	
private:
	
	static TextureCounter& get() { static TextureCounter ret; return ret; }	
	TextureCounter() : m_n(0) { }
	
	int m_n;
};

#endif // 0

#endif // _PSGLOBALSTATE_H_