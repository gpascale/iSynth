/*
 *  Texture.h
 *  Phonosynth
 *
 *  Created by Greg on 9/15/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

// A nice object oriented texture object. This way, Phonosynth-specific classes can deal with textures
// in a somewhat abstract fashion without having to care about how the underlying graphics are done.
// Since I want texture management at the high level (i.e. deciding when to upgrade/downgrade etc...)
// to be done by the Phonosyth app, this allows me to keep the engine blissfully unaware of Phonosynth's 
// exisitence

#ifndef _TEXTURE_H_
#define _TEXTURE_H_


#include <EngineTypes.h>
#include <Matrix.h>
#include <common.h>
#include <vector>
#include <Color.h>
#include <bitset>
#include <set>
#include <map>

// singleton responsible for monitoring system-wide texture memory usage
class TextureMemory
{	
	
public:	
	
	// should only be called by Texture::createGLTexture()
	static void textureCreated(int glID, int nBytes)
	{
		TextureMemory& tm = get();
		map<int, int>::iterator iter = tm.m_data.find(glID);
		if(iter != tm.m_data.end()){
			//cerr << "tm.m_data.size() = " << tm.m_data.size() << endl;
			//cerr << "glID = " << glID << endl;
//			//cerr << "isTexture = " << glIsTexture(glID) << endl;
			BREAK;
		}
		//cerr << "created ID " << glID << endl;
		//TODO: REPLACE ME: ASSERT(iter == tm.m_data.end());
		tm.m_data[glID] = nBytes;
		tm.m_totalBytes += nBytes;
	}
	
	// should only be called by Texture::~Texture()
	static void textureDeleted(int glID)
	{
		TextureMemory& tm = get();
		map<int, int>::iterator iter = tm.m_data.find(glID);
		if(iter == tm.m_data.end())
			BREAK;
		//cerr << "deleted ID " << glID << endl;
		//TODO: REPLACE ME: ASSERT(iter != tm.m_data.end());
		tm.m_totalBytes -= iter->second;
		tm.m_data.erase(iter);
	}
	
	static void printActiveTexIDs(){
		TextureMemory& tm = get();
		const map<int, int>::const_iterator beg = tm.m_data.begin(), end = tm.m_data.end();
			  map<int, int>::const_iterator iter;
		/*
		for(iter = beg; iter != end; iter++)
			//cerr << "(" << iter->first << ")";
		//cerr << endl;
		for(iter = beg; iter != end; iter++)
			//cerr << "(" << iter->first << ", " << iter->second << ")" << endl;
		 */
	}
	
	static int getTotalMemoryUsed() { return get().m_totalBytes; }	
	
private:	
	
	TextureMemory() : m_totalBytes(0) { }
	
	static TextureMemory& get() { static TextureMemory ret; return ret; }		
		
	// stores the active gl texture ids along with the size of the texture each one points to
	map<int, int>	m_data;
	// stores the total number of bytes used by all textures
	int				m_totalBytes;
	
};


struct TexCreateArgs
{
    TextureFormat   textureFormat;      // the internal texture format to use
    short           width;              // must be a power of 2, or -1 to pick the smallest possible
    short           height;             // must be a power of 2, or -1 to pick the smallest possible
    unsigned char   shrinkFactor;       // how many powers of two are we scaling down?
    TexStretchType  stretchType;        // how to stretch the image if width and height are not specified
    
    TexCreateArgs(TextureFormat fmt = TEX_FORMAT_RGB_565, short w = -1, short h = -1) :
        textureFormat(fmt), width(w), height(h), shrinkFactor(0), stretchType(STRETCH_NONE)
    { }
};


class Texture;

// manages memory shared by multiple textures. Basically serves as a reference counter
struct SharedAtlas
{	
public:
	void addTexture(Texture* t)			 { m_textures.insert(t);			}
	void removeTexture(Texture* t)		 { m_textures.erase(t);			    }
	int  count()				   const { return (int) m_textures.size();  }
private:	
	set<Texture*> m_textures;
};

    
// A "Texture" is a wrapper around an OpenGL or D3D texture. The underlying data is owned by the Texture object
// and therefore we do not allow the copying of Texture objects. The underlying data is deleted when
// the Texture object is deleted.

class Texture
{

public:    
            
    ~Texture();
    
    // create a texture from an image file
    static Texture* createTexture(const string& filename,
                                  const TexCreateArgs& args);
        
