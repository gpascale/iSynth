#ifdef IPHONE

#include "GLDrawer.h"
#include <iostream>
#include <Quad.h>
#include <GMath.h>
#include <System.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#include <VertexBuffer.h>

#include "TextureManager.h"
#include <Drawable.h>
#include <Color.h>

#define USE_DEPTH_BUFFER false
using namespace std;

static TextureIDs s_textureIDs;

GLDrawer::GLDrawer(EAGLView *glView) : 
	Drawer(),
	m_eaglView(glView),
    m_screenRotation(0.0f),
	m_activeTexture(2, NULL),
	m_textureMatrix(2, Matrix::IdentityCRef())
{
    m_sizeFromFmt[FMT_USHORT] = 1;
	m_sizeFromFmt[FMT_FLOAT2] = m_sizeFromFmt[FMT_UCHAR2] = 2;
    m_sizeFromFmt[FMT_FLOAT3] = m_sizeFromFmt[FMT_UCHAR3] = 3;
    m_sizeFromFmt[FMT_FLOAT4] = m_sizeFromFmt[FMT_UCHAR4] = 4;
    
    m_typeFromFmt[FMT_USHORT] = GL_UNSIGNED_SHORT;
    m_typeFromFmt[FMT_FLOAT2] = m_typeFromFmt[FMT_FLOAT3] = m_typeFromFmt[FMT_FLOAT4] = GL_FLOAT;
    m_typeFromFmt[FMT_UCHAR2] = m_typeFromFmt[FMT_UCHAR3] = m_typeFromFmt[FMT_UCHAR4] = GL_UNSIGNED_BYTE;
    
	/*
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);    
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
	*/
}

GLDrawer::~GLDrawer()
{
}


void
GLDrawer::cleanup()
{
	setScreenRotation(0.0f);	
	safeDisableClientState(GL_VERTEX_ARRAY);
	safeDisableClientState(GL_TEXTURE_COORD_ARRAY);
	safeDisableClientState(GL_NORMAL_ARRAY);
	safeDisableClientState(GL_COLOR_ARRAY);
	safeDisable(GL_LIGHTING);
	safeDisable(GL_BLEND);
	safeDisable(GL_TEXTURE_2D);
	setWorldViewMatrix(Matrix::IdentityCRef(), Matrix::IdentityCRef());
	setProjectionMatrix(Matrix::IdentityCRef());
	setTextureMatrix(Matrix::IdentityCRef());
	setActiveTexture(NULL);
}


void
GLDrawer::destroyBuffers()
{
	//ASSERT([NSThread currentThread] != [NSThread mainThread]);
	[m_eaglView destroyFramebuffers];
}


void
GLDrawer::createBuffers()
{
	//ASSERT([NSThread currentThread] != [NSThread mainThread]);
	[m_eaglView createFramebuffers];
}


void
GLDrawer::regenBuffers()
{
	//ASSERT([NSThread currentThread] != [NSThread mainThread]);
	[m_eaglView destroyFramebuffers];
	[m_eaglView createFramebuffers];
}


// my own implementation of glGenTextures, since I was having some problems with it
bool
GLDrawer::gregGenTextures(int n, OUT unsigned int* ret)
{
	if(n <= s_textureIDs.size()){
		FOR(i,0,n){
			ret[i] = s_textureIDs.top();
			s_textureIDs.pop();
		}
		return true;
	}
	return false;
}


bool
GLDrawer::gregDeleteTextures(int n, const unsigned int* texIDs)
{
	FOR(i,0,n){
		s_textureIDs.push(texIDs[i]);
	}
	glDeleteTextures(n, texIDs);
	ASSERT(glGetError() == GL_NO_ERROR);
	return true;
}


void
GLDrawer::setWorldMatrix(const Matrix& worldMat)
{
	worldMat.toGLMatrix(m_glWorldMatrix, false);
	
	glMatrixMode(GL_MODELVIEW);	
	glLoadMatrixf(m_glViewMatrix);
    glMultMatrixf(m_glWorldMatrix);	 
}


void
GLDrawer::setViewMatrix(const Matrix& viewMat)
{
	viewMat.toGLMatrix(m_glViewMatrix, true);
	
	glMatrixMode(GL_MODELVIEW);	
	glLoadMatrixf(m_glViewMatrix);
    glMultMatrixf(m_glWorldMatrix);	 
}


