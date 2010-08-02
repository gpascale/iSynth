#include "GregJsonProjector.h"
#include "GregJsonCollection.h"
#include "GregJsonCoordSystem.h"
#include "JsonImage.h"

#include "MiniCommon/Matrix.h"
#include "MiniCommon/Quaternion.h"


JsonProjector::JsonProjector( JsonCoordSystem &cs, int globalIndex, int localIndex ) :
    m_coordSystem(cs), 
    m_globalIndex(globalIndex),
    m_localIndex(localIndex),
    m_upVector(0.0f, 1.0f, 0.0f),
	m_panorama(-1),
	m_indexInPanorama(-1),
	m_panoNeighbors(2, NULL)
{    
    memset(m_neighbors, NULL, 10 * sizeof(JsonProjector*));
	m_peers.clear();
}


JsonProjector::~JsonProjector()
{
	//cerr << "freeing a projector, should free up " << sizeof(JsonProjector) << " bytes" << endl;
}


JsonProjector*
JsonProjector::GetOrMakeProjectorNeighbor(const JsonInAtom &neigh)
{
    JsonProjector * result = 0;
    if (neigh.get_type() == JsonTypeInt) {
        const int j = neigh;
        if (j < 0) return 0;    //no neighbor                
        result = m_coordSystem.GetOrMakeJsonProjector(j);
        //b_printf("{%d}", j);
    } 
    else{
        const JsonInArray & a = neigh.ToArray();
        const int j = a[0],
                  k = a[1];
        JsonCollection & coll = m_coordSystem.GetCollection();
        JsonCoordSystem * cs = coll.GetOrMakeJsonCoordSystem(k, false);
        b_assert(cs);        
        result = cs->GetOrMakeJsonProjector(j);
        //b_printf("{%d,%d}", j,k);
    }
    b_assert(result);
    return result;
}


Quaternion
JsonProjector::DecodedQuaternion(float x, float y, float z)
{    
    float w = 1.0f - SQR(x) - SQR(y) - SQR(z);    
    return Quaternion(x, y, z, w > 0.0f ? sqrt(w) : 0.0f);
}


Plane
JsonProjector::DecodedPlane(float x, float y, float w)
{
    if(FUZZY_GEQ(SQR(x) + SQR(y), 1.0f))
		return Vector4(x, y, 0.0f, w);
    return Plane(x, y, sqrt(1.0f - SQR(x) - SQR(y)), w);    
}


void
JsonProjector::setDominantPlane(const Plane& plane)
{
    // flip if necessary
    if(pointPlaneDistance(plane, m_position) >= 0.0f)
        m_dominantPlane = plane;
    else
        m_dominantPlane = (-plane);
}


void 
JsonProjector::generateFrustumRays(OUT Ray& ray1, OUT Ray& ray2, OUT Ray& ray3, OUT Ray& ray4) const
{
    float viewW = m_aspectRatio;
    float viewH = 1.0f;
    
    if (m_aspectRatio > 1.0f){
        viewW = 1.0f;
        viewH = 1.0f / m_aspectRatio;
    }
    
    
    // what we really want is the inverse rotation, but we can just premultiply by the rotation instead
    Matrix invRot; (-m_rotation).toMatrix(invRot);
    
    ray1.o = ray2.o = ray3.o = ray4.o = m_position;    
    ray1.d = Vector3(-0.5f * viewW, -0.5f * viewH, m_focalLength).normalize() * invRot;
    ray2.d = Vector3( 0.5f * viewW, -0.5f * viewH, m_focalLength).normalize() * invRot;
    ray3.d = Vector3(-0.5f * viewW,  0.5f * viewH, m_focalLength).normalize() * invRot;
    ray4.d = Vector3( 0.5f * viewW,  0.5f * viewH, m_focalLength).normalize() * invRot;
}


bool
JsonProjector::getQuadCenter(OUT Point3& ret) const
{
    Matrix r; m_rotation.toMatrix(r);
	Matrix invRot; (-m_rotation).toMatrix(invRot);
    const Ray ray(m_position, Vector3(0.0f, 0.0f, 1.0f) * invRot);
	return rayPlaneIntersect(ray, m_dominantPlane, ret) >= 0.0f;
}


