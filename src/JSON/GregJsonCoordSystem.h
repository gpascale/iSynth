#ifndef _GREGJSONCOORDSYSTEM_H_
#define _GREGJSONCOORDSYSTEM_H_

#include "JsonInput.h"
#include "JsonOutput.h"
#include "MiniCommon/LivePtr.h"
#include <vector>
#include <set>

class JsonProjector;
class JsonCollection;

using namespace std;

typedef vector<JsonProjector*>        Projectors;

typedef vector<int>					  Panorama;
typedef vector<Panorama>			  Panoramas;

class JsonCoordSystem : public RefpObj, public LiveObj
{
public:

    explicit            JsonCoordSystem(JsonCollection & collection, int index);
                        ~JsonCoordSystem();

    // parsing routines
    void ParseHalo(const JsonInDict & jHalo);
    void ParsePanorama(const JsonInDict & jPanorama);       
    JsonProjector*      GetOrMakeJsonProjector(int globalIndex);

    // accessors
    void                  setOrientation(int o)    { m_orientation = o; }
    int                   getOrientation()   const { return (int) m_orientation; }
    int                   getNumProjectors() const { return (int) m_projectors.size(); }
	int					  getNumPanoramas()  const { return (int) m_panoramas.size(); }  
    Projectors            getProjectors(){ return m_projectors; }    
    void                  setZRange(float zNear, float zFar);
    void                  setPointcloudURL(const char *s, int nBlobs);
    float                 getZNear(){ return m_zNear; }
    float                 getZFar(){ return m_zFar; }
    JsonCollection&       GetCollection(){ return m_collection; }	
	int                   getIndex() const { return m_index; }
	const string&		  getPointcloudURL() const { return m_pointcloudURL; }
	int					  getNumBlobs() const	   { return m_numBlobs;      }	    
	const Panorama&		  getPanorama(int panoIndex) const;
	
	float				  estimateScale(float tolerance = .9f);
	
	
	const JsonProjector&  getProjector(int localIndex) const;
		  JsonProjector&  getProjector(int localIndex);

    // debugging
    void printInfo();    
        
private:    

    int                     m_orientation;    
    int                     m_index;    
    float                   m_zNear;
    float                   m_zFar;

	JsonCollection&         m_collection;
		
	Projectors              m_projectors;
	Panoramas				m_panoramas;
	
    JsonString              m_pointcloudURL;
	int						m_numBlobs;

};

#endif