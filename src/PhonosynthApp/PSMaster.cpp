#include "PSMaster.h"
#include <TestCode.h>
#include <iostream>
#include <DataManager.h>
#include <GregJsonCollection.h>
#include <GregJsonProjector.h>
#include <JsonImage.h>
#include <fstream>
#include <Button.h>
#include <TopLayer.h>
#include <SpringyCamera.h>
#include <UI.h>
#include <PSUIManager.h>
#include <Pointcloud.h>
#include <Animator.h>
#include <CameraCone.h>
#include <PSCameraMan.h>
#include <QRect.h>
#include <PSNetwork.h>
#include <PSSGManager.h>

#include <PSGlobalState.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include <OneShotDownloader.h>
#include <SeadragonCollection.h>
#include <SeadragonTile.h>

#include <RenderThread.h>



map<string, string> JSON_URLS;
int mallocCount;
const int TRAIL_SIZE = 4;

// a simple struct used in the BFS for computing distances to Quads
struct BFSState
{
	int iCoordSystem;
	int jProjector;
	int depth;
	
	BFSState(int iCs, int jP, int D) : iCoordSystem(iCs), jProjector(jP), depth(D) { }
	
	bool operator < (const BFSState &s) const
	{
		if(depth < s.depth) return true;
		if(depth > s.depth) return false;
		
		if(iCoordSystem < s.iCoordSystem) return true;
		if(iCoordSystem > s.iCoordSystem) return false;
		
		if(jProjector < s.jProjector) return true;
		return false;
	}
};


PSMaster::PSMaster() :
	m_camera(NULL),
    m_uiManager(NULL),
	m_topLayer(NULL),
    m_network(NULL),
    m_curCoordSystem(0),
	m_uiActive(false),
	m_trail(TRAIL_SIZE, -1),
	m_mode(MODE_NONE),
	m_panorama(-1),
	m_active(true),
	m_frequentUpdate(false),
	m_showImages(true),
	m_showCameraCones(false),
	m_showPointcloud(true),
	m_centeredOnImage(true),
	m_slideshowTimer(MemberFunctor<PSMaster>(this, &PSMaster::updateSlideshow)),
	m_inactivityTimer(MemberFunctor<PSMaster>(this, &PSMaster::fadeOutUI), 0.2f),
	m_renderThread(NULL),
	m_textureThread(NULL)
{	
    PSSettings::Settings().setApp(this);
	m_now.update();
	m_lastSlideshowUpdate = m_lastEvent = m_now;
	mallocCount = 0;
	//cout << "PhotosynthMaster::PhotosynthMaster()" << endl;		
}


void
PSMaster::init(PSSGManager* sgManager,
               PSUIManager* uiManager,
			   TextureManager* textureManager,
               PSNetwork* network,
               TopLayer* topLayer)
{

	m_scenegraphManager = sgManager;    
    m_uiManager = uiManager;
	m_textureManager = textureManager;
    m_network = network;
	m_topLayer = topLayer;
    
    //set up the camera and tell the scene about it
    m_camera = new SpringyCamera();
    m_camera->setSpring(&m_cameraSpring);
    m_cameraSpring.setTarget(m_camera);
    m_cameraSpring.setParent(this);
    
    m_cameraMan = new PSCameraMan(m_camera);    
    m_dataManager = new DataManager();	

}


PSMaster::~PSMaster()
{
	// PSMaster does not manage resources. Top Layer handles that
}




void
PSMaster::printSceneSummary(string path)
{
	return;
	
    ofstream fout(path.c_str());
    JsonCollection *coll = m_dataManager->getCollection();

    CoordSystems coordSystems  = coll->getCoordSystems();        
    JsonImages images          = coll->getImages();

    fout << "Summary of parsed information for scene " << m_dataManager->getSynthName() << endl;

    FOR(i,0,coordSystems.size()){
        fout << "Coordinate System " << i << ":" << endl;
        fout << "\t# Projectors: " << coordSystems[i]->getNumProjectors() << endl;
        Projectors projectors = coordSystems[i]->getProjectors();
        FOR(j,0,projectors.size()){
            assert(projectors[j]->getLocalIndex() == j);
            fout << "\tProjector " << j << ":" << endl;
            fout << "\t\tPosition: " << projectors[j]->getPosition() << endl;
            fout << "\t\tRotation: " << projectors[j]->getRotation() << endl;
            fout << "\t\tRotation Matrix: " << endl << projectors[j]->getRotationMatrix() << endl;
            fout << "\t\tDominant Plane: " << projectors[j]->getDominantPlane() << endl;
            fout << "\t\tAspect Ratio: " << projectors[j]->getAspectRatio() << endl;  
            fout << "\t\tGlobal ID: " << projectors[j]->getGlobalIndex() << endl;
        }
    }
}


void
PSMaster::printCameraInfo()
{    
    //cout << "Camera position = " << m_camera->getPosition() << endl;
    //cout << "View matrix = " << endl << m_camera->getViewMatrix() << endl;
}


void
PSMaster::beginViewingSynth()
{
    
    m_network->start();
    m_uiManager->hideMenu();
    m_uiManager->modeChanged(MODE_NORMAL);
	fadeInUI();
    
	m_scenegraphManager->showQuads();
	m_scenegraphManager->showPointcloud();
	m_scenegraphManager->hideCameraCones();
	
    m_scenegraphManager->showQuads(m_curCoordSystem, 0.0f);
    m_scenegraphManager->showPointclouds(m_curCoordSystem, 0.0f);
    m_scenegraphManager->hideCameraCones(m_curCoordSystem, 0.0f);
    
    m_mode = MODE_NORMAL;
    goToImageByCSAndProjectorIndex(m_curCoordSystem, m_curProjector, true);

	// start up the render thread		
	
	/*
	if(m_renderThread){
		m_renderThread->resume();
	}
	else{
	 */
		m_renderThread = new RenderThread(m_scenegraphManager, m_uiManager, m_topLayer->getDrawer(), 
										  m_camera, m_cameraMan);
		m_renderThread->start();
	
	//}
	
	
	// start up the texture thread	
	m_textureThread = new GThread();
	MemberFunctor<TextureManager> f(m_textureManager, &TextureManager::run);
	m_textureThread->setFunction(f);
	m_textureThread->start();
	
	// start the timer which fades out the UI after 5 seconds of inactivity
	m_inactivityTimer.start();
	
	// TODO: take this out
	TextureMemory::printActiveTexIDs();
}





