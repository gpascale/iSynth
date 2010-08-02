/*
 *  Subdivision.h
 *  Phonosynth
 *
 *  Created by Greg on 10/12/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _SUBDIVISION_H_
#define _SUBDIVISION_H_

#include <GMath.h>
#include <map>
#include <vector>
#include <Vertex.h>

class Quad;

enum
SubdivideAction
{
	SUBDIVIDE,
	SPLIT_AB,
	SPLIT_BC,
	SPLIT_AC,
	NONE
};

typedef map< pair<Vertex::XYZWUV, Vertex::XYZWUV>, Vertex::XYZWUV > SubdivisionMap;

struct Triangle
{
	
	Vertex::XYZWUV A;
	Vertex::XYZWUV B;
	Vertex::XYZWUV C;
	
	Triangle(){ }	
    
	Triangle(const Vertex::XYZWUV &a, const Vertex::XYZWUV &b, const Vertex::XYZWUV &c) : 
        A(a), B(b), C(c) 
    { }	
    
	Triangle(const Triangle &t) : A(t.A), B(t.B), C(t.C) { }
    
	Triangle& operator = (const Triangle &t){ A = t.A; B = t.B; C = t.C; return *this; }	
	
    bool operator < (const Triangle &t) const {
		if(A < t.A) return true;
		if(A > t.A) return false;
		if(B < t.B) return true;
		if(B > t.B) return false;
		if(C < t.C) return true;
		if(C > t.C) return false;
        return true;
	}
    		
	Vector4 getOrthoCenter() const {
		return (A.position() + B.position() + C.position()) / 3.0f;
    }
    
	float getDistortion(const Matrix &viewMatrix, const Matrix &projectionMatrix);		
	float screenSpaceArea(const Matrix &viewMatrix, const Matrix &projectionMatrix);
		
	void splitAB(vector<Triangle> &ret, const Quad &q, SubdivisionMap &m) const;	
	void splitBC(vector<Triangle> &ret, const Quad &q, SubdivisionMap &m) const;	
	void splitAC(vector<Triangle> &ret, const Quad &q, SubdivisionMap &m) const;
		
	void subdivide(vector<Triangle> &ret, const Quad &q) const;		
	void subdivide(vector<Triangle> &ret, const Quad &q, SubdivisionMap &m) const;
	
};


extern Vertex::XYZWUV interpolateVertex(const Vertex::XYZWUV &v1, 
                                        const Vertex::XYZWUV &v2, 
                                        const Quad &q);

extern void doNaiveSubdivide(Quad& q, 
                             vector<Triangle>& v, 
                             int depth);

extern void doAdaptiveSubdivide(Quad& q, 
                                vector<Triangle> &v, 
                                float maxDistortion);

extern void computeSubdivisionVertices(Quad& q, 
                                       const int numSubdivides, 
                                       OUT Vertex::XYZWUV** positions, 
                                       OUT int& numVertices);





#endif