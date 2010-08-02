/*
 *  Subdivision.cpp
 *  Phonosynth
 *
 *  Created by Greg on 10/12/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "Subdivision.h"
#include <Quad.h>

class Quad;






float 
Triangle::getDistortion(const Matrix &viewMatrix, const Matrix &projectionMatrix)
{
    
    Vector4 ABWorld = 0.5f * (A.position() + B.position());
    Vector4 ACWorld = 0.5f * (A.position() + C.position());
    Vector4 BCWorld = 0.5f * (B.position() + C.position());
    
    Vector3 AScreen  = (A.position() * viewMatrix * projectionMatrix).homogenized3();
    Vector3 BScreen  = (B.position() * viewMatrix * projectionMatrix).homogenized3();
    Vector3 CScreen  = (C.position() * viewMatrix * projectionMatrix).homogenized3();
    
    Vector3 ABScreen = 0.5f * (AScreen + BScreen);
    Vector3 ACScreen = 0.5f * (AScreen + CScreen);
    Vector3 BCScreen = 0.5f * (BScreen + CScreen);
    
    Vector3 realABScreen  = (ABWorld * viewMatrix * projectionMatrix).homogenized3();
    Vector3 realACScreen  = (ACWorld * viewMatrix * projectionMatrix).homogenized3();
    Vector3 realBCScreen  = (BCWorld * viewMatrix * projectionMatrix).homogenized3();
    
    return MAX3((ABScreen - realABScreen).magnitude(),
                (ACScreen - realACScreen).magnitude(),
                (BCScreen - realBCScreen).magnitude());
        
}


float
Triangle::screenSpaceArea(const Matrix &viewMatrix, const Matrix &projectionMatrix)
{    
    Vector4 Apos = A.position() * viewMatrix * projectionMatrix;
    Vector4 Bpos = B.position() * viewMatrix * projectionMatrix;
    Vector4 Cpos = C.position() * viewMatrix * projectionMatrix;
    
    Vector3 Ah = Apos.homogenized3();
    Vector3 Bh = Bpos.homogenized3();
    Vector3 Ch = Cpos.homogenized3();
    
    float s1 = (Ah - Bh).magnitude();
    float s2 = (Ah - Ch).magnitude();
    float s3 = (Bh - Ch).magnitude();
    
    return heronArea(s1, s2, s3);
}


void
Triangle::splitAB(vector<Triangle> &ret, const Quad &q, SubdivisionMap &m) const
{
    SubdivisionMap::iterator iter;
    Vertex::XYZWUV AB;
    if((iter = m.find(make_pair(A, B))) == m.end() &&
       (iter = m.find(make_pair(B, A))) == m.end()){
        AB = interpolateVertex(A, B, q);
        m[make_pair(A, B)] = AB;
    }
    else{
        AB = iter->second;
    }
    
    ret.push_back(Triangle(A, C, AB));
    ret.push_back(Triangle(B, C, AB));
}


void
Triangle::splitBC(vector<Triangle> &ret, const Quad &q, SubdivisionMap &m) const
{
    SubdivisionMap::iterator iter;
    Vertex::XYZWUV BC;
    if((iter = m.find(make_pair(B, C))) == m.end() &&
       (iter = m.find(make_pair(C, B))) == m.end()){
        BC = interpolateVertex(B, C, q);
        m[make_pair(B, C)] = BC;
    }
    else{
        BC = iter->second;
    }
    
    ret.push_back(Triangle(A, B, BC));
    ret.push_back(Triangle(A, C, BC));
}


void
Triangle::splitAC(vector<Triangle> &ret, const Quad &q, SubdivisionMap &m) const
{
    SubdivisionMap::iterator iter;
    Vertex::XYZWUV AC;
    if((iter = m.find(make_pair(A, C))) == m.end() &&
       (iter = m.find(make_pair(C, A))) == m.end()){
        AC = interpolateVertex(A, C, q);
        m[make_pair(A, C)] = AC;
    }
    else{
        AC = iter->second;
    }
    
    ret.push_back(Triangle(A, B, AC));
    ret.push_back(Triangle(B, C, AC));
}


void
Triangle::subdivide(vector<Triangle> &ret, const Quad &q) const
{
    
    Vertex::XYZWUV AB = ::interpolateVertex(A, B, q);
    Vertex::XYZWUV AC = ::interpolateVertex(A, C, q);		
    Vertex::XYZWUV BC = ::interpolateVertex(B, C, q);
    
    ret.push_back(Triangle(AC,  A, AB));
    ret.push_back(Triangle(AB,  B, BC));
    ret.push_back(Triangle(AC,  C, BC));
    ret.push_back(Triangle(AB, BC, AC));
    
}

void
Triangle::subdivide(vector<Triangle> &ret, const Quad &q, SubdivisionMap &m) const
{
    
    Vertex::XYZWUV AB, AC, BC;
    
    SubdivisionMap::iterator iter;
    if((iter = m.find(make_pair(A, B))) == m.end() && (iter = m.find(make_pair(B, A))) == m.end()){
        AB = ::interpolateVertex(A, B, q);
        m[make_pair(A, B)] = AB;
    }
    else{
        AB = iter->second;			
    }
    if((iter = m.find(make_pair(A, C))) == m.end() && (iter = m.find(make_pair(C, A))) == m.end()){
        AC = ::interpolateVertex(A, C, q);
        m[make_pair(A, C)] = AC;
    }
    else{
        AC = iter->second;			
    }
    if((iter = m.find(make_pair(B, C))) == m.end() && (iter = m.find(make_pair(C, B))) == m.end()){
        BC = ::interpolateVertex(B, C, q);
        m[make_pair(B, C)] = BC;
    }
    else{
        BC = iter->second;			
    }
    
    ret.push_back(Triangle(AC,  A, AB));
    ret.push_back(Triangle(AB,  B, BC));
    ret.push_back(Triangle(AC,  C, BC));
    ret.push_back(Triangle(AB, BC, AC));
    
}


// Extern functions

Vertex::XYZWUV
interpolateVertex(const Vertex::XYZWUV &v1, const Vertex::XYZWUV &v2, const Quad &q)
{
    
	
	const Matrix &viewMatrix		= q.getViewMatrix();
	const Matrix &projectionMatrix	= q.getProjectionMatrix();
	        
    Vertex::XYZWUV ret;
    
	// Position: Here's the tricky part. Need to project into screen
	// space to figure out where to split the two vertices best
	const Vector4 &v1World = v1.position();
	const Vector4 &v2World = v2.position();
	
	const Vector3 v1Screen = (v1.position() * viewMatrix * projectionMatrix).homogenized3();
	const Vector3 v2Screen = (v2.position() * viewMatrix * projectionMatrix).homogenized3();
            
    
    /* Binary search FTW!!!1 */
	
	float	ans = 0.5f, lo = 0.0f, hi = 1.0f, r, d1, d2;
	Vector4 interpWorld;
    Vector3 interpScreen;		
	
	int c = 0;
	
	do{ 
        
		ans = (lo + hi) / 2.0f;
        
		interpWorld  = Vector4::interpolate(v1World, v2World, ans);
		interpScreen = projectAndHomogenize3(interpWorld, viewMatrix, projectionMatrix);
		
        d1 = SQR(interpScreen.y - v1Screen.y) + SQR(interpScreen.x - v1Screen.x);
        d2 = SQR(interpScreen.y - v2Screen.y) + SQR(interpScreen.x - v2Screen.x);
		r = d1 / d2;
        
		if(r > 1.0f)
            hi = (lo + hi) / 2.0f;
		else if(r < 1.0f)
            lo = (lo + hi) / 2.0f;
        else
            break;		
        
	} while(FUZZY_NEQ(r, 1.0f, 0.01f) && c++ < 10);	
    
	ret.position() = interpWorld;    	
    const float u = interpScreen.x * 0.5f + 0.5f, v = interpScreen.y * 0.5f + 0.5f;    
    ret.texCoords() = Vector2(1-u, 1-v);
    
	return ret;		
}


