/*
 *  PSNetwork.cpp
 *  Phonosynth
 *
 *  Created by Greg on 9/27/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#if 0

#include "PSNetwork.h"



PSNetwork::PSNetwork() :
    m_imageFetcher(new MacDataFetcher(&m_imageReceiver, 10)),
    m_pointcloudFetcher(new MacDataFetcher(&m_pointcloudReceiver, 2))
{
    
}


void
PSNetwork::addImageRequest(const unsigned requestID, const string& url)
{
    m_imageRequests.push_back(PrioritizedRequest(requestID, url));
}


void
PSNetwork::addPointcloudRequest(const unsigned requestID, const string& url)
{
    m_pointcloudRequests.push_back(PrioritizedRequest(requestID, url));
}


void
PSNetwork::setConcurrentDownloads(const int nImages, const int nPCs)
{
    m_imageFetcher->setMaxConcurrentConnections(nImages);
    m_pointcloudFetcher->setMaxConcurrentConnections(nPCs);
}


void
PSNetwork::begin()
{
    m_imageFetcher->beginFetchingData();
    m_pointcloudFetcher->beginFetchingData();
}


void
PSNetwork::reprioritize(const map<unsigned, int>& priorities)
{
    for(int i = 0; i < (int) m_imageRequests.size(); i++){
        m_imageRequests[i].priority = priorities[m_imageRequests[i].requestID];        
    }
}


void
PSNetwork::downloadedImage(const pair<int, int> imgID, ByteArray data)
{
    cout << "PSNetwork::downloadedImage(" << imgID.first << ", " << imgID.second << ")" << endl;

}


void
PSNetwork::downloadedPointcloud(const pair<int, int> pointcloudID, ByteArray data)
{
    cout << "PSNetwork::downloadedPointcloud()" << endl;
}

#endif // 0



