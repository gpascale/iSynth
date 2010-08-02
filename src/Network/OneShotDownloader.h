/*
 *  OneShotDownloader.h
 *  Phonosynth
 *
 *  Created by Greg on 1/7/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#include <DataFetcher.h>
#include <Functions.h>
#include <common.h>
#include <GMutex.h>

#ifndef _ONESHOTDOWNLOADER_H_
#define _ONESHOTDOWNLOADER_H_


enum DownloadStatus
{
	DS_DOWNLOADING,
	DS_SUCCEEDED,
	DS_FAILED
};
	


class OneShotDownloader : public DataClient
{
public:
	
	// constructors
	OneShotDownloader();	
	// creates a OneShotDownloader that immediately begins handling the specified request
	OneShotDownloader(const Request& r, FunctorA<ByteArray>* callback);	
	
	~OneShotDownloader();
			
	void asynchronousGET(const string& url,
						 const FunctorA<ByteArray>& successCallback,
						 const Functor& failureCallback);
	
	void asynchronousGET(const string& url,
						 OUT ByteArray* data,
						 OUT DownloadStatus* status);
	
	void asynchronousPOST(const string& url, const string& postBody,
						  const FunctorA<ByteArray>& successCallback,
						  const Functor& failureCallback);
	
	void asynchronousPOST(const string& url, const string& postBody,
						  OUT ByteArray* data,
						  OUT DownloadStatus* status);
	
	// DataClient overrides
	void onRequestComplete(const Request& r, ByteArray data);
	void onRequestFailed(const Request& r);
	bool getNextRequest(OUT Request& r);
	
	void cancel();

private:
			
	DataFetcher*			m_dataFetcher;	
	string					m_url;
	string					m_body;
	
	FunctorA<ByteArray>*	m_successCallback;	
	Functor*				m_failureCallback;
	
	ByteArray*				m_data;
	DownloadStatus*			m_status;
	
	GMutex					m_lock;
	bool					m_cancelled;
	
};

#endif // _ONESHOTDOWNLOADER_H_