void
GLDrawer::setWorldViewMatrix(const Matrix& worldMat, const Matrix& viewMat)
{
    worldMat.toGLMatrix(m_glWorldMatrix, false);
    viewMat.toGLMatrix(m_glViewMatrix, true);
    
    glMatrixMode(GL_MODELVIEW);	
	glLoadMatrixf(m_glViewMatrix);
    glMultMatrixf(m_glWorldMatrix);	 	
}


void
GLDrawer::setProjectionMatrix(const Matrix &projMat)
{	
	projMat.toGLMatrix(m_glProjectionMatrix, false);
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glRotatef(m_screenRotation, 0.0f, 0.0f, -1.0f);
	glMultMatrixf(m_glProjectionMatrix);
}


void
GLDrawer::setTextureMatrix(const Matrix &texMat, int textureUnit)
{
	ASSERT(textureUnit == 0 || textureUnit == 1);
	if(textureUnit == 0)
		glActiveTexture(GL_TEXTURE0);
	else
		glActiveTexture(GL_TEXTURE1);	

	float mat[16];
    texMat.toGLMatrix(mat, false);
    
    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf(mat);
}





void
GLDrawer::setViewport(int x0, int y0, int w, int h)
{
	glViewport(x0, y0, w, h);
}


void
GLDrawer::setScreenRotation(float angle)
{
    m_screenRotation = radToDeg(angle);
}


void
GLDrawer::beginOnscreen() const
{    
	[m_eaglView beginOnscreen];
}


void
GLDrawer::beginOnscreen(const Color4F &clearColor) const
{    
	[m_eaglView beginOnscreen: &clearColor];
}


void
GLDrawer::beginOffscreen() const
{
    static const Color4F black(0.0f, 0.0f, 0.0f, 1.0f);
	[m_eaglView beginOffscreen: &black];
}


void
GLDrawer::beginOffscreen(const Color4F &clearColor) const
{    
	[m_eaglView beginOffscreen: &clearColor];
}


void
GLDrawer::renderOffscreenBuffer()
{
	static const float U1 = 0.0f, U2 = 320.0 / 512.0,
					   V1 = 0.0f, V2 = 480.0 / 512.0;
	
	static const float v[8]  = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
	static const float tc[8] = { U1, V1, U2, V1, U1, V2, U2, V2 };	
	
	disableTextureUnit(1);
	enableTextureUnit(0);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
	
	setWorldViewMatrix(Matrix::IdentityCRef(), Matrix::IdentityCRef());
	setTextureMatrix(Matrix::IdentityCRef());
	
	setPositionSource(v, FMT_FLOAT2, 0);
	setTexCoordSource(tc, FMT_FLOAT2, 0);
	setColor(Color4F::Black());
	setDrawMode(DRAWMODE_TRIANGLESTRIP);
	
	glBindTexture(GL_TEXTURE_2D, OFFSCREEN_TEXTURE_ID);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	draw(4, NULL);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);			
	
}


void
GLDrawer::end() const
{
	[m_eaglView present];
}


void
GLDrawer::setActiveTexture(Texture* texture, int textureUnit)
{
	ASSERT(textureUnit == 0 || textureUnit == 1);
	if(textureUnit == 0)
		glActiveTexture(GL_TEXTURE0);
	else
		glActiveTexture(GL_TEXTURE1);
	m_activeTexture[textureUnit] = texture;
    unsigned int tex = (texture) ? texture->getGLTexture() : 0;
    glBindTexture(GL_TEXTURE_2D, tex);
}


const Texture*
GLDrawer::getActiveTexture(int textureUnit) const
{
	ASSERT(textureUnit == 0 || textureUnit == 1);
	return m_activeTexture[textureUnit];
}


void
GLDrawer::enableTextureUnit(int textureUnit)
{
	if(textureUnit == 0)
		glActiveTexture(GL_TEXTURE0);
	else
		glActiveTexture(GL_TEXTURE1);
	
	glEnable(GL_TEXTURE_2D);
}


void
GLDrawer::disableTextureUnit(int textureUnit)
{
	if(textureUnit == 0)
		glActiveTexture(GL_TEXTURE0);
	else
		glActiveTexture(GL_TEXTURE1);
	
	glDisable(GL_TEXTURE_2D);
}


