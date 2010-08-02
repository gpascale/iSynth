/*
 *  TextureManager.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/8/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_


#include <common.h>
#include <Quad.h>
#include <string>
#include <map>
#include <ContinuousDownloader.h>
#include <queue>
#include <vector>
#include <GMutex.h>
#include <GTime.h>
#include <Texture.h>

#include <GregJsonCollection.h>

#ifdef IPHONE
	@class NSURL;
	#import <QuartzCore/QuartzCore.h>
#endif


using namespace std;

// A simple struct that helps us keep track of an image that has been downloaded but not handled yet.
struct DownloadedImage
{
    int             imageIndex;
	ByteArray       bytes;
	short			level;
	
	DownloadedImage(){ }
	
	DownloadedImage(const DownloadedImage &d){
        imageIndex = d.imageIndex;
        bytes = d.bytes;
        level = d.level;
	}
};

// Struct for storing cached JPEG data for an image
struct CachedImage
{
    ByteArray       bytes;
    int             level;
    
    CachedImage(){ }
    CachedImage(ByteArray _bytes, int _level) : bytes(_bytes), level(_level) { }
	
	void cleanup() { bytes.release(DESTRUCT_FREE); }
};

class Scene;
class SceneGraph;
class PSSGManager;
class SeadragonTile;
class GThread;
    
// TextureManager encapsulates the work of keeping track of the image and texture data associated
// with all the pictures in a synth. For each image, we store a pointer to its raw JPEG data and
// a current Texture* object which we can use to render it. This task is made a bit challenging by
// the fact that we often wish to change Texture quality for a given image in the synth. When the viewer
// focuses in on an image, we want to use a high quality texture, but when the user navigates away,
// we only hang on to a low quality texture to conserve memory
class TextureManager
{
				
	typedef map<int, Texture*>                 TextureMap;
    typedef map<int, CachedImage>              ImageMap;

	typedef vector<vector<Texture*> >		   TextureMap2;
	typedef vector<vector<ByteArray> >		   ImageMap2;
	
public:

	TextureManager(PSSGManager* sgManager);
	~TextureManager();
	

	void initialize(int nImages, int nLevels);
	
	void		cleanup();
		
				
    void        run();	
    void		pause()			   { m_paused = true;  }
	void		resume()		   { m_paused = false; }
	
	void		sleepUntilTime(const GTime& time);
	void		sleepForDelay(float numSeconds);
    
    // called when an image download has completed. simply pops the downloaded image onto 
    // a queue to be dealt with later, since creating textures is an expensive operation
    void        downloadedImage(const pair<int, int>& imageID, ByteArray data);
	
	void        downloadedSeadragonTile(const SeadragonTile& tile, ByteArray data);
	
	
	
	// returns the level of the highest texture cached in the TextureMap for the given image
	int			highestTextureCached2(int imgIndex, int maxLevel = 9, OUT Texture** ret = NULL);
    
    // returns the level of the highest image (raw JPEG data) cached in the ImageMap for the given image
	int			highestImageCached2(int imgIndex, int maxLevel = 9, OUT ByteArray* ret = NULL);
		
	
    // these routines are the meat of what TextureManager does. They convert raw image data into 
	// OpenGL-usable textures. texturizeOneImage is used on Jpegs which contain a single image and
	// texturizeOneSDTile is used to create a texture atlas from a SeadragonTile
	bool		texturizeOneImage();    
	bool		texturizeOneSDTile();
    
	// these functions allow outside classes to manipulate the texturization queue. Only PSNetwork
	// should use these
	void		clearTexturizeQueue();
	void		addTexturizeRequest(int imgIndex, int level);	
	
	// TODO: Call this on memory warning to free up cached textures and images.
	// Think about how to do this right
	void cleanCache();
    
    //void setImages(const JsonImages& images) { m_images = images; }
	
	int computeMemoryUsedByTextures();
	int computeMemoryUsedByImages();
	int computeMemoryUsedTotal();
	
	float scaleTextureCacheCapacity(float factor);
	float scaleImageCacheCapacity(float factor);
	
private:    		
	
	// attempts to delete the specified texture, fails if it is marked for use by the render thread
	bool deleteImage(int level, int imageIndex);
	// attempts to delete the specified image, should never fail
	bool deleteTexture(int level, int imageIndex);
	void deleteAllTexturesAboveLevel(int level, int imageIndex);
	void deleteAllTexturesBelowLevel(int level, int imageIndex);
	void deleteAllImagesAboveLevel(int level, int imageIndex);
	void deleteAllImagesBelowLevel(int level, int imageIndex);
	

	void _cleanCache();
	
	// for debugging
	vector<int>		m_allocatedTextures;
	
    JsonImages		m_images;
    
    // ideally TextureManager shouldn't have to know about Scene. See comments in handleNextDownload
    // for the reason why it does
    PSSGManager*    m_scenegraphManager;
		
    // when images are downloaded, they aren't texturized immediately, since creating a texture
    // is expensive. Instead, when an image is downloaded, it is pushed onto m_downloadedQueue.
    // Periodically, we pop items off of this queue and create textures or otherwise handle them
	queue<DownloadedImage>  m_downloadedQueue;	
        
	
	TextureMap2				m_textureMap2;
	ImageMap2				m_imageMap2;
	
	vector<pair<int, int> > m_texturizeQueue;
    
	queue<pair<SeadragonTile*, ByteArray> > m_sdDownloads;
	
    EAGLSharegroup*         m_shareGroup;
    EAGLContext*            m_context;
	
	GMutex					m_texturizeQueueMutex;
	GMutex					m_textureMapMutex;
	GMutex					m_imageMapMutex;
	GMutex					m_sleepMutex;
	
	GTime					m_wakeupTime;
		
	bool					m_needToCleanCache;			
	int						m_imageMemoryUsed;
	
	int						m_textureCacheCapacity;
	int						m_imageCacheCapacity;
	
	bool					m_paused;

};



#endif _TEXTUREMANAGER_H_
