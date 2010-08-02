/*
 *  PSSGManager.cpp
 *  Phonosynth
 *
 *  Created by Greg on 11/24/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "PSSGManager.h"
#include <Quad.h>
#include <CameraCone.h>
#include <Pointcloud.h>
#include <QRect.h>
#include <OpacitySpring.h>
#include <Animator.h>
#include <GTimer.h>
#include <Functions.h>
#include <PSGlobalState.h>
#include <GTimer.h>
#include <GregJsonProjector.h>


PSSGManager::PSSGManager(SceneGraph* sg) :
	m_scenegraph(sg),
	m_camera(NULL),
	m_rootNode(NULL),
	m_quadsNode(NULL),
	m_pointcloudsNode(NULL),
	m_cameraConesNode(NULL),
	m_rectsNode(NULL)
{

}


PSSGManager::~PSSGManager()
{
    
}




void
PSSGManager::init(GenericCamera* camera)
{		
	m_camera = camera;
	
	m_rootNode			= new GroupNode();
	m_pointcloudsNode   = new PointcloudGroup(NULL, m_camera);
	m_cameraConesNode	= new CameraConeGroup(NULL, m_camera);
	m_quadsNode			= new QuadGroup(NULL, m_camera);
	m_rectsNode			= new QRectGroup(NULL, m_camera);

	ASSERT(m_scenegraph);
	
	m_scenegraph->setRoot(m_rootNode);	
    m_rootNode->addChild(m_pointcloudsNode);
    m_rootNode->addChild(m_cameraConesNode);
    m_rootNode->addChild(m_quadsNode);
	m_rootNode->addChild(m_rectsNode);
	
	// defaults, probably redundant but whatevs
	m_quadsNode->setActive(true);
	m_rectsNode->setActive(true);	
	m_pointcloudsNode->setActive(false); // TODO: change pointcloud default visibility to true
	m_cameraConesNode->setActive(false);
		
}


void
PSSGManager::cleanup()
{
	m_scenegraph->cleanup();	 // clean up the scenegraph
	
	// the actual nodes will be deleted by the scenegraph, but nullify the reference
	m_rootNode		  = NULL;
	m_quadsNode		  = NULL;
	m_pointcloudsNode = NULL;
	m_rectsNode		  = NULL;
	m_cameraConesNode = NULL;
	
	m_allQuads.clear();
	m_quadsByCSAndLocalIndex.clear();
	m_quadsByImageIndex.clear();	
	
	m_allPointclouds.clear();
	m_pointcloudsByCSAndBlobIndex.clear();
	
	m_allQRects.clear();
	m_qRectsByCSAndLocalIndex.clear();
	m_visibleQRects.clear();

}


Quad*
PSSGManager::getQuad(int globalIdx)
{
	ASSERT(globalIdx < m_quadsByCSAndLocalIndex.size());
	return m_allQuads[globalIdx];
}


const Quad*
PSSGManager::getQuad(int globalIdx) const
{
	ASSERT(globalIdx < m_quadsByCSAndLocalIndex.size());
	return m_allQuads[globalIdx];	
}


const Quad*
PSSGManager::getQuad(int coordSystem, int index) const
{
	ASSERT(m_quadsNode);	
	ASSERT(coordSystem < m_quadsByCSAndLocalIndex.size());
	ASSERT(index < m_quadsByCSAndLocalIndex[coordSystem].size());
	
	return m_quadsByCSAndLocalIndex[coordSystem][index];
}


Quad*
PSSGManager::getQuad(int coordSystem, int index)
{
    ASSERT(m_quadsNode);
	ASSERT(coordSystem < m_quadsByCSAndLocalIndex.size());
	ASSERT(index < m_quadsByCSAndLocalIndex[coordSystem].size());
	
	return m_quadsByCSAndLocalIndex[coordSystem][index];
}


void
PSSGManager::addQuad(int coordSystem, int localIndex, Quad* q)
{
	
	ASSERT(m_quadsNode);
	ASSERT(q);
	
	m_quadsNode->addChild(q);
	q->setParent(m_quadsNode);
	q->setOpacity(0.0f);
	q->setActive(false);
	 
	 
	// update quads indexed by image index
    const int imageIdx = q->getImageIndex();    
    if(imageIdx >= m_quadsByImageIndex.size())
        m_quadsByImageIndex.resize(imageIdx + 1);    
    m_quadsByImageIndex[imageIdx].push_back(q);
		
	// update quads indexed by CS and local index
	if(m_quadsByCSAndLocalIndex.size() >= coordSystem)
		m_quadsByCSAndLocalIndex.resize(coordSystem + 1, vector<Quad*>());
	if(m_quadsByCSAndLocalIndex[coordSystem].size() >= localIndex)
		m_quadsByCSAndLocalIndex[coordSystem].resize(localIndex + 1, NULL);
	m_quadsByCSAndLocalIndex[coordSystem][localIndex] = q;
	
	// update all quads vector
	m_allQuads.push_back(q);
}



const CameraCone*
PSSGManager::getCameraCone(int coordSystem, int index) const
{
    ASSERT(m_cameraConesNode);
	ASSERT(coordSystem < (const int) m_cameraConesByCSAndLocalIndex.size());
	ASSERT(index < (const int) m_cameraConesByCSAndLocalIndex[coordSystem].size());
	
	return m_cameraConesByCSAndLocalIndex[coordSystem][index];	
}


CameraCone*
PSSGManager::getCameraCone(int coordSystem, int index)
{
    ASSERT(m_cameraConesNode);
	ASSERT(coordSystem < (const int) m_cameraConesByCSAndLocalIndex.size());
	ASSERT(index < (const int) m_cameraConesByCSAndLocalIndex[coordSystem].size());
	
	return m_cameraConesByCSAndLocalIndex[coordSystem][index];
}


void
PSSGManager::addCameraCone(int coordSystem, int index, CameraCone* cc)
{
	
    ASSERT(m_cameraConesNode);
    GroupNode* parent = m_cameraConesNode->getChild(coordSystem);
    
    if(parent == NULL){
        parent = new GroupNode();
        m_cameraConesNode->addChild(parent);
		parent->setActive(false);
    }    	
	
	parent->addChild(cc);
	cc->setParent(parent);
	cc->setOpacity(1.0f);
	
	if(coordSystem >= (const int) m_cameraConesByCSAndLocalIndex.size())
		m_cameraConesByCSAndLocalIndex.resize(coordSystem + 1, vector<CameraCone*>());
	if(index >= (const int) m_cameraConesByCSAndLocalIndex[coordSystem].size())
		m_cameraConesByCSAndLocalIndex[coordSystem].resize(index + 1, NULL);
	m_cameraConesByCSAndLocalIndex[coordSystem][index] = cc;
	
	m_allCameraCones.push_back(cc);
	
}


const Pointcloud*
PSSGManager::getPointcloud(int coordSystem) const
{
    ASSERT(m_pointcloudsNode);
    ASSERT(coordSystem < (const int) m_allPointclouds.size());
	return m_allPointclouds[coordSystem];
}


Pointcloud*
PSSGManager::getPointcloud(int coordSystem)
{
    ASSERT(m_pointcloudsNode);
    ASSERT(coordSystem < (const int) m_allPointclouds.size());
	return m_allPointclouds[coordSystem];
}


void
PSSGManager::addPointcloud(int coordSystem, Pointcloud* p)
{
    ASSERT(m_pointcloudsNode);
	
	if(coordSystem >= (const int) m_allPointclouds.size())
		m_allPointclouds.resize(coordSystem + 1, NULL);
	m_allPointclouds[coordSystem] = p;

    m_pointcloudsNode->addChild(p);	
    p->setParent(m_pointcloudsNode); 
	p->setOpacity(0.0f);
	p->setActive(false);
}


void
PSSGManager::addQRect(int coordSystem, int index, QRect* r)
{
	ASSERT(m_rectsNode);
	m_rectsNode->addChild(r);
	r->setParent(m_rectsNode);
	r->setOpacity(0.0f);
	r->setActive(true);
			
	if(coordSystem >= m_qRectsByCSAndLocalIndex.size())
		m_qRectsByCSAndLocalIndex.resize(coordSystem + 1, vector<QRect*>(0));
	if(index >= m_qRectsByCSAndLocalIndex[coordSystem].size())
		m_qRectsByCSAndLocalIndex[coordSystem].resize(index + 1, NULL);
	
	// TODO: This is wrong, but it shouldn't matter
	m_allQRects.push_back(r);
	
	m_qRectsByCSAndLocalIndex[coordSystem][index] = r;
}


QRect*
PSSGManager::getQRect(int globalIdx)
{
	ASSERT(globalIdx < m_allQRects.size());
	return m_allQRects[globalIdx];
}


const QRect*
PSSGManager::getQRect(int globalIdx) const
{
	ASSERT(globalIdx < m_allQRects.size());
	return m_allQRects[globalIdx];
}


const QRect*
PSSGManager::getQRect(int coordSystem, int index) const
{
	ASSERT(coordSystem < m_qRectsByCSAndLocalIndex.size());
	ASSERT(index < m_qRectsByCSAndLocalIndex[coordSystem].size());
	
	return m_qRectsByCSAndLocalIndex[coordSystem][index];
}


QRect*
PSSGManager::getQRect(int coordSystem, int index)
{
	ASSERT(coordSystem < m_qRectsByCSAndLocalIndex.size());
	ASSERT(index < m_qRectsByCSAndLocalIndex[coordSystem].size());
	
	return m_qRectsByCSAndLocalIndex[coordSystem][index];
}


int
PSSGManager::getTextureAnimatorsForImageIndex(int imgIndex, OUT vector<TextureAnimator*>& ret)
{
    if(imgIndex >= m_quadsByImageIndex.size())
        return 0;
    
    ret.clear();
    
    const vector<Quad*>& quads = m_quadsByImageIndex[imgIndex];
    const int nQuads = quads.size();
    
    FOR(i,0,nQuads){
        TextureAnimator* tAnim = quads[i]->getTextureAnimator();
        if(tAnim != NULL)
            ret.push_back(tAnim);
    }
    
    return (int) ret.size();
}


bool
PSSGManager::showQuads()
{
	bool ret = m_quadsNode->getActive();
	m_quadsNode->setActive(true);
	m_scenegraph->invalidate();
	return ret;
}


bool
PSSGManager::hideQuads()
{
	bool ret = m_quadsNode->getActive();
	m_quadsNode->setActive(false);
	m_scenegraph->invalidate();
	return ret;
}


bool
PSSGManager::toggleQuads()
{
	if(quadsVisible())
		hideQuads();
	else
		showQuads();
	return !quadsVisible();
}


bool
PSSGManager::showPointcloud()
{
	bool ret = m_pointcloudsNode->getActive();
	m_pointcloudsNode->setActive(true);
	m_scenegraph->invalidate();
	return ret;
}


bool
PSSGManager::hidePointcloud()
{
	bool ret = m_pointcloudsNode->getActive();
	m_pointcloudsNode->setActive(false);
	m_scenegraph->invalidate();
	return ret;
}


bool
PSSGManager::togglePointcloud()
{
	if(pointcloudsVisible())
		hidePointcloud();
	else
		showPointcloud();
	return !pointcloudsVisible();
}
	

bool
PSSGManager::showCameraCones()
{
	bool ret = m_cameraConesNode->getActive();
	m_cameraConesNode->setActive(true);
	m_scenegraph->invalidate();
	return ret;
}


bool
PSSGManager::hideCameraCones()
{
	bool ret = m_cameraConesNode->getActive();
	m_cameraConesNode->setActive(false);
	m_scenegraph->invalidate();
	return ret;
}


bool
PSSGManager::toggleCameraCones()
{
	if(cameraConesVisible())
		hideCameraCones();
	else
		showCameraCones();
	return !cameraConesVisible();
}


bool
PSSGManager::showQRects()
{
	bool ret = m_rectsNode->getActive();
	m_rectsNode->setActive(true);
	m_scenegraph->invalidate();
	return ret;
}


bool
PSSGManager::hideQRects()
{
	bool ret = m_rectsNode->getActive();
	m_rectsNode->setActive(false);
	m_scenegraph->invalidate();
	return ret;
}


bool
PSSGManager::toggleQRects()
{
	if(qRectsVisible())
		hideQRects();
	else
		showQRects();
	return !qRectsVisible();
}


void
PSSGManager::showQuads(int coordSystem, float duration)
{
	m_quadsNode->setActive(true);
	
	/* TODO: take out
	GroupNode* cq = dynamic_cast<GroupNode*>(m_quadsNode->getChild(coordSystem));
	ASSERT(cq);
	cq->setActive(true);
	 */
	
	m_scenegraph->invalidate();
}


