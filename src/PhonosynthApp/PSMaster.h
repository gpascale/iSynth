#ifndef _PSMaster_H_
#define _PSMaster_H_

#include <Engine.h>
#include <queue>
#include "CameraSpring.h"
#include "OpacitySpring.h"
#include <GThread.h>
#include <PSSettings.h>
#include <PSTypes.h>
#include <GTimer.h>

class TopLayer;
class DataManager;
class PSSGManager;
class TextureManager;
class PSNetwork;
class GenericCamera;
class SpringyCamera;
class ArcballCamera;
class UI;
class PSUIManager;
class PSCameraMan;
class RenderThread;

enum UserEvent
{
	EVENT_ROTATE				 = 0,
    EVENT_ROTATE_LEFT            = 1,
    EVENT_ROTATE_RIGHT           = 2,
    EVENT_ROTATE_UP              = 3,
    EVENT_ROTATE_DOWN            = 4,	
    EVENT_MOVE_LEFT              = 5,
    EVENT_MOVE_RIGHT             = 6,
    EVENT_MOVE_UP                = 7,
    EVENT_MOVE_DOWN              = 8,
    EVENT_MOVE_FORWARD           = 9,
    EVENT_MOVE_BACKWARD          = 10,
	EVENT_ZOOM_IN				 = 11,
	EVENT_ZOOM_OUT				 = 12,
    EVENT_GOTO_LEFTNEIGHBOR      = 13,
    EVENT_GOTO_RIGHTNEIGHBOR     = 14,
    EVENT_GOTO_UPNEIGHBOR        = 15,
    EVENT_GOTO_DOWNNEIGHBOR      = 16,
    EVENT_GOTO_INNEIGHBOR        = 17,
    EVENT_GOTO_OUTNEIGHBOR       = 18,
	EVENT_GOTO_NEXTIMAGE		 = 19,
	EVENT_GOTO_PREVIMAGE		 = 20,
    EVENT_GOTO_RANDOMNEIGHBOR    = 21,
    EVENT_GOTO_RANDOMIMAGE       = 22,	
	EVENT_END_VIEWING_SYNTH      = 23,
	EVENT_SETTINGS				 = 24,
	EVENT_HELP					 = 25,
	EVENT_ENTER_SLIDESHOW		 = 26,
	EVENT_LEAVE_SLIDESHOW		 = 27,
	EVENT_PLAY_SLIDESHOW		 = 28,
	EVENT_PAUSE_SLIDESHOW		 = 29,
	EVENT_ENTER_ARCBALL          = 30,
	EVENT_LEAVE_ARCBALL          = 31,
    EVENT_TOGGLE_POINTCLOUD      = 32,
    EVENT_TOGGLE_IMAGES          = 33,
    EVENT_TOGGLE_CAMCONES        = 34,
    EVENT_ROTATE_SCREEN          = 35,
	EVENT_NEXT_3DGROUP			 = 36		
};

enum EventParamType
{
	EPTYPE_INT			= 1,	
	EPTYPE_INT_INT		= 2,
	EPTYPE_FLOAT		= 3,
	EPTYPE_FLOAT_FLOAT  = 4,
	EPTYPE_USERPTR		= 5,
	EPTYPE_NONE			= 6
};
	

struct EventParam
{	

	EventParam() :
		type(EPTYPE_NONE)
	{ }
	
	EventParam(int i1) :
		int1(i1), type(EPTYPE_INT)
	{ }
	
	EventParam(int i1, int i2) : 
		int1(i1), int2(i2), type(EPTYPE_INT_INT) 
	{ }
	
	EventParam(float f1) :
		float1(f1), type(EPTYPE_FLOAT)
	{ }
	
	EventParam(float f1, float f2) :
		float1(f1), float2(f2), type(EPTYPE_FLOAT_FLOAT)
	{ }
	
