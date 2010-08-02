#ifdef IPHONE

#include "MacTopLayer.h"
#include <GLDrawer.h>
#include <Engine.h>
#include <GThread.h>
#include <PSMaster.h>
#include <AccelerometerDelegate.h>
#include <UI.h>
#include <ContextManager.h>
#include <PSSGManager.h>
#include <PSUIManager.h>
#import  <PhonosynthAppDelegate.h>
#import  <ViewControllers.h>
#include <RenderThread.h>
#include <Functions.h>
#include <GTask.h>
#include <Popup.h>
#include <fstream>
#include <InputHandler.h>


bool s_synthLoadSuccess; // stores the return result of an attempt to load a synth


MacTopLayer::MacTopLayer(MenuViewController* mvc, PhonosynthAppDelegate* appDel) : 
    TopLayer(),
	m_numTicks(-1),
	m_tickCount(0),
    m_loadSynthStatusBuffer("")
{    
    //cerr << "yeah mofo im here" << endl;
	
	m_appDel = appDel;
	
    m_menuViewController = mvc;
    [m_menuViewController setTopLayer: this];
    
    // TODO: initialize GL here? perhaps load GL view lazily in future?
    // TODO: remove dependence on EAGLView member fns
    
    EAGLViewController* evc = [m_menuViewController getEAGLViewController];
    [evc initializeGL: m_shareGroup];
    [evc setInputHandler: m_inputHandler];
    [[evc getUnderlyingView] setInputHandler: m_inputHandler];
	m_drawer = new GLDrawer([evc getUnderlyingView]);
    m_ui->setDrawer(m_drawer);    	
	
	m_inputHandler->setView([evc getUnderlyingView]);

    initializeViews();	
}



MacTopLayer::~MacTopLayer()
{
}


void
MacTopLayer::cleanup()
{
	screenRotated(0);
	m_drawer->cleanup();
}


void
MacTopLayer::initializeViews()
{
	
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* documentsDirectory = [paths objectAtIndex: 0];
	NSString* file = [documentsDirectory stringByAppendingPathComponent: @"iSynthState.txt"];	

	if([[NSFileManager defaultManager] fileExistsAtPath: file]){
		ifstream fin(CSTRING(file));
		
		if(fin.bad())
			goto defaultString;
		
		string popString;
		fin >> popString;
		
		NSString* popStr = NSSTRING(popString.c_str());
		
		// make sure the string read in was valid
		if(![popStr hasPrefix: @"USER"] &&
		   ![popStr hasPrefix: @"SEARCH"] &&
		   ![popStr hasPrefix: @"EXPLORE"])
		{
			goto defaultString;
		}
		
		[m_menuViewController populateMenu: popStr
							 updateTabBars: YES];
	}
	else{
		
	defaultString:		
		
		[m_menuViewController populateMenu: @"EXPLORE:MOST_VIEWED_ALLTIME"
							 updateTabBars: YES];
		
	}
	
	[m_appDel setAutoLock: (BOOL) PSSettings::Settings().getAutoLock()];
}
   

void
MacTopLayer::enterLowPowerState()
{
	if(m_app)
		m_app->enterLowPowerState();
}


void
MacTopLayer::leaveLowPowerState()
{
	if(m_app)
		m_app->leaveLowPowerState();
}


void
MacTopLayer::setAutoLock(bool b)
{
	[m_appDel setAutoLock: (BOOL) b];
}


void
MacTopLayer::writeDataToFile(NSString* popString)
{	
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* documentsDirectory = [paths objectAtIndex: 0];
	NSString* file = [documentsDirectory stringByAppendingPathComponent: @"iSynthState.txt"];		
	ofstream fout(CSTRING(file));
	
	ASSERT(!fout.bad());
	
	if(popString == nil)
		popString = [m_menuViewController getLastPopulateString];
	
	ASSERT([popString hasPrefix: @"USER"] ||
		   [popString hasPrefix: @"EXPLORE"] ||
		   [popString hasPrefix: @"SEARCH"]);
	
	fout << CSTRING(popString) << endl;
	
	if([[m_menuViewController navigationController] topViewController] == [m_menuViewController getEAGLViewController])
		m_app->doCleanup();
	
	// put this in its own function	
	EAGLViewController* evc = [m_menuViewController getEAGLViewController];
	[[evc getUnderlyingView] destroyFramebuffers];
}


