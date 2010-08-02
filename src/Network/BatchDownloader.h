/*
 *  BatchDownloader.h
 *  Phonosynth
 *
 *  Created by Greg on 1/13/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#ifndef _BATCHDOWNLOADER_H_
#define _BATCHDOWNLOADER_H_

#include <ContinuousDownloader.h>

template<class T>
class BatchDownloader : public ContinuousDownloader<T>
{

public:

	BatchDownloader(int nConcurrentConnections = 1);
	virtual ~BatchDownloader();

	void setRequests(const vector<T>& ids, const vector<string>& requests);
	void setRequests(const Request* requests, int nRequests);

	void downloadSucceeded(const T requestID, ByteArray data) = 0;
	void downloadFailed(const T requestID)					  = 0;		
	
private:
			

};

#include "BatchDownloader.hpp"

#endif