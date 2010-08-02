/*
 *  SeadragonTile.h
 *  Phonosynth
 *
 *  Created by Greg on 12/29/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _SEADRAGONTILE_H_
#define _SEADRAGONTILE_H_

#include <common.h>
#include <string>
#include <vector>

using namespace std;

class SeadragonException : public Exception
{
public:
	SeadragonException(const string& why = "Unknown SeadragonException",
					   int line = -1) : 
	Exception(why, line)
	{
		//cerr << "throwing SeadragonException at line " << line << ": " << why << endl;
	}
	
	SeadragonException(int line) : Exception(line)
	{
		//cerr << "throwing SeadragonException at line " << line << endl;
	}
	
};

#define SEADRAGONTHROW(why) throw SeadragonException(why, __LINE__)

// SeadragonImage models an image within a Seadragon tile. For now it only contains width and height
struct SeadragonImage
{
	int width;
	int height;	
	SeadragonImage(int w, int h) : width(w), height(h) { }
};

class SeadragonTile
{
public:

    SeadragonTile(int idx, int level, const string& url, 
                  int size, int nImages);
	
	~SeadragonTile();
    
    const string& getJpegURL()   const { return m_jpegURL;			 }
	int			  getSize()      const { return m_size;				 }
	int			  getNumImages() const { return m_nImages;			 }
	int			  getIndex()     const { return m_indexInCollection; }
	int			  getLevel()	 const { return m_level;		     }
	
	void		  setImages(const vector<SeadragonImage>& v) { m_images = v; }
	
	void		  setImages(const vector<SeadragonImage>::const_iterator beg,
							const vector<SeadragonImage>::const_iterator end){
		m_images.assign(beg, end);
	}
	
	const SeadragonImage& getImage(int index) const { return m_images[index]; }
	
	// retrieves the one-dimensional index of an image in the tile given its x,y index
	int getImageIndex(int x, int y)					 const;
	int getImageIndexHelper(int level, int x, int y) const;
	
	// retrieves the x,y index (not position) of an image in the tile given its one-dimensional index
	// (this is exactly the same as getTileLocation in SeadragonCollection)
	static void getImageLocation(int tileIndex, OUT int& x, OUT int& y);
    static void getImageLocationHelper(int atDepth, int cur, OUT int& x, OUT int& y);

private:
        
	vector<SeadragonImage>	m_images;
	
	int		m_level;
    int     m_nImages;
    int     m_indexInCollection;
    int     m_size;        
    string  m_jpegURL;       // where to find the JPEG for this tile
    
};

#endif // _SEADRAGONTILE_H_