void
PSMaster::doCleanup()
{
	//cerr << "cleanup begin" << endl;
	
	// first things first, stop rendering
	
	m_renderThread->cancel();
	// wait for the render thread to actually exit
	while(m_renderThread->isRunning())
		GThread::sleep(0.05f);
	SAFE_DELETE_AND_CLEAR(m_renderThread);


	// stop the texture thread. When the texture thread exits, it will automatically clean up
	// the texture manager, so we don't need to do that
	m_textureThread->cancel();
	// wait for the texture thread to actually exit, probably not necessary but why not?
	while(m_textureThread->isRunning())
		GThread::sleep(0.05f);
	
	SAFE_DELETE_AND_CLEAR(m_textureThread);
	
	
	m_network->stop();
	
	// cancel any pending request to fade in peers. This would cause a crash since we're
	// ripping apart the scenegraph
	GTimer::cancelPerformFunctorRequest(m_delayedFadeRequest);
		
	// stop the slideshow if it's playing	
	m_slideshowTimer.stop();
	
	// stop checking for inactivity
	m_inactivityTimer.stop();
	
	m_dataManager->reset();
        
	m_showImages = true;
	m_showPointcloud = false;
	m_showCameraCones = false;
	
	m_uiManager->modeChanged(MODE_NORMAL);
	m_mode = MODE_NORMAL;
	
	PSGlobalState::cleanup();
	m_dataManager->cleanup();
	m_cameraMan->cleanup();
	m_network->cleanup();
    m_scenegraphManager->cleanup();
	m_uiManager->cleanup();
	m_topLayer->cleanup();
	
	
	m_activeProjectors.clear();
	
	//cerr << "cleanup end" << endl;

}


void
PSMaster::enterLowPowerState()
{
	if(m_renderThread)
		m_renderThread->pause();
	if(m_textureThread)
		m_textureManager->pause();
	if(m_mode == MODE_SLIDESHOW_PLAYING)
		pauseSlideshowMode();
}


void
PSMaster::leaveLowPowerState()
{
	if(m_renderThread)
		m_renderThread->resume();
	if(m_textureManager)
		m_textureManager->resume();
	if(m_mode == MODE_SLIDESHOW_PAUSED)
		enterSlideshowMode();
}


ByteArray	   g_sdcData;			// container for the raw bytes of the Seadragon collection file
DownloadStatus g_sdcDownloadStatus;	// status of the Seadragon download, will be set to true when it completes


void
PSMaster::initPhonosynth()
{    	
	
	APP_PRINTF("initPhonosynth()");		
	
    JsonCollection *coll = m_dataManager->getCollection();
    
    static  CoordSystems    coordSystems;
    static  Projectors      projectors;    
    static  JsonImages      images;
	
	int n = 0;	
	// before we get started doing any heavy duty work, submit an asynchronous request to download 
	// the Seadragon collection in the background. Hopefully it will be finished by the time we're
	// done building the scene
	OneShotDownloader d;
	d.asynchronousGET(PSGlobalState::getCurrentSynthURL() + ".dzc",
					  &g_sdcData,
					  &g_sdcDownloadStatus);
	
	// initialize the scenegraph manager for the current synth.
	m_scenegraphManager->init(m_camera);
	
    coordSystems    = coll->getCoordSystems();        
    images          = coll->getImages();    
	
	
	m_topLayer->updateLoadStatus("done\n");
	m_topLayer->updateLoadStatus("> Arranging Photos...");	
	int numP = 0;
	FOR(i,0,coordSystems.size()){
		numP += coordSystems[i]->getNumProjectors();
	}
	
	//m_scaleEstimates.resize(coordSystems.size(), 10.0f);
	

    for(int i = 0; i < (int) coordSystems.size(); i++){		
		
        JsonCoordSystem *cs = coordSystems[i];        
        projectors = cs->getProjectors();		
        
		//m_scaleEstimates[i] = coordSystems[i]->estimateScale(0.9f);
		
        for(int j = 0; j < (int) projectors.size(); j++){
			
            JsonProjector *projector = projectors[j];
			
            // create a Quad that will display this projector's image. Configure it using the projector
            Quad* q = new Quad(m_camera);
            q->initializeWithProjector(projector);                                                                                   
            m_scenegraphManager->addQuad(i, j, q);
            
            // Create a camera cone that will represent this projector in arcball mode
            CameraCone* cc = new CameraCone(projector, 1.0f);
			m_scenegraphManager->addCameraCone(i, j, cc);
			
			// add a QRect for this projector
			QRect* qr = new QRect(projector);
			m_scenegraphManager->addQRect(i, j, qr);		
			
        }
        
        Pointcloud* pc = new Pointcloud();
        m_scenegraphManager->addPointcloud(i, pc);		
		
    }							            
	 
	 
	m_topLayer->updateLoadStatus("done\n");	
	m_topLayer->updateLoadStatus("> Finalizing...");
	
	// at this point, the scene is constructed. Only a little bit more initialization is needed
	
	// set all the portal matrices initially to the identity
	
	PSGlobalState::setPortalMatrix(coordSystems.size(), Matrix::IdentityCRef());
	
	// initialize the texture manager
	m_textureManager->initialize(coll->getImages().size(), 9);		
	// initialize the network
    initializeNetwork();	
	
	// set the starting location
	coll->getStartingPoint(m_curCoordSystem, m_curProjector);
	
	m_activeProjectors.resize(coordSystems.size(), 0);
	
	m_topLayer->updateLoadStatus("done\n");
	
	/*
	FOR(i,0,(int) coordSystems.size()){
		//cerr << "SCALE ESTIMATE FOR COORDINATE SYSTEM " << i << ": " << coordSystems[i]->estimateScale(0.9f) << endl;
	}
	 */
	
}




