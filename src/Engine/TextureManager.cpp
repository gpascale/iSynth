/*
 *  TextureManager.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/8/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "TextureManager.h"
#include <SceneGraph.h>
#include <PSSGManager.h>
#include <Quad.h>
#include <ContextManager.h>
#ifdef IPHONE
	#include <MacDataFetcher.h>
#endif
#include <Texture.h>
#include <GThread.h>
#include <Animator.h>
#include <EngineTypes.h>
#include <SeadragonTile.h>
#include <PSGlobalState.h>
#include <JsonImage.h>
#include <map>
#include <GThread.h>
#include <PSSettings.h>
#include <SynthUtils.h>

#include <Popup.h>

const int START_TEXTURE_CACHE_SIZE = 12000000;
const int START_IMAGE_CACHE_SIZE = 6000000;


short getLevelFromURL(const string &s)
{	
	int n = s.find("0_0.jpg");
	return s[n-2] - '0';
}

short getLevelFromURL(const URL &url)
{
	return getLevelFromURL(url.getString());
}





TextureManager::TextureManager(PSSGManager* sgManager) :
	m_scenegraphManager(sgManager),
	m_allocatedTextures(10000, 0),
	m_needToCleanCache(false),
	m_imageMemoryUsed(0),
	m_textureCacheCapacity(START_TEXTURE_CACHE_SIZE),
	m_imageCacheCapacity(START_IMAGE_CACHE_SIZE)
{	
}



TextureManager::~TextureManager()
{
	
}


void
TextureManager::cleanup()
{

	// clean up some other stuff
	while(!m_sdDownloads.empty()){
		m_sdDownloads.front().second.release(DESTRUCT_FREE);
		m_sdDownloads.pop();
	}
	
	
	// free all textures
	FOR(i,0,m_textureMap2.size())
		FOR(j,0,m_textureMap2[i].size())
			deleteTexture(i, j);
		
	
	// free all images
	FOR(i,0,m_imageMap2.size())
		FOR(j,0,m_imageMap2[i].size())
			if(m_imageMap2[i][j].data() != NULL)
				deleteImage(i, j);
			
	

	m_texturizeQueue.clear();
	m_imageMap2.clear();
	m_textureMap2.clear();
	
	//cerr << "after cleanup, m_imageMemoryUsed = " << m_imageMemoryUsed << endl;
	//cerr << "after cleanup, textureMemoryUsed = " << TextureMemory::getTotalMemoryUsed() << endl;
	
	TextureMemory::printActiveTexIDs();
	
	
	ASSERT(m_imageMemoryUsed == 0);

}


void
TextureManager::initialize(int numImages, int numLevels)
{
	m_textureMap2.clear();
	m_textureMap2.resize(numLevels + 1, vector<Texture*>(numImages, NULL));
	m_imageMap2.clear();
	m_imageMap2.resize(numLevels + 1, vector<ByteArray>(numImages, ByteArray(NULL, 0)));
}


void
TextureManager::run()
{    			
    [EAGLContext setCurrentContext: ContextManager::getTextureContext()];
    while(1){				
		
		// first, check to see if we've been cancelled. If so, clean up and exit
		if(GThread::isCurThreadCancelled()){			
			break;
		}
		
		// check to see if the texture thread is paused. This is different from the sleep below in
		// that its a more long term affair, used when the app needs to go into a low power state
		// Note we don't need a mutex since the texture thread never writes to m_paused
		if(m_paused){
			GThread::sleep(1.0f);
			continue;
		}
						
		// create an autorelease pool to deal with temporary objects
		NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		
		// check to see if we need to clean out the cache
		if(TextureMemory::getTotalMemoryUsed() > m_textureCacheCapacity || 
		   m_imageMemoryUsed > m_imageCacheCapacity ||
		   m_needToCleanCache)
		{
			_cleanCache();
		}
		
		// check to see if we should be sleeping. This happens to keep up the framerate during image transitions
		m_sleepMutex.lock();
		if(m_wakeupTime > GTime::Now()){
			m_sleepMutex.unlock();
			sleep(0.05f);
			[pool release];
			continue;
		}
		m_sleepMutex.unlock();
		
		// finally, create a texture out of an image or seadragon tile if there is work to be done		
        if(!texturizeOneImage() && !texturizeOneSDTile())
            GThread::sleep(0.1f);
		 
		// release the pool
		[pool release];
		
    }
	
	// Texture thread is done running. Clean up and release the context
	cleanup();
	ContextManager::releaseCurrentContext();
}


void
TextureManager::sleepUntilTime(const GTime& time)
{
	m_sleepMutex.lock();
	m_wakeupTime = time;
	m_sleepMutex.unlock();
}


void
TextureManager::sleepForDelay(float numSeconds)
{
	m_sleepMutex.lock();
	m_wakeupTime = GTime::Now() + numSeconds;
	m_sleepMutex.unlock();
}


float
TextureManager::scaleTextureCacheCapacity(float factor)
{
	m_textureCacheCapacity = max((int) (m_textureCacheCapacity * factor), 8000000);
	return m_textureCacheCapacity;								 
}


float
TextureManager::scaleImageCacheCapacity(float factor)
{
	m_imageCacheCapacity = max((int) (m_textureCacheCapacity * factor), 4000000);
	return m_imageCacheCapacity;
}


bool
TextureManager::texturizeOneImage()
{		
	// pop the next image to texturize off the queue (this is synchronized since it is possible to 
	// manipulate the queue from another thread)
	m_texturizeQueueMutex.lock();	
	if (m_texturizeQueue.empty()){
		m_texturizeQueueMutex.unlock();
		return false;
	}    		
	const pair<int, int> imgID = m_texturizeQueue.front();
	m_texturizeQueue.erase(m_texturizeQueue.begin());
	m_texturizeQueueMutex.unlock();
	
	const int level		   = imgID.first;
	const int imgIndex     = imgID.second;
	const ByteArray& bytes = m_imageMap2[level][imgIndex];
	
	int htc = highestTextureCached2(imgIndex);
	
	if(level > htc){		
		const GTime start = GTime::Now();
		TexCreateArgs args(PSSettings::Settings().getColorDepth() == 16 ? TEX_FORMAT_RGB_565 : TEX_FORMAT_RGBA_8888);
        Texture* newTex = Texture::createTexture(bytes, IMG_FMT_JPEG, args);		
		const float time = GTime::Now() - start;
		if(newTex){
			////cerr << "took " << time << " seconds to create a texture of size " << newTex->getWidth() << " x " << newTex->getHeight() << endl;
			////cerr << "created texture " << imgIndex << ", " << level << " with GL id " << newTex->getGLTexture() << endl;
			m_textureMapMutex.lock();
			m_textureMap2[level][imgIndex] = newTex;
			m_textureMapMutex.unlock();
			vector<TextureAnimator*> texAnims;
			m_scenegraphManager->getTextureAnimatorsForImageIndex(imgIndex, texAnims);
			const int nTexAnims = (const int) texAnims.size();
			FOR(i,0,nTexAnims)
				texAnims[i]->setNewTexture(newTex, 0.5f);
		}
	}					

    
	return true;
}
	

bool
TextureManager::texturizeOneSDTile()
{
	if(m_sdDownloads.empty())
		return false;
	
	const SeadragonTile* tile = m_sdDownloads.front().first;
	      ByteArray data      = m_sdDownloads.front().second;
	m_sdDownloads.pop();
	
	const int level   = tile->getLevel();
	const int nImages = tile->getNumImages();
	ASSERT(nImages > 0);
	const int n = 1 << (8 - level);
	const int imgSz = tile->getSize() / n;
	ASSERT(tile->getSize() % n == 0);
	const int startIdx = tile->getIndex() * SQR(n);
						
	vector<GRect> rects;
	
	FOR(i,0,nImages){
		int x, y;
		float w, h;
		const SeadragonImage& img = tile->getImage(i);
		tile->getImageLocation(i, x, y);
		
		// getImageLocation returns a 2D index, so convert that to an actual position
		x *= imgSz;	
		y *= imgSz;
		
		// chop the dimensions in half until both fit in the space of an image in the tile
		w = img.width;
		h = img.height;
		
		while(w > imgSz || h > imgSz){
			w *= 0.5f;
			h *= 0.5f;
		}
		
		rects.push_back(GRect(x, y, w, h));
	}
	
	Texture** textures;
	
	if(PSSettings::Settings().getColorDepth() == 16){
		textures = Texture::createTextureAtlas(data, tile->getSize(), tile->getSize(),
											   IMG_FMT_JPEG, TexCreateArgs(), rects);
	}
	else{
		textures = Texture::createTextureAtlas(data, tile->getSize(), tile->getSize(), 
											   IMG_FMT_JPEG, TexCreateArgs(TEX_FORMAT_RGBA_8888), rects);
	}
		
	FOR(i,0,nImages){
		if(textures[i]){
			const int imgIndex = startIdx + i;
								
			int htc = highestTextureCached2(imgIndex);
			if(level > htc){				
				vector<TextureAnimator*> texAnims;
				m_scenegraphManager->getTextureAnimatorsForImageIndex(imgIndex, texAnims);
				const int nTexAnims = (const int) texAnims.size();
				FOR(j,0,nTexAnims)
					texAnims[j]->setNewTexture(textures[i]);
			}
			
			m_textureMapMutex.lock();
			m_textureMap2[level][imgIndex] = textures[i];
			m_textureMapMutex.unlock();
		
			deleteAllTexturesBelowLevel(level, imgIndex);
		}
	}
		
	// free the image data now that we've made a texture out of it
	data.release(DESTRUCT_FREE);
	
	return true;
}
	
	

// done
void
TextureManager::downloadedImage(const pair<int, int>& imageID, ByteArray data)
{
	ASSERT(m_imageMap2[imageID.second][imageID.first].data() == NULL);
	m_imageMap2[imageID.second][imageID.first] = data;
	m_imageMemoryUsed += data.length();
	
	//ImageCounter::inc();
	////cerr << "ImageCounter: " << ImageCounter::getCount() << endl;
	
	m_texturizeQueueMutex.lock();
	m_texturizeQueue.push_back(make_pair(imageID.second, imageID.first));
	m_texturizeQueueMutex.unlock();
}		


void
TextureManager::downloadedSeadragonTile(const SeadragonTile& tile, ByteArray data)
{	
	m_sdDownloads.push(make_pair(&tile, data));
}


int
TextureManager::highestTextureCached2(int imageIndex, int maxLevel, OUT Texture** ret)
{
	RFOR(i,maxLevel,-1){
		if(m_textureMap2[i][imageIndex] != NULL){
			if(ret != NULL)
				*ret = m_textureMap2[i][imageIndex];
			return i;
		}
	}
	if(ret != NULL)
		*ret = NULL;
	return -1;
}


int
TextureManager::highestImageCached2(int imageIndex, int maxLevel, OUT ByteArray* ret)
{
	RFOR(i,maxLevel,-1){
		if(m_imageMap2[i][imageIndex].data() != NULL){
			if(ret != NULL)
				*ret = m_imageMap2[i][imageIndex];
			return i;
		}
	}
	if(ret != NULL)
		*ret = NULL;
	return -1;
}


// done
bool
TextureManager::deleteImage(int level, int imageIndex)
{	
	m_imageMapMutex.lock();
	ByteArray toDel = m_imageMap2[level][imageIndex];
	if(toDel.data() == NULL){
		m_imageMapMutex.unlock();
		return false;
	}
	m_imageMap2[level][imageIndex] = NULL;
	m_imageMemoryUsed -= toDel.length();	
	m_imageMapMutex.unlock();
	toDel.release(DESTRUCT_FREE);
	
	//ImageCounter::dec();
	////cerr << "ImageCounter: " << ImageCounter::getCount() << endl;
	
	return true;
}


// done
bool
TextureManager::deleteTexture(int level, int imageIndex)	
{
		
	m_textureMapMutex.lock();
	Texture* toDel = m_textureMap2[level][imageIndex];
	if(toDel == NULL){
		m_textureMapMutex.unlock();
		return false;
	}
	m_textureMap2[level][imageIndex] = NULL;	
	
	m_textureMapMutex.unlock();
	
	
	// make sure no animators are still trying to use the deleted texture. Set any such animators
	// to use the next highest texture		
	Texture* nhtc;
	highestTextureCached2(imageIndex, level - 1, &nhtc);
	
	vector<TextureAnimator*> texAnims;
	m_scenegraphManager->getTextureAnimatorsForImageIndex(imageIndex, texAnims);
	FOR(i,0,texAnims.size())
		if(texAnims[i]->getCurrentTexture() == toDel)
			texAnims[i]->setNewTexture(nhtc);
	
	SAFE_DELETE(toDel);
	
}


void
TextureManager::cleanCache()
{	
	m_needToCleanCache = true; //signify to the texture thread that it should clean the cache ASAP
}


void
TextureManager::_cleanCache()
{	
	const JsonProjector* curProjector = PSGlobalState::getCurProjector();
	
	vector<int> imagesInOrder;
	SynthUtils::orderImagesByDistanceFromProjector(curProjector, imagesInOrder);	
	const int nImages = (const int) imagesInOrder.size();
	
/*  DEBUG STUFF	
 
	FOR(i,0,nImages)
		//cerr << "Texture 9, " << i << " = " << m_textureMap2[9][i] << endl;
	FOR(i,0,nImages)
		hit[imagesInOrder[i]] = 1;
	FOR(i,0,nImages)
		//cerr << "imagesInOrder[" << i << "] = " << imagesInOrder[i] << endl;
	FOR(i,0,nImages)
		//cerr << "hit[" << i << "] = " << hit[i] << endl;
*/		

	
	// determine which images are currently visible so we can avoid deleting those textures
	set<int> indicesToKeep;
	indicesToKeep.insert(curProjector->getImageIndex());	
	const vector<const JsonProjector*> peers = curProjector->getPeers();
	FOR(i,0,peers.size())
		indicesToKeep.insert(peers[i]->getImageIndex());
	
	
	// first, make a pass over the images freeing all the level 9 textures we can		
	FOR(i,0,nImages){			
		const int imgIndex = imagesInOrder[i];
		if(m_textureMap2[9][imgIndex] && indicesToKeep.count(imgIndex) == 0){			
			////cerr << "Deleting texture " << 9 << ", " << imgIndex << endl;
			deleteTexture(9, imgIndex);
		}		
	}
	
	// if the texture cache is still half full or more, free all the level 8 images
	if(TextureMemory::getTotalMemoryUsed() > 0.5f * m_textureCacheCapacity){
		FOR(i,0,nImages){			
			const int imgIndex = imagesInOrder[i];
			if(m_textureMap2[8][imgIndex] && indicesToKeep.count(imgIndex) == 0){
				////cerr << "Deleting texture " << 8 << ", " << imgIndex << endl;
				deleteTexture(8, imgIndex);
			}		
		}
	}		
	
	// free level 9 images until the image cache is less than half full
	for(int i = 0; i < nImages && m_imageMemoryUsed > 0.5f * m_imageCacheCapacity; i++){
		const int imgIndex = imagesInOrder[i];
		if(m_imageMap2[9][imgIndex].data() != NULL){
			//cerr << "Deleting image " << 9 << ", " << imgIndex << endl;
			deleteImage(9, imgIndex);
		}
	}
	
	// if it's still more than half full, continue freeing level 8 images
	for(int i = 0; i < nImages && m_imageMemoryUsed > 0.5f * m_imageCacheCapacity; i++){
		const int imgIndex = imagesInOrder[i];
		if(m_imageMap2[8][imgIndex].data() != NULL){
			//cerr << "Deleting image " << 8 << ", " << imgIndex << endl;
			deleteImage(8, imgIndex);					
		}
	}
	
	
