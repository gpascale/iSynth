#ifdef IPHONE

#ifndef _MACTOPLAYER_H_
#define _MACTOPLAYER_H_

#include <TopLayer.h>
#include <iostream>
#include <GLDrawer.h>
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#include <GTimer.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import <EAGLView.h>

// Top Layer handles the flow of control at a level above one particular synth. In particular, this 
// means things like switching between views, opening and closing menus, loading new synths etc...
// In other words, TopLayer manages the high-level state of the application.

@class PhonosynthAppDelegate;
@class AccelerometerDelegate;
@class MainView;
@class OptionsView;
@class LoadingView;
@class EAGLView;
@class MenuView;

@class MenuViewController;

class GTimer;

using namespace std;

class PSMaster;
class GThread;
@class PhonosynthAppDelegate;

struct LoadSynthArgs
{
    string name;
    string url;
};



class MacTopLayer : public TopLayer
{
public:

	MacTopLayer(MenuViewController* mvc, PhonosynthAppDelegate* appDel);
	~MacTopLayer();	    	
    
	void cleanup();
	
    // main flow of control functions       
    
    void beginViewingSynth(const string& synthName, const string& synthURL);            
    void endViewingSynth();
    
    void showOptionsMenu();
    void showHelpScreen();
    
	void enterLowPowerState();
	void leaveLowPowerState();
	
	void setAutoLock(bool b);
	
    //callback function to update the message displayed to the user while loading a synth
    void screenRotated(int newRotation);
	void updateLoadStatus(const string& statusMsg);	                
	
	void writeDataToFile(NSString* popString = nil);
	
	void receivedMemoryWarning();
	
private:
	
    // helper function used during construction
    void initializeViews(); 
    
    // threaded function to do the work of loading and viewing a new synth
    void loadSynthThreaded();
    void loadSynthComplete();    
    
    // timer actions
	void update();
    void loadATexture();	
	void loadNewSynth();		
    
    // TODO: redesign this.
    void render();
    
    
    string                  m_loadSynthStatusBuffer;
    
	PhonosynthAppDelegate	*m_appDel;	
	AccelerometerDelegate   *m_accDel;	
	int						m_tickCount;
	int						m_numTicks; 
    
    GThread*                m_loadSynthThread;
    GThread*                m_textureThread;
    GThread*                m_networkThread;    
    
    // the various views used in the program    
    
    MenuViewController*     m_menuViewController;
    
    LoadSynthArgs           m_loadSynthArgs;
    
    EAGLSharegroup*         m_shareGroup;
	
};

#endif

#endif