void 
PSSGManager::hideQuads(int coordSystem, float duration)
{
	ASSERT(coordSystem < m_quadsByCSAndLocalIndex.size());
	const int nToHide = (const int) m_quadsByCSAndLocalIndex[coordSystem].size();
	FOR(i,0,nToHide)
		m_quadsByCSAndLocalIndex[coordSystem][i]->fadeOut(duration);
	
	m_scenegraph->invalidate();
}


void 
PSSGManager::showCameraCones(int coordSystem, float duration)
{
	GroupNode* ccNode = dynamic_cast<GroupNode*>(m_cameraConesNode->getChild(coordSystem));
	m_cameraConesNode->getSpring()->setToOpacity(1.0f, duration);
	ccNode->setActive(true);
	m_cameraConesNode->setActive(true);
	
	m_scenegraph->invalidate();
}


void 
PSSGManager::hideCameraCones(int coordSystem, float duration)
{
	GroupNode* ccNode = dynamic_cast<GroupNode*>(m_cameraConesNode->getChild(coordSystem));
	m_cameraConesNode->getSpring()->setToOpacity(0.0f, duration);
	MemberFunctorA<SceneGraphNode, bool> functor(ccNode, &SceneGraphNode::setActive, false);
	GTimer::performFunctorAfterDelay(functor, duration);
	
	m_scenegraph->invalidate();
}