bool
JsonProjector::getQuadCorners(OUT Point3 &p1, OUT Point3 &p2, OUT Point3 &p3, OUT Point3 &p4) const
{
    Ray ray[4];
    generateFrustumRays(ray[0], ray[1], ray[2], ray[3]);
    
    float d1 = rayPlaneIntersect(ray[0], m_dominantPlane, p1);
    float d2 = rayPlaneIntersect(ray[1], m_dominantPlane, p2);
    float d3 = rayPlaneIntersect(ray[2], m_dominantPlane, p3);
    float d4 = rayPlaneIntersect(ray[3], m_dominantPlane, p4);  	
    
    return FUZZY_GEQ(d1, 0.0f) && FUZZY_GEQ(d2, 0.0f) && FUZZY_GEQ(d3, 0.0f) && FUZZY_GEQ(d4, 0.0f);    
}


bool
JsonProjector::getFrustumCorners(OUT Point3 &p1, OUT Point3 &p2, 
                                 OUT Point3 &p3, OUT Point3 &p4,
                                 float shootDist) const
{    
    Ray ray[4];
    generateFrustumRays(ray[0], ray[1], ray[2], ray[3]);
    
    p1 = ray[0].shootFixedDistance(shootDist);
    p2 = ray[1].shootFixedDistance(shootDist);
    p3 = ray[2].shootFixedDistance(shootDist);
    p4 = ray[3].shootFixedDistance(shootDist);
    
    return true;
}


void
JsonProjector::computeViewMatrix()
{
    Matrix::ViewMatrix(m_viewMatrix, m_position.xyz(), m_rotation);
	m_viewMatrix.getColumnVector(m_lookVector, 2);
	m_lookVector.normalize();
    ////cout << "THE VIEW MATRIX IS " << m_viewMatrix << endl;
}

// computes a rotated view matrix with the up vector 0, 0, 1
void
JsonProjector::computeCorrectedViewMatrix()
{
    
    Matrix::ViewMatrix(m_viewMatrix, m_position.xyz(), m_rotation);
    m_correctedViewMatrix = m_viewMatrix;

    Vector3 cSide;   m_viewMatrix.getColumnVector(cSide, 0);
    Vector3 cUp;     m_viewMatrix.getColumnVector(cUp, 1);
    Vector3 cLook;   m_viewMatrix.getColumnVector(cLook, 2);        

    Vector3 Z(0.0f, 0.0f, 1.0f);
    
    ASSERT(FUZZY_EQ(cSide.magnitudeSq(), 1.0f, 0.001f));
    ASSERT(FUZZY_EQ(cUp.magnitudeSq(), 1.0f, 0.001f));
    ASSERT(FUZZY_EQ(cLook.magnitudeSq(), 1.0f, 0.001f));

    ASSERT(FUZZY_EQ0(cSide.dot(cUp), .001f));
    ASSERT(FUZZY_EQ0(cSide.dot(cLook), .001f));
    ASSERT(FUZZY_EQ0(cLook.dot(cUp), .001f));

    Vector3 look = cLook;
    Vector3 side = look.cross(Z);
    side.normalize();
    Vector3 up = look.cross(side);

    ASSERT(FUZZY_EQ(side.magnitudeSq(), 1.0f, 0.001f));
    ASSERT(FUZZY_EQ(up.magnitudeSq(), 1.0f, 0.001f));
    ASSERT(FUZZY_EQ(look.magnitudeSq(), 1.0f, 0.001f));
    
    ASSERT(FUZZY_EQ0(side.dot(up), .001f));
    ASSERT(FUZZY_EQ0(side.dot(look), .001f));
    ASSERT(FUZZY_EQ0(look.dot(up), .001f));

    m_correctedViewMatrix.setColumnVector(0, side);
    m_correctedViewMatrix.setColumnVector(1, up);
    m_correctedViewMatrix.setColumnVector(2, look);
	
	// look vector shouldn't have changed, but might as well do this anyway
	m_correctedViewMatrix.getColumnVector(m_lookVector, 2);	
}


void
JsonProjector::computeProjectionMatrix()
{
    Matrix::ProjectionMatrixPerspRH(m_projectionMatrix,
                                  FOVY(m_aspectRatio, m_focalLength),
                                  m_aspectRatio, 
                                  0.001f, 
                                  1000.0f);
}


