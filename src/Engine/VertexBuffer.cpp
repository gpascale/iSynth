/*
 *  VertexBuffer.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VertexBuffer.h"
#include <common.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>

VertexBuffer::VertexBuffer() :
	m_size(0),
	m_type(VB_STATIC),
	m_data(NULL),
	m_usingAPI(false)
{
#ifdef IPHONE
	m_glVertexBuffer = 0;
#endif
	
}


VertexBuffer::VertexBuffer(const VertexBuffer &rhs) :
	m_size(rhs.getSize()),
	m_type(rhs.getType()),
	m_data(NULL),
	m_glVertexBuffer(NULL),
	m_usingAPI(rhs.isUsingAPI())
{
	if(m_usingAPI)
		m_glVertexBuffer = rhs.getUnderlyingBuffer();
	else{
		m_data = new unsigned char[m_size];
		memcpy(m_data, (const void*) rhs.getUnderlyingBuffer(), m_size);
	}
}
	
	



VertexBuffer::~VertexBuffer()
{
	if(m_usingAPI){
		#ifdef IPHONE
			if(m_glVertexBuffer > 0)
				glDeleteBuffers(1, &m_glVertexBuffer);
		#endif
	}
	else{
		SAFE_DELETE_ARRAY(m_data);
	}
}


void
VertexBuffer::setData(unsigned size, 
					  const void* data,
                      VertexFormat fmt,
					  bool apiReady,
					  VBType type)
{
	m_type = type;
	m_size = size;
    m_vertexFormat = fmt;
	m_usingAPI = apiReady;
	
	if(apiReady){
		#ifdef IPHONE
			setDataGL(size, data, type);
		#endif
		#ifdef WINDOWS
			setDataD3D(size, data, type);
		#endif
	}
	else{
		m_data = new unsigned char[size];
		memcpy(m_data, data, size);
	}	
}



#ifdef IPHONE
void
VertexBuffer::setDataGL(unsigned size,
						const void* data,
						VBType type)
{
	SAFE_DELETE_AND_CLEAR_ARRAY(m_data);
	
	if(m_glVertexBuffer == 0){
		glGenBuffers(1, &m_glVertexBuffer);
 	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_glVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 
				 size, 
				 data, 
				 type == VB_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

		
}
#endif


unsigned int
VertexBuffer::getUnderlyingBuffer() const
{

	if(m_usingAPI){
		#ifdef IPHONE
			return m_glVertexBuffer;
		#endif
	}
	
	return (unsigned int) m_data;
	
}



IndexBuffer::IndexBuffer() :
	m_size(0),
	m_data(NULL),
	m_usingAPI(false)
{
#ifdef IPHONE
	m_glIndexBuffer = 0;
#endif
}



IndexBuffer::IndexBuffer(const IndexBuffer& rhs) :
	m_size(rhs.getSize()),
	m_data(NULL),
	m_glIndexBuffer(NULL),
	m_usingAPI(rhs.isUsingAPI())
{
	
	if(m_usingAPI){
		m_glIndexBuffer = rhs.getUnderlyingBuffer();
	}
	else{
		m_data = new unsigned char[m_size];
		memcpy(m_data, (const void*) rhs.getUnderlyingBuffer(), m_size);
	}
	
}



IndexBuffer::~IndexBuffer()
{
	if(m_usingAPI){
		#ifdef IPHONE
			if(m_glIndexBuffer > 0)
				glDeleteBuffers(1, &m_glIndexBuffer);
		#endif
	}
	else{
		SAFE_DELETE_ARRAY(m_data);
	}
}



void
IndexBuffer::setData(unsigned size,
                     const void* data,
                     VertexFormat fmt,
                     bool apiReady)
{
	SAFE_DELETE_AND_CLEAR_ARRAY(m_data);
	
	m_size = size;
    m_vertexFormat = fmt;
	m_usingAPI = apiReady;
	
	if(apiReady){
		#ifdef IPHONE
			setDataGL(size, data);
		#endif
		#ifdef WINDOWS
			setDataD3D(size, data);
		#endif
	}
	else{
		m_data = new unsigned char[m_size];
		memcpy(m_data, data, size);
	}
}


#ifdef IPHONE
void
IndexBuffer::setDataGL(unsigned size, const void* data)
{
		
	if(m_glIndexBuffer == 0)
		glGenBuffers(1, &m_glIndexBuffer);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 size,
				 data,
				 GL_STATIC_DRAW);
	
	//printf("generated index buffer %d\n", m_glIndexBuffer);

}
#endif


unsigned int
IndexBuffer::getUnderlyingBuffer() const
{
	
	if(m_usingAPI){
		#ifdef IPHONE
			return m_glIndexBuffer;
		#endif
	}
	
	return (unsigned int) m_data;
	
}