void
doNaiveSubdivide(Quad& q, vector<Triangle>& v, int depth)
{
    if(depth <= 0) return;
	
	vector<Triangle> vv;
	
	SubdivisionMap m;
	m.clear();
	
	FOR(i,0,v.size()){
		v[i].subdivide( vv, q, m );
	}
	
	v = vv;
	
	doNaiveSubdivide(q, v, depth - 1);
}

void
doAdaptiveSubdivide(Quad& q, vector<Triangle> &v, float maxDistortion)
{
	SubdivisionMap m;
	m.clear();	
    
    const Matrix& viewMatrix       = q.getViewMatrix();
    const Matrix& projectionMatrix = q.getProjectionMatrix();
	
	while(1){
		
		vector<SubdivideAction> actions(v.size(), NONE);
		vector<Triangle> next;		
        
		// subdivide those that need it
		bool done = true;
		FOR(i,0,v.size()){
			const float distortion = v[i].getDistortion(viewMatrix, 
														projectionMatrix);
			if(distortion > maxDistortion){
				actions[i] = SUBDIVIDE;
				v[i].subdivide(next, q, m);
				done = false;
			}			
		}
		if(done) break;
		
		// subdivide those that now need it
		SubdivisionMap::iterator iter;
		bool moreToSubdivide = true;
        
		while(moreToSubdivide){					
			moreToSubdivide = false;
			FOR(i,0,v.size()){
				if(actions[i] != SUBDIVIDE){
					const Vertex::XYZWUV &A = v[i].A;
					const Vertex::XYZWUV &B = v[i].B;
					const Vertex::XYZWUV &C = v[i].C;
                    
					if ((iter = m.find(make_pair(A, B))) != m.end() ||
						(iter = m.find(make_pair(B, A))) != m.end()){
						if(actions[i] != NONE){
							actions[i] = SUBDIVIDE;
							v[i].subdivide(next, q, m);
							moreToSubdivide = false;
							continue;
						}
						else
							actions[i] = SPLIT_AB;						
					}
					if ((iter = m.find(make_pair(A, C))) != m.end() ||
						(iter = m.find(make_pair(C, A))) != m.end()){
						if(actions[i] != NONE){
							actions[i] = SUBDIVIDE;
							v[i].subdivide(next, q, m);
							moreToSubdivide = false;
							continue;
						}
						else
							actions[i] = SPLIT_AC;						
					}
					if ((iter = m.find(make_pair(B, C))) != m.end() ||
						(iter = m.find(make_pair(C, B))) != m.end()){
						if(actions[i] != NONE){
							actions[i] = SUBDIVIDE;
							v[i].subdivide(next, q, m);
							moreToSubdivide = false;
							continue;
						}
						else
							actions[i] = SPLIT_BC;						
					}
				}
			}			
		}
		
        
		FOR(i,0,v.size()){			
            if(actions[i] == SPLIT_AB)
                v[i].splitAB(next, q, m);
			else if(actions[i] == SPLIT_AC)
                v[i].splitAC(next, q, m);
			else if(actions[i] == SPLIT_BC)
                v[i].splitBC(next, q, m);
			else if(actions[i] == NONE)
                next.push_back(v[i]);
		}				
		
		//swap and do it again
		v = next;
        
	}			
}

