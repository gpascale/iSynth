/*
 *  Pointcloud.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Pointcloud.h"
#include <GMath.h>
#include <Drawer.h>
#include <BinaryDataReader.h>
#include <PSTypes.h>

#include <PSGlobalState.h>

int	 randomPermutation[5000];
bool randomPermutationInitialized = 0;


Pointcloud::Pointcloud() :
	m_numPoints(0),
	m_pointPositions(NULL),
	m_pointColors(NULL)
{
	if(!randomPermutationInitialized)
		FOR(i,0,5000) randomPermutation[i] = i;
	randomPermutationInitialized = 1;
    setDrawPriority(PRIORITY_PC);
}



Pointcloud::~Pointcloud()
{
	FOR(i,0,m_pointPositions.size())
	SAFE_DELETE_ARRAY(m_pointPositions[i]);
	FOR(i,0,m_pointColors.size())
	SAFE_DELETE_ARRAY(m_pointColors[i]);
	
	m_pointPositions.clear();
	m_pointColors.clear();
}


int
Pointcloud::getSize() const
{
	int n = 0;
	FOR(i,0,m_pointPositions.size()){
		n += m_blobSizes[i] * 16;
	}
	return n;
}


void
Pointcloud::setVertices(const Vertex::XYZ* positions, 
						const Color4UC* colors,
						int numVerts)
{
	m_numPoints = numVerts;
	
	FOR(i,0,m_pointPositions.size())
	SAFE_DELETE_ARRAY(m_pointPositions[i]);
	FOR(i,0,m_pointColors.size())
	SAFE_DELETE_ARRAY(m_pointColors[i]);
	
	m_pointPositions.clear();
	m_pointColors.clear();
	
	m_pointPositions.push_back(new Vertex::XYZ[numVerts]);
	m_pointColors.push_back(new Color4UC[numVerts]);
	
	memcpy(m_pointPositions.back(), positions, numVerts * sizeof(Vertex::XYZ));
	memcpy(m_pointColors.back(), colors, numVerts * sizeof(Color4UC));
}



void
Pointcloud::addVerticesFromBinaryData(ByteArray data, float factor)
{
	ENGINE_DEBUG(printf("addVerticesFromBinaryData\n");
				 printf("factor = %f\n", factor));

	BinaryDataReader bdr;
	bdr.setDataToRead(data);
	
	short major      = 0;
	short minor		 = 0;
	int	  numCameras = 0;
	int	  numRanges  = 0;
	int   numPoints  = 0;
	
	bdr.readUncompressedShort(major);
	bdr.readUncompressedShort(minor);	
	bdr.readCompressedInt(numCameras);	
	BINARY_PRINTF("major: %d\n", major);
	BINARY_PRINTF("minor: %d\n", minor);		
	BINARY_PRINTF("numCameras: %d\n", numCameras);
	
	FOR(i,0,numCameras){
		bdr.readCompressedInt(numRanges); // dont do anything with these
		FOR(j,0,numRanges){
			int rangeX, rangeY;
			bdr.readCompressedInt(rangeX);
			bdr.readCompressedInt(rangeY);
		}
	}
	
	bdr.readCompressedInt(numPoints);
	BINARY_PRINTF("numPoints: %d\n", numPoints);
	m_numPoints += numPoints;
	
	Vertex::XYZ*	positions = new Vertex::XYZ[numPoints];
    Color4UC*       colors	  = new Color4UC[numPoints];
	
	//sort(randomPermutation, randomPermutation + 5000);
	//random_shuffle(randomPermutation, randomPermutation + numPoints);
	
	int *idx = randomPermutation;
	
	FOR(i,0,numPoints * factor){		
		
		// read the data
		float x, y, z;
		short c;
		bdr.readUncompressedFloat(x);
		bdr.readUncompressedFloat(y);
		bdr.readUncompressedFloat(z);
		bdr.readUncompressedShort(c);
		
		//BINARY_PRINTF("Point %d: x y z c = %f %f %f %d\n", i, x, y, z, c);
		
		// assign the data
		positions[*idx] = Vector3(x, y, z);		
		colorFromShort(colors[*idx], c);
		idx++;		
		
		
	}		
		
	m_pointPositions.push_back(positions);
	m_pointColors.push_back(colors);
	m_blobSizes.push_back(numPoints);	
	
	data.release(DESTRUCT_FREE);
}


void
Pointcloud::readVertex(const void *bytes, Vertex::XYZ &pos, Color4UC &color)
{
	// we assume that the first 12 bytes represent the position as 3 floats
	// and the last 2 bytes represnt the color in 5_6_5 format
	
	// copy the position
	pos = Vector3((float*) bytes);	
	
	// extract and set the color;
	short c  = (short) *((unsigned char*) bytes + 12);
	colorFromShort(color, c);
}


void
Pointcloud::colorFromShort(Color4UC &ret, short c)
{
	static const short mask5 = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);
	static const short mask6 = mask5 | (1 << 5);
	
	ret[0] = (c & 0xF800) >> 8;
	ret[1] = (c & 0x7E0)  >> 3;
	ret[2] = (c & 0x1F)   << 3;
	ret[3] = 255;	
}


void
Pointcloud::draw(Drawer& d)
{    
    // assume state has already been configured by group node
    
    const float pctToDraw = PSGlobalState::getPointcloudDensity();	
	const int numFiles = ceil(m_pointPositions.size() * pctToDraw);	
	
	FOR(i,0,numFiles){        
        d.setPositionSource(m_pointPositions[i], FMT_FLOAT3, 0);
        d.setColorSource(m_pointColors[i], FMT_UCHAR4, 0);
        d.draw(m_blobSizes[i], NULL);
	}					
}
				   
				   
				   

