/*
 *  Drawable.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/24/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include <common.h>
#include "Drawable.h"


Drawable::Drawable(VertexArray* vertices, bool deepCopyVertices,
				   IndexArray* indices, bool deepCopyIndices,
				   const Matrix& wMat) :
    m_vertices(vertices),
    m_indices(indices),
    m_worldMatrix(wMat),
    m_textureMatrix(Matrix::IdentityCRef()),
    m_color(1.0f, 1.0f, 1.0f, 1.0f),
	m_deepCopiedVertices(deepCopyVertices),
	m_deepCopiedIndices(deepCopyIndices)
{
}


Drawable::Drawable(VertexArray* vertices, bool deepCopyVertices,
				   const Matrix& wMat) :
    m_vertices(vertices),
    m_indices(NULL),
    m_worldMatrix(wMat),
    m_textureMatrix(Matrix::IdentityCRef()),
    m_color(1.0f, 1.0f, 1.0f, 1.0f),
	m_deepCopiedVertices(deepCopyVertices),
	m_deepCopiedIndices(false)
{
}


Drawable::Drawable(const Drawable& rhs) :
    m_worldMatrix(rhs.m_worldMatrix),
    m_textureMatrix(rhs.m_textureMatrix),
    m_color(rhs.m_color),
	m_vertices(rhs.m_vertices),
	m_indices(rhs.m_indices),
	m_deepCopiedVertices(rhs.m_deepCopiedVertices),
	m_deepCopiedIndices(rhs.m_deepCopiedIndices)
{
	if(m_deepCopiedVertices)
		m_vertices = new VertexArray(*rhs.m_vertices);
	if(m_deepCopiedIndices)
		m_indices = new IndexArray(*rhs.m_indices);
	
}


Drawable::~Drawable()
{
    if(m_deepCopiedVertices) SAFE_DELETE(m_vertices);
	if(m_deepCopiedIndices)  SAFE_DELETE(m_indices);
}


void
Drawable::setColor(const Color4F& c)
{
	m_color = c;
}


void
Drawable::setOpacity(float a)
{
	m_color.a = a;
}


void
Drawable::setVertexArray(VertexArray* vertices, bool deepCopy)
{
	if(m_deepCopiedVertices)
		SAFE_DELETE_AND_CLEAR(m_vertices);
	
    if(deepCopy)
        m_vertices = new VertexArray(*vertices);
    else
        m_vertices = vertices;
	
	m_deepCopiedVertices = deepCopy;
}


void
Drawable::setIndexArray(IndexArray* indices, bool deepCopy)
{
	if(m_deepCopiedIndices)
		SAFE_DELETE_AND_CLEAR(m_indices);
	
    if(deepCopy)
        m_indices = new IndexArray(*indices);
    else
        m_indices = indices;
	
	m_deepCopiedIndices = deepCopy;
}