void
GLDrawer::setMultitextureType(MultitextureType mtType)
{
	switch (mtType)
	{
		case MULTITEXTURE_INTERPOLATE:
			glActiveTexture(GL_TEXTURE1);
			
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
			
			// currently bound texture
			glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			
			// previous texture (GL_TEXTURE0)
			glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
			
			// interpolate via a user-defined floating point value
			glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_CONSTANT);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);
			break;
		default:
			ASSERT(0 && "Unsupported multitexture type");
	}
}


void
GLDrawer::setMultitextureInterpolationParameter(float f)
{
	// TODO: Can I use just a single float instead of a color array?
	static float c[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	c[3] = f;
	glActiveTexture(GL_TEXTURE1);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, c);
}


void
GLDrawer::setColor(const Color4F& c)
{
    safeDisableClientState(GL_COLOR_ARRAY);
    glColor4f(c.r, c.g, c.b, c.a);
}


void
GLDrawer::setPositionSource(const void* data, VertexFormat fmt, int stride)
{
    if(data == NULL){
        safeDisableClientState(GL_VERTEX_ARRAY);
        return;
    }
    
    safeEnableClientState(GL_VERTEX_ARRAY);    
    //glBindBuffer(GL_ARRAY_BUFFER, NULL);

    glVertexPointer(m_sizeFromFmt[fmt], m_typeFromFmt[fmt], stride, (const void*) data);        
}


void
GLDrawer::setPositionSource(const VertexBuffer* vb, int stride)
{
    if(vb == NULL){
        safeDisableClientState(GL_VERTEX_ARRAY);
        return;
    }

    safeEnableClientState(GL_VERTEX_ARRAY);
    const VertexFormat fmt = vb->getVertexFormat();

    if(vb->isUsingAPI()){
        //glBindBuffer(GL_ARRAY_BUFFER, vb->getUnderlyingBuffer());
        glVertexPointer(m_sizeFromFmt[fmt], m_typeFromFmt[fmt], stride, 0);
    }
    else{
        //glBindBuffer(GL_ARRAY_BUFFER, NULL);
        glVertexPointer(m_sizeFromFmt[fmt], m_typeFromFmt[fmt], stride, (const void*) vb->getUnderlyingBuffer());
    }
    
}


void
GLDrawer::setTexCoordSource(const void* data, VertexFormat fmt, int stride)
{
    if(data == NULL){
        safeDisableClientState(GL_TEXTURE_COORD_ARRAY);
        return;
    }
    
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);    
    //glBindBuffer(GL_ARRAY_BUFFER, NULL);
    
    glTexCoordPointer(m_sizeFromFmt[fmt], m_typeFromFmt[fmt], stride, (const void*) data);        
}


void
GLDrawer::setTexCoordSource(const VertexBuffer* vb, int stride)
{
    if(vb == NULL){
        safeDisableClientState(GL_TEXTURE_COORD_ARRAY);
        return;
    }

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    const VertexFormat fmt = vb->getVertexFormat();
    
    if(vb->isUsingAPI()){
        //glBindBuffer(GL_ARRAY_BUFFER, vb->getUnderlyingBuffer());
        glTexCoordPointer(m_sizeFromFmt[fmt], m_typeFromFmt[fmt], stride, 0);
    }
    else{
        //glBindBuffer(GL_ARRAY_BUFFER, NULL);
        glTexCoordPointer(m_sizeFromFmt[fmt], m_typeFromFmt[fmt], stride, (const void*) vb->getUnderlyingBuffer());
    }
}


void
GLDrawer::setNormalSource(const void* data, VertexFormat fmt, int stride)
{
    if(data == NULL){
        safeDisableClientState(GL_NORMAL_ARRAY);
        return;
    }

    safeEnableClientState(GL_NORMAL_ARRAY);            
    //glBindBuffer(GL_ARRAY_BUFFER, NULL);

    ASSERT(m_sizeFromFmt[fmt] == 3);
    glNormalPointer(m_typeFromFmt[fmt], stride, (const void*) data);
}


void
GLDrawer::setNormalSource(const VertexBuffer* vb, int stride)
{
    if(vb == NULL){
        safeDisableClientState(GL_NORMAL_ARRAY);
        return;
    }

    safeEnableClientState(GL_NORMAL_ARRAY);
    const VertexFormat fmt = vb->getVertexFormat();

    ASSERT(m_sizeFromFmt[fmt] == 3);
    if(vb->isUsingAPI()){
        glBindBuffer(GL_ARRAY_BUFFER, vb->getUnderlyingBuffer());
        glNormalPointer(m_typeFromFmt[fmt], stride, 0);
    }
    else{
        //glBindBuffer(GL_ARRAY_BUFFER, NULL);
        glNormalPointer(m_typeFromFmt[fmt], stride, (const void*) vb->getUnderlyingBuffer());
    }
}



