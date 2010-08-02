#include "GregJsonCoordSystem.h"
#include "GregJsonProjector.h"
#include "GregJsonCollection.h"
#include <iostream>

using namespace std;

JsonCoordSystem::JsonCoordSystem(JsonCollection & collection, int index) :
    m_collection(collection),
    m_index(index),
    m_pointcloudURL(""),
    m_numBlobs(0)
{
}

JsonCoordSystem::~JsonCoordSystem()
{
	int numProjectors = (int) m_projectors.size();
	FOR(i,0,numProjectors)
		SAFE_DELETE(m_projectors[i]);
}


JsonProjector*
JsonCoordSystem::GetOrMakeJsonProjector(int globalIndex)
{
	// TODO: There IS NO GLOBAL INDEX!!! TAKE IT OUT!!!	    
	
	if(globalIndex >= m_projectors.size())
		m_projectors.resize(globalIndex + 1, NULL);
	if(m_projectors[globalIndex] == NULL)
		m_projectors[globalIndex] = new JsonProjector(*this, globalIndex, globalIndex);
		
    return m_projectors[globalIndex];			
}


void
JsonCoordSystem::setZRange(float zNear, float zFar)
{
	m_zNear = zNear;
    m_zFar = zFar;
}


void
JsonCoordSystem::setPointcloudURL(const char *s, int nBlobs)
{
	JSON_PRINTF("setPointcloudURL(%s, %d)\n", s, nBlobs);
    m_pointcloudURL = s;
	m_numBlobs = nBlobs;
}



void
JsonCoordSystem::ParseHalo(const JsonInDict &jHalo)
{
    ////cout << "JsonCoordSystem::ParseHalo" << endl;
}


void
JsonCoordSystem::ParsePanorama(const JsonInDict &jPanorama)
{
	if (const JsonInAtom *a = jPanorama.if_key("a")){
		const JsonInArray &v = a->ToArray();
		const int jBest = v[0];
		const float x          = (float) (double) v[1],
					y          = (float) (double) v[2],
					z          = (float) (double) v[3],
					rBubble    = (float) (double) v[4],
					bubbleSize = (float) (double) v[5];
		
		const JsonInArray &jMembers = jPanorama("i").ToArray();
		const int nMembers = jMembers.size();		
		
		m_panoramas.push_back(Panorama());
		Panorama &newPanorama = m_panoramas.back();		
		
		
		const JsonProjector *prevP = NULL, *curP = NULL, *nextP = GetOrMakeJsonProjector(jMembers[0]);
		
		/*
		FOR(i,0,nMembers)
			//cerr << "[" << (int) jMembers[i] << "]";
		//cerr << endl;
		 */
		
		FOR(i,0,nMembers){						
			
			prevP = curP;
			curP = nextP;
			nextP = (i+1 < nMembers) ? GetOrMakeJsonProjector(jMembers[i+1]) : NULL;			
			const int projLocalIndex = curP->getLocalIndex();				
			newPanorama.push_back(projLocalIndex);
			
			// TODO: are these still necessary?
			curP->setPanorama(m_panoramas.size() - 1);
			curP->setIndexInPanorama(i);
			
			// provide links to neighbors
			curP->setPanoNeighbors(prevP, nextP);			
		}
	}
}


void
JsonCoordSystem::printInfo()
{
    ////cout << "Coordinate System " << m_index << endl;
    ////cout << "zNear: " << m_zNear << endl;
    ////cout << "zFar: " << m_zFar << endl;
    ////cout << m_projectors.size() << " projectors: ..." << endl;
   
	/*
    for(int i = 0; i < (int) m_projectors.size(); i++)
        m_projectors[i]->printInfo();
	 */
}


const JsonProjector&
JsonCoordSystem::getProjector(int localIndex) const
{
    return *(m_projectors[localIndex]);
}


JsonProjector&
JsonCoordSystem::getProjector(int localIndex)
{
    return *(m_projectors[localIndex]);
}


const Panorama&
JsonCoordSystem::getPanorama(int panoIndex) const
{	
	//JSON_PRINTF("panoIndex = %d\n", panoIndex);
	//JSON_PRINTF("size = %d\n", m_panoramas.size());
	
	ASSERT( panoIndex < (int) m_panoramas.size() );
	return (const Panorama&) m_panoramas[panoIndex];		
}		


float
magQuadCenter(const JsonProjector& p)
{
	return p.getQuadCenter().xyz().magnitude();
}


struct ProjectorComparator : public binary_function<const JsonProjector*, const JsonProjector*, bool>
{
	bool operator () (const JsonProjector* lhs, const JsonProjector* rhs) const
	{
		return magQuadCenter(*lhs) < magQuadCenter(*rhs);
	}
};


// attempts to roughly estimate the scale of coordinate system based on the Quad in or
// closest to the 90th percentile in terms of distance to the origin. Quad center is used rather
// than projector position since it will hopefully be more resilient to differences in focal length

float
JsonCoordSystem::estimateScale(float tolerance)
{	
	Projectors pByQDist(m_projectors.begin(), m_projectors.end());	
	sort(pByQDist.begin(), pByQDist.end(), ProjectorComparator());
		
	int idx = tolerance * (const int) pByQDist.size();
	
	return magQuadCenter(*pByQDist[idx]);	
}
	
	
	
	
	
	





