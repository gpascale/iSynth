#ifndef _DRAWER_H_
#define _DRAWER_H_


#include <GMath.h>
#include <Quad.h>
#include <GhettoQuad.h>
#include <vector>
#include <EngineTypes.h>
#include "TextureManager.h"
#include <bitset>

/*
 * Drawer is the class that actually interacts with the underlying rendering API and puts pixels on the screen
 * It exists to abstraxt away the details of the particular API being used so that the rest of the code can
 * be legitimately cross-platform. Drawer implements a drawX routine for each renderable object X, which is 
 * the method by which an X is drawn to the screen. Drawer operates somewhat like a state machine. It maintains
 * several pieces of state, most importantly View, Projection and World matrices, which are utilized each
 * time a draw routine is called. The steps to draw a primitive (e.g. a Quad) are as follows
 *
 * 1. Call begin() or begin(c) where c is the clear color to initiate drawing
 * 2. Set the View and Projection matrices according to the position of the camera in the scene
 * 3. Set the World matrix if the Quad is specified in local coordinates
 *    or call setDoWorldTransform(false) if the Quad is specified in world coordinates
 * 4. Call setActiveTexture, enableBlending... etc to configure the rest of the state.
 * 5. Call drawQuad(q), passing in a Quad q to render it.
 * 6. Repeat steps 2-5 for all subsequent objects to be rendered
 * 7. Call end() to make it all show up
 */

class VertexBuffer;
class IndexBuffer;

class Drawer
{
	
public:    

	Drawer();
	virtual ~Drawer() { }

	virtual void cleanup() { }
	
	virtual void destroyBuffers() { };
	virtual void createBuffers()  { };
	virtual void regenBuffers()   { };
	
	// camera manipulation			
	virtual void setAspectRatio(float r)     { m_aspectRatio = r;      }
    virtual void setFocalLength(float f)     { m_focalLength = f;      }    
	virtual void setNearClipPlane(float d)   { m_nearClipPlane = d;    }
	virtual void setFarClipPlane(float d)    { m_farClipPlane = d;     }
    virtual void setWireframe(bool b)        { m_wireframe = b;	       }
    virtual void setDoWorldTransform(bool b) { m_doWorldTransform = b; }
    
// state   
    
    
    virtual void setColor(const Color4F& c) = 0;
    virtual void setDrawMode(DrawMode dm) = 0;
    
	virtual DrawMode getDrawMode() const = 0;
	
    virtual void enableLighting()  = 0;
    virtual void disableLighting() = 0;
    
	virtual void enableBlending()  = 0;
    virtual void disableBlending() = 0;    
	
    virtual void enableDepthTesting()  = 0;
    virtual void disableDepthTesting() = 0;
    
	virtual void setLineThickness(float f) = 0;
	
    virtual void setWorldMatrix(const Matrix& m)									= 0;
    virtual void setViewMatrix(const Matrix& m)										= 0;
    virtual void setWorldViewMatrix(const Matrix& w, const Matrix& v)				= 0;
	virtual void setProjectionMatrix(const Matrix& m)								= 0;
    virtual void setTextureMatrix(const Matrix& textureMatrix, int textureUnit = 0) = 0;
    
    virtual const Matrix& getWorldMatrix()						const = 0;
    virtual const Matrix& getViewMatrix()						const = 0;
	virtual const Matrix& getProjectionMatrix()					const = 0;
    virtual const Matrix& getTextureMatrix(int textureUnit = 0) const = 0;

	
	virtual void setActiveTexture(Texture* texture, int textureUnit = 0)	    = 0;	
	virtual const Texture* getActiveTexture(int textureUnit = 0)		  const = 0;
	
	virtual void enableTextureUnit(int n)					    = 0;		
	virtual void disableTextureUnit(int n)					    = 0;		
	virtual void setMultitextureType(MultitextureType mtType)   = 0;
	virtual void setMultitextureInterpolationParameter(float f) = 0;
    
// vertex information
    
    virtual void setPositionSource(const VertexBuffer* vb, int stride = 0)             = 0;
    virtual void setPositionSource(const void* data, VertexFormat fmt, int stride = 0) = 0;
    
    virtual void setTexCoordSource(const VertexBuffer* vb, int stride = 0)             = 0;
    virtual void setTexCoordSource(const void* data, VertexFormat fmt, int stride = 0) = 0;
    
    virtual void setNormalSource(const VertexBuffer* vb, int stride = 0)               = 0;
    virtual void setNormalSource(const void* data, VertexFormat fmt, int stride = 0)   = 0;
    
    virtual void setColorSource(const VertexBuffer* vb, int stride = 0)                = 0;
    virtual void setColorSource(const void* data, VertexFormat fmt, int stride = 0)    = 0;
    
    virtual void setVertexSource(const VertexArray* vertices)                          = 0;
    
    virtual void draw(int n, const IndexBuffer* ib)                                    = 0;
    virtual void draw(int n, const void* indices, VertexFormat fmt)                    = 0;
    

	// drawing routines
	virtual void beginOnscreen() const = 0;
	virtual void beginOnscreen(const Color4F& clearColor) const = 0;
	
	virtual void beginOffscreen(const Color4F& clearColor) const = 0;
	virtual void renderOffscreenBuffer() = 0;
	
	
	virtual void end() const = 0;			
	
    
	 	
	

	
	virtual void setViewport(int x0, int y0, int w, int h) = 0;
	
	virtual void setScreenRotation(float f) = 0;
    
    
    // error state
    virtual int  getErrorCode()    = 0;
    
	
	// temporary, remove this soon
	const TextureManager* getTextureManager() const { return m_textureManager; }
	
	void  setTextureManager(TextureManager* texManager){ m_textureManager = texManager; }
	        
    
    
protected:			    
	
	
    // camera parameters
	float				m_aspectRatio;
    float               m_focalLength;
    float				m_nearClipPlane;
	float				m_farClipPlane;

    // random crap
    bool                m_doWorldTransform;
    bool                m_wireframe;   	  
	
	// texture stuff
	TextureManager		*m_textureManager;
    
    
    
    
};

#endif _DRAWER_H_
