/*
 *  DataFetcher.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DataFetcher.h"
#include <common.h>

DataFetcher::DataFetcher(DataClient* client,
						 int maxConcurrentConnections) :
	m_client(client),
	m_maxConcurrentConnections(maxConcurrentConnections),
	m_numActiveConnections(0)
{

}



DataFetcher::~DataFetcher()
{
	
}



