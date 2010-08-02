/*
 *  Pointcloud.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_


#include <common.h>
#include <Vertex.h>
#include <vector>
#include <SceneGraph.h>
#include <Drawable.h>

class Drawer;

class Pointcloud : public Drawable
{

public:

	Pointcloud();
	~Pointcloud();
	
	void					setVertices(const Vertex::XYZ* positions, 
										const Color4UC* colors,
										int numVerts);
	void					addVerticesFromBinaryData(ByteArray data,
													  float factor);
	
    // TEMPORARY HACK
    void    draw(Drawer& d);	
	
	int		getNumBlobs()                  const  { return (int) m_blobSizes.size(); }
	int		getNumPointsForBlob(int blob)  const  { return m_blobSizes[blob]; }
	const Vertex::XYZ* getPointsForBlob(int blob) { return m_pointPositions[blob]; };
	
	int		getSize() const;
	
	int		getNumPoints() const { return m_numPoints; }
	
private:

	static void			readVertex(const void*      bytes,
								   Vertex::XYZ&     pos,
								   Color4UC&        color);
	
	static void			colorFromShort(Color4UC &ret, short c);
	
	int							m_numPoints;
	vector<Vertex::XYZ*>		m_pointPositions;
	vector<Color4UC*>           m_pointColors;	
	vector<int>					m_blobSizes;
	

};


#endif _POINTCLOUD_H_