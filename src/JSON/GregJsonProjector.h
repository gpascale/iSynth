#ifndef _JSONPROJECTOR_H_
#define _JSONPROJECTOR_H_

#include "JsonInput.h"
#include "JsonOutput.h"
#include "MiniCommon/LivePtr.h"
#include "common.h"
#include <GMath.h>
#include <vector>

using namespace std;

class JsonCoordSystem;
class JsonImage;

class JsonProjector : public RefpObj, public LiveObj
{

public:

    JsonProjector( JsonCoordSystem &cs, int globalIndex, int localIndex );   
    ~JsonProjector();

    // fills in data members
    void parse(const JsonInDict &jProj);    
    
    const Vector4&   getDominantPlane() const { return m_dominantPlane; }
    float            getFocalLength  () const { return m_focalLength;   }
    float            getAspectRatio  () const { return m_aspectRatio;   }    
    int              getOrientation  () const { return m_orientation;   }    
    int              getImageIndex   () const { return m_imageIndex;    }
	const JsonImage* getImage	     () const { return m_image;			}
    int              getLocalIndex   () const { return m_localIndex;    }    
    int              getGlobalIndex  () const { return m_globalIndex;   }
    float            getFOVY         () const { return FOVY(m_aspectRatio, m_focalLength); }
	const Vector3&	 getLookVector   () const { return m_lookVector;	}
    
    // returns the corners of the camera frustum obtained by shooting four rays a distance
    // of 'shootDist'
	bool getFrustumCorners(OUT Point3 &p1, OUT Point3 &p2, 
                           OUT Point3 &p3, OUT Point3 &p4,
                           float shootDist = 0.25f) const;
    
    // returns the corners of the quad inscribed by tracing the camera frustum out to
    // the dominant plane
    bool getQuadCorners(OUT Point3 &p1, OUT Point3 &p2, OUT Point3 &p3, OUT Point3 &p4)    const;
    bool getQuadCenter(OUT Point3& ret)                                                    const;
	
	Point3 getQuadCenter() const { Point3 ret; getQuadCenter(ret); return ret; }
	
    /*
    bool    getQuadPosition (float &x1, float &y1, float &z1,
                             float &x2, float &y2, float &z2,
                             float &x3, float &y3, float &z3,
                             float &x4, float &y4, float &z4) const;	
     */

    const Quaternion&       getRotation()    const { return (const Quaternion&) m_rotation; }
    const Vector4&          getPosition()    const { return (const Vector4&) m_position; }
    const JsonCoordSystem&  getCoordSystem() const { return m_coordSystem; }
        

    Matrix                  getRotationMatrix(){ return m_rotation.toMatrix(); }
                    
    // for debugging
    void printInfo();

    void             computeViewMatrix();
    void             computeCorrectedViewMatrix();
    void             computeProjectionMatrix();

    const Matrix&    getViewMatrix()          const { return m_viewMatrix;          }
    const Matrix&    getCorrectedViewMatrix() const { return m_correctedViewMatrix; }
    const Matrix&    getProjectionMatrix()    const { return m_projectionMatrix;    }
					
	
	float			 getZNear()	const { return m_zNear; }
	float			 getZFar()  const { return m_zFar;  }
	
	// quads
	void			 addQuad(const JsonProjector* quad){ m_quads.push_back(quad); }
	const vector<const JsonProjector*>& getQuads() const { return m_quads; }
	
	// peers
	void			 addPeer(const JsonProjector* peer){ m_peers.push_back(peer); }
	const vector<const JsonProjector*>& getPeers() const { return m_peers; }	

	// portals
	void addPortalNeighbor(const JsonProjector* pNeighbor) { m_portalNeighbors.push_back(pNeighbor); }
	const vector<const JsonProjector*>& getPortalNeighbors() const { return m_portalNeighbors; }
	bool isPortal()	const { return m_portalNeighbors.size() > 0; }
	
	// neighbors
    const JsonProjector*         getLeftNeighbor ()     const { return m_leftNeighbor;     }
    const JsonProjector*         getRightNeighbor()     const { return m_rightNeighbor;    }
	const JsonProjector*         getUpNeighbor   ()     const { return m_upNeighbor;       }
    const JsonProjector*         getDownNeighbor ()     const { return m_downNeighbor;     }
    const JsonProjector*         getInNeighbor   ()     const { return m_inNeighbor;       }
    const JsonProjector*         getOutNeighbor  ()     const { return m_outNeighbor;      }
    const JsonProjector*         getPrevNeighbor ()     const { return m_prevNeighbor;     }
    const JsonProjector*         getNextNeighbor ()     const { return m_nextNeighbor;     }
    const JsonProjector*         getFromNeighbor ()     const { return m_fromNeighbor;     }
    const JsonProjector*         getToNeighbor   ()     const { return m_toNeighbor;       }    
    const JsonProjector* const * getAllNeighbors ()     const { return m_neighbors;        }
    const JsonProjector*         getNeighbor(int index) const { return m_neighbors[index]; }
		
	    
	bool hasLeftNeighbor ()     const { return m_leftNeighbor != NULL;     }
    bool hasRightNeighbor()     const { return m_rightNeighbor != NULL;    }
	bool hasUpNeighbor   ()     const { return m_upNeighbor != NULL;       }
    bool hasDownNeighbor ()     const { return m_downNeighbor != NULL;     }
    bool hasInNeighbor   ()     const { return m_inNeighbor != NULL;       }
    bool hasOutNeighbor  ()     const { return m_outNeighbor != NULL;      }
    bool hasPrevNeighbor ()     const { return m_prevNeighbor != NULL;     }
    bool hasNextNeighbor ()     const { return m_nextNeighbor != NULL;     }
    bool hasFromNeighbor ()     const { return m_fromNeighbor != NULL;     }
    bool hasToNeighbor   ()     const { return m_toNeighbor != NULL;       }
 	bool hasNeighbor(int index) const { return m_neighbors[index] != NULL; }
    
	
	
	
	// panorama stuff
	