void 
PSSGManager::showPointclouds(int coordSystem, float duration)
{
	m_pointcloudsNode->setActive(true);	
	m_scenegraph->invalidate();
}


void 
PSSGManager::hidePointclouds(int coordSystem, float duration)
{
	m_pointcloudsNode->setActive(false);	
	m_scenegraph->invalidate();
}





void
PSSGManager::downloadedImage(const pair<int, int>& imageID, ByteArray data)
{
    m_textureManager->downloadedImage(imageID, data);
}


//TODO: fill this shit in
void
PSSGManager::downloadedPointcloudBlob(const pair<int, int>& blobID, ByteArray data)
{
    Pointcloud* pc = getPointcloud(blobID.first);
    pc->addVerticesFromBinaryData(data, 1.0f);
}


void
PSSGManager::downloadedSeadragonTile(const SeadragonTile& tile, ByteArray data)
{
	m_textureManager->downloadedSeadragonTile(tile, data);
}


void
PSSGManager::coordinateSystemChanged(int oldCoordSystem, int newCoordSystem)
{
	m_scenegraph->invalidate();
}


// start the recursive drawing process at the root
void
PSSGManager::render(Drawer& drawer)
{    
    m_scenegraph->render(drawer);
}


void
PSSGManager::renderFullPointcloud(Drawer& drawer)
{
	float oldDensity = PSGlobalState::getPointcloudDensity();
	PSGlobalState::setPointcloudDensity(1.0f);	
	m_scenegraph->render(drawer, m_pointcloudsNode);		
	PSGlobalState::setPointcloudDensity(oldDensity);
}


