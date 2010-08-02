/*
 *  Quad.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/18/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "Quad.h"
#include <Drawer.h>
#include <VertexBuffer.h>
#include <OpacitySpring.h>
#include <TextureManager.h>
#include <GregJsonProjector.h>
#include <GenericCamera.h>
#include <Functions.h>
#include <PSGlobalState.h>
#include <GLDrawer.h> // temporary for debugging
#import  <OpenGLES/EAGLDrawable.h>
#import  <OpenGLES/EAGL.h>
#import  <OpenGLES/ES1/gl.h>
#import  <OpenGLES/ES1/glext.h>


//       B    G    R    A
const unsigned char WHITE[4]   = {255, 255, 255, 255};
const unsigned char RED[4]     = {  0,   0, 255, 255};
const unsigned char BLUE[4]    = {255,   0,   0, 255};
const unsigned char GREEN[4]   = {  0, 255,   0, 255};
const unsigned char YELLOW[4]  = {  0, 255,   0, 255};


Quad::Quad(GenericCamera* camera) :
    Drawable(),
    m_camera(camera),
    m_textureAnimator(NULL),
    m_opacitySpring(NULL),
	m_drawFnPtr(&Quad::draw2D)
{    	
    m_color.a = 0.0f;
	setActive(false);
}


Quad::~Quad()
{
	// vertices and indices are allocated by the quad itself and not deep-copied in the calls to setVertices and
	// setIndices, so it is the Quad's responsibility for deleting them
    SAFE_DELETE(m_vertices);
	SAFE_DELETE(m_indices);
}


Quad::Quad(const Quad &q) :
    Drawable(q),
    m_opacitySpring(q.m_opacitySpring),
    m_textureAnimator(q.m_textureAnimator)
{
    m_imageIndex  = q.m_imageIndex;
}


void
Quad::initializeWithProjector(JsonProjector* projector)
{
    // set the corners and center of the quad according the the projector's projection
    projector->getQuadCorners(m_corners[0], m_corners[1], m_corners[2], m_corners[3]); 
    projector->getQuadCenter(m_center);    
    m_projector = projector;
    
    // image
    m_imageIndex = projector->getImageIndex();
    
    // springs
    setOpacitySpring(new OpacitySpring());
    setTextureAnimator(new TextureAnimator(false)); 
    
    // finally, tesselate self via subdivision
    computeVertices(2);
}


void
Quad::setTextureAnimator(TextureAnimator* tAnim)
{
    m_textureAnimator = tAnim;
    addAnimator(tAnim);
    tAnim->setTarget(this);
}


void
Quad::setOpacitySpring(OpacitySpring* spr)
{
    m_opacitySpring = spr;
    addAnimator(spr);
    spr->setTarget(this);
}


OpacitySpring*
Quad::getSpring()
{
    return m_opacitySpring;
}


const OpacitySpring*
Quad::getSpring() const
{
    return m_opacitySpring;
}


const TextureAnimator*
Quad::getTextureAnimator() const 
{
    return m_textureAnimator;
}


TextureAnimator*
Quad::getTextureAnimator()
{
    return m_textureAnimator;
}


void
Quad::fadeIn(float toOpacity, float duration)
{
	setActive(true);	// activate the quad since we want to see it	
	m_opacitySpring->setToOpacity(toOpacity, duration);
	m_opacitySpring->setEndFunctor(Functor::Identity());
}


void
Quad::fadeOut(float duration)
{
	if(FUZZY_EQ0(m_color.a) && FUZZY_EQ0(m_opacitySpring->getToOpacity()))
		return;
	m_opacitySpring->setToOpacity(0.0f, duration);	
	// once the quad is totally invisible (when the spring is done), go ahead and deactivate it
	// to save the cpu and gpu some work
	m_opacitySpring->setEndFunctor(MemberFunctorA<Quad, bool>(this, &Quad::setActive, false));
}	



void
Quad::setWorldCoords(const Vector4 &corner1,
                     const Vector4 &corner2,
                     const Vector4 &corner3,
                     const Vector4 &corner4,
                     const Vector4 &center)
{
    m_corners[0] = corner1;
    m_corners[1] = corner2;
    m_corners[2] = corner3;
    m_corners[3] = corner4;	
	m_center     = center;	            
}



const Vector4&
Quad::getPoint(int iPoint) const
{
    ASSERT(iPoint >= 0 && iPoint <= 4);
    return (const Vector4&) m_corners[iPoint];
}


void
Quad::set3DRendering(bool b)
{
    if(b)
        m_drawFnPtr = &Quad::draw3D;
    else
        m_drawFnPtr = &Quad::draw2D;
}


void
Quad::draw(Drawer& d)
{   	
    if(m_color.a <= 0.01f)
        return;    
 	
	d.setColor(m_color);
	(this->*m_drawFnPtr)(d);
}


void
Quad::draw3D(Drawer& d)
{
    d.setVertexSource(m_vertices);   
    d.draw(m_indices->getNumIndices(), m_indices->getIndices(), FMT_USHORT);        
}


void
Quad::draw2D(Drawer& d)
{         
    static Vector4 pos[100];  // static since there is only one render thread
    
    const int nVertices = m_vertices->getNumVertices();
    const int nIndices  = m_indices ? m_indices->getNumIndices() : nVertices;    
    
	const Matrix& viewMat = m_camera->getViewMatrix();
	const Matrix& projMat = m_camera->getProjectionMatrix();
	
    // this is a little tricky, we are going to manually project the 3-dimensional quad into clip space
    FOR(i,0,nVertices){
        const Vector4 worldPt(m_vertices->getPosition(i));
		
        // project the current vertex using the view and projection matrices
        pos[i] = projectAndHomogenize4(worldPt, viewMat, projMat);
		
        // inspect w to see if this point came from behind the camera. If so, throw it out
        if(pos[i].w >= 0.0f)
            pos[i].z = FLT_MAX;
		
    }	
    
    d.setPositionSource(pos, FMT_FLOAT3, 4 * sizeof(float));
	if(m_textureAnimator->effectActive()){
		glClientActiveTexture(GL_TEXTURE1);
		d.setTexCoordSource(m_vertices->getTexCoords(0), FMT_FLOAT2, 6 * sizeof(float));	
	}
	glClientActiveTexture(GL_TEXTURE0);
	d.setTexCoordSource(m_vertices->getTexCoords(0), FMT_FLOAT2, 6 * sizeof(float));
    d.draw(nIndices, m_indices ? m_indices->getIndices() : NULL, FMT_USHORT);        
}


void
Quad::computeVertices(int numSubdivides)
{
    
    Vertex::XYZWUV* vertices;
    int numVertices;
    computeSubdivisionVertices(*this, numSubdivides, &vertices, numVertices);
    
    VertexArray naiveArray(Vertex::XYZWUV::getDesc(), (float*) vertices, numVertices, false);        
    IndexArray*  indexArray;
    VertexArray* cleverArray;
    
    makeIndexed(&naiveArray, &cleverArray, &indexArray);    	
	
    setVertexArray(cleverArray, false);
    setIndexArray(indexArray, false);
	
	SAFE_DELETE_ARRAY(vertices);	// vertices are deep copied by makeIndexed

}



