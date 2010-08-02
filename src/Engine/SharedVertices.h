/*
 *  SharedVertices.h
 *  Phonosynth
 *
 *  Created by Greg on 2/11/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#include <VertexArray.h>

#ifndef _SHAREDVERTICES_H_
#define _SHAREDVERTICES_H_

class SharedVertices
{
public:
	
	SharedVertices(const VertexArray* va = NULL, const IndexArray* ia = NULL) :
		m_vertexArray(va),
		m_indexArray(ia)
	{ }
	
	const VertexArray* getSharedVertexArray() const { return m_vertexArray; }
	const IndexArray*  getSharedIndexArray()  const { return m_indexArray;  }
	
	bool hasVertexArray() const { return m_vertexArray != NULL; }
	bool hasIndexArray()  const { return m_indexArray  != NULL; }
	
	void setSharedVertexArray(const VertexArray* va) { m_vertexArray = va; }
	void setSharedIndexArray(const IndexArray* ia)   { m_indexArray = ia;  }
	
private:
		
	const VertexArray* m_vertexArray;
	const IndexArray*  m_indexArray;
	
};


class SharedVerticesFactory
{
public:	
	
	static SharedVertices* createCameraConeVertices()
	{
		/*
		float V[20] = {0.0f, 0.0f, 0.0f, 1.0f,
					   1.0f, -0.5f, -0.5f, 1.0f,
					   1.0f, 0.5f, -0.5f, 1.0f,
					   1.0f, -0.5f, 0.5f, 1.0f,
					   1.0f, 0.5f, 0.5f, 1.0f};
		 */
		
		
		float V[20] = {0.0f, 0.0f, 0.0f, 1.0f,
					   -0.5f, -0.5f, 1.0f, 1.0f,
					   0.5f, -0.5f, 1.0f, 1.0f,
					   -0.5f, 0.5f, 1.0f, 1.0f,
					   0.5f, 0.5f, 1.0f, 1.0f};
		 
		/*
		float V[20] = {0.0f, 0.0f, 0.0f, 1.0f,
					   -0.5f, 1.0f, -0.5f, 1.0f,
					   0.5f, 1.0f, -0.5f, 1.0f,
					   -0.5f, 1.0f, 0.5f, 1.0f,
					   0.5f, 1.0f, 0.5f, 1.0f};
		*/
		
		unsigned short I[16] = {0, 1, 0, 2, 0, 3, 0, 4, 1, 2, 2, 4, 4, 3, 3, 1};
		
		/*
		Vertex::XYZW*   vertices = new Vertex::XYZW[5];
		unsigned short* indices  = new unsigned short[16];
		 */
        
		VertexArray* va = new VertexArray(Vertex::XYZW::getDesc(), (float*) V, 5, true);
		IndexArray*  ia = new IndexArray(I, 16, true);
		
		return new SharedVertices(va, ia);
		
	}
	
};
	
inline
SharedVertices* getCameraConeSharedVertices()
{
	static SharedVertices* ret = SharedVerticesFactory::createCameraConeVertices();
	return ret;
}

#endif // _SHAREDVERTICES_H_