	EventParam(void* ptr, int len) :
		userPtr(ptr), ptrLen(len), type(EPTYPE_USERPTR)
	{ }
	
	EventParamType getType() const{
		return type;
	}
	
	float getFloat1() const{
		ASSERT(type == EPTYPE_FLOAT || type == EPTYPE_FLOAT_FLOAT);
		return float1;
	}
	
	float getFloat2() const{
		ASSERT(type == EPTYPE_FLOAT_FLOAT);
		return float2;
	}
	
	float getInt1() const{
		ASSERT(type == EPTYPE_INT || type == EPTYPE_INT_INT);
		return int1;
	}
	
	float getInt2() const{
		ASSERT(type == EPTYPE_INT_INT);
		return int2;
	}
	
	
private:
    
	EventParamType type;
	union{
		struct{
			int int1;
			int int2;
		};
		struct{
			float float1;
			float float2;
		};
		double		dubs;
		long long	int64;
		struct{
			void		*userPtr;
			int			ptrLen;
		};
	};
};

enum TextureUpdateType
{
	TEXTURE_UPDATE_UPGRADE,
	TEXTURE_UPDATE_DOWNGRADE
};


struct TextureUpdate
{

	TextureUpdate(int iCS, int jProj, TextureUpdateType t) :
		iCoordSystem(iCS),
		jProjector(jProj),
		type(t)
	{ }
	
	int				  getCoordSystem()    const { return iCoordSystem; }
	int				  getProjector()      const { return jProjector;   }
	TextureUpdateType getType()			  const { return type;         }
	
	TextureUpdate(const TextureUpdate &t) :
		iCoordSystem(t.getCoordSystem()),
		jProjector(t.getProjector()),
		type(t.getType())
	{ }
	
private:
	int					iCoordSystem;
	int					jProjector;
	TextureUpdateType	type;
};
	


class PSMaster : public CameraSpringUser
{
public:

	PSMaster();
	~PSMaster();
	
    // setup and initialization
	void		init(PSSGManager *sgManager,
                     PSUIManager* uiManager,
					 TextureManager* textureManager,
                     PSNetwork* network,
                     TopLayer *topLayer);
    
	bool		loadSynth(const string &synthName, const string& synthURL);        
    
    // flow of control   
    void		beginViewingSynth();

    void		enterSlideshowMode();
	void		pauseSlideshowMode();
	void		leaveSlideshowMode();	
	void		enterArcballMode();
	void		leaveArcballMode(int projectorToGoTo);	
    void		toggleArcballMode();
	
	void		slideshowSpeedChanged(float newSpeed);
	void		transitionSpeedChanged(float newSpeed);
	void		pointcloudSimplificationChanged(float newPSS);
	void		autorotationChanged(bool newAR);	
	
    // update/rendering routines
    void        update(); // updates the world and renders to screen
    // called when the camera starts/stops moving. When the camera starts moving, the redraw
    // rate is pumped up and heavier computation is delayed. When the camera stops moving, redraws
    // are stopped and heavy computation resumes
    void		onCameraSpringStart();
	void		onCameraSpringStop();
    void		setNeedsUpdate(bool b, int numTicks = -1);    
    
    // event handling
	void		queueEvent(UserEvent e, EventParam p = EventParam());	
    void        respondToEvent();

    // interaction with Scene
    bool        highlightProjector(int touchX, int touchY);
    bool		selectProjector(int touchX, int touchY);
	bool		selectPoint(int touchX, int touchY);
	bool		selectQRect(int touchX, int touchY, bool goToImage = false);
	void		unselectAllQRects();
			    
	void		enterLowPowerState();
	void		leaveLowPowerState();
	
    // debugging
    void        printSceneSummary(string path);
    void        printCameraInfo();	
	        				
    // random garbage
	PhonosynthMode		getMode() const { return m_mode; }		
	
    
	void				printPortalImages();
	