void
PSMaster::initializeNetwork()
{	
	
    JsonCollection *coll = m_dataManager->getCollection();        
    // 1) Configure pointclouds to download
    vector<pair<int, int> > indices;
	vector<string>          urls;    
	FOR(i,0,coll->getNumCoordSystems()){
		const JsonCoordSystem &curCS = coll->getCoordSystem(i);
		const int numBlobs = curCS.getNumBlobs();
		if(i > 2 && numBlobs <= 1)
			continue;
        FOR(j,0,min(curCS.getNumBlobs(), 10)){
            ostringstream oss;
            oss << coll->getBaseURL() << "/points_" << i << "_" << j << ".bin";
            indices.push_back(make_pair(i, j));
            urls.push_back(oss.str());
        }
	}
    
    m_network->setImagesToDownload(coll->getImages());
	m_network->setPointcloudsToDownload(indices, urls);
	
	
	
	// wait for the seadragon collection file to finish downloading
	while(g_sdcDownloadStatus == DS_DOWNLOADING)
		GThread::sleep(0.05f);
		
	if(g_sdcDownloadStatus == DS_SUCCEEDED){
		try{
			SeadragonCollection sdc(g_sdcData,
									PSGlobalState::getCurrentSynthURL() + "_files",
									coll->getImages());			
			vector<SeadragonTile> tiles;
			sdc.getTiles(4, tiles);
			if(coll->getNumImages() < 800)
				sdc.getTiles(5, tiles);
			if(coll->getNumImages() < 300)	// download level 6 tiles unless the synth is ginormous
				sdc.getTiles(6, tiles);
			if(coll->getNumImages() < 50)   // download level 7 tiles if the synth is pretty small
				sdc.getTiles(7, tiles);
			m_network->setSeadragonTilesToDownload(tiles);	
		}
		catch (SeadragonException& e){
			//cerr << "error parsing Seadragon Collection file" << endl;
		}
	}
    
}
            

// clear the current synth and load a new one
bool
PSMaster::loadSynth(const string& synthName, const string& jsonURL)
{		
	// the whole DataManager thing is really antiquated and crappy. Should rewrite it when there's time	
	m_dataManager->setSynthName(synthName);
	m_dataManager->setSynthURL(jsonURL);	
	
	if(!m_dataManager->loadJson(true))
		return false; // return false if there was an error
		
	// initialize the synth and go
    initPhonosynth();
	m_active = true;
	
	return true;
}


void
PSMaster::printPortalImages()
{
	const JsonImages& images = m_dataManager->getCollection()->getImages();
	vector<vector<pair<int, int> > > v(images.size());
	
	CoordSystems coordSystems    = m_dataManager->getCollection()->getCoordSystems();
	
    for(int i = 0; i < (int) coordSystems.size(); i++){
		
		m_topLayer->updateLoadStatus("> Building Coord System " + i2s(i) + "...");		
        JsonCoordSystem *cs = coordSystems[i];        
		Projectors projectors = cs->getProjectors();
        
        for(int j = 0; j < (int) projectors.size(); j++){
			int imgIndex = projectors[j]->getImageIndex();
			v[imgIndex].push_back(make_pair(i, j));
		}
	}
	
	/*
	FOR(i,0,v.size()){
		//cerr << "Image " << i << ":";
		FOR(j,0,v[i].size()){			
			if(j != 0)
				//cerr << ", ";
			//cerr << "(" << v[i][j].first << ", " << v[i][j].second << ")";
		}
		//cerr << endl;
	}
	 */
}



void
PSMaster::goToImageByCSAndProjectorIndex(int iCoordSystem, 
										 int jProjector, 
										 bool moveCamera,
										 float fadeDuration)
{	    			
	const float moveDuration = PSSettings::Settings().getTransitionSpeed();
	
	if(fadeDuration < 0.0f){
		if(m_mode == MODE_NORMAL)
			fadeDuration = moveDuration;
		else
			fadeDuration = min(PSSettings::Settings().getSlideshowSpeed(), moveDuration);
	}
	
	printf("going to image %d %d\n", iCoordSystem, jProjector);
    
	Quad* oq = m_scenegraphManager->getQuad(m_curCoordSystem, m_curProjector);
    Quad* nq  = m_scenegraphManager->getQuad(iCoordSystem, jProjector);

	// tell the network manager that we're moving to a new image so it can reprioritize and
	// otherwise get its shit together
	PSGlobalState::resetImagePriorities();	
	PSGlobalState::setCurProjector(nq->getProjector());
	
	m_network->movedToProjector(nq->getProjector());
    
	//printf("image index: %d\n", nq->getImageIndex());
	
    const JsonProjector* p = nq->getProjector();
    
	const JsonCollection *coll = m_dataManager->getCollection();
	
	// coord systems
	const int iOldCoordSystem = m_curCoordSystem;
	const int iNewCoordSystem = iCoordSystem;
	ASSERT(iNewCoordSystem < coll->getNumCoordSystems());
	const JsonCoordSystem& oldCoordSystem  = coll->getCoordSystem(iOldCoordSystem);
	const JsonCoordSystem& newCoordSystem  = coll->getCoordSystem(iNewCoordSystem);		
	
	// projectors
	const int jOldProjector	= m_curProjector;
	const int jNewProjector	= jProjector;
	ASSERT(jNewProjector < newCoordSystem.getNumProjectors());
	const JsonProjector& oldProjector = oldCoordSystem.getProjector(jOldProjector);
    const JsonProjector& newProjector = newCoordSystem.getProjector(jNewProjector);		
		
    ////cout << "Projector at " << p->getPosition() << endl;    
    ////cout << "Quad center at " << nq->getCenter() << endl;
    ////cout << "Desired look vector: " << (nq->getCenter() - p->getPosition()).normalized() << endl;
    
	if(iCoordSystem != m_curCoordSystem)
		m_scenegraphManager->coordinateSystemChanged(m_curCoordSystem, iCoordSystem);
			   

	// where camera will move from-to
    const Point3& startPos = m_camera->getPosition();
    const Point3  endPos   = newProjector.getPosition();
		 
	Quad* q;
		
	if(newProjector.getCoordSystem().getIndex() != m_curCoordSystem){
		const vector<const JsonProjector*>& portalNeighbors = oldProjector.getPortalNeighbors();		
		FOR(i,0,portalNeighbors.size()){
			const JsonProjector* pnbr = portalNeighbors[i];
			if(pnbr->getCoordSystem().getIndex() == iCoordSystem){

				
				CameraSpring* spr = m_camera->getSpring();
				spr->stop(true);
				spr->setStartPosition(pnbr->getPosition());
				spr->setStartRotation(pnbr->getCorrectedViewMatrix().toQuaternion());
				spr->setStartFocalLength(pnbr->getFocalLength());									
				 				
				Quad* oldQuad = m_scenegraphManager->getQuad(m_curCoordSystem, m_curProjector);
				Quad* neighQuad = m_scenegraphManager->getQuad(iCoordSystem, pnbr->getLocalIndex());
				
				neighQuad->setActive(true);
				neighQuad->setDrawPriority(PRIORITY_QUAD_MAIN);
				neighQuad->setOpacity(oldQuad->getOpacity());
				neighQuad->getSpring()->setFromOpacity(oldQuad->getOpacity());
				
				FOR(j,0,oldProjector.getCoordSystem().getProjectors().size()){
					q = m_scenegraphManager->getQuad(m_curCoordSystem, j);
					q->setOpacity(0.0f);
					q->getSpring()->setFromOpacity(0.0f);
					q->setActive(false);
				}
				
				/*
				Matrix portalMat;
				//Matrix::InferAffineTransform(portalMat, oldProjector.getCorrectedViewMatrix(), pnbr->getCorrectedViewMatrix());
				
				Matrix pnbrm = pnbr->getCorrectedViewMatrix();
				Matrix opm = oldProjector.getCorrectedViewMatrix();
								
				Matrix::InferAffineTransform(portalMat, pnbrm, opm);
				PSGlobalState::setPortalMatrix(m_curCoordSystem, portalMat);
				PSGlobalState::setPortalMatrix(iCoordSystem, Matrix::IdentityCRef());
								
				
				 */														
								
				break;
			}
		}
	}
	
	PSGlobalState::setCurCoordSystem(iCoordSystem);	
	
   
	// first, fade out all quads
	m_scenegraphManager->fadeOutAllQuads(fadeDuration);
		
	// peers will be brought in the camera stops for more FPS during the transition
	
	// finally fade in the central quad to full opacity
	q = m_scenegraphManager->getQuad(iCoordSystem, jProjector);
	q->setDrawPriority(PRIORITY_QUAD_MAIN);
	q->fadeIn(1.0f, fadeDuration);	
	
	//m_scenegraphManager->fadeOutAllQRects();
	

	
	
	// show rects
	// TODO: Am I calling these fucking things quads or rects?		
	
	m_scenegraphManager->fadeOutAllQRects(1.0f, true);
	const vector<const JsonProjector*>& rects = newProjector.getQuads();
	const int nRects = (const int) rects.size();
	
	FOR(i,0,nRects){
		if(rects[i]->getCoordSystem().getIndex() == iCoordSystem){
			const int idx = rects[i]->getLocalIndex();		
			QRect* cur = m_scenegraphManager->getQRect(iCoordSystem, idx);
			cur->turnOn();
		}
	}
	 
	
    
    if(m_mode != MODE_SLIDESHOW_PLAYING)
		m_uiManager->updateControlButtons(getNeighbors(newProjector));
    
    if(moveCamera){
        m_cameraMan->goToProjector(newProjector, moveDuration);
    }
    
    // update the pointcloud
    Pointcloud* oldPC = m_scenegraphManager->getPointcloud(m_curCoordSystem);
    Pointcloud* newPC = m_scenegraphManager->getPointcloud(iCoordSystem);
    oldPC->setActive(false); // do draw new pointcloud
    newPC->setActive(true);
    	
	
	m_scenegraphManager->invalidateScenegraph();
	
    m_curCoordSystem = iCoordSystem;
    m_curProjector   = jProjector;
	
	m_panorama = newProjector.getPanorama();
	
	m_centeredOnImage = true;
	
	m_activeProjectors[m_curCoordSystem] = m_curProjector;
	
	if(m_delayedFadeRequest)
		GTimer::cancelPerformFunctorRequest(m_delayedFadeRequest);
		
	MemberFunctor<PSMaster> fadeInPeersFunctor(this, &PSMaster::fadeInPeers);
	m_delayedFadeRequest = GTimer::performFunctorAfterDelay(fadeInPeersFunctor, moveDuration);
	
	// put the texture thread to sleep while we're transitioning to keep the framerate up
	m_textureManager->sleepForDelay(0.9f * moveDuration);
}


