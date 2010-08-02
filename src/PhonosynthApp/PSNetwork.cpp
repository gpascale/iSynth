/*
 *  PSNetwork.cpp
 *  Phonosynth
 *
 *  Created by Greg on 10/14/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "PSNetwork.h"
#include "PSSGManager.h"
#include "PSSettings.h"
#include <JsonImage.h>
#include <SeadragonTile.h>
#include <Quad.h>
#include <PSWeb.h>
#include <set>

void
ImageDownloader::downloadSucceeded(const pair<int, int> imgID, ByteArray data)
{   
    m_parent->downloadedImage(imgID, data);    
}


void
ImageDownloader::downloadFailed(const pair<int, int> imgID)
{
    //cout << "image download failed" << endl;
}


void
PointcloudDownloader::downloadSucceeded(const pair<int, int> blobID, ByteArray data)
{
    m_parent->downloadedPointcloudBlob(blobID, data);
}

void
PointcloudDownloader::downloadFailed(const pair<int, int> pointcloudID)
{
    //cout << "pointcloud download failed" << endl;
}


void
SeadragonDownloader::downloadSucceeded(const int tileID, ByteArray data)
{
    m_parent->downloadedSeadragonTile(tileID, data);
}


void
SeadragonDownloader::downloadFailed(const int tileID)
{
    //cout << "Seadragon Tile download failed" << endl;
}


void
BImageDownloader::downloadSucceeded(const pair<int, int> imageID, ByteArray data)
{
	////cerr << "downloaded image " << imageID.first << ", " << imageID.second << endl;
	m_parent->downloadedImage(imageID, data);
}


void
BImageDownloader::downloadFailed(const pair<int, int> imageID)
{
	//cout << "Image " << imageID.first << ", " << imageID.second << " failed" << endl;
}



PSNetwork::PSNetwork(PSSGManager* sgm, TextureManager* tm) :
    m_scenegraphManager(sgm),
	m_textureManager(tm),
    m_imageDownloader(this, 3),
    m_pointcloudDownloader(this, 1),
    m_seadragonDownloader(this, 1),
	m_batchDownloader(this, PSSettings::Settings().getNumThreads())
{
    m_imagePriorities.clear();
}


PSNetwork::~PSNetwork()
{
    
}


void
PSNetwork::cleanup()
{
	// reset the downloaders
	m_imageDownloader.reset();
	m_pointcloudDownloader.reset();
	m_seadragonDownloader.reset();
	
	// clear state
	m_imagePriorities.clear();
	m_images.clear();
	m_seadragonTiles.clear();
}


void
PSNetwork::setPointcloudsToDownload(const vector<pair<int, int> >& indices,
                                    const vector<string>& urls)
{
    ASSERT(indices.size() == urls.size());
    FOR(i,0,indices.size())
        m_pointcloudDownloader.addRequest(indices[i], urls[i]);
}


void
PSNetwork::setImagesToDownload(const JsonImages& jsonImages)
{
    m_images = jsonImages;
	string s;
}


void
PSNetwork::setSeadragonTilesToDownload(const SeadragonTiles& tiles)
{
    m_seadragonTiles = tiles;
    FOR(i,0,tiles.size()){
        m_seadragonDownloader.addRequest(i, tiles[i].getJpegURL(), i);
    }
}

 

void
PSNetwork::downloadedImage(const pair<int, int> imgID, ByteArray data)
{
    ////cout << "downloaded an image!" << endl;
    m_scenegraphManager->downloadedImage(imgID, data);
}


void
PSNetwork::downloadedPointcloudBlob(const pair<int, int> blobID, ByteArray data)
{
    ////cout << "downloaded a pointcloud" << endl;
    m_scenegraphManager->downloadedPointcloudBlob(blobID, data);
}


void
PSNetwork::downloadedSeadragonTile(const int tileID, ByteArray data)
{
    ////cerr << "downloaded a seadragon tile (" << tileID << ")" << endl;
    m_scenegraphManager->downloadedSeadragonTile(m_seadragonTiles[tileID], data);
}


void
PSNetwork::start()
{
    m_pointcloudDownloader.start();
    m_seadragonDownloader.start();
}


// haven't really thought about this. Maybe need to do something else here
void
PSNetwork::stop()
{
	m_pointcloudDownloader.stop();
	m_seadragonDownloader.stop();
	m_batchDownloader.stop();
}


void
PSNetwork::getURLForImageWithLevel(const JsonImage& img, int level, OUT string& ret) const
{			
	////cerr << "getURL called for level " << level << endl;
	
    ret = toJsonString8(img.GetPath());
    ////cerr << "ret is initially " << ret << endl;
    
	ASSERT(ret.length() > 4);
	ASSERT(ret.substr(ret.length() - 4) == ".jpg");
    if(ret[0] == '/')
		ret = ret.substr(1);
    
    if(level == 9)  // if level 9, return the 512 image ret
        return ret;
    
    // otherwise, we need to hack up the ret a bit to get an image from the dzc
    
	int n = ret.find("_files");
	ASSERT(n != -1);
	ret = ret.substr(0, n + 6) + "/" + i2s(level) + "\/0_0.jpg";
	
	ASSERT(ret != "");
}





void
PSNetwork::movedToProjector(const JsonProjector* p)
{
	// stop all downloading and texture creation.
	// Note: It would be more efficient to first make sure the current download/texturization
	// isn't something we actually need and leave it if so, but I think it would be tricky to
	// get right and isn't worth it at the moment
	m_batchDownloader.stop();
	m_textureManager->clearTexturizeQueue();
	
	vector<string> urls;
	vector<pair<int, int> > ids;		
	string s;
	int htc, hic, idx;	
	const JsonImage* cImg;
	pair<int, int> loID, hiID;

	
	// BIG TODO: Prioritization should work completely differently in slideshow mode. We probably want
	// to prioritize imminent images in the slideshow ahead of the current image and no longer care
	// about neighbors or peers.
	
	// remember which image-level pairs we've handled when dealing with the peers and neighbors. There is
	// overlap and we don't want to download the same image or load the same texture twice
	// note: was originally just checking for the same projector, but there were a few cases where there
	// were two projectors, one a peer, the other a neighbor, sharing the same image
	set<pair<int, int> > seen;

	// The main image, our highest priority. We want to get a high resolution image in here
	// as fast as possible before worrying about the neighbors.
	cImg = p->getImage();
	idx = cImg->GetIndex();	
	htc = m_textureManager->highestTextureCached2(idx);
	hic = m_textureManager->highestImageCached2(idx);
	loID = make_pair(idx, 8);
	hiID = make_pair(idx, 9);
	seen.insert(loID);
	seen.insert(hiID);
	if(htc < 9){
		if(hic == 9)			
			m_textureManager->addTexturizeRequest(idx, 9);
		else if(hic == 8){
			if(htc < 8)
				m_textureManager->addTexturizeRequest(idx, 8);
			getURLForImageWithLevel(*cImg, 9, s);
			urls.push_back(s);
			ids.push_back(hiID);
		}
		else{
			getURLForImageWithLevel(*cImg, 8, s);
			urls.push_back(s);
			ids.push_back(loID);
			getURLForImageWithLevel(*cImg, 9, s);			
			urls.push_back(s);
			ids.push_back(hiID);
		}
	}


	// Next, we worry about the main image's peers since they are visible
	const vector<const JsonProjector*>& peers = p->getPeers();
	const int nPeers = min(PSSettings::Settings().getNumPeers(), (const int) peers.size());
	
	FOR(i,0,nPeers){
		
		const JsonProjector* cPeer = peers[i];
		cImg = peers[i]->getImage();
		idx = cImg->GetIndex();
		loID = make_pair(idx, 8);		
		
		if(seen.count(loID) == 0){ // make sure we haven't already handled this projector
			seen.insert(loID);
			htc = m_textureManager->highestTextureCached2(idx);
			if(htc < 8){
				hic = m_textureManager->highestImageCached2(idx);
				if(hic >= 8)
					m_textureManager->addTexturizeRequest(idx, 8);
				else{
					getURLForImageWithLevel(*cImg, 8, s);
					urls.push_back(s);
					ids.push_back(loID);
				}
			}
		}
		
	}
	
	// Finally, we upgrade neighbors of the main image since we are likely to visit them soon
	const JsonProjector * const * neighbors = p->getAllNeighbors();
	FOR(i,0,6){
		
		const JsonProjector* cNeighbor = neighbors[i];
		if(cNeighbor == NULL)
			continue;
		cImg = cNeighbor->getImage();
		idx = cImg->GetIndex();
		loID = make_pair(idx, 8);
		
		if(seen.count(loID) == 0){	// make sure we haven't already handled this projector
			seen.insert(loID);
			htc = m_textureManager->highestTextureCached2(idx);
			if(htc < 8){
				hic = m_textureManager->highestImageCached2(idx);
				if(hic >= 8){
				//	m_textureManager->addTexturizeRequest(idx, 8);
				}
				else{
					getURLForImageWithLevel(*cImg, 8, s);
					urls.push_back(s);
					ids.push_back(loID);
				}
			}
		}
	}

	m_batchDownloader.setRequests(ids, urls);
	m_batchDownloader.start();
}


void
PSNetwork::initializeImagePriorities(const map<const Quad*, int> &distances,
									 int loLevel,
									 int hiLevel,
									 int hiPenalty)
{			                

	map<const Quad*, int>::const_iterator iter;
	for(iter = distances.begin(); iter != distances.end(); iter++){        

		const int imgIndex = iter->first->getImageIndex();

		const pair<int, int> loImgID = make_pair(imgIndex, loLevel);
		const pair<int, int> hiImgID = make_pair(imgIndex, hiLevel);

		ImagePriorities::iterator citer;
		
		m_imagePriorities[loImgID] = iter->second;
		m_imagePriorities[hiImgID] = iter->second + hiPenalty;

		string loURL, hiURL;
		getURLForImageWithLevel(*m_images[imgIndex], loLevel, loURL);
		getURLForImageWithLevel(*m_images[imgIndex], hiLevel, hiURL);

		m_imageDownloader.addRequest(loImgID, loURL, iter->second);
		//m_imageDownloader.addRequest(hiImgID, hiURL, iter->second + hiPenalty);
	}    
	
}



void	
PSNetwork::updateImagePriorities(const map<const Quad*, int> &distances,
								 const int loLevel,
								 const int hiLevel,
								 const int hiPenalty)
{	

	ImagePriorities::iterator p_iter;	    
	map<const Quad*, int>::const_iterator d_iter;

	for(d_iter = distances.begin(); d_iter != distances.end(); d_iter++){

		const int imgIndex = d_iter->first->getImageIndex();
		
		const pair<int, int> loImgID = make_pair(imgIndex, loLevel);
		const pair<int, int> hiImgID = make_pair(imgIndex, hiLevel);

		if((p_iter = m_imagePriorities.find(loImgID)) != m_imagePriorities.end())
			p_iter->second = min(p_iter->second, d_iter->second);
		if((p_iter = m_imagePriorities.find(hiImgID)) != m_imagePriorities.end())
			p_iter->second = min(p_iter->second, d_iter->second + hiPenalty);
		
	}
	
	m_imageDownloader.reprioritize(m_imagePriorities);	

}
 


