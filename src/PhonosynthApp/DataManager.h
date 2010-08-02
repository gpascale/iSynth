#ifndef _DATAMANAGER_H_
#define _DATAMANAGER_H_

#include <Json.h>
#include <JsonSynthManager.h>
#include <string>
#include <vector>
#include <GMath.h>

class JsonCollection;
class JsonProjector;

using namespace std;

struct SynthData
{
    int             numCoordSystems;
    int             numImages;
    Vector3         startingPoint;
    string          format;
    string          syntherVersion;
    string          dataVersion;
};

class DataManager
{
public:
    DataManager(void);
    ~DataManager(void);
	
	void				cleanup();

    int                 getCurCoordSystem(){ return m_curCoordSystem; }
    int                 getNumCoordSystems() const;
    JsonCollection*     getCollection(){ return m_collection; }
    bool                loadJson(bool remote = true);
	bool				loadJsonLocal();
	bool				loadJsonRemote();

	void				setSynthURL(JsonString s);
    void                setSynthRoot(JsonString s);
    void                setSynthName(JsonString s);

	const JsonString&	getSynthURL() { return m_synthURL;  }
    const JsonString&   getSynthRoot(){ return m_synthRoot; }
    const JsonString&   getSynthName(){ return m_synthName; }
	
	const JsonProjector& getProjector (int iCoordSystem, 
									   int jProjIndex) const ;
    

	// called when loading a new synth, should release all memory used
	void				reset();

private:

    JsonString                  m_synthRoot;
    JsonString                  m_synthName;
	JsonString					m_synthURL;
    JsonCollection*             m_collection;
    int                         m_curCoordSystem;
    SynthData                   m_synthData;

};

#endif