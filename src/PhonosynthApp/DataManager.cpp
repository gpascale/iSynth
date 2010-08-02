#include "DataManager.h"
#include <common.h>
#include <GregJsonCollection.h>
#include <JsonImage.h>
#include <iostream>
#include <fstream>
#include <GregJsonProjector.h>
#include <PSGlobalState.h>
#include <OneShotDownloader.h>

DataManager::DataManager(void) :
	m_collection(NULL)
{
}

DataManager::~DataManager(void)
{
}


void
DataManager::cleanup()
{
	SAFE_DELETE(m_collection);	
}


void
DataManager::setSynthRoot(JsonString s)
{
    if(s[s.length() - 1] == '/')
        m_synthRoot = s.substr(0, s.length() - 1);
    else
        m_synthRoot = s;
}


void
DataManager::setSynthName(JsonString s)
{
    ASSERT(s[s.length() - 1] != '/');
    m_synthName = s;
}


void
DataManager::setSynthURL(JsonString url)
{
	ASSERT(url != "");
	m_synthURL = url;
}


bool
DataManager::loadJson(bool remote)
{
	return remote ? loadJsonRemote() : loadJsonLocal();
}


void
DataManager::reset()
{
	APP_PRINTF("DataManager::reset()\n");
}


bool
DataManager::loadJsonRemote()
{
	//ASSERT( [NSThread currentThread] != [NSThread mainThread]);	
	
	ByteArray		  jsonContents;
	DownloadStatus    status;
	OneShotDownloader downloader;
	
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	downloader.asynchronousGET(m_synthURL, &jsonContents, &status);
	while(status == DS_DOWNLOADING)
		sleep(0.02f);
	
	
	if(status == DS_FAILED || jsonContents.data() == NULL){
		//cerr << "error while downloading Json file" << endl;
		return false;
	}

	try{
		JsonParser J;
		JsonSynthAliases aliases;
		
		//cerr << "before parse, nInstances_ = " << JsonInAtom::nInstances_ << endl;
		
		int nCharsParsed = J.parse((const char*) jsonContents.data(), aliases, false); // don't specialize						
		
		//cerr << "after parse, nInstances_ = " << JsonInAtom::nInstances_ << endl;
		
		jsonContents.release(DESTRUCT_FREE);
		// asserts that this is in fact a JSON synth, and that the version is >= 1.0
		if (double(J("_json_synth")) < 1.01) {
			JsonThrow("pre-1.01 version of json-synth");
		}
		
		const JsonInDict & jCollections = J("l").ToDict();
        JsonArrayStringV collectionKeys;
        jCollections.keys(collectionKeys);
        const int nColl = (const int) collectionKeys.size();
		ASSERT(nColl == 1);
		
		
		FOR(i,0,nColl){
            const JsonString & collKey = collectionKeys[i];
			m_collection = new JsonCollection();
            JsonInDict jCollection = jCollections(collKey).ToDict();
			JSON_PRINTF("parsing collection %d\n", i);
            m_collection->Parse(jCollection);
        }
		
		
    }		
	catch (JsonException & ) {
		//cerr << "Json parse error" << endl;
		return false;
    }
	
	[pool release];
				
	//cerr << "parser should be gone now, nInstances_ = " << JsonInAtom::nInstances_ << endl;
	
	if(m_collection == NULL){
		//cerr << "m_collection was null" << endl;
		return false;
	}
	
	string baseURL = m_synthURL.substr(0, m_synthURL.length() - 6);
	m_collection->setBaseURL(baseURL);
    
	PSGlobalState::setCurrentCollection(m_collection);
    PSGlobalState::setCurrentSynthURL(baseURL.substr(0, baseURL.length() - 13));
	
    return true;
}


bool
DataManager::loadJsonLocal()
{          
	return false;
}

int
DataManager::getNumCoordSystems() const
{
    return m_collection->getNumCoordSystems();
}



const JsonProjector&
DataManager::getProjector(int iCoordSystem, int jProjIndex) const
{
	ASSERT(iCoordSystem < m_collection->getNumCoordSystems());	
	const JsonCoordSystem &cs = m_collection->getCoordSystem(iCoordSystem);	
	ASSERT(jProjIndex < cs.getNumProjectors());	
	return cs.getProjector(jProjIndex);
}
	
	
	
	
	
	

