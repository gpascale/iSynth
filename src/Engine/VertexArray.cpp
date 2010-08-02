/*
 *  VertexArray.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/19/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "VertexArray.h"


IndexArray::IndexArray(unsigned short* data, int nIndices, bool deepCopy) :
    m_data(data), 
    m_size(nIndices),
	m_deepCopied(deepCopy)
{	
	if(deepCopy){
		m_data = new unsigned short[m_size];
		memcpy(m_data, data, m_size * sizeof(unsigned short));
	}
}


IndexArray::IndexArray(const IndexArray& rhs) :
	m_data(rhs.m_data),
	m_size(rhs.m_size),
	m_deepCopied(rhs.m_deepCopied)
{
	if(m_deepCopied){
		m_data = new unsigned short[m_size];
		memcpy(m_data, rhs.m_data, m_size * sizeof(unsigned short));
	}
}


IndexArray::~IndexArray()
{
    if(m_deepCopied)
		SAFE_DELETE_ARRAY(m_data);
}


VertexArray::VertexArray(const VertexDesc& desc, float* data, int nVerts, bool deepCopy) :
    m_desc(desc),
    m_data(data),
    m_nVerts(nVerts),
	m_deepCopied(deepCopy),
    m_positionOffsetInBytes(desc.getPositionOffset()),
    m_positionOffsetInFloats(desc.getPositionOffset() / 4),
    m_texCoordOffsetInBytes(desc.getTexCoordOffset()),
    m_texCoordOffsetInFloats(desc.getTexCoordOffset() / 4),
    m_colorOffsetInBytes(desc.getColorOffset()),
    m_colorOffsetInFloats(desc.getColorOffset() / 4),
    m_normalOffsetInBytes(desc.getNormalOffset()),
    m_normalOffsetInFloats(desc.getNormalOffset() / 4),
    m_sizeInBytes(desc.getSize()),
    m_sizeInFloats(desc.getSize() / 4)
{    
    ASSERT(m_sizeInBytes % 4 == 0);
    ASSERT(m_positionOffsetInBytes % 4 == 0);
    ASSERT(m_texCoordOffsetInBytes % 4 == 0);
    ASSERT(m_colorOffsetInBytes % 4 == 0);
    ASSERT(m_normalOffsetInBytes % 4 == 0);
	
	if(deepCopy){
		m_data = new float[m_nVerts * m_sizeInFloats];
		memcpy(m_data, data, m_nVerts * m_sizeInBytes);
	}
}
    

VertexArray::VertexArray(const VertexArray& rhs) : 
	m_desc(rhs.m_desc),
	m_data(rhs.m_data),
	m_nVerts(rhs.m_nVerts),
	m_deepCopied(rhs.m_deepCopied),
	m_positionOffsetInBytes(rhs.m_positionOffsetInBytes),
	m_positionOffsetInFloats(rhs.m_positionOffsetInFloats),
	m_texCoordOffsetInBytes(rhs.m_texCoordOffsetInBytes),
	m_texCoordOffsetInFloats(rhs.m_texCoordOffsetInFloats),
	m_colorOffsetInBytes(rhs.m_colorOffsetInBytes),
	m_colorOffsetInFloats(rhs.m_colorOffsetInFloats),
	m_normalOffsetInBytes(rhs.m_normalOffsetInBytes),
	m_normalOffsetInFloats(rhs.m_normalOffsetInFloats),
	m_sizeInBytes(rhs.m_sizeInBytes),
	m_sizeInFloats(rhs.m_sizeInFloats)
{
	if(m_deepCopied){
		m_data = new float[m_nVerts * m_sizeInFloats];
		memcpy(m_data, rhs.m_data, m_nVerts * m_sizeInBytes);
	}
}


VertexArray::~VertexArray()
{
    if(m_deepCopied)
		SAFE_DELETE_ARRAY(m_data);
}

const VertexDesc&
VertexArray::getDesc() const
{
    return m_desc;
}


int 
VertexArray::getNumVertices() const
{
    return m_nVerts;
}


const float* 
VertexArray::getData() const
{
    return m_data;
}


float*
VertexArray::getData()
{
    return m_data;
}


const float* 
VertexArray::getVertex(int index) const
{
    ASSERT(index < m_nVerts);
    return m_data + (index * m_sizeInFloats);
}


float* 
VertexArray::getVertex(int index)
{
    ASSERT(index < m_nVerts);
    return m_data + (index * m_sizeInFloats);
}


const float*
VertexArray::getPosition(int index) const
{
    ASSERT(index < m_nVerts);
    return m_data + (index * m_sizeInFloats + m_positionOffsetInFloats);
}


float*
VertexArray::getPosition(int index)
{
    ASSERT(index < m_nVerts);
    return m_data + (index * m_sizeInFloats + m_positionOffsetInFloats);
}

const float*
VertexArray::getTexCoords(int index) const
{
    ASSERT(index < m_nVerts);
    return m_data + (index * m_sizeInFloats + m_texCoordOffsetInFloats);
}


float* 
VertexArray::getTexCoords(int index)
{
    ASSERT(index < m_nVerts);
    return m_data + (index * m_sizeInFloats + m_texCoordOffsetInFloats);
}


const float*
VertexArray::getColor(int index) const
{
    ASSERT(index < m_nVerts);
    return m_data + (index * m_sizeInFloats + m_colorOffsetInFloats);
}


float* 
VertexArray::getColor(int index)
{
    ASSERT(index < m_nVerts);
    return m_data + (index * m_sizeInFloats + m_colorOffsetInFloats);
}

const float* 
VertexArray::getNormal(int index) const
{
    ASSERT(index < m_nVerts);
    return m_data + (index * m_sizeInFloats + m_normalOffsetInFloats);
}


float* 
VertexArray::getNormal(int index)
{
    ASSERT(index < m_nVerts);
    return m_data + (index * m_sizeInFloats + m_normalOffsetInFloats);
}


// convenience functions below not yet implemented
Vector2 
VertexArray::getPosition2(int index) const
{
    return Vector2();
}


Vector3 
VertexArray::getPosition3(int index) const
{
    return Vector3();
}


Vector4 
VertexArray::getPosition4(int index) const
{
    return Vector4();
}


Vector2 
VertexArray::getTexCoord2(int index) const
{
    return Vector2();
}


Vector3 
VertexArray::getTexCoord3(int index) const
{
    return Vector3();
}


Vector4 
VertexArray::getTexCoord4(int index) const
{
    return Vector4();
}


Vector2 
VertexArray::getColor2(int index) const
{
    return Vector2();
}


Vector3 
VertexArray::getColor3(int index) const
{
    return Vector3();
}


Vector4 
VertexArray::getColor4(int index) const
{
    return Vector4();
}


Vector2 
VertexArray::getNormal2(int index) const
{
    return Vector2();
}


Vector3 
VertexArray::getNormal3(int index) const
{
    return Vector3();
}


Vector4 
VertexArray::getNormal4(int index) const
{
    return Vector4();
}


// Code below allows for transforming a redundant vertex array into a smaller vertex array with a
// corresponding index array


// hacked less-than operator for fuzzy vertex comparisons. 
template <int N>
struct VertexComparator : public binary_function<const float*, const float*, bool>
{
    bool operator () (const float* v1, const float* v2){
        
        // first check to see if the vertices are equal. If so return false. This is independent of order
        // so if A is within epsilon of B, A<B and B<A will evaluate to false
		
        bool equal = true;        
        FOR(i,0,N){			
			/*
			if((v1[i] - v2[i] > .001f || v1[i] - v2[i] < -.001f) && 
			   (v2[i] - v1[i] > .001f || v2[i] - v1[i] < -.001f))
			 */
			if(fabs(v1[i] - v2[i]) < .001f)
			{
				equal = false;
				break;
			}
		}
		if(equal) return false;
        
        // if the vertices are not equal, we need some sort of strict ordering. Look for the first index
        // at which the vertices differ. The one with a smaller value in that index is the lesser vertex
        FOR(i,0,N){
			if(v1[i] < v2[i])
				return true;
			else if(v1[i] > v2[i])
				return false; 		
		}
		return false;
    }
};