void
PSMaster::fadeInPeers()
{
	// make sure we haven't entered arcball mode in the time between when the fade request was made and
	// now. Should be 1.5 seconds
	if(m_mode == MODE_ARCBALL)
		return;
	
	JsonCollection* coll = m_dataManager->getCollection();
	JsonCoordSystem& cs = coll->getCoordSystem(m_curCoordSystem);
	JsonProjector& p = cs.getProjector(m_curProjector);			
	
	const vector<const JsonProjector*>& peers = p.getPeers();
	const int nPeers = min(PSSettings::Settings().getNumPeers(), (const int) peers.size());
	
	FOR(i,0,nPeers){
		const JsonProjector* proj = peers[i];
		Quad* q = m_scenegraphManager->getQuad(proj->getCoordSystem().getIndex(), 
											   proj->getLocalIndex());
		q->setDrawPriority(PRIORITY_QUAD_NEIGHBOR);
		q->fadeIn(0.5f, 0.5f);		
	}
	
	m_scenegraphManager->invalidateScenegraph();
}


void
PSMaster::onCameraSpringStop()
{		
	PSGlobalState::setCameraMoving(false);
	
	if(m_mode == MODE_ARCBALL){
		if(m_renderThread)
			m_renderThread->setUseOffscreenBuffer(true);
	}
	else{
		// if we've rotated away, snap back to the current image
		if(!m_centeredOnImage){
			const JsonCollection *coll = m_dataManager->getCollection();
			const JsonCoordSystem& cs  = coll->getCoordSystem(m_curCoordSystem);
			const JsonProjector& p = cs.getProjector(m_curProjector);	
			m_cameraMan->goToProjector(p, 1.0f);
			m_centeredOnImage = true;
		}
		else{										
			if(m_renderThread)
				m_renderThread->setUseOffscreenBuffer(true);
		}
	}
}


void
PSMaster::onCameraSpringStart()
{
    // update the global state to reflect that the camera is moving
    PSGlobalState::setCameraMoving(true);
    
	if(m_renderThread)
		m_renderThread->setUseOffscreenBuffer(false);
	
	if(m_mode == MODE_ARCBALL)		
		m_renderThread->startRendering();
	
}



bool
PSMaster::goToNeighbor(int iNeighbor)
{
	
    JsonCollection *coll      = m_dataManager->getCollection();
    const JsonCoordSystem &c  = coll->getCoordSystem(m_curCoordSystem);      
    const JsonProjector &p    = c.getProjector(m_curProjector);
    
    const JsonProjector* toProj = p.getNeighbor(iNeighbor);
    if(toProj != NULL){
        goToImageByCSAndProjectorIndex( toProj->getCoordSystem().getIndex(),
									    toProj->getLocalIndex());
        return true;
    }	

    return false;
}


void
PSMaster::goToNextCoordSystem()
{
	const int nCoordSystems = m_dataManager->getCollection()->getNumCoordSystems();
	const int newCoordSystem = (m_curCoordSystem + 1) % nCoordSystems;
	goToImageByCSAndProjectorIndex(newCoordSystem, m_activeProjectors[newCoordSystem]);
}

