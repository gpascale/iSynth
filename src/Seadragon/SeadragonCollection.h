/*
 *  SeadragonCollection.h
 *  Phonosynth
 *
 *  Created by Greg on 12/29/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _SEADRAGONCOLLECTION_H_
#define _SEADRAGONCOLLECTION_H_

#include "SeadragonTile.h"
#include <JsonImage.h>
#include <common.h>
#include <vector>
		

class SeadragonCollection
{
public:

	// constructor which takes in a URL to a seadragon collection file. Does a synchronous download
    SeadragonCollection(const string& seadragonRootURL,
                        const map<int, JsonImage*>& images);
	
	// constructor which takes the raw bytes of a seadragon collection file. Preferable to the one above
	// since it doesn't block while waiting for a synchronous download to complete
	SeadragonCollection(const ByteArray& seadragonData,
						const string& seadragonRootURL,
						const map<int, JsonImage*>& images);
    
    ~SeadragonCollection() { }
    
    bool    getTiles(int level, OUT vector<SeadragonTile>& tiles);
	
	static void test();
    
private:

    static void getTileLocation(int tileIndex, OUT int& xIndex, OUT int& yIndex);
    static void getTileLocationHelper(int atDepth, int cur, OUT int& x, OUT int& y);
    
	vector<SeadragonImage>	m_sdImages;
    map<int, JsonImage*>    m_images;
    string                  m_seadragonRootURL;
	
	// parameters read from Seadragon file
	int						m_tileSize;
	int						m_maxLevel;

};

#endif // _SEADRAGONCOLLECTION_H_