typedef map<const float*, int, VertexComparator<6> >       VertexMap;
typedef VertexMap::iterator							       VertexMapIterator;
typedef set<const float*, VertexComparator<6> >            VertexSet;
typedef VertexSet::iterator								   VertexSetIterator;

void
makeIndexed(const VertexArray* vIn, OUT VertexArray** vOut, OUT IndexArray** iOut)
{            
    VertexMap         vertexMap;
    VertexMapIterator iter;
    int               curIdx = 0;
    const int         numIndices = vIn->getNumVertices();
    const int         vertexSize = vIn->getDesc().getSize() / 4; // hacky, size in floats
    
    vector<const float*> tempVertices;        
    
    FOR(i,0,numIndices){
        const float* cur = vIn->getVertex(i);
        iter = vertexMap.find(cur);				
        if(iter == vertexMap.end()){
			vertexMap[cur] = curIdx++;
            tempVertices.push_back(cur);
        }
    }
    
    const int numVertices = tempVertices.size();
                
    float          vertices[vertexSize * vertexMap.size()];
    unsigned short indices[numIndices];

    for(iter = vertexMap.begin(), curIdx = 0; iter != vertexMap.end(); iter++, curIdx++){
        float* to = &vertices[vertexSize * curIdx]; // index * size of vertex in floats, since to is a float*
        const float* from = tempVertices[curIdx];  // vertex in the vectorl
        memcpy(to, from, vertexSize * sizeof(float));
    }
    
    FOR(i,0,numIndices){
        const float* curVertex = vIn->getVertex(i);
        VertexMapIterator iter = vertexMap.find(curVertex);
		
		if(iter == vertexMap.end()){
			//cerr << *((Vertex::XYZWUV*) curVertex) << " not in map" << endl;
			
			VertexMapIterator iter2;
			
			// debug info
			for(iter2 = vertexMap.begin(); iter2 != vertexMap.end(); iter2++)
				if(fabs(*(iter2->first) - *curVertex) < 0.01f){
					//cerr << "Vertex: " << *((Vertex::XYZWUV*) iter2->first) << endl;
					const float* v1 = iter2->first;
					VertexComparator<6> vcomp;
					//cerr << "v < v1 = " << vcomp(curVertex, iter2->first) << endl;
					//cerr << "v1 < v = " << VertexComparator<6>()(iter2->first, curVertex) << endl;
				}
			
			BREAK;
		}
		
        ASSERT(iter != vertexMap.end());
        indices[i] = iter->second;
    }

    *vOut = new VertexArray(vIn->getDesc(), vertices, numVertices, true);
    *iOut = new IndexArray(indices, numIndices, true);
	
}