    void				zoomToPoint(int touchX, int touchY);
	
	
    //convenience hooks for debugging
    void                toggleTexturing();
    void                toggleWireframe();
    
    GenericCamera*      getCamera() { return (GenericCamera*) m_camera; }
	
	void				rotateToOrientation(ScreenOrientation newOrientation);
	
	void				fadeInUI();
	void				fadeOutUI();
	void				resetInactivityTimer();
						
	void				fadeInPeers();
	
	// put this somewhere better
	void				setNumNetworkThreads(int n);
	void			    setAutoLock(bool b);
	
	// cleans up state and resources in between synths
	void				doCleanup();
	
private:
	
	// flow of control helpers	
	void				updateSlideshow();		
	void				updatePanorama();	
	void				showPanoramaImageByProjector(const JsonProjector* p);
	
    bool                goToNeighbor(int iNeighbor);
	void				goToNextCoordSystem();
	void                toggleScreenOrientation();	
	
    // related to quad rendering
    void				makeImageVisible(int iCS, int jProj, float priority = 0.0f);
	void				makeImageInvisible(int iCS, int jProj);
    void				invisifyInvisibleImages();
	void				invisifyAllImages();
	
	
    // this is the MAIN initialization routine. It parses the JSON file and builds everything!
    // called in its own thread, so it can't make GL calls and has to do some funky things as a result
    void                initPhonosynth();
    void                initializeNetwork();        
				    
    
    // this is a very very important function. It moves the camera to the projector specified by
    // iCoordSystem and jProjector. If moveCamera is set to false, it does the appropriate fading
    // in and out of images, but leaves the camera where it is. This is for panoramas, when we wish
    // to show other images without moving
    void                goToImageByCSAndProjectorIndex(int iCoordSystem, 
													   int jProjector,
													   bool moveCamera = true,
													   float fadeDuration = -1.0f);    


	
	void				recomputeDistances( map<const Quad*, int> &distances,
										    int rootCoordSystem,
											int rootProjector);
			
	
            
    // convenience functions which returns the current projector
    const JsonProjector& currentProjector() const;
    
    // convenience function which returns a bitmask representing the current projector's neighbors
    // used primarily for updating the UI control buttons
	unsigned char        getNeighbors(const JsonProjector& projector) const;        
    
    
	// member variables	
	
    CameraSpring                        m_cameraSpring;
    //map<int, vector<OpacitySpring> >    m_opacitySprings;
	
    // kind of a weird data structure used for rendering. As a consequence, we end up only iterating
    // through the visible quads on each rendering pass and skip a lot of unnecessary work
    
    /*
	vector<GPtr<PrioritizedQuad> >		m_visibleQuads;
	vector<vector<PrioritizedQuad*> >	m_prioritizedQuads;
     */
    
    // used for multiview when m_multiviewType is set to MULTIVIEW_TYPE_TRAIL
	vector<int>			m_trail;
    
    
		
	// store action events waiting to be responded to
    queue<UserEvent>    m_eventQueue;
	queue<EventParam>	m_eventParamQueue;								
	
    // used for prioritizing download order based on distance from current image
	map<const Quad*, int>     m_distances;			    	
    
    GTimer				m_slideshowTimer;
	GTimer				m_inactivityTimer;
	
    // references to major Phonosynth constructs
//   PSButtonManager*    m_buttonManager;
    PSSGManager*        m_scenegraphManager;    
    PSUIManager*        m_uiManager;
	TextureManager*		m_textureManager;
    PSNetwork*          m_network;
    PSCameraMan*        m_cameraMan;
    DataManager*        m_dataManager;    	
	TopLayer*           m_topLayer;
    
    // we maintain two cameras, one springy and one arcball. Only one is active at a time,
    // but this way we can switch between the two without newing anything
    //SpringyCamera*              m_springyCamera;    
    //ArcballCamera*              m_arcballCamera;
	SpringyCamera*              m_camera;   
    
