#include "GregJsonCollection.h"
#include "GregJsonProjector.h"
#include <iostream>
#include "JsonImage.h"

JsonCollection::~JsonCollection()
{
	clearImages();
	clearCoordSystems();
}


void
JsonCollection::clearCoordSystems()
{    
	FOR(i,0,m_coordSystems.size()){
		JSON_PRINTF("Deleting coordinate system %d\n", i);
		SAFE_DELETE_AND_CLEAR(m_coordSystems[i]);
	}
	m_coordSystems.clear();
}


void
JsonCollection::clearImages()
{
	JsonImages::iterator it;
	for(it = m_images.begin(); it != m_images.end(); it++){
		SAFE_DELETE(it->second);
	}
	
	m_images.clear();
}




JsonCoordSystem*
JsonCollection::GetOrMakeJsonCoordSystem(int index, bool allowMake)
{
    JsonCoordSystem *ret = NULL;
    if(index >= m_coordSystems.size())
        m_coordSystems.resize(index + 1);
    if((ret = m_coordSystems[index]) != NULL)
        return ret;
    if(allowMake)
        return (m_coordSystems[index] = new JsonCoordSystem(*this, index));
    //cout << "Coord System " << index << " does not exist and allowMake = false!!!" << endl;
    exit(1);
    return NULL;
}


JsonImage*
JsonCollection::GetOrMakeJsonImage(int index, bool allowMake)
{
    map<int, JsonImage*>::iterator it;
    if((it = m_images.find(index)) != m_images.end())
        return it->second;
    if(allowMake){
        m_images[index]->printInfo();
        return (m_images[index] = new JsonImage(this, index));    
    }
    //cout << "Image " << index << " does not exist and allowMake = false" << endl;
    exit(1);
    return NULL;
}


void
JsonCollection::setStartingPoint(int iCoordSystem, int jProjector)
{
    m_startingPoint.first  = iCoordSystem;
    m_startingPoint.second = jProjector;
}


void
JsonCollection::getStartingPoint(OUT int& coordSystem, OUT int& projector)
{
	coordSystem = m_startingPoint.first;
	projector   = m_startingPoint.second;
}


void
JsonCollection::setDefaultOrientation(int orientation)
{
    m_defaultOrientation = orientation;
}


void
JsonCollection::setThumbnail(const char *thumbnail)
{
    //cout << "JsonCollection::SetThumbnail(" << thumbnail << ")" << endl;
}


void
JsonCollection::setThumbnail(const wchar_t *thumbnail)
{
    //cout << "JsonCollection::SetThumbnail(" << thumbnail << ")" << endl;
}


const JsonCoordSystem&
JsonCollection::getCoordSystem(int index) const
{
    return  *(m_coordSystems[index]);
}


JsonCoordSystem&
JsonCollection::getCoordSystem(int index)
{
    return *(m_coordSystems[index]);
}



const JsonImage&
JsonCollection::getImage(int index) const
{
    ASSERT(m_images.find(index) != m_images.end());
    JsonImages::const_iterator it = m_images.find(index);    
    return *(it->second);
}