void
PSMaster::zoomToPoint(int touchX, int touchY)
{
	// TODO:
}


void
PSMaster::recomputeDistances(map<const Quad*, int>& distances,
                             int rootCoordSystem,
                             int rootProjector)
{	
	if(m_mode == MODE_PANORAMA)
		return;
	
	const JsonCollection *coll = m_dataManager->getCollection();
	
	distances.clear();
	
	queue<BFSState> Q;
	Q.push(BFSState(rootCoordSystem, rootProjector, 0));
	distances[m_scenegraphManager->getQuad(rootCoordSystem, rootProjector)] = 0;
	
	// run the bfs
	while(!Q.empty()){		
		const BFSState &curState   = Q.front();		
		int iCoordSystem           = curState.iCoordSystem;
		int jProjector             = curState.jProjector;
		int depth                  = curState.depth;
		Q.pop();
		
		const JsonCoordSystem   &cs		   = coll->getCoordSystem(iCoordSystem);
		const JsonProjector     &proj      = cs.getProjector(jProjector);
		
		// explore neighbors in connectedness-order (used during normal browsing)
		if(m_mode == MODE_NORMAL || m_mode == MODE_PANORAMA){		
			const JsonProjector* const * neighbors  = proj.getAllNeighbors();		
			FOR(i,0,6){
				if(neighbors[i]){				
					int newCS   = neighbors[i]->getCoordSystem().getIndex();
					int newProj = neighbors[i]->getLocalIndex();				
					BFSState newBFSState(newCS, newProj, depth + 1);				
					const Quad* key = m_scenegraphManager->getQuad(newCS, newProj);								
					if(distances.find(key) == distances.end()){
						distances[key] = depth + 1;
						Q.push(newBFSState);
					}				
				}
			}
		}
		
		// explore neighbors in Travelling Salesman order (used during slideshow mode)
		else if(m_mode == MODE_SLIDESHOW_PLAYING || m_mode == MODE_SLIDESHOW_PAUSED){
			JsonProjector *next = proj.getNextNeighbor();
			ASSERT(next);
			int newCS   = next->getCoordSystem().getIndex();
			int newProj = next->getLocalIndex();
			BFSState newBFSState(newCS, newProj, depth + 1);
			const Quad* key = m_scenegraphManager->getQuad(newCS, newProj);								
			if(distances.find(key) == distances.end()){
				distances[key] = depth + 1;
				Q.push(newBFSState);
			}
		}        
	}
	
	FOR(i,0,coll->getNumCoordSystems()){
		const JsonCoordSystem &cs = coll->getCoordSystem(i);
		FOR(j,0,cs.getNumProjectors()){
			const Quad* key = m_scenegraphManager->getQuad(i, j);				
			if(distances.find(key) == distances.end()){												
				// set unreachable nodes to have "infinite" distance
				distances[key] = 10000;
			}
		}
	}		
}



// hooks for settings changes

void
PSMaster::slideshowSpeedChanged(float newSpeed)
{
	m_slideshowTimer.setTicksPerSec(1.0f / newSpeed);
}


void
PSMaster::transitionSpeedChanged(float newSpeed)
{
	// no-op, included for consistency
}


void
PSMaster::pointcloudSimplificationChanged(float newPSS)
{

}



void
PSMaster::autorotationChanged(bool newAR)
{
	
}


void 
PSMaster::updateSlideshow()
{
	const JsonCollection *coll = m_dataManager->getCollection();
	const JsonCoordSystem &cs = coll->getCoordSystem(m_curCoordSystem);
	const JsonProjector &curProj = cs.getProjector(m_curProjector);
	
	const JsonProjector* nextProj = curProj.getNextNeighbor();
	
	int   iNextCoordSystem = nextProj->getCoordSystem().getIndex();
	int   jNextProjector = nextProj->getLocalIndex();
	
	goToImageByCSAndProjectorIndex(iNextCoordSystem, jNextProjector);
}


void
PSMaster::fadeOutUI()
{
	m_inactivityTimer.stop();
	m_uiManager->fadeOutControls();
	m_uiActive = false;	
	PSGlobalState::setUIActive(false);
}


void
PSMaster::fadeInUI()
{
	m_uiManager->fadeInControls(getNeighbors(currentProjector()));
	m_uiActive = true;
	PSGlobalState::setUIActive(true);
	m_inactivityTimer.start();
}


void
PSMaster::resetInactivityTimer()
{
	m_inactivityTimer.stop();
	m_inactivityTimer.start();
}


void
PSMaster::setNumNetworkThreads(int n)
{
	if(m_network)
		m_network->setNumConcurrentDownloads(n);
}


void
PSMaster::setAutoLock(bool b)
{
	ASSERT(m_topLayer);
	m_topLayer->setAutoLock(b);
}


void
PSMaster::enterSlideshowMode()
{    	
    m_uiManager->modeChanged(MODE_SLIDESHOW_PLAYING);
    m_mode = MODE_SLIDESHOW_PLAYING;
	
	m_slideshowTimer.setTicksPerSec(1.0f / PSSettings::Settings().getSlideshowSpeed());
	m_slideshowTimer.start();
}


void
PSMaster::pauseSlideshowMode()
{
	if(m_mode == MODE_SLIDESHOW_PAUSED)
		return;
	
	ASSERT(m_mode == MODE_SLIDESHOW_PLAYING);
	
	m_uiManager->modeChanged(MODE_SLIDESHOW_PAUSED);
	m_mode = MODE_SLIDESHOW_PAUSED;
	
	m_slideshowTimer.stop();
}


void
PSMaster::leaveSlideshowMode()
{
    m_uiManager->modeChanged(MODE_NORMAL);
	m_mode = MODE_NORMAL;
	
	m_slideshowTimer.stop();	
	goToImageByCSAndProjectorIndex(m_curCoordSystem, m_curProjector, true);
}



void
PSMaster::enterArcballMode()
{	
    m_scenegraphManager->hideQuads(m_curCoordSystem, 2.0f);
	m_scenegraphManager->showPointclouds(m_curCoordSystem, 1.0f);
    m_scenegraphManager->showCameraCones(m_curCoordSystem, 2.0f);
        
	m_showImages = false;
	m_showPointcloud = true;
	m_showCameraCones = true;
	
    m_uiManager->modeChanged(MODE_ARCBALL);
    m_cameraMan->enterArcballMode(10.0f, 3.0f);        	
	m_mode = MODE_ARCBALL;    
}