void
MacTopLayer::updateLoadStatus(const string& text)
{	
	LoadingViewController* lvc = [m_menuViewController getLoadingViewController];
	[lvc performSelectorOnMainThread: @selector(appendStatusText:) 
						  withObject: NSSTRING(text.c_str())
					   waitUntilDone: NO];
}


void
MacTopLayer::screenRotated(int newRotation)
{
    [[[m_menuViewController getEAGLViewController] view] setTag: newRotation];
}



void
MacTopLayer::update()
{
	if(m_numTicks != -1){
		if(++m_tickCount >= m_numTicks){
			m_numTicks = -1;
			m_tickCount = 0;	
		}
	}	
	m_app->update();
}



// This function is called when the user clicks on a synth in the synth menu.
// It spawns off a thread to do the work of loading and building the synth so that
// UI updates can continue to occur and be displayed to the user. Also starts a timer
// to check for the completeness of the loading thread. When that timer detects that
// loading has finished, it does some basic cleanup and begins the viewing experience
// The timer is necessary because that stuff needs to happen from the main thread

void synthDoneCallback()
{
	while(1){
		//cerr << "synth is done biyatch" << endl;
	}
}


bool synthLoadSuccess;


void
MacTopLayer::beginViewingSynth(const string& synthName, const string& synthURL)
{
    
// TODO: make this shit work    
      
	//cerr << "beginViewingSynth" << endl;
	
    [m_menuViewController showLoadingView];
    LoadingViewController* lvc = [m_menuViewController getLoadingViewController];
    [lvc turnOnActivityThingy];
    [lvc setStatusText: @"> Downloading synth..."];    
    
    m_loadSynthStatusBuffer = "";
    
    m_loadSynthArgs.name = synthName;
    m_loadSynthArgs.url  = synthURL;
	MemberFunctor<MacTopLayer> work(this, &MacTopLayer::loadSynthThreaded);
	MemberFunctor<MacTopLayer> callback(this, &MacTopLayer::loadSynthComplete);		   
	
	//cerr << "starting task" << endl;
	
	GTask* task = new GTask(work, callback);
	task->performTask();
	
}


void
MacTopLayer::endViewingSynth()
{
    [m_menuViewController hideEAGLView];
}


void
MacTopLayer::showOptionsMenu()
{
    [m_menuViewController showOptionsView: NO];
}


void
MacTopLayer::showHelpScreen()
{
	[m_menuViewController showAboutView: NO];
}


void
MacTopLayer::loadSynthThreaded()
{        
    s_synthLoadSuccess = m_app->loadSynth(m_loadSynthArgs.name, m_loadSynthArgs.url);
}


void
MacTopLayer::loadSynthComplete()
{	
	if(s_synthLoadSuccess){	
		LoadingViewController* lvc = [m_menuViewController getLoadingViewController];	
		[m_menuViewController showEAGLView];
		[lvc turnOffActivityThingy];		
		
		m_app->beginViewingSynth();
	}
	else{
		
		Popup* p = Popup::OneButtonPopup("", "An unknown error occurred while trying to load the synth",
										 Functor::Identity(), "Ok");
		[m_menuViewController hideLoadingView];
		p->show();
		 
	}
}




void
MacTopLayer::receivedMemoryWarning()
{
	/*
	const string lowMemoryStr = "The system is indicating that it is low on memory. This shouldn't cause iSynth to crash, but it will run faster if you free up some memory by closing extra Safari tabs or turning your phone off and on.";
	
	if([m_menuViewController navigationController].topViewController != [m_menuViewController getEAGLViewController]){
		Popup* p = Popup::OneButtonPopup("", lowMemoryStr, Functor::Identity(), "OK");
		p->show();
	}
	 */
	 
	if(m_textureManager != NULL){
		// shrink the maximum cache capacity and then clear out the cache
		//cerr << "Shrinking texture capacity to " << m_textureManager->scaleTextureCacheCapacity(0.9f);
		//cerr << "Shrinking image capacity to " << m_textureManager->scaleImageCacheCapacity(0.9f);
		m_textureManager->cleanCache();		
	}
	 
}



#endif