void
PSSGManager::setQuadArcballRendering(bool b)
{
    m_quadsNode->setArcballRendering(b);
    const int nQuads = (const int) m_allQuads.size();
    FOR(i,0,nQuads){
        m_allQuads[i]->set3DRendering(b);
    }
}

// convenience stuff used by PSMaster

void 
PSSGManager::fadeOutAllQuads(float duration)
{
    const int nQuads = (const int) m_allQuads.size();
    FOR(i,0,nQuads){
		m_allQuads[i]->fadeOut(duration);
	}
	m_scenegraph->invalidate();
}


// TODO: use the duration once I implement springs for rects
void
PSSGManager::fadeOutAllQRects(float duration, bool turnOff)
{
	const int nRects = (const int) m_allQRects.size();
	FOR(i,0,nRects){
		if(m_allQRects[i]->getActive())
			m_allQRects[i]->hide(duration);
		if(turnOff)
			m_allQRects[i]->turnOff();
	}
	m_scenegraph->invalidate();
}


int
PSSGManager::pickProjector(int coordSystem, int clickX, int clickY)
{
	GroupNode* ccNode = dynamic_cast<GroupNode*>(m_cameraConesNode->getChild(coordSystem));
	const int nChildren = (const int) ccNode->numChildren();
	
	int bestP = -1;
	float best = 50000.0f;
	
	
	Vector2 normalizedClick(-clickX, clickY);
	const int o = PSGlobalState::getScreenOrientation();
	if(o & 1){
		normalizedClick.x = normalizedClick.x / System::getScreenHeight() * 2.0f + 1.0f;
		normalizedClick.y = normalizedClick.y / System::getScreenWidth() * 2.0f - 1.0f;
	}
	else{
		normalizedClick.x = normalizedClick.x / System::getScreenWidth() * 2.0f + 1.0f;
		normalizedClick.y = normalizedClick.y / System::getScreenHeight() * 2.0f - 1.0f;		
	}	
	
	FOR(i,0,nChildren){
		CameraCone* cc = dynamic_cast<CameraCone*>(ccNode->getChild(i));
		float cur = cc->hitTest(normalizedClick, m_camera->getViewMatrix(), m_camera->getProjectionMatrix());
		if(cur < best){
			best = cur;
			bestP = i;		    
		}
	}
		
	return bestP;
}


