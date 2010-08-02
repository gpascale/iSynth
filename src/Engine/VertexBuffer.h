/*
 *  VertexBuffer.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VERTEXBUFFER_H_
#define _VERTEXBUFFER_H_

#include <EngineTypes.h>

class VertexBuffer
{
	
public:	

    
    
	VertexBuffer();
	VertexBuffer(const VertexBuffer& rhs);
	~VertexBuffer();

	void			setData(unsigned size, 
							const void* data,
                            VertexFormat fmt,
							bool apiReady = false,
							VBType type = VB_STATIC);	
	
#ifdef IPHONE
	void			setDataGL(unsigned size, 
							  const void* data, 
							  VBType type = VB_STATIC);	
#endif	
	
	
#ifdef WINDOWS	
	void			setDataD3D(unsigned size, 
							   const void* data, 
							   VBType type = VB_STATIC);	
#endif
	
    VertexFormat    getVertexFormat() const { return m_vertexFormat; }
	bool			isUsingAPI()      const { return m_usingAPI; }	
	int				getSize()         const { return m_size;     }
	VBType			getType()         const { return m_type;     }
    
    void            useAPI() { setData(m_size, m_data, m_vertexFormat, true, m_type); }
	
	unsigned int	getUnderlyingBuffer() const;
	
private:
	
#ifdef IPHONE		
	unsigned int	m_glVertexBuffer;
#endif
	
	VBType			m_type;
	unsigned char*	m_data;
	int				m_size;
	bool			m_usingAPI;
    VertexFormat    m_vertexFormat;

};


class IndexBuffer
{
	
public:
	
	IndexBuffer();
	IndexBuffer(const IndexBuffer& rhs);
	~IndexBuffer();

    VertexFormat    getVertexFormat()     const { return m_vertexFormat; }
    bool			isUsingAPI()          const { return m_usingAPI;     }
	int				getSize()             const { return m_size;         }
	unsigned int	getUnderlyingBuffer() const;	
	
	void			setData(unsigned size, 
							const void* data,
                            VertexFormat fmt,
							bool apiReady = false);		
	
#ifdef IPHONE	
	void			setDataGL(unsigned size, 
							  const void* data);
#endif
	
	
#ifdef WINDOWS
	void			setDataD3D(unsigned size, 
							   const void* data);
#endif
	
private:
	
#ifdef IPHONE
	unsigned int	m_glIndexBuffer;
#endif
	
	unsigned char*	m_data;
	int				m_size;
	bool			m_usingAPI;
    VertexFormat    m_vertexFormat;

};

#endif _VERTEXBUFFER_H_
