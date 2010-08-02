/*
 *  BatchDownloader.hpp
 *  Phonosynth
 *
 *  Created by Greg on 1/13/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */


// this class is kinda stupid. TODO: Downloaders could be tweaked


template<class T>
BatchDownloader<T>::BatchDownloader(int nConcurrentConnections) :
	ContinuousDownloader<T>(nConcurrentConnections)
{

}


template<class T>
BatchDownloader<T>::~BatchDownloader()
{

}


template<class T>
void
BatchDownloader<T>::setRequests(const vector<T>& ids,
								const vector<string>& requests)
{
	ASSERT(ids.size() == requests.size());
	reset();
	FOR(i,0,requests.size())
		addRequest(ids[i], requests[i], i);
}