void 
JsonCollection::Parse(const JsonInDict & jCollection)
{
    // Even if we're doing nothing with these values for now,
    // this serves as type checking:

/*
#define PRINTHASKEY(a, b) //cout << "collection " << \
                            (a.has_key(b) ? "has " : "DOES NOT HAVE ") << \
                            "the key " << b << endl;

    PRINTHASKEY(jCollection, "__format");
    PRINTHASKEY(jCollection, "__version_data");
    PRINTHASKEY(jCollection, "__version_synther");
    PRINTHASKEY(jCollection, "_num_coord_systems");
*/
	

    if (const JsonInAtom * a = jCollection.if_key("__format")){        
		JsonString8 fmt = DecodeJsonStringTo8(*a); 
        JSON_PRINTF("format: %s\n", fmt.c_str()); 
	}
    if (const JsonInAtom * a = jCollection.if_key("__version_data"    )){
		double vd  = *a; 
        JSON_PRINTF("vdata: %lf\n", vd); 
	}
    if (const JsonInAtom * a = jCollection.if_key("__version_synther" )){
		//double vs  = *a; 
      //  JSON_PRINTF("vsynther: %lf\n", vs);  
	}
    if (const JsonInAtom * a = jCollection.if_key("_num_coord_systems")){        
        int n = *a;        
        JSON_PRINTF("numCoordSystems: %d\n", n);
        if (n<0 || n>=32768) {
            JsonThrow("number of coordinate systems out of bounds");
        }
        // clear coordinate systems at this point-- we should be at
        // the beginning of the stream.
        // then create coordinate systems 0..n-1, and from here on out
        // we need to ensure that we're only using existing images.
        //cout << "this = " << this << endl;
        clearCoordSystems();
        for (int i=0; i<n; ++i) {
            GetOrMakeJsonCoordSystem(i,true);
        }
    }
	
    if (const JsonInAtom * a = jCollection.if_key("_num_images"      )) {
        int n = *a;
        JSON_PRINTF("numImages %d\n", n);
        if (n<0 || n>=32768) {
            JsonThrow("number of images out of bounds");
        }
        // clear images at this point-- we should be at the beginning of the stream.
        // then create images 0..n-1, and from here on out we need to ensure that
        // we're only using existing images.
        clearImages();
        for (int i=0; i<n; ++i) {
            GetOrMakeJsonImage(i,true);
        }
    }
    if (const JsonInAtom * a = jCollection.if_key("_starting_point")) {
        JsonInArray x = a->ToArray();
        const int i = x[0],
                  j = x[1],
                  k = x[2];        
        
        JsonCoordSystem * cs = GetOrMakeJsonCoordSystem(k, true);
        setStartingPoint(k, j);
        JSON_PRINTF("start (%d,%d)\n", k, j); 
    }
    if (const JsonInAtom * a = jCollection.if_key("thumbnail_url")) {
        JsonStringW s = DecodeJsonStringToW(*a);
        setThumbnail(s.c_str());
    }
    if (const JsonInAtom * a = jCollection.if_key("y")) { // orientation
        setDefaultOrientation(int(*a));
    }
    else{
        setDefaultOrientation(0);
    }
    if (const JsonInAtom * a = jCollection.if_key("image_map")) {
        const JsonInDict & jIms = a->ToDict();
        JsonArrayStringV imKeys;
        jIms.keys(imKeys);

        const size_t nIms = imKeys.size();
        JSON_PRINTF("%d images:\n", nIms);

        for (size_t iIm=0; iIm<nIms; ++iIm) {
            const JsonString & imKey = imKeys[iIm];
            const int iImage = JsonParser(imKey);
            JSON_PRINTF("image %d:\n", iImage);

            JsonImage *im = GetOrMakeJsonImage(iImage, false);
            const JsonInDict & jIm = jIms(imKey).ToDict();
            im->Parse(jIm);

        }
    }
    
    if (const JsonInAtom * a = jCollection.if_key("x")) { // coord_systems

        const JsonInDict & jCoordSystems = a->ToDict();
        JsonArrayStringV coordSystemKeys;
        jCoordSystems.keys(coordSystemKeys);

        const size_t nCoordSystems = coordSystemKeys.size();
        JSON_PRINTF("%d coordinate systems:\n", nCoordSystems);

        for (size_t iCoordSystem=0; iCoordSystem<nCoordSystems; ++iCoordSystem) {
            const JsonString & coordSystemKey = coordSystemKeys[iCoordSystem];
            const int kCoordSystem = JsonParser(coordSystemKey);
            JSON_PRINTF("coordinate system %d:\n", kCoordSystem);

            JsonCoordSystem * cs = GetOrMakeJsonCoordSystem(kCoordSystem,                                                             
                                                            false);

            cs->setOrientation(m_defaultOrientation);

            const JsonInDict & jCs = jCoordSystems(coordSystemKey).ToDict();
            if (const JsonInAtom * a = jCs.if_key("z")) { // z_range
                const JsonInArray & x = a->ToArray();
                const double zNear = x[0],
                             zFar  = x[1];
                cs->setZRange(zNear, zFar);
                JSON_PRINTF("    z = [%lf, %lf]\n", zNear, zFar);
            }
			
			/******** ADDED BECAUSE Z RANGE SOMETIMES MISSING ******/
			else{
				cs->setZRange(0.00001f, 100000.0f);
			}
			
            if (const JsonInAtom * a = jCs.if_key("r")) { // projectors
                const JsonInDict & jProjs = a->ToDict();
                JsonArrayStringV projectorKeys;
                jProjs.keys(projectorKeys);

                const size_t nProjs = projectorKeys.size();
                JSON_PRINTF("    %d projectors:\n", nProjs);

                for (size_t iProj=0; iProj<nProjs; ++iProj) {
                    const JsonString & projKey = projectorKeys[iProj];
                    const int jProjector = JsonParser(projKey);
                    JSON_PRINTF("      projector %d: \n", jProjector);
                    JsonProjector * proj = cs->GetOrMakeJsonProjector(jProjector);
                    const JsonInDict & jProj = jProjs(projKey).ToDict();
                    proj->parse(jProj);
					
					JSON_PRINTF("\n");
                }

            }
            if (const JsonInAtom * a = jCs.if_key("k")) { // pointcloud
                const JsonInArray & pc = a->ToArray();
                const JsonStringW s = DecodeJsonStringToW(pc[0]);
                const int nBlobs = pc[1];
                cs->setPointcloudURL(toJsonString8(s).c_str(), nBlobs);
            }
            if (const JsonInAtom * a = jCs.if_key("o")) { // halos
                const JsonInDict & jHalos = a->ToDict();
                JsonArrayStringV haloKeys;
                jHalos.keys(haloKeys);

                const size_t nHalos = haloKeys.size();
                JSON_PRINTF("    %d halos:\n", nHalos);

                for (size_t i=0; i<nHalos; ++i) {
                    const JsonString & haloKey = haloKeys[i];
                    const int j = JsonParser(haloKey);
                    JSON_PRINTF("      halo %d: ", j);
                    const JsonInDict & jHalo = jHalos(haloKey).ToDict();
                    cs->ParseHalo(jHalo);
                    JSON_PRINTF("\n");
                }
            }

            // panoramas
            if (const JsonInAtom * a = jCs.if_key("m")) {
                const JsonInDict & jPanoramas = a->ToDict();
                JsonArrayStringV panoramaKeys;
                jPanoramas.keys(panoramaKeys);

                const size_t nPanoramas = panoramaKeys.size();
                JSON_PRINTF("    %d panoramas:\n", nPanoramas);

                for (size_t i=0; i<nPanoramas; ++i) {
                    const JsonString & panoramaKey = panoramaKeys[i];
                    const int j = JsonParser(panoramaKey);
                    JSON_PRINTF("      pano %d: ", j);
                    const JsonInDict & jPanorama = jPanoramas(panoramaKey).ToDict();
                    cs->ParsePanorama(jPanorama);
                    JSON_PRINTF("\n");
                }
            }
            if (const JsonInAtom * a = jCs.if_key("one_meter")) {}
            if (const JsonInAtom * a = jCs.if_key("world_up")) {}
            if (const JsonInAtom * a = jCs.if_key("origin")) {}
            if (const JsonInAtom * a = jCs.if_key("coord_target")) {}
            if (const JsonInAtom * a = jCs.if_key("latitude_longitude")) {}
            if (const JsonInAtom * a = jCs.if_key("elevation")) {}
        }
    }
    
    JSON_PRINTF("\nconnecting portals: ");
    connectPortalImages();
    JSON_PRINTF("\ndone.\n");
}


