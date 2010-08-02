/*
 *  PSNetwork.h
 *  Phonosynth
 *
 *  Created by Greg on 10/14/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _PSNetwork_H_
#define _PSNetwork_H_

#include <ContinuousDownloader.h>
#include <BatchDownloader.h>
#include <vector>
#include <map>
#include <string>
#include <GregJsonCollection.h>

class PSNetwork;
class PSSGManager;
class Quad;
class SeadragonTile;
class TextureManager;

// Very simple sublcass of ContinuousDownloader<T> for downloading images
class ImageDownloader : public ContinuousDownloader<pair<int, int> >
{
    PSNetwork* m_parent;
    
    void downloadSucceeded(const pair<int, int> imgIndex, ByteArray data);
    void downloadFailed(const pair<int, int> imgIndex);
    
public:
    
    ImageDownloader(PSNetwork* parent, const unsigned nDownloads = 5) :
        ContinuousDownloader<pair<int, int> >(nDownloads),
        m_parent(parent)
    { }
    
};

// Very simple sublcass of ContinuousDownloader<T> for downloading pointclouds
class PointcloudDownloader : public ContinuousDownloader<pair<int, int> >
{
    PSNetwork* m_parent;
    
    void downloadSucceeded(const pair<int, int> requestID, ByteArray data); 
    void downloadFailed(const pair<int, int> requestID);
    
public:
    
    PointcloudDownloader(PSNetwork* parent, const int nConcurrentConnections = 5) :
        ContinuousDownloader<pair<int, int> >(nConcurrentConnections),
        m_parent(parent) 
    { }
    
};


class SeadragonDownloader : public ContinuousDownloader<int>
{
    PSNetwork* m_parent;
    
    void downloadSucceeded(const int tileID, ByteArray data); 
    void downloadFailed(const int tileID);
        
public:
    
    SeadragonDownloader(PSNetwork* parent, const int nConcurrentConnections = 1) :
        ContinuousDownloader<int>(nConcurrentConnections),
        m_parent(parent)
    { }
    
};


class BImageDownloader : public BatchDownloader<pair<int, int> >
{
	PSNetwork* m_parent;
	
	void downloadSucceeded(const pair<int, int> imageID, ByteArray data);
	void downloadFailed(const pair<int, int> imageID);
	
public:
	
	BImageDownloader(PSNetwork* parent, const int nConcurrentConnections = 1) :
		BatchDownloader<pair<int, int> >(nConcurrentConnections),
		m_parent(parent)
	{ }	
	
	};



// PSNetwork is meant to encapsulate some of the dirty work involved in downloading all
// of the data that is pulled in on the fly during synth viewing. It provides a bit of more specific
// functionality around the general ContinuousDownloader class, which forms its backbone.
// The main things it handles specifically is dynamic prioritzation of image downloads

class PSNetwork
{
public:

    // We use a map to prioritize all image requests
    typedef map<pair<int, int>, int> ImagePriorities;    
    // store urls of images to download keyed on image index, we use a vector since the
    // image indices happen to be contiguous
    typedef vector<string>           ImageURLs;
    
    typedef vector<SeadragonTile>    SeadragonTiles;
    
    
    PSNetwork(PSSGManager* sgm, TextureManager* tm);
    ~PSNetwork();
	
	void cleanup();
    
    void setSGManager(PSSGManager* sgm) { m_scenegraphManager = sgm; }
    
    // pointcloud downloads are specified by int-int pairs of the form {<coord system>, <blob number>}    
    void setPointcloudsToDownload(const vector<pair<int, int> >& indices, const vector<string>& urls);
    
    // PSNetwork holds on to the JsonImages for the images it will need to download,
    // that way it can retrieve URLs for arbitrary JPEG levels whenever it wants
    void setImagesToDownload(const JsonImages& jsonImages);
    
    void setSeadragonTilesToDownload(const SeadragonTiles& tiles);
    
	void setNumConcurrentDownloads(int n) { m_batchDownloader.setNumConcurrentDownloads(n); }
	
    // the client is responsible for assigning each Quad* a distance (presumably from the current one),
    // as well as the low and high levels and the amount to penalize high. With that, this class
    // handles prioritization of requests the rest of the way
    void initializeImagePriorities(const map<const Quad*, int>& priorities,
                                   const int loLevel = 7,
                                   const int hiLevel = 9,
                                   const int hiPenalty = 3);
    
    // updates the priorities described above according to the distances and other parameters passed in
    void updateImagePriorities(const map<const Quad*, int>& priorities,
                               const int loLevel = 7,
                               const int hiLevel = 9,
                               const int hiPenalty = 3);
        
     
	// new
	void movedToProjector(const JsonProjector* p);
        
    void start();   // begin downloading
    void stop();    // stop downloading
    void reset();   // reset state so this instance can be used for another synth    
    
     
     
private:        
    
    // helper function to retrieve the exact URL to download a particular image
    // at a particular level of quality
    void getURLForImageWithLevel(const JsonImage& image, int level, OUT string& ret) const;
    
    
    
    // Callbacks for downloaders
    
    // called when the continuous image downloader has downloaded an image
    // imgID is a pair representing the global index of the image and quality of the download
    void downloadedImage(const pair<int, int> imgID, ByteArray data);
    
    // called when the continuous pointcloud downloader has downloaded a pointcloud blob
    // imgID is a pair representing the coordinate system of the pointcloud and the blob index
    void downloadedPointcloudBlob(const pair<int, int> blobID, ByteArray data);
    
    void downloadedSeadragonTile(const int tileID, ByteArray data);
    
    // so that our ContinuousDownloaders can access the above callbacks but nobody else can
    friend class            ImageDownloader;            
    friend class            PointcloudDownloader;
    friend class            SeadragonDownloader;
	friend class			BImageDownloader;
    
    PSSGManager*            m_scenegraphManager;
    
    ImageDownloader         m_imageDownloader;          // continuously downloads images
    PointcloudDownloader    m_pointcloudDownloader;     // continuously downloads pointclouds
    SeadragonDownloader     m_seadragonDownloader;      // continuously downloades Seadragon tiles
	BImageDownloader		m_batchDownloader;		
    
    ImagePriorities         m_imagePriorities;    
    JsonImages              m_images;    
    SeadragonTiles          m_seadragonTiles;
	
	
	TextureManager*			m_textureManager;

};





#endif // _PSNetwork_H_