	bool isMemberOfPanorama() const { return m_panorama != -1;  }
	int  getPanorama() const        { return m_panorama;        }
	void setPanorama(int panoIndex) { m_panorama = panoIndex;   }
	int  setIndexInPanorama(int n)  { m_indexInPanorama = n;	}
	int  getIndexInPanorama() const { return m_indexInPanorama; }
	
	// clean this up
	void setPanoNeighbors(const JsonProjector* prev, const JsonProjector* next){
		m_panoNeighbors[0] = prev;
		m_panoNeighbors[1] = next;
	}
	
	const JsonProjector* getPrevPanoNeighbor() const { return m_panoNeighbors[0]; }
	const JsonProjector* getNextPanoNeighbor() const { return m_panoNeighbors[1]; }

private:    

    // internal helper functions            
    
    void generateFrustumRays(OUT Ray& ray1, OUT Ray& ray2, OUT Ray& ray3, OUT Ray& ray4) const;
    
    JsonProjector* GetOrMakeProjectorNeighbor(const JsonInAtom & neigh);

    void setLeftNeighbor (JsonProjector *neigh)      { m_leftNeighbor  = neigh; }
    void setRightNeighbor(JsonProjector *neigh)      { m_rightNeighbor = neigh; }
	void setUpNeighbor   (JsonProjector *neigh)      { m_upNeighbor    = neigh; }
    void setDownNeighbor (JsonProjector *neigh)      { m_downNeighbor  = neigh; }
    void setInNeighbor   (JsonProjector *neigh)      { m_inNeighbor    = neigh; }
    void setOutNeighbor  (JsonProjector *neigh)      { m_outNeighbor   = neigh; }
    void setPrevNeighbor (JsonProjector *neigh)      { m_prevNeighbor  = neigh; }
    void setNextNeighbor (JsonProjector *neigh)      { m_nextNeighbor  = neigh; }
    void setFromNeighbor (JsonProjector *neigh)      { m_fromNeighbor  = neigh; }
    void setToNeighbor   (JsonProjector *neigh)      { m_toNeighbor    = neigh; } 
    void setAllNeighbors (JsonProjector *neighs[])   { memcpy(m_neighbors, neighs, 10); }    

    void    setDominantPlane(const Vector4 &plane);    
    void    setOrientation  (int o){ ASSERT(o <= 4 && o >= 0); m_orientation = o; }

    static Vector4          DecodedPlane(float x, float y, float w);
    static Quaternion       DecodedQuaternion(float x, float y, float z);

    void                    init(const Point3 &pos,
                                 const Quaternion &rot,
                                 int index, float aspect, float focalLen,
                                 float zNear, float zFar);    

    // storage info
    JsonCoordSystem&        m_coordSystem;
    int                     m_orientation;
    int                     m_globalIndex;
    int                     m_localIndex;    
    int                     m_imageIndex;    

    // orientation parameters
    Point3                  m_position;
    Vector3                 m_upVector;
    Quaternion              m_rotation;
    Matrix                  m_viewMatrix;
    Matrix                  m_projectionMatrix;
    Matrix                  m_correctedViewMatrix;
    Vector4                 m_dominantPlane;
	Point3					m_quadCenter;
	Vector3					m_lookVector;

	int						m_panorama;
	int						m_indexInPanorama;
	
    // camera lens parameters
    float                   m_aspectRatio;
    float                   m_focalLength;
	float					m_zNear;
	float					m_zFar;
	
	JsonImage*				m_image;

	vector<const JsonProjector*>  m_panoNeighbors;
	vector<const JsonProjector*>  m_peers;
	vector<const JsonProjector*>  m_quads;
	vector<const JsonProjector*>  m_portalNeighbors;

    // neigbors
    union{        
        struct{
            JsonProjector *m_leftNeighbor;
            JsonProjector *m_rightNeighbor;
			JsonProjector *m_upNeighbor;
            JsonProjector *m_downNeighbor;
            JsonProjector *m_inNeighbor;
            JsonProjector *m_outNeighbor;
            JsonProjector *m_prevNeighbor;
            JsonProjector *m_nextNeighbor;
            JsonProjector *m_fromNeighbor;
            JsonProjector *m_toNeighbor;
        };
        JsonProjector* m_neighbors[10];
    };
};

#endif