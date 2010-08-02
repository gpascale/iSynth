/*
 *  SeadragonCollection.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/29/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "SeadragonCollection.h"
#include <sstream>
#include <PSWeb.h>
#include <LightweightXML.h>

SeadragonCollection::SeadragonCollection(const string& seadragonRootURL,
                                         const map<int, JsonImage*>& images) :
    m_seadragonRootURL(seadragonRootURL),
    m_images(images)
{
	ByteArray data;
	const string dzcURL = seadragonRootURL.substr(0, seadragonRootURL.length() - 6) + ".dzc";
	PSWeb::synchronousGET(dzcURL, data);
	
	XML::Parser parser;
	parser.setXMLData(string((const char*)(const unsigned char*) data, data.length()));
	XML::Parsed parsedDZC;
	
	try{
		// parse the XML Seadragon Collection description file
		if(!parser.parse(parsedDZC))			
			SEADRAGONTHROW("XML parse error in Seadragon Collection file (unknown error)");	
		
		// read the max level and tile size (tile size should always be 256 unless Seadragon changes)
		vector<const XML::Node*> collectionNodes;
		parsedDZC.getAllNodesWithName("Collection", collectionNodes);
		if(collectionNodes.size() != 1)
			SEADRAGONTHROW("XML parse error in Seadragon Collection file (couldn't find Collection Node)");				
		if(!collectionNodes[0]->getAttribute("MaxLevel", m_maxLevel))
		    SEADRAGONTHROW("XML parse error in Seadragon Collection file (couldn't read max. level)");
		if(!collectionNodes[0]->getAttribute("TileSize", m_tileSize))
			SEADRAGONTHROW("XML parse error in Seadragon Collection file (couldn't read tile size)");		
		
	    // read the sizes of the individual images
		vector<const XML::Node*> imageNodes;
		parsedDZC.getAllNodesWithName("Size", imageNodes);
		const int nImages = (const int) imageNodes.size();
		FOR(i,0,nImages){
			int w, h;
			if(!imageNodes[i]->getAttribute("Width", w))
				SEADRAGONTHROW("XML parse error in Seadragon Collection file (couldn't read image width)");
			if(!imageNodes[i]->getAttribute("Height", h))
				SEADRAGONTHROW("XML parse error in Seadragon Collection file (couldn't read image height)");
			m_sdImages.push_back(SeadragonImage(w, h));
		}		
	}	
	catch(XMLException& e){
		data.release(DESTRUCT_DELETE_ARRAY);
		SEADRAGONTHROW("XML parse error in Seadragon Collection file (unknown error)");
	}
	
	data.release(DESTRUCT_DELETE_ARRAY);
}


SeadragonCollection::SeadragonCollection(const ByteArray& seadragonData,
										 const string& seadragonRootURL,
										 const map<int, JsonImage*>& images) :
	m_images(images),
	m_seadragonRootURL(seadragonRootURL)
{
	
	XML::Parser parser;
	parser.setXMLData(string((const char*)(const unsigned char*) seadragonData, seadragonData.length()));
	XML::Parsed parsedDZC;	
		
	try{
		// parse the XML Seadragon Collection description file
		if(!parser.parse(parsedDZC))			
			SEADRAGONTHROW("XML parse error in Seadragon Collection file (unknown error)");
		
		// read the max level and tile size (tile size should always be 256 unless Seadragon changes)
		vector<const XML::Node*> collectionNodes;
		parsedDZC.getAllNodesWithName("Collection", collectionNodes);
		if(collectionNodes.size() != 1)
			SEADRAGONTHROW("XML parse error in Seadragon Collection file (couldn't find Collection Node)");				
		if(!collectionNodes[0]->getAttribute("MaxLevel", m_maxLevel))
			SEADRAGONTHROW("XML parse error in Seadragon Collection file (couldn't read max. level)");
		if(!collectionNodes[0]->getAttribute("TileSize", m_tileSize))
			SEADRAGONTHROW("XML parse error in Seadragon Collection file (couldn't read tile size)");
		   
		// read the sizes of the individual images		
		vector<const XML::Node*> imageNodes;
		parsedDZC.getAllNodesWithName("Size", imageNodes);
		const int nImages = (const int) imageNodes.size();
		FOR(i,0,nImages){
			int w, h;		
			if(!imageNodes[i]->getAttribute("Width", w))
				SEADRAGONTHROW("XML parse error in Seadragon Collection file (couldn't read image width)");
			if(!imageNodes[i]->getAttribute("Height", h))
				SEADRAGONTHROW("XML parse error in Seadragon Collection file (couldn't read image height)");
			m_sdImages.push_back(SeadragonImage(w, h));
		}						   		   		   
	}
	catch(XMLException& e){
		SEADRAGONTHROW("XML parse error in Seadragon Collection file");
	}
}


bool
SeadragonCollection::getTiles(int level, OUT vector<SeadragonTile>& tiles)
{
    const int nImages = (const int) m_images.size();
    const int nImagesPerTile = 1 << (2 * (8 - level));
	const int nImagesOnLastTile = (nImages % nImagesPerTile == 0) ? (nImagesPerTile) : (nImages % nImagesPerTile);
    const int nTiles = ceil((nImages + 0.0f) / nImagesPerTile);    

    int x, y;     
    
    FOR(i,0,nTiles){
        getTileLocation(i, x, y);
        ostringstream oss;
        oss << m_seadragonRootURL << "/" << level << "/" << x << "_" << y << ".jpg";
        
        SeadragonTile tile(i,           // index
						   level,		// level
                           oss.str(),   // url
                           m_tileSize,  // assume 256x256 tiles for now TODO: read this from the .dzc file instead
                           (i == (nTiles - 1)) ? (nImagesOnLastTile) : (nImagesPerTile));
		
        tile.setImages(m_sdImages.begin() + i * nImagesPerTile,
					   (i == nTiles - 1) ? 
					   (m_sdImages.end()) : 
					   (m_sdImages.begin() + ((i+1) * nImagesPerTile)));
		
        tiles.push_back(tile);      
    }
    
    return true;
}


void
SeadragonCollection::getTileLocation(int tileIndex, OUT int& xIndex, OUT int& yIndex)
{
    xIndex = yIndex = 0;
    getTileLocationHelper(8, tileIndex, xIndex, yIndex);
}


void
SeadragonCollection::getTileLocationHelper(int atDepth, int cur, OUT int& x, OUT int& y)
{
    if(atDepth < 0)
        return;
    
	// as we move one level deeper, we double the number of tiles in each direction
	// so x and y must both be multiplied by 2
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
    
    getTileLocationHelper(atDepth - 1, cur % p, x, y);
        
}


/* Old
 
void
SeadragonCollection::test()
{
	int x, y;
	FOR(level,0,10){
		FOR(i,0,10){
			const int index = randInt(0, 100);
			getTileLocation(index, x, y);
			//cout << "Level " << level << ", Index " << index << " maps to (" << x << ", " << y << ")" << endl;
		}
	}
}
*/