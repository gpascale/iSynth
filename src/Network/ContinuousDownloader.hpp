/*
 *  ContinuousDownloader.cpp
 *  Phonosynth
 *
 *  Created by Greg on 9/28/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

template<typename T>
ContinuousDownloader<T>::ContinuousDownloader(const int nConcurrentConnections) :
	m_nextID(0)
{    
    m_fetcher = new MacDataFetcher(this, nConcurrentConnections);
    m_idToRequestMap.clear();
    m_requestToIDMap.clear();
	m_requests.clear();
}


template<typename T>
ContinuousDownloader<T>::~ContinuousDownloader()
{
    SAFE_DELETE_AND_CLEAR(m_fetcher);
}

    
template<typename T>
void
ContinuousDownloader<T>::setNumConcurrentDownloads(const int n)
{
    m_fetcher->setMaxConcurrentConnections(n);
}


template<typename T>
void
ContinuousDownloader<T>::reprioritize(const map<T, int>& priorities)
{
    FOR(i,0,m_requests.size())
        m_requests[i].priority = priorities[m_requests[i].requestID];
    
    SORT(m_requests);
}


template<typename T>
void
ContinuousDownloader<T>::addRequest(const T requestID, const string& url, const int pri)
{    
    m_requestToIDMap[m_nextID] = requestID;
    m_requests.push_back(PrioritizedRequest<T>(requestID, Request(m_nextID, url), pri));
	m_nextID++;
}


template<typename T>
void
ContinuousDownloader<T>::start()
{
    m_fetcher->beginFetchingData();
}


template<typename T>
void
ContinuousDownloader<T>::stop(bool cancelCurrentDownloads)
{
    m_fetcher->reset(cancelCurrentDownloads);
}


template<typename T>
void
ContinuousDownloader<T>::reset()
{
    stop();
    m_requestToIDMap.clear();
    m_requests.clear();
}


template<typename T>
void
ContinuousDownloader<T>::downloadFailed(const T requestID)
{
    ASSERT(0 && "ContinuousDownloader::downloadFailed not overridden!! SHOULD NOT GET HERE");
}



// overridden DataClient functions

template<typename T>
bool
ContinuousDownloader<T>::getNextRequest(OUT Request& r)
{
    if(m_requests.empty()) 
        return false;
    r = m_requests.front().request;
    m_requests.erase(m_requests.begin());
    return true;
}

template<typename T>
void
ContinuousDownloader<T>::onRequestComplete(const Request& r, ByteArray data)
{
	const typename map<int, T>::iterator iter = m_requestToIDMap.find(r.requestID);
    ASSERT(iter != m_requestToIDMap.end());
    downloadSucceeded(iter->second, data);
	m_requestToIDMap.erase(iter);
}


template<typename T>
void
ContinuousDownloader<T>::onRequestFailed(const Request& r)
{
	const typename map<int, T>::iterator iter = m_requestToIDMap.find(r.requestID);
    ASSERT(iter != m_requestToIDMap.end());
	downloadFailed(iter->second);
	m_requestToIDMap.erase(iter);
}