void
JsonProjector::init(const Point3 &pos,          //position
                    const Quaternion &rot,
                    int index,
                    float aspect, float focalLen,
                    float zNear, float zFar)
{
    m_position = pos;
    m_rotation = rot;
    m_aspectRatio = aspect;
    m_focalLength = focalLen;
    m_imageIndex = index;
	
	
	m_zNear = zNear;
	m_zFar = zFar;

    computeProjectionMatrix();
    computeViewMatrix();
    computeCorrectedViewMatrix();

	/*
    //cout << "JsonProjector::init complete ... " << endl;
    //cout << "m_position = " << m_position << endl;
    //cout << "m_rotation = " << m_rotation << endl;
    //cout << "m_aspectRatio = " << m_aspectRatio << endl;
    //cout << "m_focalLength = " << m_focalLength << endl;
    //cout << "m_imageIndex = " << m_imageIndex << endl;
	 */
}


void
JsonProjector::parse(const JsonInDict &jProj)
{
    // image_position_rotation_aspect_focallength
    if (const JsonInAtom * a = jProj.if_key("j")) {         
        const JsonInArray & p = a->ToArray();
        const int iImage = p[0];
        const double x   = p[1],
                     y   = p[2],
                     z   = p[3],
                     qx  = p[4],
                     qy  = p[5],
                     qz  = p[6],
                     af  = p[7],
                     f   = p[8];        
        init(Point3(x,y,z,1.0f),
             DecodedQuaternion(qx,qy,qz),
             iImage,
             af, 
             f,
             m_coordSystem.getZNear(),
             m_coordSystem.getZFar());
        b_printf(" (%.3lf,%.3lf,%.3lf)\n", x,y,z);
    }

    // dominant_plane
    if (const JsonInAtom * a = jProj.if_key("p")) { 
        
        const JsonInArray & p = a->ToArray();
        const double x   = p[0],
                     y   = p[1],
                     w   = p[2];
        setDominantPlane(DecodedPlane(x,y,w));
        b_printf(" pl(%.3lf,%.3lf,%.3lf)", x,y,w);
        
    }

    // discrete neighbors
    if (const JsonInAtom * a = jProj.if_key("n")) {
        const JsonInArray & p = a->ToArray();
        b_printf(" n:");
        setLeftNeighbor (GetOrMakeProjectorNeighbor(p[0]));
        setRightNeighbor(GetOrMakeProjectorNeighbor(p[1]));
        setInNeighbor   (GetOrMakeProjectorNeighbor(p[2]));
        setOutNeighbor  (GetOrMakeProjectorNeighbor(p[3]));
        setUpNeighbor   (GetOrMakeProjectorNeighbor(p[4]));
        setDownNeighbor (GetOrMakeProjectorNeighbor(p[5]));
        setPrevNeighbor (GetOrMakeProjectorNeighbor(p[6]));
        setNextNeighbor (GetOrMakeProjectorNeighbor(p[7]));
    }

    // quads, would be great to use these in next version. TODO
    if (const JsonInAtom * a = jProj.if_key("q")) { 
        const JsonInArray & p = a->ToArray();
        b_printf(" q:");
        const size_t n = p.size();
        for (size_t i=0; i<n; ++i) {
            addQuad(GetOrMakeProjectorNeighbor(p[i]));
        }
    }

	// JsonImage
	
	m_image = m_coordSystem.GetCollection().GetOrMakeJsonImage(m_imageIndex, false);
	
    // peers
    if (const JsonInAtom * a = jProj.if_key("e")) { 
        
        const JsonInArray & p = a->ToArray();
        b_printf(" p:");
        const size_t n = p.size();
        for (size_t i=0; i<n; ++i) {
            addPeer(GetOrMakeProjectorNeighbor(p[i]));
        }
        
    }

    // orientation
    if (const JsonInAtom * a = jProj.if_key("y")) {
        setOrientation(int(*a));
    }
    else{
        setOrientation(m_coordSystem.getOrientation());
    }
	
	// store quad-center
	getQuadCenter(m_quadCenter);
	
}



void
JsonProjector::printInfo()
{
	/*
    //cout << "projector " << m_imageIndex << ":" << endl;
    //cout << "\tm_position = " << m_position << endl;
    //cout << "\tm_rotation = " << m_rotation << endl;
    //cout << "\tm_aspectRatio = " << m_aspectRatio << endl;
    //cout << "\tm_focalLength = " << m_focalLength << endl;
    //cout << "\tm_orientation = " << m_orientation << endl;
	 */
}
