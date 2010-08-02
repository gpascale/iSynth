/*
 *  GhettoQuad.cpp
 *  Phonosynth
 *
 *  Created by Greg on 10/13/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "GhettoQuad.h"
#include <Texture.h>
#include <Drawer.h>
#include <VertexArray.h>
#include <System.h>
#import  <OpenGLES/EAGL.h>
#import  <OpenGLES/ES1/gl.h>
#import  <OpenGLES/ES1/glext.h>

GhettoQuad::GhettoQuad() :
    m_x(0),
    m_y(0),
    m_width(0),
    m_height(0),
    m_texture(NULL)
{ 
    
    static Vertex::XYUV vertices[4] = {Vertex::XYUV(-0.5f, -0.5f, 0.0f, 0.0f),
                                       Vertex::XYUV( 0.5f, -0.5f, 1.0f, 0.0f),
                                       Vertex::XYUV(-0.5f,  0.5f, 0.0f, 1.0f),
                                       Vertex::XYUV( 0.5f,  0.5f, 1.0f, 1.0f)};
    
    VertexArray* vertArray = new VertexArray(Vertex::XYUV::getDesc(), (float*) vertices, 4);
    setVertexArray(vertArray);
    setColor(Color4UC(255, 255, 255, 255));
}



GhettoQuad::GhettoQuad(const GhettoQuad& gq) :	
    m_x(gq.m_x),
    m_y(gq.m_y),
    m_width(gq.m_width),
    m_height(gq.m_height),
    m_texture(gq.m_texture),
	m_translationMatrix(gq.m_translationMatrix),
	m_rotationMatrix(gq.m_rotationMatrix),
	m_scaleMatrix(gq.m_scaleMatrix),
    Drawable(gq)
{ 
    memcpy(m_colors, gq.m_colors, sizeof(m_colors));
}


void
GhettoQuad::setX(int x)
{
    setLocation(x, m_y);
}


void
GhettoQuad::setY(int y)
{
    setLocation(m_x, y);
}


void
GhettoQuad::setLocation(int x, int y)
{
    m_x = x;
    m_y = y;    
    m_translationMatrix(3, 0) = m_x;
    m_translationMatrix(3, 1) = m_y;

	m_worldMatrix = m_scaleMatrix * m_rotationMatrix * m_translationMatrix;	
}


void
GhettoQuad::setWidth(float width)
{
    setSize(width, m_height);
}


void
GhettoQuad::setHeight(float height)
{
    setSize(m_width, height);
}


void
GhettoQuad::setSize(float width, float height)
{
    m_width = width;
    m_height = height;
    m_scaleMatrix(0, 0) = width;
    m_scaleMatrix(1, 1) = height;
	
	m_worldMatrix = m_scaleMatrix * m_rotationMatrix * m_translationMatrix;
}


void
GhettoQuad::setRotation(float angle)
{
	Matrix::RotationMatrix(m_rotationMatrix, angle, 0.0f, 0.0f, 1.0f);
	m_worldMatrix = m_scaleMatrix * m_rotationMatrix * m_translationMatrix;
}


void
GhettoQuad::setColor(const Color4UC& cAll)
{
    setColor(cAll, cAll, cAll, cAll);
}


void
GhettoQuad::setColor(const Color4UC& c1, const Color4UC& c2, const Color4UC& c3, const Color4UC& c4)
{
    m_colors[0] = c1;
    m_colors[1] = c2;
    m_colors[2] = c3;
    m_colors[3] = c4;
}


void
GhettoQuad::getColor(OUT Color4UC& c1, OUT Color4UC& c2, OUT Color4UC& c3, OUT Color4UC& c4) const
{
    c1 = m_colors[0];
    c2 = m_colors[1];
    c3 = m_colors[2];
    c4 = m_colors[3];
}
    

void
GhettoQuad::setOpacity(float newOpacity)
{
    for(int i = 0; i < 4; i++)
        m_colors[i].a = newOpacity * 255;
}


float
GhettoQuad::getOpacity() const
{
    return m_colors[0].a / 255.0f;
}


bool
GhettoQuad::setTextureWithImage(const char* textureFile, short width, short height)
{
    return setTextureWithImage(string(textureFile), width, height);
}


bool
GhettoQuad::setTextureWithImage(const string& textureFile, short width, short height)
{
    TexCreateArgs args(TEX_FORMAT_RGBA_4444, width, height);
    args.stretchType = STRETCH_NONE;
    
    m_texture = Texture::createTexture(textureFile, args);	
	
    if(m_texture == NULL)
        return false;
    
    Matrix::ScaleMatrix(m_textureMatrix, m_texture->getHRatio(), m_texture->getVRatio(), 1.0f);
    return true;
}


bool
GhettoQuad::setTextureWithText(const char* text, short width, short height,
							   const Color4F& textColor,
							   const Color4F& backgroundColor)
{
    return setTextureWithText(string(text), width, height);
}


bool
GhettoQuad::setTextureWithText(const string& text, short width, short height,
							   const Color4F& textColor,
							   const Color4F& backgroundColor)
{
    m_texture = Texture::createTextureFromText(text, width, height, textColor, backgroundColor, "Marker Felt");
	
    if(m_texture == NULL)
        return false;
    
    Matrix::ScaleMatrix(m_textureMatrix, m_texture->getHRatio(), m_texture->getVRatio(), 1.0f);
    return true;
}


void GhettoQuad::setTexture(Texture* t)
{	
    m_texture = t;
    Matrix::ScaleMatrix(m_textureMatrix, m_texture->getHRatio(), m_texture->getVRatio(), 1.0f);
}


const Texture*
GhettoQuad::getTexture()
{
    return m_texture;
}


bool 
GhettoQuad::clickIntersects(int clickX, int clickY, int err) const
{
    if(clickX < m_x - (0.5f * m_width)	- err) return false;
    if(clickX > m_x + (0.5f * m_width)  + err) return false;
    if(clickY < m_y - (0.5f * m_height) - err) return false;
    if(clickY > m_y + (0.5f * m_height) + err) return false;
    return true;
}



void
GhettoQuad::draw(Drawer& d)
{
	d.setWorldViewMatrix(m_worldMatrix, Matrix::IdentityCRef());
    d.setTextureMatrix(m_textureMatrix);
    d.setActiveTexture(m_texture);

    // draw the quad    
    d.setVertexSource(m_vertices);
    d.setColorSource(m_colors, FMT_UCHAR4);            
    d.draw(4, NULL);    
}