// TODO: Is there a reasonably efficient way to do this without having PSSGManager know about projectors?
// perhaps quads should be linked to eachother more directly without having to go through the Json stuff
int
PSSGManager::pickQRect(int coordSystem, int clickX, int clickY)
{
	static int lastResult;
	
	int   bestR = -1;
	float bestD = 50000.0f;
	
	Vector2 normalizedClick(-clickX, clickY);
	const int o = PSGlobalState::getScreenOrientation();
	if(o & 1){
		normalizedClick.x = normalizedClick.x / System::getScreenHeight() * 2.0f + 1.0f;
		normalizedClick.y = normalizedClick.y / System::getScreenWidth() * 2.0f - 1.0f;
	}
	else{
		normalizedClick.x = normalizedClick.x / System::getScreenWidth() * 2.0f + 1.0f;
		normalizedClick.y = normalizedClick.y / System::getScreenHeight() * 2.0f - 1.0f;		
	}
	
	const int nQRects = (const int) m_qRectsByCSAndLocalIndex[coordSystem].size();
	FOR(i,0,nQRects){
		const QRect* cur = m_qRectsByCSAndLocalIndex[coordSystem][i];
		if(cur->isOn()){
			float dist = cur->hitTest(normalizedClick, *m_camera);
			if(dist < bestD){
				bestD = dist;
				bestR = i;
			}
		}
	}
	
	//cerr << "bestR = " << bestR << ", bestD = " << bestD << endl;
	
	if(bestR != lastResult){
		fadeOutAllQRects(0.5f, false);
		lastResult = bestR;
		if(bestR != -1)
			m_qRectsByCSAndLocalIndex[coordSystem][bestR]->show();
	}
		
	return bestR;
}


