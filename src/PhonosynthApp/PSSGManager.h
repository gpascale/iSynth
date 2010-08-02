/*
 *  PSSGManager.h
 *  Phonosynth
 *
 *  Created by Greg on 11/24/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _PSSGManager_H_
#define _PSSGManager_H_

#include <SceneGraph.h>
#include <Engine.h>
#include <GroupNodes.h>

class Drawer;

class CameraCone;
class Quad;
class Pointcloud;
class QRect;
class TextureManager;
class GenericCamera;

class PSSGManager
{
public:

    PSSGManager(SceneGraph* sg);
    ~PSSGManager();
    
	// initialization and cleanup
    void init(GenericCamera* camera);
	void cleanup();
    
    // adds the quad 'q' to the scenegraph for the specified coordinate system
    void addQuad(int coordSystem, int index, Quad* q);
    
    // adds a camera cone to the scenegraph for the specified coordinate system
    void addCameraCone(int coordSystem, int index, CameraCone* cc);
    
    // adds a pointcloud to the scenegraph for the specified coordinate system
    void addPointcloud(int coordSystem, Pointcloud* p);
	
	// adds a QRect to the scenegraph for the specified coordinate system
	void addQRect(int coordSystem, int index, QRect* r);

    // retrieves the quad specified.returns NULL if there is no such quad
		  Quad* getQuad(int globalIdx);
	const Quad* getQuad(int globalIdx) const;
          Quad* getQuad(int coordSystem, int index);
    const Quad* getQuad(int coordSystem, int index) const;
    
    // retrieves the quad specified by coordSystem and index, returns NULL if there is no such
    // camera cone in the scenegraph
          CameraCone* getCameraCone(int coordSystem, int index);
    const CameraCone* getCameraCone(int coordSystem, int index) const;
    
    // retrieves the Pointcloud for the specified coordinate system. Returns NULL if there is
    // no such pointcloud in the scenegraph
          Pointcloud* getPointcloud(int coordSystem);
    const Pointcloud* getPointcloud(int coordSystem) const;
	
		  QRect* getQRect(int globalIdx);
	const QRect* getQRect(int globalIdx) const;
		  QRect* getQRect(int coordSystem, int index);
	const QRect* getQRect(int coordSystem, int index) const;
    
    // high-level visibility controls
	

	bool showQuads();
	bool hideQuads();
	bool toggleQuads();
	bool quadsVisible() const { return m_quadsNode->getActive(); }
	
	bool showPointcloud();
	bool hidePointcloud();
	bool togglePointcloud();
	bool pointcloudsVisible() const { return m_pointcloudsNode->getActive(); }
	
	bool showCameraCones();
	bool hideCameraCones();
	bool toggleCameraCones();
	bool cameraConesVisible() const { return m_cameraConesNode->getActive(); }
	
	bool showQRects();
	bool hideQRects();
	bool toggleQRects();
	bool qRectsVisible() const { return m_rectsNode->getActive(); }
	
    void showQuads(int coordSystem, float duration);
    void hideQuads(int coordSystem, float duration);
    
    void showCameraCones(int coordSystem, float duration);
    void hideCameraCones(int coordSystem, float duration);	
        
    void showPointclouds(int coordSystem, float duration);
    void hidePointclouds(int coordSystem, float duration);

    // PSNetwork will call these functions upon receiving data
    void downloadedImage(const pair<int, int>& imageID, ByteArray data);    
    void downloadedPointcloudBlob(const pair<int, int>& blobID, ByteArray data);
	void downloadedSeadragonTile(const SeadragonTile& tile, ByteArray data);
    
    void setTextureManager(TextureManager* tm) { m_textureManager = tm; }        
    
    
    // convenience functions for PSMaster
    void fadeOutAllQuads(float duration);
	void fadeOutAllQRects(float duration, bool turnOff = false);
    
    
    void coordinateSystemChanged(int oldCoordSystem, int newCoordSystem);
    
    // returns all quads for whom image index is imgIndex    
    void getQuadsForImageIndex(int imgIndex, OUT vector<Quad*>& ret);
    
    // returns a quad's texture animator
    TextureAnimator* getTextureAnimatorForQuad(int coordSystem, int index);    
    
    // returns a quad's opacity spring
    OpacitySpring*   getOpacitySpringForQuad(int coordSystem, int index);
 
 
    // returns all texture animators for quads whose image index is imgIndex
    int getTextureAnimatorsForImageIndex(int imgIndex, OUT vector<TextureAnimator*>& ret);
 
    void setQuadArcballRendering(bool b);
    
    void render(Drawer& drawer);
	
	void renderFullPointcloud(Drawer& drawer);
	
	void invalidateScenegraph() { m_scenegraph->invalidate(); }
    
	
	int pickProjector(int coordSystem, int clickX, int clickY);
	int pickQRect(int coordSystem, int clickX, int clickY);
    
private:

    // pointer to the application's scenegraph
    SceneGraph*      m_scenegraph;    
   
    // texture management
    TextureManager*  m_textureManager;
    
    GroupNode*       m_rootNode;
    QuadGroup*       m_quadsNode;
    PointcloudGroup* m_pointcloudsNode;
    CameraConeGroup* m_cameraConesNode;
	QRectGroup*		 m_rectsNode;
    
	GenericCamera*	 m_camera;
    
	// Originally, the scenegraph itself was organized in such a
	// way as to reflect the structure of the synth, but this was getting in the way of efficient
	// rendering. Now the scenegraph structure is very simple and PSSGManager is responsible 
	// for keeping track of and providing access to specific primitives. There is a lot of redundant storage
	// happening here but the memory cost is quite low (a few KB at most) when weighed against the increase 
	// in efficiency and convenience gained by having primitives indexed in all these different ways
	// The underlying idea here is that the scenegraph structure should be fairly generic while the
	// PS scenegraph manager is tailored specifically towards Photosynth.
    vector<Quad*>            m_allQuads;
	vector<vector<Quad*> >   m_quadsByCSAndLocalIndex;
    vector<vector<Quad*> >   m_quadsByImageIndex;
	
	vector<Pointcloud*>			 m_allPointclouds;
	vector<vector<Pointcloud*> > m_pointcloudsByCSAndBlobIndex;
	
	vector<CameraCone*>			 m_allCameraCones;
	vector<vector<CameraCone*> > m_cameraConesByCSAndLocalIndex;
	
	vector<QRect*>			m_allQRects;
	vector<vector<QRect*> > m_qRectsByCSAndLocalIndex;
	vector<QRect*>			m_visibleQRects;
	
};




#endif // _PSSGManager_H_
