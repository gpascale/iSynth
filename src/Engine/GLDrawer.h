
#ifdef IPHONE

#ifndef _GLDRAWER_H_
#define _GLDRAWER_H_

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import <EAGLView.h>

#include <queue>
#include <map>
#include <algorithm>
#include "Drawer.h"

using namespace std;

class Texture;
class Quad;
class Drawable3D;

/*
 * This is the OpenGL ES implementation of Drawer
 */


class TextureIDs : public priority_queue<unsigned int, vector<unsigned int>, greater<unsigned int> >
{
public:	
	TextureIDs()
	{
		FOR(i,1,1001) push(i);
	}
};


class GLDrawer : public Drawer
{
	

public:
	GLDrawer(EAGLView *glView = NULL);
	~GLDrawer(void);
	
    void cleanup();
	
	
	void destroyBuffers();
	void createBuffers();
	void regenBuffers();
	
	void beginOnscreen() const;
	void beginOnscreen(const Color4F& clearColor) const;
	
	void beginOffscreen() const;
	void beginOffscreen(const Color4F& clearColor) const;
	void renderOffscreenBuffer();
	
	void end() const;
	
    // state
    void setProjectionMatrix(const Matrix &projMat);
	void setViewMatrix(const Matrix &viewMat);
	void setWorldMatrix(const Matrix &worldMat);
    void setWorldViewMatrix(const Matrix& w, const Matrix& v);
    void setTextureMatrix(const Matrix& textureMatrix, int textureUnit = 0);
    
    const Matrix& getWorldMatrix() const { ASSERT(0); /*GLDrawer does not store matrices*/ }
    const Matrix& getViewMatrix() const { ASSERT(0); /*GLDrawer does not store matrices*/ }
	const Matrix& getProjectionMatrix() const { ASSERT(0); /*GLDrawer does not store matrices*/ }
    const Matrix& getTextureMatrix(int textureUnit = 0) const { ASSERT(0); /*GLDrawer does not store matrices*/ }
    
	
    void		   setActiveTexture(Texture* texture, int textureUnit = 0);	
	const Texture* getActiveTexture(int textureUnit = 0) const;
		
	void enableTextureUnit(int n);
	void disableTextureUnit(int n);
	void setMultitextureType(MultitextureType mtType);
	void setMultitextureInterpolationParameter(float f);
	
    void setViewport(int x0, int y0, int w, int h);
    void setScreenRotation(float angle);
    
    /****************************************************************************************/
    /*                              NEW STUFF                                               */
    /****************************************************************************************/       	
    
	void enableBlending()      { safeEnable(GL_BLEND);       }	
	void disableBlending()     { safeDisable(GL_BLEND);      }    
    
    void enableDepthTesting()  { safeEnable(GL_DEPTH_TEST);  }
    void disableDepthTesting() { safeDisable(GL_DEPTH_TEST); }
    
    void enableLighting()      { safeEnable(GL_LIGHTING);    }	
	void disableLighting()     { safeDisable(GL_LIGHTING);   }
	
	void setLineThickness(float f) { glLineWidth(f); }
    
    void setColor(const Color4F& c);	
    void setDrawMode(DrawMode dm);
    
    void setPositionSource(const VertexBuffer* vb, int stride);
    void setPositionSource(const void* data, VertexFormat fmt, int stride);
    
    void setTexCoordSource(const VertexBuffer* vb, int stride);
    void setTexCoordSource(const void* data, VertexFormat fmt, int stride);
    
    void setNormalSource(const VertexBuffer* vb, int stride);
    void setNormalSource(const void* data, VertexFormat fmt, int stride);
    
    void setColorSource(const VertexBuffer* vb, int stride);
    void setColorSource(const void* data, VertexFormat fmt, int stride);
    
    void setVertexSource(const VertexArray* vertices);
    
    void draw(int n, const IndexBuffer* ib);
    void draw(int n, const void* indices, VertexFormat fmt);
    				
	DrawMode getDrawMode() const { return m_mode; }
	
	
	// my own implementation of glGenTextures, since I was having some problems with it
	static bool gregGenTextures(int n, OUT unsigned int* ret);
	static bool gregDeleteTextures(int n, const unsigned int* texIDs);
	
	
    /****************************************************************************************/
    /*                             END NEW STUFF                                            */
    /****************************************************************************************/
    
    
    /*
	void drawGhettoQuadPrepare();
	void drawGhettoQuad(const GhettoQuad* gq);
	*/
    
    int  getErrorCode();
    
   	void setEAGLView(EAGLView *view){ m_eaglView = view; }
	

private:
    
    GLenum              m_glDrawMode;	    
        
    
    float               m_glWorldMatrix[16];
    float               m_glViewMatrix[16];
    float               m_glProjectionMatrix[16];   // not being used
    float               m_glTextureMatrix[16];      // not being used
    
	EAGLView			*m_eaglView; 
	
	DrawMode			m_mode;
	
    // GL ES does not check for redundant state modification requests
    bitset<64000>       m_glState;
    GLenum              m_sizeFromFmt[10];
    GLenum              m_typeFromFmt[10];
    
    inline void         safeEnable(GLenum e);
    inline void         safeDisable(GLenum e);    
    inline void         safeEnableClientState(GLenum e);
    inline void         safeDisableClientState(GLenum e);    
    
    Matrix              m_worldMatrix;
    Matrix              m_viewMatrix;
    Matrix              m_projectionMatrix;
    vector<Matrix>      m_textureMatrix;
        
    float               m_screenRotation;
		
			
	TextureIDs			m_textureIDs;
	vector<Texture*>	m_activeTexture;
};


inline void
GLDrawer::safeEnable(GLenum e)
{
    if(m_glState.test(e))
        return;
    glEnable(e);
    m_glState.set(e, 1);
}


inline void
GLDrawer::safeDisable(GLenum e)
{
    if(!(m_glState.test(e)))
        return;
    glDisable(e);
    m_glState.set(e, 0);
}


inline void
GLDrawer::safeEnableClientState(GLenum e)
{
    if(m_glState.test(e))
        return;
    glEnableClientState(e);
    m_glState.set(e, 1);
}


inline void
GLDrawer::safeDisableClientState(GLenum e)
{
    if(!(m_glState.test(e)))
        return;
    glDisableClientState(e);
    m_glState.set(e, 0);
}


inline void
GLDrawer::setDrawMode(DrawMode mode)
{
    switch(mode)
    {
        case DRAWMODE_POINTS:            
            m_glDrawMode = GL_POINTS;
            break;
        case DRAWMODE_LINES:
            m_glDrawMode = GL_LINE_STRIP;
            break;
		case DRAWMODE_LINE_LOOP:
			m_glDrawMode = GL_LINE_LOOP;
			break;
        case DRAWMODE_TRIANGLES:
            m_glDrawMode = GL_TRIANGLES;
            break;        
        case DRAWMODE_TRIANGLESTRIP:
            m_glDrawMode = GL_TRIANGLE_STRIP;
            break;
        case DRAWMODE_TRIANGLEFAN:
            m_glDrawMode = GL_TRIANGLE_FAN;
            break;
        default:
            m_glDrawMode = GL_LINE_STRIP;
    } 
	
	m_mode = mode;
}



#endif _GLDRAWER_H_

#endif // IPHONE