void
PSMaster::leaveArcballMode(int projectorToGoTo)
{    
    m_scenegraphManager->showQuads(m_curCoordSystem, 2.0f);
    m_scenegraphManager->hideCameraCones(m_curCoordSystem, 2.0f);
    m_scenegraphManager->showPointclouds(m_curCoordSystem, 2.0f);
	
	m_showImages = true;
	m_showPointcloud = true;
	m_showCameraCones = false;
	
    m_uiManager->modeChanged(MODE_NORMAL);
    m_cameraMan->leaveArcballMode();
	m_mode = MODE_NORMAL;	
        
	goToImageByCSAndProjectorIndex(m_curCoordSystem, projectorToGoTo);
    
}



void
PSMaster::toggleArcballMode()
{
	if(m_mode == MODE_ARCBALL)
		leaveArcballMode(m_curProjector);
	else
		enterArcballMode();
}



void
PSMaster::updatePanorama()
{
	if(m_mode != MODE_NORMAL || m_panorama < 0)
		return;
	
	
	const JsonCollection *coll = m_dataManager->getCollection();
	const JsonCoordSystem &cs = coll->getCoordSystem(m_curCoordSystem);
	const Panorama& curPano = cs.getPanorama(m_panorama);
	
	
	Vector3 targetAxis;	// either the current look vector of the camera or the one it is rotating to
	float   targetAngle;		// not used
	
	//m_camera->getSpring()->getEndRotation().toAxisAngle(targetAxis, targetAngle);
	const Matrix vm = m_camera->getSpring()->getEndRotation().toMatrix();
	vm.getColumnVector(targetAxis, 2);
	targetAxis.normalize();	
	
	// if we haven't rotated far enough from the current photo, don't switch images
	const Vector3& imgAxis = cs.getProjector(m_curProjector).getLookVector();
	if(imgAxis.dot(targetAxis) > 0.95f)
		return;		
	
		  Panorama::const_iterator iter;	
	const Panorama::const_iterator beg = curPano.begin();
	const Panorama::const_iterator end = curPano.end();
	
	float bestAngle = -FLT_MAX;	
	int idx, best = -1;			
	
	for(iter = beg, idx = 0; iter != end; iter++, idx++){
		const JsonProjector &curProj = cs.getProjector(*iter);
		Point3 quadCenter;
		curProj.getQuadCenter(quadCenter);
		
		// make sure the target quad is somewhat perpendicular to the current look direction
		const Vector3& normal = curProj.getLookVector();
		if(fabs(targetAxis.dot(normal)) < 0.8f)
			continue;
		
		// measure the angle between the new and current look directions. Make sure it is not too small		
		const Vector3 dir = (quadCenter - m_camera->getPosition()).xyz().normalized();
		const float angle = min(1.0f, dir.dot(targetAxis));				
		if(angle > bestAngle && angle > 0.5f){
			bestAngle = angle;
			best = *iter;
		}		
	}
	
	if(best != -1 && best != m_curProjector){
		goToImageByCSAndProjectorIndex(m_curCoordSystem, best, false, 0.75f);
		m_centeredOnImage = false;
	}
	
	
	// The approach below doesn't work because the panorama array is not ordered properly. There must be
	// some code in SynthNavToolkit that does something to it, because it seems to be ordered by increasing
	// local projector index, which doesn't seem like it could be correlated with panorama order.
	
	// check to see if either panorama neighbor is a better fit with the target look vector
		
	/*
	const JsonProjector& p = cs.getProjector(m_curProjector);		
	const JsonProjector* prev = p.getPrevPanoNeighbor();
	const JsonProjector* next = p.getNextPanoNeighbor();
	
	Vector3 pAxis    = p.getLookVector().normalized();		
	float   cosTheta = pAxis.dot(targetAxis);
	
	//cerr << "targetAxis = " << targetAxis << endl;
	//cerr << "cosTheta = " << cosTheta << endl;
	
	
	if(prev){		
		Vector3 prevAxis = prev->getLookVector().normalized();
		//cerr << "prevAxis = " << prevAxis << endl;
		//cerr << "cosTheta = " << prevAxis.dot(targetAxis) << endl;
		if(prevAxis.dot(targetAxis) > cosTheta){
			showPanoramaImageByProjector(prev);
			return;	// no point in checking the other neighbor if we've already decided to update
		}
	}
	
	if(next){
		Vector3 nextAxis = next->getLookVector().normalized();
		//cerr << "nextAxis = " << nextAxis << endl;
		//cerr << "cosTheta = " << nextAxis.dot(targetAxis) << endl;
		if(nextAxis.dot(targetAxis) > cosTheta)
			showPanoramaImageByProjector(next);
    }
	 */
}

		   
void
PSMaster::showPanoramaImageByProjector(const JsonProjector* p)
{

	//cerr << "p index = " << p->getIndexInPanorama() << endl;
	
	m_scenegraphManager->fadeOutAllQuads(0.5f);
		
	const int idx = p->getLocalIndex();
	Quad* q = m_scenegraphManager->getQuad(m_curCoordSystem, idx);
	q->fadeIn(1.0f, 0.5f);
	
	const JsonProjector* pp;
	if(pp = p->getPrevPanoNeighbor()){
		q = m_scenegraphManager->getQuad(m_curCoordSystem, pp->getLocalIndex());
		q->fadeIn(0.5f, 0.5f);
	}
	if(pp = p->getNextPanoNeighbor()){
		q = m_scenegraphManager->getQuad(m_curCoordSystem, pp->getLocalIndex());
		q->fadeIn(0.5f, 0.5f);
	}
	
	m_curProjector = p->getLocalIndex();
	m_scenegraphManager->invalidateScenegraph();
}
	

bool
PSMaster::selectProjector(int touchX, int touchY)
{
	if(m_mode != MODE_ARCBALL || !m_showCameraCones) 
		return false;
	
	APP_PRINTF("PSMaster::selectProjector(%d %d)\n", touchX, touchY);
    
    // TODO: get this working again with scenegraph
	int newProj = m_scenegraphManager->pickProjector(m_curCoordSystem, touchX, touchY);
	if(newProj >= 0){		
		leaveArcballMode(newProj);
		return true;
	}
	
	return false;
}


