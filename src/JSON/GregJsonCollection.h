#ifndef _GREGJSONCOLLECTION_H_
#define _GREGJSONCOLLECTION_H_

#include "Json.h"
#include "MiniCommon/LivePtr.h"
#include "JsonInput.h"
#include "GregJsonCoordSystem.h"
#include <GMath.h>
#include <vector>

class JsonImage;
class JsonCoordSystem;
class JsonProjector;

using namespace std;

class JsonSynthAliases {

    JsonDictStringV jsonShortNames_;

public:

    operator const JsonDictStringV * () {
        JsonDictStringV & M = jsonShortNames_;
        if (M.empty()) {
            M["image_position_rotation_aspect_focallength"	] = "j";
            M["dominant_plane"								] = "p";
            M["discrete_neighbors"							] = "n";
            M["quads"										] = "q";
            M["peers"										] = "e";
            M["orientation"									] = "y";
            M["representative_center_radius_bubblesize"     ] = "a";
            M["representative_center_radius_bubblesize_ \
				rinner_router_theta0_theta1_phi0_phi1"		] = "t";
            M["members"										] = "i";
            M["z_range"										] = "z";
            M["projectors"									] = "r";
            M["pointcloud"									] = "k";
            M["license_url"									] = "c";
            M["author"										] = "b";
            M["attribution_url"								] = "s";
            M["halos"										] = "o";
            M["panoramas"									] = "m";
            M["collections"									] = "l";
            M["coord_systems"								] = "x";
            M["image_hash"									] = "h";
            M["image_path"									] = "u";
        }
        return &M;
    }

};

typedef vector<JsonCoordSystem*>      CoordSystems;
typedef map<int, JsonImage*>            JsonImages;

class JsonCollection : public RefpObj, public LiveObj 
{

public:

    JsonCollection(JsonString name){ m_name = name; }
    JsonCollection(){ clearCoordSystems(); }
    JsonCollection(const JsonCollection &jsc){ clearCoordSystems(); }    
    ~JsonCollection();
                

    void                connectPortalImages();       
        
    // parsing routines
    void Parse(const JsonString &input, JsonSynthAliases aliases, bool specialize){ }
    void Parse(const JsonInDict &jCollection);
    
    JsonCoordSystem*    GetOrMakeJsonCoordSystem(int index, bool allowMake);
    JsonImage*          GetOrMakeJsonImage(int index, bool allowMake);
    
    // coordinate systems
    int                     getNumCoordSystems()  const { return (int) m_coordSystems.size(); }
    const CoordSystems&     getCoordSystems()     const { return m_coordSystems; }
    const JsonCoordSystem&  getCoordSystem(int i) const;
		  JsonCoordSystem&  getCoordSystem(int i);

    // images
    int                 getNumImages()  const { return (int) m_images.size(); }    
    const JsonImages&   getImages()     const { return m_images; }
    const JsonImage&    getImage(int i) const;

    // random stuff
    JsonString      getName(){ return m_name; }
    void            getStartingPoint(int &iCoordSystem, int &jProjector);
	void			setBaseURL(const string &baseURL){ m_baseURL = baseURL; }
	const string&	getBaseURL() const				 { return m_baseURL;	}
	
	
private:
    
    void    setStartingPoint(int iCoordSystem, int jProjector);
    void    setThumbnail(const char *thumbnail);
    void    setThumbnail(const wchar_t *thumbnail);
    void    setDefaultOrientation(int orientation);
    int     getDefaultOrientation() const { return m_defaultOrientation; }    
    void    clearCoordSystems();
    void    clearImages();

	string				m_baseURL;
    int                 m_defaultOrientation;
    pair<int, int>      m_startingPoint;
    CoordSystems        m_coordSystems;
    JsonImages          m_images;
    JsonString          m_name;

};

#endif