    // create a texture from an image    
    static Texture* createTexture(const ByteArray& data,    
                                  const ImageFormat dataFormat, // must be PNG or JPG
                                  const TexCreateArgs& args);
    
    static Texture* createTexture(const ByteArray& data, int width, int height,
                                  const ImageFormat dataFormat,
                                  const TexCreateArgs& args);
    
    // creates several textures from a single JPEG
    static Texture** createTextureAtlas(const ByteArray& data,
                                        const ImageFormat dataFormat,
                                        const TexCreateArgs,
                                        const vector<GRect>& rects);
    
    static Texture** createTextureAtlas(const ByteArray& data, int width, int height,
                                        const ImageFormat dataFormat,
                                        const TexCreateArgs,
                                        const vector<GRect>& rects);
    
	static Texture*	 createTextureFromText(const string& text,
										   int w, int h,
										   const Color4F& textColor,
										   const Color4F& backgroundColor,
										   const string& fontName = "Times New Roman");
										   														
    
    TextureFormat       getTextureFormat() const { return m_textureFormat;  }
    unsigned short      getWidth()         const { return m_width;          }
    unsigned short      getHeight()        const { return m_height;         }
    unsigned short      getSlabWidth()     const { return m_slabWidth;      }
    unsigned short      getSlabHeight()    const { return m_slabHeight;     }
    unsigned short      getLevel()         const { return m_level;          }
    float               getHRatio()        const { return m_hRatio;         }
    float               getVRatio()        const { return m_vRatio;         }
    float               getHOffset()       const { return m_hOffset;        }
    float               getVOffset()       const { return m_vOffset;        }    
	const Matrix&		getMatrix()		   const { return m_matrix;			}
	int					getMemoryUsed()    const { return m_memoryUsed;		}
    
#ifdef IPHONE
	
    unsigned int        getGLTexture()     const { return m_glTexID;        }
#endif //IPHONE
    
    
private:
    
    // Textures are created via the static functions above. Don't want to allow them
    // to be created in any other way or copied
    Texture();    
    Texture(const Texture& rhs);
    
#ifdef IPHONE        
    
    static unsigned int createGLTexture(const ByteArray& data, int width, int height,
                                        const ImageFormat dataFormat,
                                        const TexCreateArgs& args);
    
    // storage for the gl texture identifier    
    unsigned int m_glTexID;    
#endif //IPHONE
    
    // decompresses an image of the format 'fmtIn' to the format 'fmtOut'.
    // fmtIn must bet either IMG_FMT_PNG or IMG_FMT_JPEG
    // For now, fmtOut must be IMG_FMT_RAW_8888RGBA since this is all I use and
    // it isn't worth my time at the moment to handle others
    // width and height of the decompressed image are returned in 'width' and 'height'
    static ByteArray            decompress(const ByteArray& data,
                                           ImageFormat fmtIn, 
                                           ImageFormat fmtOut,
                                           bool pow2Dimensions,
                                           OUT int& widthOut,
                                           OUT int& heightOut);
    
    static ByteArray            decompress(const UIImage* uiImage,
                                           ImageFormat fmtOut,
                                           bool pow2Dimensions,
                                           OUT int& widthOut,
                                           OUT int& heightOut);
	
	static ByteArray            decompress(const CGImageRef cgImage,
                                           ImageFormat fmtOut,
                                           bool pow2Dimensions,
                                           OUT int& widthOut,
                                           OUT int& heightOut);
	
	static ByteArray            fastDecompress(const ByteArray& jpegData,
											   ImageFormat fmtOut,
											   bool pow2Dimensions,
											   OUT int& widthOut,
											   OUT int& heightOut);
    
    unsigned short              m_level;
    unsigned short              m_width;
    unsigned short              m_height;
    unsigned short              m_slabWidth;
    unsigned short              m_slabHeight;
    TextureFormat               m_textureFormat;
    
    SharedAtlas*				m_atlas;
	
    // these parameters describe the orientation of the actual image within the chunk of
    // texture memory. These are used to manipulate the texture matrix
    // For non-atlased textures, m_hOffset and m_vOffset will both be 0
    
    float                       m_hOffset;  // horizontal offset
    float                       m_vOffset;  // vertical offset
    float                       m_hRatio;   // ratio of width of image to chunk width
    float                       m_vRatio;   // ratio of width of image to chunk width	
	
	Matrix						m_matrix;
	
	int							m_memoryUsed;
                                                                    
};

extern int jpeglibtest();


#endif _TEXTURE_H_