/*
void computeSubdivisionVertices(Quad& q, 
                                int numSubdivides,
                                OUT Vertex::XYZ** positions,
                                OUT Vertex::UV** texCoords,
                                OUT int& numVertices)
{
    // do the subdivision
    Vertex::XYZWUV* vertices;
    computeSubdivisionVertices(q, numSubdivides, &vertices, numVertices);
    
    // copy from Vertex::XYZWUV to Vertex::XYZ and Vertex::UV
    *positions = new Vertex::XYZ[numVertices];
    *texCoords = new Vertex::UV[numVertices];
    FOR(i,0,numVertices){		
		(*positions)[i] = Vertex::XYZ(vertices[i].getPosition());
        (*texCoords)[i] = Vertex::UV(vertices[i].getU(), vertices[i].getV());
	}
    
    SAFE_DELETE_ARRAY(vertices);
}
*/
 
void computeSubdivisionVertices(Quad& q, 
                                int numSubdivides,
                                OUT Vertex::XYZWUV** vertices,
                                OUT int& numVertices)
{
    
	Triangle t1( Vertex::XYZWUV(q.m_corners[0], Vector2(0.0f, 0.0f)),
                 Vertex::XYZWUV(q.m_corners[1], Vector2(1.0f, 0.0f)),
                 Vertex::XYZWUV(q.m_corners[2], Vector2(0.0f, 1.0f)));
	
    Triangle t2( Vertex::XYZWUV(q.m_corners[1], Vector2(1.0f, 0.0f)),
                 Vertex::XYZWUV(q.m_corners[3], Vector2(1.0f, 1.0f)),
                 Vertex::XYZWUV(q.m_corners[2], Vector2(0.0f, 1.0f)));			
	
	vector<Triangle> tris(1, t1);
	tris.push_back(t2);
	
	ASSERT(tris.size() == 2);
	
	
	
	doNaiveSubdivide(q, tris, numSubdivides);    
	//doAdaptiveSubdivide(q, tris, 0.02f);		
	numVertices = 3 * (tris.size());	
	
	*vertices = new Vertex::XYZWUV[numVertices];	
    
	FOR(i,0,tris.size()){		
        (*vertices)[3 * i + 0] = tris[i].A;
		(*vertices)[3 * i + 1] = tris[i].B;
		(*vertices)[3 * i + 2] = tris[i].C;
	}	
	
}