#ifdef DEBUG
	/*
	ostringstream oss;
	oss << "T Cap: " << m_textureCacheCapacity << endl;
	oss << "I Cap: " << m_imageCacheCapacity << endl;
	oss << "T Used: " << TextureMemory::getTotalMemoryUsed() << endl;
	oss << "I Used: " << m_imageMemoryUsed << endl;
	Popup* p = Popup::OneButtonPopup("Cleaned the cache", oss.str(), Functor::Identity(), "Ok");
	p->show();		
	//cerr << m_imageMemoryUsed << " bytes of image memory being used" << endl;
	//cerr << TextureMemory::getTotalMemoryUsed() << " bytes of texture memory being used" << endl;
	 */
#endif
	
	m_needToCleanCache = false;
	
}


void
TextureManager::addTexturizeRequest(int imageIndex, int level)
{
	m_texturizeQueueMutex.lock();	// synchronize since this is probably being called from the main thread
	m_texturizeQueue.push_back(make_pair(level, imageIndex));
	m_texturizeQueueMutex.unlock();
}


void
TextureManager::clearTexturizeQueue()
{
	m_texturizeQueueMutex.lock();	// synchronize since this is probably being called from the main thread
	m_texturizeQueue.clear();
	m_texturizeQueueMutex.unlock();
}


void
TextureManager::deleteAllTexturesBelowLevel(int level, int imageIndex)
{
	RFOR(i,level-1,-1){
		deleteTexture(i, imageIndex);
	}
}
		

void
TextureManager::deleteAllTexturesAboveLevel(int level, int imageIndex)
{
	FOR(i,level+1,m_textureMap2.size()){
		deleteTexture(i, imageIndex);
	}
}
	

void
TextureManager::deleteAllImagesBelowLevel(int level, int imageIndex)
{
	RFOR(i,level-1,-1){
		deleteImage(i, imageIndex);
	}
}


void
TextureManager::deleteAllImagesAboveLevel(int level, int imageIndex)
{
	FOR(i,level+1,m_imageMap2.size()){
		deleteImage(i, imageIndex);
	}
}