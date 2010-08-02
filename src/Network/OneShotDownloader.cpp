/*
 *  OneShotDownloader.cpp
 *  Phonosynth
 *
 *  Created by Greg on 1/7/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#include "OneShotDownloader.h"
#include <GThread.h>

#ifdef IPHONE
	#include <MacDataFetcher.h>
#endif

OneShotDownloader::OneShotDownloader() :
	m_successCallback(NULL),
	m_failureCallback(NULL),
	m_url(""),
	m_cancelled(false)
{
#ifdef IPHONE
	m_dataFetcher = new MacDataFetcher(this);
#endif
}


OneShotDownloader::~OneShotDownloader()
{
	SAFE_DELETE(m_successCallback);
	SAFE_DELETE(m_failureCallback);
}


void
OneShotDownloader::cancel()
{
	m_dataFetcher->cancel();
	m_cancelled = true;
}
	

void
OneShotDownloader::asynchronousGET(const string& url,
								   const FunctorA<ByteArray>& successCallback,
								   const Functor& failureCallback)
{
	m_status		  = NULL;
	m_data			  = NULL;
	m_successCallback = successCallback.copy();
	m_failureCallback = failureCallback.copy();
	
	m_url = url;
	m_body = "";
	MemberFunctor<DataFetcher> f(m_dataFetcher, &DataFetcher::beginFetchingData);
	GThread::performFunctorOnMainThread(f);
}


void
OneShotDownloader::asynchronousGET(const string& url, 
								   OUT ByteArray* data,
								   OUT DownloadStatus* status)
{
    m_status		  = status;
	m_data			  = data;
	m_successCallback = NULL;
	m_failureCallback = NULL;
	
	*m_status = DS_DOWNLOADING;
	*m_data   = ByteArray(NULL, 0);

	m_url = url;
	m_body = "";
	MemberFunctor<DataFetcher> f(m_dataFetcher, &DataFetcher::beginFetchingData);
	GThread::performFunctorOnMainThread(f);
}


void
OneShotDownloader::asynchronousPOST(const string& url, const string& body,
									const FunctorA<ByteArray>& successCallback,
									const Functor& failureCallback)
{
	m_status		  = NULL;
	m_data			  = NULL;
	m_successCallback = successCallback.copy();
	m_failureCallback = failureCallback.copy();
	
	m_url = url;
	m_body = body;
	MemberFunctor<DataFetcher> f(m_dataFetcher, &DataFetcher::beginFetchingData);
	GThread::performFunctorOnMainThread(f);
}


void
OneShotDownloader::asynchronousPOST(const string& url, const string& body,
									OUT ByteArray* data,
									OUT DownloadStatus* status)
{
	m_status		  = status;
	m_data			  = data;
	m_successCallback = NULL;
	m_failureCallback = NULL;
	
	*m_status = DS_DOWNLOADING;
	*m_data   = ByteArray(NULL, 0);
	
	m_url = url;
	m_body = body;	
	MemberFunctor<DataFetcher> f(m_dataFetcher, &DataFetcher::beginFetchingData);
	GThread::performFunctorOnMainThread(f);
}

// DataClient overrides

void
OneShotDownloader::onRequestComplete(const Request& r, ByteArray data)
{
	if(m_successCallback != NULL)
		m_successCallback->call(data);
	else{
		*m_data   = data;
		*m_status = DS_SUCCEEDED;
	}
}


void
OneShotDownloader::onRequestFailed(const Request& r)
{
	if(m_failureCallback != NULL)
		m_failureCallback->call();
	else{
		*m_data   = NULL;
		*m_status = DS_FAILED;
	}
}


bool
OneShotDownloader::getNextRequest(OUT Request& r)
{
	if(m_url == "")
		return false;
	
	r.requestID = 0;
	r.url		= m_url;
	r.body      = m_body;
	m_url		= "";
	return true;
}