/*
 *  SeadragonTile.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/29/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */


#include "SeadragonTile.h"


SeadragonTile::SeadragonTile(int idx, int level, const string& url, 
							 int size, int nImages) :
	m_indexInCollection(idx),
	m_level(level),
	m_jpegURL(url),
	m_size(size),
	m_nImages(nImages)
{

}


SeadragonTile::~SeadragonTile()
{
	
}


int
SeadragonTile::getImageIndex(int x, int y) const
{
	return getImageIndexHelper(8, x, y);
}


int
SeadragonTile::getImageIndexHelper(int level, int x, int y) const
{
	if(level < 0)
		return 0;
	
	const int n = 1 << level;
	
	int ret = 0;
	if(x >= n){
		ret += SQR(n);
		x -= n;
	}
	if(y >= n){
		ret += 2 * SQR(n);
		y -= n;
	}
	return ret + getImageIndexHelper(level - 1, x, y);
}



// the same algorithm as getTileLocation in SeadragonCollection

void
SeadragonTile::getImageLocation(int imageIndexInTile, OUT int& x, OUT int& y)
{
	x = y = 0;
    getImageLocationHelper(8, imageIndexInTile, x, y);
}


void
SeadragonTile::getImageLocationHelper(int atDepth, int cur, OUT int& x, OUT int& y)
{
    if(atDepth < 0)
        return;
    
	// as we move one level deeper, we double the number of tiles in each direction
	x <<= 1;
	y <<= 1;
	
    const int p = 1 << (atDepth * 2);
    const int q = cur / p; 
    
    switch(q){
        case 1:		// upper-right quadrant, increase x
            x++;
            break;
        case 2:		// lower-left quadrant, increase y
            y++;
            break;
        case 3:		// lower-right quadrant, increase both x and y
            x++;
            y++;
            break;                      
    }
    
    getImageLocationHelper(atDepth - 1, cur % p, x, y);
	
}