void
GLDrawer::setColorSource(const void* data, VertexFormat fmt, int stride)
{
    if(data == NULL){
        safeDisableClientState(GL_COLOR_ARRAY);
        return;
    }

    safeEnableClientState(GL_COLOR_ARRAY);            
    //glBindBuffer(GL_ARRAY_BUFFER, NULL);

    glColorPointer(m_sizeFromFmt[fmt], m_typeFromFmt[fmt], stride, (const void*) data);
}


void
GLDrawer::setColorSource(const VertexBuffer* vb, int stride)
{
    if(vb == NULL){
        safeDisableClientState(GL_COLOR_ARRAY);
        return;
    }

    safeEnableClientState(GL_COLOR_ARRAY);
    const VertexFormat fmt = vb->getVertexFormat();

    if(vb->isUsingAPI()){
        //glBindBuffer(GL_ARRAY_BUFFER, vb->getUnderlyingBuffer());
        glColorPointer(m_sizeFromFmt[fmt], m_typeFromFmt[fmt], stride, 0);
    }
    else{
        //glBindBuffer(GL_ARRAY_BUFFER, NULL);
        glColorPointer(m_sizeFromFmt[fmt], m_typeFromFmt[fmt], stride, (const void*) vb->getUnderlyingBuffer());
    }
}


void
GLDrawer::setVertexSource(const VertexArray* vertices)
{   
    const VertexDesc& desc   = vertices->getDesc();
    const int         nPos   = desc.getNumPositionComponents();
    const int         nTex   = desc.getNumTexCoordComponents();
    const int         nCol   = desc.getNumColorComponents();
    const int         nNorm  = desc.getNumNormalComponents();            
    const int         stride = (const int) desc.getSize();
            
    glBindBuffer(GL_ARRAY_BUFFER, NULL);
    
    if(nPos > 0){
        safeEnableClientState(GL_VERTEX_ARRAY);        
        glVertexPointer(nPos, GL_FLOAT, stride, vertices->getPosition(0));        
    }
    else{
        safeDisableClientState(GL_VERTEX_ARRAY);
    }
    
    if(nTex > 0){
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);        
        glTexCoordPointer(nTex, GL_FLOAT, stride, vertices->getTexCoords(0));        
    }
    else{
        safeDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    
    if(nCol > 0){
        safeEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(nCol, GL_FLOAT, stride, vertices->getColor(0));        
    }
    else{
        safeDisableClientState(GL_COLOR_ARRAY);
    }
    
    if(nNorm > 0){
        ASSERT(nNorm == 3);
        safeEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, stride, vertices->getNormal(0));        
    }
    else{
        safeDisableClientState(GL_NORMAL_ARRAY);
    }
    
    
}
                     
                     
void
GLDrawer::draw(int n, const IndexBuffer* ib)
{   
	glActiveTexture(GL_TEXTURE0);
	
    if(ib == NULL){        
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
        glDrawArrays(m_glDrawMode, 0, n);
    }    
    else if (ib->isUsingAPI()){
        const VertexFormat fmt = ib->getVertexFormat();
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getUnderlyingBuffer());
        glDrawElements(m_glDrawMode, n, m_typeFromFmt[fmt], 0);
    }
    else{
        const VertexFormat fmt = ib->getVertexFormat();
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
        glDrawElements(m_glDrawMode, n, m_typeFromFmt[fmt], (const void*) ib->getUnderlyingBuffer());
    }                                              
}


void
GLDrawer::draw(int n, const void* indices, VertexFormat fmt)
{
	glActiveTexture(GL_TEXTURE0);
	
    if(indices == NULL){        
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
        glDrawArrays(m_glDrawMode, 0, n);
    }
    else{
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
        glDrawElements(m_glDrawMode, n, m_typeFromFmt[fmt], (const void*) indices);
    }
    
}
                     
                                                                                    
                                                                                                         
int
GLDrawer::getErrorCode()
{
    return glGetError();
}


#endif