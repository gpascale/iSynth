/*
 *  VertexArray.h
 *  Phonosynth
 *
 *  Created by Greg on 12/19/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */


#ifndef _VERTEXARRAY_H_
#define _VERTEXARRAY_H_

#include <GMath.h>
#include <Vertex.h>
#include <vector>
#include <map>
#include <set>

class IndexArray
{
public:
    
    IndexArray(unsigned short* data = NULL, int nIndices = 0, bool deepCopy = false);
	IndexArray(const IndexArray& rhs);
    ~IndexArray();
    
    const unsigned short* getIndices()    const { return m_data; }    
    int                   getNumIndices() const { return m_size; }
    
private:    	
	
    unsigned short* m_data;
    int             m_size;	
	bool			m_deepCopied;
    
};


class VertexArray
{
    
public:    
    
    VertexArray(const VertexDesc& desc, float* data = NULL, int nVerts = 0, bool deepCopy = false);
	VertexArray(const VertexArray& rhs);
    ~VertexArray();
    
    const VertexDesc& getDesc() const;
    
    int          getNumVertices()         const;
    
    const float* getData()                const;
          float* getData();
    
    const float* getVertex(int index)     const;
          float* getVertex(int index);
    
    const float* getPosition(int index)   const;
          float* getPosition(int index);
    
    const float* getTexCoords(int index)  const;
          float* getTexCoords(int index);
    
    const float* getColor(int index)      const;
          float* getColor(int index);
    
    const float* getNormal(int index)     const;
          float* getNormal(int index);

    Vector2 getPosition2(int index) const;
    Vector3 getPosition3(int index) const;
    Vector4 getPosition4(int index) const;
    
    Vector2 getTexCoord2(int index) const;
    Vector3 getTexCoord3(int index) const;
    Vector4 getTexCoord4(int index) const;
    
    Vector2 getColor2(int index) const;
    Vector3 getColor3(int index) const;
    Vector4 getColor4(int index) const;
    
    Vector2 getNormal2(int index) const;
    Vector3 getNormal3(int index) const;
    Vector4 getNormal4(int index) const;
    
private:    
    
    int        m_positionOffsetInBytes;
    int        m_positionOffsetInFloats;
    
    int        m_texCoordOffsetInBytes;
    int        m_texCoordOffsetInFloats;
    
    int        m_colorOffsetInBytes;
    int        m_colorOffsetInFloats;
    
    int        m_normalOffsetInBytes;
    int        m_normalOffsetInFloats;
    
    int        m_sizeInBytes;
    int        m_sizeInFloats;
    
    float*     m_data;
    int        m_nVerts;
    VertexDesc m_desc;  
	
	bool	   m_deepCopied;
    
};



extern void makeIndexed(const VertexArray* vIn, OUT VertexArray** vOut, OUT IndexArray** iOut);


#endif _VERTEXARRAY_H_