bool
PSMaster::selectQRect(int touchX, int touchY, bool goToImage)
{
	static int lastSelected;	
	
	// a little hacky, but this is used as a way to clear the current QRect selection.
	// PSInputHandler calls selectQRect(-1, -1) to deselect anything that may currently be selected
	if(m_mode != MODE_NORMAL || touchX < 0 || touchY < 0){
		m_scenegraphManager->fadeOutAllQRects(1.0f, false);
		lastSelected = -1;
		return false;
	}		

    
    // TODO: get this working again with scenegraph
	
	if(goToImage){
		if(lastSelected != -1){
			QRect* qr = m_scenegraphManager->getQRect(m_curCoordSystem, lastSelected);
			qr->hide(PSSettings::Settings().getTransitionSpeed());
			goToImageByCSAndProjectorIndex(m_curCoordSystem, lastSelected);
		}
		return true;
	}
	else{
		int newQRect = m_scenegraphManager->pickQRect(m_curCoordSystem, touchX, touchY);
		lastSelected = newQRect;
		if(newQRect >= 0){			
			QRect* qr = m_scenegraphManager->getQRect(m_curCoordSystem, newQRect);
			qr->show();
			m_scenegraphManager->invalidateScenegraph();
			return true;
		}		
		return false;
	}
}

/*
bool
PSMaster::highlightProjector(int touchX, int touchY)
{        
    if(m_mode != MODE_ARCBALL || !m_showCameraCones) 
		return false;
    
    // TODO: get this working again
    int newProj = //m_scene->selectProjector(m_curCoordSystem, touchX, touchY);
	newProj = rand() % 30;
	if(newProj >= 0){		
        		
        Quad* q = m_scenegraphManager->getQuad(m_curCoordSystem, newProj);
        OpacitySpring* spr = q->getSpring();
        
        spr->setToOpacity(0.25, 1.0);
		
		//cerr << "newProj is " << newProj << endl;
        
		return true;
	}
	
	return false;     
}


bool
PSMaster::selectPoint(int touchX, int touchY)
{
	if(m_mode != MODE_ARCBALL) 
		return false;
    Vector3 bestPt;
    
    // TODO: get this working again
  
	if(m_scene->selectPoint(m_curCoordSystem, touchX, touchY, bestPt)){
        m_cameraSpring.setEndArcballCenter(bestPt);
        m_cameraSpring.addFlags(SPRING_ARCBALL_CENTER | SPRING_ARCBALL_ROTATION);
        m_cameraSpring.start(1.0f, GTime());
        return true;
    } 
  
    return false;
}

*/


void
PSMaster::rotateToOrientation(ScreenOrientation newOrientation)
{
	const ScreenOrientation oldOrientation = PSGlobalState::getScreenOrientation();
	
	if(oldOrientation == newOrientation)
		return;
	
	// relay the change to the global state tracker
    PSGlobalState::setScreenOrientation(newOrientation);
    
    // notify the camera man
    m_cameraMan->setScreenOrientation(newOrientation);
    
    // notify the drawer and UI manager
    Drawer* drawer = m_topLayer->getDrawer();    
	
	
	drawer->setScreenRotation(newOrientation * PIOVER2);
	m_uiManager->screenRotated(newOrientation * PIOVER2);

    // notify the TopLayer that the orientation was changed so it can relay that information to the underlying UIView
    m_topLayer->screenRotated(newOrientation); 
}


void
PSMaster::toggleScreenOrientation()
{
    const ScreenOrientation oldOrientation = PSGlobalState::getScreenOrientation();        
    const ScreenOrientation newOrientation = (ScreenOrientation) (4 + (int) oldOrientation - 1) % 4;
    rotateToOrientation(newOrientation);
       
}

// 
const JsonProjector&
PSMaster::currentProjector() const
{
    const  JsonCollection* coll = m_dataManager->getCollection();
    const  JsonCoordSystem& cs  = coll->getCoordSystem(m_curCoordSystem);
    return cs.getProjector(m_curProjector);
}


// helper. Returns the neighbors of the projector specified by the input arguments as a bitmask
unsigned char
PSMaster::getNeighbors(const JsonProjector& projector) const
{
    unsigned char ret = 0;
    ret |= ((unsigned char) projector.hasLeftNeighbor() << 0);
    ret |= ((unsigned char) projector.hasRightNeighbor() << 1);
    ret |= ((unsigned char) projector.hasUpNeighbor() << 2);
    ret |= ((unsigned char) projector.hasDownNeighbor() << 3);
    ret |= ((unsigned char) projector.hasInNeighbor() << 4);
    ret |= ((unsigned char) projector.hasOutNeighbor() << 5);
    return ret;
}


void
PSMaster::update()
{	

	if(!m_active) return;	

	while(!m_eventQueue.empty()){
		respondToEvent();
		m_lastEvent.update();
        if(!m_uiActive){
            m_uiManager->fadeInControls(getNeighbors(currentProjector()));
            m_uiActive = true;
        }
	}
   
	
	if(m_mode == MODE_SLIDESHOW_PLAYING && 
	    m_now - m_lastSlideshowUpdate > PSSettings::Settings().getSlideshowSpeed()){
		m_lastSlideshowUpdate.update();
		updateSlideshow();
	}
			
	if(m_uiActive && m_now - m_lastEvent > 5.0f){
		m_uiManager->fadeOutControls();
        m_uiActive = false;
	}
    
}

	

	
void
PSMaster::queueEvent(UserEvent e, EventParam p)
{
	m_eventQueue.push(e);
	m_eventParamQueue.push(p);
    respondToEvent();
}