void
JsonCollection::connectPortalImages()
{
	vector<vector<pair<int, int> > > vi(m_images.size());
	vector<vector<JsonProjector*> >  vp(m_images.size());
	
	// group the projectors by image
	FOR(i,0,m_coordSystems.size()){
		Projectors projectors = m_coordSystems[i]->getProjectors();
		FOR(j,0,projectors.size()){
			int imgIndex = projectors[j]->getImageIndex();
			vi[imgIndex].push_back(make_pair(i, j));
			vp[imgIndex].push_back(projectors[j]);
		}
	}
	
	// print the projectors associated with each image
	/*
	FOR(i,0,vi.size()){
		//cerr << "Image " << i << ": ";
		FOR(j,0,vi[i].size()){			
			if(j != 0)
				//cerr << ", ";
			//cerr << "(" << vi[i][j].first << ", " << vi[i][j].second << ")";
		}
		//cerr << endl;
	}
	 */
	
	// connect the neighbors
	FOR(i,0,vp.size())
		FOR(j,0,vp[i].size())
			FOR(k,j+1,vp[i].size()){
				vp[i][j]->addPortalNeighbor(vp[i][k]);
				vp[i][k]->addPortalNeighbor(vp[i][j]);
				/*
				//cerr << "(" << vi[i][j].first << ", " << vi[i][j].second << ") --> (" << vi[i][k].first << ", " << vi[i][k].second << ")" << endl;
				//cerr << "(" << vi[i][k].first << ", " << vi[i][k].second << ") --> (" << vi[i][j].first << ", " << vi[i][j].second << ")" << endl;
				 */
			}
}