    RenderThread*               m_renderThread;
	GThread*					m_textureThread;
    
    // state
    PhonosynthMode		m_mode;                 // the current viewing mode
    GTime				m_now;                  // current time, refreshed when update() is called
	GTime				m_lastEvent;            // the time at which the last event was responded to
	GTime				m_lastSlideshowUpdate;  // the time at which the last slideshow update occurred
    int                 m_curCoordSystem;       // the index of the current coordinate system
    int                 m_curProjector;         // the coordinate-system-wide index of the current projector
	int					m_curPanoramaImg;       // panorama-wide index of the current image in the current panorama
    int					m_panorama;             // the index of the current panorama
    bool                m_frequentUpdate;       // true if we are redrawing frequently (camera is moving)
    bool                m_uiActive;             // whether or not the ui is currently active
	bool				m_active;               // whether or not we are currently viewing a synth
	
	bool				m_showPointcloud;       // whether or not to render the pointcloud
	bool				m_showImages;           // whether or not to render images
	bool				m_showCameraCones;      // whether or not to render camera cones
	
	bool				m_centeredOnImage;      // true when the camera is looking directly at an image  
  	
	PerformFunctorRequestID m_delayedFadeRequest;
	
	
	vector<int>			m_activeProjectors;		// contains the active projector in each coord system
	vector<float>		m_scaleEstimates;		// contains estimates of scale for each coord system
};


/* old stuff
 
 // texture updates waiting to be performed are stored in this structure until the camera stops moving
 TextureUpdateSet		m_textureUpdateSet; 
 
inline
void
PSMaster::makeImageVisible(int iCS, int jProj, float opacity)
{
	//APP_PRINTF("making %d %d visible\n", iCS, jProj);
	ASSERT(iCS < m_prioritizedQuads.size());
	ASSERT(jProj < m_prioritizedQuads[iCS].size());
	
	m_prioritizedQuads[iCS][jProj]->setPriority(opacity);
	
	//m_textureUpdateSet.insert(TextureUpdate(iCS, jProj, TEXTURE_UPDATE_UPGRADE));
}
	

inline
void
PSMaster::makeImageInvisible(int iCS, int jProj)
{
	//APP_PRINTF("making %d %d invisible\n", iCS, jProj);
	ASSERT(iCS < m_prioritizedQuads.size());
	ASSERT(jProj < m_prioritizedQuads[iCS].size());
	
	m_prioritizedQuads[iCS][jProj]->setPriority(2.0f);
	
	//m_textureUpdateSet.insert(TextureUpdate(iCS, jProj,	TEXTURE_UPDATE_DOWNGRADE));
}
 
 // a simple data structure for handling Texture Updates and avoioding duplication	
 

 struct TextureUpdateSet
 {
 set<pair<int, int> > m_upgrades;
 set<pair<int, int> > m_downgrades;
 
 int  size(){ return m_upgrades.size() + m_downgrades.size(); }
 
 bool insert(const TextureUpdate &t)
 {
 if(t.getType() == TEXTURE_UPDATE_DOWNGRADE){
 m_upgrades.erase(make_pair(t.getCoordSystem(), t.getProjector()));
 m_downgrades.insert(make_pair(t.getCoordSystem(), t.getProjector()));
 }
 else{
 m_downgrades.erase(make_pair(t.getCoordSystem(), t.getProjector()));
 m_upgrades.insert(make_pair(t.getCoordSystem(), t.getProjector()));
 }
 }
 
 void clear(){ m_upgrades.clear(); m_downgrades.clear(); }
 
 const set<pair<int, int> >& getUpgrades()   const { return m_upgrades;   }
 const set<pair<int, int> >& getDowngrades() const { return m_downgrades; }
 
 };



typedef vector<GPtr<PrioritizedQuad> > DrawVector;
	
*/
	
	
	
	
	
	
	
	
	
	



#endif