void
PSMaster::respondToEvent()
{	
    m_lastEvent.update();
    
    if(m_eventQueue.empty())
        return;
    
    UserEvent e = m_eventQueue.front();
    m_eventQueue.pop();
    
	ASSERT( !m_eventParamQueue.empty() );
	EventParam p = m_eventParamQueue.front();
	m_eventParamQueue.pop();
	
    
	if(m_mode == MODE_SLIDESHOW_PLAYING || m_mode == MODE_SLIDESHOW_PAUSED){
		if(e == EVENT_PLAY_SLIDESHOW)
			enterSlideshowMode();
		else if (e == EVENT_PAUSE_SLIDESHOW)
			pauseSlideshowMode();
		else if (e == EVENT_LEAVE_SLIDESHOW){
			leaveSlideshowMode();            
        }
	}
    
	
    // TODO: something less shitty than this
    static const float rotSpeed  = 0.01f;
    static const float moveSpeed = 0.10f;
	static const float zoomSpeed = 0.1f;		

	
	// maybe this isn't the right place to do this, but we reinterpret
	// rotates as moves if we are in pan/zoom mode

    float spd;
		
    switch(e)
    {		
        case EVENT_ROTATE_LEFT:
			if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();				
            else
                spd = rotSpeed;
            
            if(m_mode == MODE_ARCBALL)
                m_camera->rotateLeftArcball(spd);
            else
                m_camera->rotateLeft(min(spd / m_camera->getFocalLength(), 0.1f));
			updatePanorama();
            break;			
        case EVENT_ROTATE_RIGHT:
            if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();				
            else
                spd = rotSpeed;
            
            if(m_mode == MODE_ARCBALL)
                m_camera->rotateRightArcball(spd);
            else
                m_camera->rotateRight(min(spd / m_camera->getFocalLength(), 0.1f));
			updatePanorama();
            break;
        case EVENT_ROTATE_UP:
            if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();				
			else
                spd = rotSpeed;
                        
            if(m_mode == MODE_ARCBALL)
                m_camera->rotateUpArcball(spd);
            else
                m_camera->rotateUp(min(spd / m_camera->getFocalLength(), 0.1f));
			updatePanorama();
            break;
        case EVENT_ROTATE_DOWN:
            if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();				
			else
                spd = rotSpeed;
            
            if(m_mode == MODE_ARCBALL)
                m_camera->rotateDownArcball(spd);
            else
                m_camera->rotateDown(min(spd / m_camera->getFocalLength(), 0.1f));
			updatePanorama();
            break;
        case EVENT_MOVE_LEFT:			
            if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();
			else
                spd = moveSpeed;
            
            if(m_mode == MODE_ARCBALL)
                m_camera->moveLeftArcball(spd);
            break;			
        case EVENT_MOVE_RIGHT:
            if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();
			else
                spd = moveSpeed;
            
            if(m_mode == MODE_ARCBALL)
                m_camera->moveRightArcball(spd);
            break;			
        case EVENT_MOVE_UP:
            if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();
			else
                spd = moveSpeed;
            
            if(m_mode == MODE_ARCBALL)
                m_camera->moveUpArcball(spd);
            break;
        case EVENT_MOVE_DOWN:
			if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();
			else
                spd = moveSpeed;
            
            if(m_mode == MODE_ARCBALL)
                m_camera->moveDownArcball(spd);
            break;
        case EVENT_MOVE_FORWARD:
            if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();
			else
                spd = moveSpeed;
            
            if(m_mode == MODE_ARCBALL)
                m_camera->moveForwardArcball(spd);
            else
                m_camera->moveForward(spd);
            break;
        case EVENT_MOVE_BACKWARD:
            if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();
			else
                spd = moveSpeed;
            
            if(m_mode == MODE_ARCBALL)
                m_camera->moveBackwardArcball(spd);
            else
                m_camera->moveBackward(spd);
            break;
        case EVENT_ZOOM_IN:
            if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();
			else
                spd = zoomSpeed;
            
            // if we're in arcball mode, we move forward instead of zooming in
            if(m_mode == MODE_ARCBALL)
                m_camera->moveForwardArcball(spd);
            else
                m_camera->zoomIn(spd);
            break;			
        case EVENT_ZOOM_OUT:
            if(p.getType() == EPTYPE_FLOAT)
                spd = p.getFloat1();
			else
                spd = zoomSpeed;
            
            // if we're in arcball mode, we move backward instead of zooming out
            if(m_mode == MODE_ARCBALL)
                m_camera->moveBackwardArcball(spd);
            else
                m_camera->zoomOut(spd);
            break;			
        case EVENT_GOTO_LEFTNEIGHBOR:
            goToNeighbor(0);
            m_renderThread->resume();            
            break;
        case EVENT_GOTO_RIGHTNEIGHBOR:
            goToNeighbor(1);
            m_renderThread->resume();            
            break;
        case EVENT_GOTO_UPNEIGHBOR:
            goToNeighbor(2);
            m_renderThread->resume();            
            break;
        case EVENT_GOTO_DOWNNEIGHBOR:
            goToNeighbor(3);
            m_renderThread->resume();            
            break;
        case EVENT_GOTO_INNEIGHBOR:
            goToNeighbor(4);
            m_renderThread->resume();     
            break;
        case EVENT_GOTO_OUTNEIGHBOR:            
            goToNeighbor(5);
            m_renderThread->resume();
            break;
        case EVENT_GOTO_RANDOMNEIGHBOR:
            ASSERT("Event \"EVENT_GOTO_RANDOMNEIGHBOR\" not currently supported" && 0);
            break;                        
        case EVENT_GOTO_RANDOMIMAGE:
            ASSERT("Event \"EVENT_GOTO_RANDOMIMAGE\" not currently supported" && 0);
            break;
		case EVENT_GOTO_PREVIMAGE:
			goToNeighbor(6);
			break;			
		case EVENT_GOTO_NEXTIMAGE:
			goToNeighbor(7);
			break;
		case EVENT_NEXT_3DGROUP:
			goToNextCoordSystem();
			break;						
		case EVENT_SETTINGS:
			m_topLayer->showOptionsMenu();
			break;
		case EVENT_HELP:
			m_topLayer->showHelpScreen();
			break;
		case EVENT_END_VIEWING_SYNTH:
			doCleanup();
			m_topLayer->endViewingSynth();
			break;  
		case EVENT_ENTER_SLIDESHOW:
			enterSlideshowMode();
			break;		
        case EVENT_TOGGLE_POINTCLOUD:
			/* old tri-state behavior, now just turn quads on/off
			if(m_showImages && m_showPointcloud){
				m_scenegraphManager->hidePointcloud();
				m_showPointcloud = false;
			}
			else if(m_showImages){
				m_scenegraphManager->hideQuads();
				m_scenegraphManager->showPointcloud();
				m_showImages = false;
				m_showPointcloud = true;
			}
			else{
				m_scenegraphManager->showQuads();
				m_scenegraphManager->showPointcloud();
				m_showImages = true;
				m_showPointcloud = true;
			}
			 */
			m_scenegraphManager->toggleQuads();
            break;
        case EVENT_TOGGLE_IMAGES:
			m_showImages = !m_showImages;
			if(m_showImages)
				m_scenegraphManager->showQuads();
			else
				m_scenegraphManager->hideQuads();
            break;
        case EVENT_TOGGLE_CAMCONES:
			m_showCameraCones = !m_showCameraCones;
			if(m_showCameraCones)
				m_scenegraphManager->showCameraCones();
			else
				m_scenegraphManager->hideCameraCones();
            break;
        case EVENT_ROTATE_SCREEN:
            toggleScreenOrientation();
            break;
		case EVENT_LEAVE_ARCBALL:
			leaveArcballMode(m_curProjector);
			break;
    }
}
    
    
