/*
 *  GroupNodes.h
 *  Phonosynth
 *
 *  Created by Greg on 12/26/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _GROUPNODES_H_
#define _GROUPNODES_H_

#include <Drawer.h>
#include <SceneGraph.h>
#include <PSTypes.h>
#include <GenericCamera.h>
#include <OpacitySpring.h>
#include <Pointcloud.h>
#include <SharedVertices.h>

class PointcloudGroup : public GroupNode, public AdjustableOpacity
{
    
public:
    
    PointcloudGroup(SceneGraphNode* parent = NULL,
                    GenericCamera* camera) :
        GroupNode(parent),
        m_camera(camera),
		m_opacity(0.0f)
    {
        setDrawPriority(PRIORITY_PC_GROUP);
		m_spring = new OpacitySpring(this);
		addAnimator(m_spring);
    }

	~PointcloudGroup()
	{
		// nothing special to do here
	}	
    
	
	void writeToStream(ostream& stream, const string& tab)
	{
		stream << tab << "PointcloudGroup: Pri " << getDrawPriority() << endl;
	}
	
	
	void  setOpacity(float f)       { m_opacity = f;    }
	float getOpacity()        const { return m_opacity; }
	
	
    // set the drawing state for a pointcloud
    void draw(Drawer& d)
    {				
        d.setWorldViewMatrix(Matrix::IdentityCRef(), m_camera->getViewMatrix());
        d.setProjectionMatrix(m_camera->getProjectionMatrix());        
        d.disableBlending();
        d.disableTextureUnit(0);
		d.disableTextureUnit(1);
        d.disableDepthTesting();
        d.setDrawMode(DRAWMODE_POINTS);                
    }
	
	OpacitySpring* getSpring() { return m_spring; }	
    
private:
    
	float		   m_opacity;
	OpacitySpring* m_spring;
    GenericCamera* m_camera;
    
};


class CameraConeGroup : public GroupNode, public AdjustableOpacity
{
    
public:    
    
    CameraConeGroup(SceneGraphNode* parent = NULL, GenericCamera* camera) :
        GroupNode(parent),
        m_camera(camera),
		m_opacity(0.0f)
    {
        setDrawPriority(PRIORITY_CC_GROUP);
		m_spring = new OpacitySpring(this);
		addAnimator(m_spring);
    }
	
	
	~CameraConeGroup()
	{
		// nothing special to do here
	}
    
	
	void writeToStream(ostream& stream, const string& tab)
	{
		stream << tab << "CameraConeGroup: Pri " << getDrawPriority() << endl;
	}
	
	
	void  setOpacity(float f)       { m_opacity = f;    }
	float getOpacity()        const { return m_opacity; }
	
    
    // set the drawing state for a pointcloud
    void draw(Drawer& d)
    {
		/*
		static const SharedVertices* sv = SharedVertices::getCameraConeSharedVertices();
		static const VertexArray* v     = sv->getSharedVertexArray();
		 
		d.setVertexSource(v);
		*/
		 
		d.disableTextureUnit(1);						
        d.setWorldViewMatrix(Matrix::IdentityCRef(), m_camera->getViewMatrix());
        d.setProjectionMatrix(m_camera->getProjectionMatrix());
        d.disableTextureUnit(0);
        d.enableBlending();
        d.setDrawMode(DRAWMODE_LINES);
		d.setLineThickness(1.0f);
		d.setColor(Color4F(1.0f, 1.0f, 1.0f, m_opacity));
    }

	OpacitySpring* getSpring() { return m_spring; }
	
private:
    
	float		   m_opacity;
	OpacitySpring* m_spring;
    GenericCamera* m_camera;
    
};


class QRectGroup : public GroupNode
{
	
public:
	
	QRectGroup(SceneGraphNode* parent = NULL, GenericCamera* camera) :
		GroupNode(parent),
		m_camera(camera)
	{
		setDrawPriority(PRIORITY_QRECT_GROUP);
	}
	
	
	~QRectGroup()
	{

	}
	
	
	void writeToStream(ostream& stream, const string& tab)
	{
		stream << tab << "QRectGroup: Pri " << getDrawPriority() << endl;
	}
	
	
	void draw(Drawer& d)
	{
		d.setWorldViewMatrix(Matrix::IdentityCRef(), m_camera->getViewMatrix());
		d.setProjectionMatrix(m_camera->getProjectionMatrix());
		d.disableTextureUnit(0);
		d.disableTextureUnit(1);
		d.enableBlending();
		d.disableDepthTesting();
		d.setDrawMode(DRAWMODE_LINE_LOOP);
		d.setLineThickness(3.0f);		
	}
	
private:
	
	GenericCamera* m_camera;
	
};

class QuadGroup : public GroupNode
{
    
public:    
    
    QuadGroup(SceneGraphNode* parent = NULL, GenericCamera* camera) :
		GroupNode(parent),
		m_camera(camera),
		m_arcballRendering(false)
	{
        setDrawPriority(PRIORITY_QUAD_GROUP);
        Matrix::ProjectionMatrixOrthoRH(m_orthoProjMat, 1.0f, -1.0f, 1.0f, -1.0f, -1.01f, -0.99f);
    }
            	
	~QuadGroup()
	{
		// nothing special to do here
	}	
	
	
	void writeToStream(ostream& stream, const string& tab)
	{
		stream << tab << "QuadGroup: Pri " << getDrawPriority() << endl;
	}
	
	
    // this is a big hack. In arcball mode, we render quads in 3d so the depth testing works appropriately.
    // I couldn't figure out how to get depth testing to work properly while mixing orthographic projection
    // for the quads and perspective projection for the camera cones. However, since we only really need 
    // to see the camera cones in arcball mode, we only render the quads using perspective projection then.
    // Otherwise we use the normal 2d trick for quad rendering
    void setArcballRendering(bool b) { m_arcballRendering = b; }
    
    // set the drawing state for a pointcloud
    void draw(Drawer& d)
    {
		////cout << "Quad Group: " <<  numChildren() << " children" << endl;
		
		d.disableTextureUnit(1);
		
        if(m_arcballRendering){
            d.setWorldViewMatrix(Matrix::IdentityCRef(), m_camera->getViewMatrix());
            d.setProjectionMatrix(m_camera->getProjectionMatrix());        
            d.enableBlending();
            d.enableTextureUnit(0);
            d.enableDepthTesting();
            d.setDrawMode(DRAWMODE_TRIANGLES);            			
        }
        else{
            d.setWorldViewMatrix(Matrix::IdentityCRef(), Matrix::IdentityCRef());
            d.setProjectionMatrix(m_orthoProjMat);
            d.enableBlending();
            d.enableTextureUnit(0);
            d.disableDepthTesting();
            d.setDrawMode(DRAWMODE_TRIANGLES);
        } 		 
    }

private:
    
    Matrix          m_orthoProjMat;
    GenericCamera*  m_camera;
    bool            m_arcballRendering;
    
};

#endif