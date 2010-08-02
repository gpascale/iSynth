/*
 *  CameraCone.h
 *  Phonosynth
 *
 *  Created by Greg on 10/11/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include <GMath.h>
#include <Drawable.h>
#include <Vertex.h>
#include <Drawer.h>
#include <SceneGraph.h>
#include <PSTypes.h>
#include <System.h>
#include <SharedVertices.h>

class CameraCone : public Drawable
{
        
public:
        
    CameraCone(const JsonProjector* projector, float scale = 1.0f) :
        Drawable()
    {
        
        
		m_corners[0] = projector->getPosition(); // eye point
        projector->getFrustumCorners(m_corners[1], m_corners[2], m_corners[3], m_corners[4], 0.25f);
        
		unsigned short indices[16];
        indices[0]  = 0;
        indices[1]  = 1;
        indices[2]  = 0;
        indices[3]  = 2;
        indices[4]  = 0;
        indices[5]  = 3;
        indices[6]  = 0;
        indices[7]  = 4;
            
        indices[8]  = 1;
        indices[9]  = 2;
        indices[10] = 2;
        indices[11] = 4;
        indices[12] = 4;
        indices[13] = 3;
        indices[14] = 3;
        indices[15] = 1;
        
        setVertexArray(new VertexArray(Vertex::XYZW::getDesc(), (float*) m_corners, 5, false), false);
        setIndexArray(new IndexArray(indices, 16, true), false);
		
		setDrawPriority(PRIORITY_CC);
		
		/*
		SharedVertices* sv = SharedVertices::getCameraConeSharedVertices();
		setVertexArray(sv->getSharedVertexArray(), false);
		setIndexArray(sv->getSharedIndexArray(), false);		 		
		
		Point3 p = projector->getPosition();				
		Matrix r, s, t;
						
		const Vector3 L = projector->getLookVector();
		const Vector3 S = Vector3(0.0f, 0.0f, 1.0f).cross(L);
		const Vector3 U = S.cross(L);
		r.setRowVector(2, L);
		r.setRowVector(1, U);
		r.setRowVector(0, S);		
				
		float sAmt = scale / 40.0f;
		
		Matrix::ScaleMatrix(s, sAmt, sAmt, sAmt);
		Matrix::TranslationMatrix(t, p.x, p.y, p.z);		
		setWorldMatrix(s * r * t);		
		*/
		
    }
	
	~CameraCone(){
		SAFE_DELETE(m_vertices);
		SAFE_DELETE(m_indices);
	}
        
    void  draw(Drawer& d);		
           
    float hitTest(const Vector2& p, const Matrix& viewMatrix, const Matrix& projMatrix)
	{
		const Point3 eye = Point3(m_vertices->getVertex(0));
		const Point3 c1  = Point3(m_vertices->getVertex(1));
		const Point3 c2  = Point3(m_vertices->getVertex(2));
		const Point3 c3  = Point3(m_vertices->getVertex(3));
		const Point3 c4  = Point3(m_vertices->getVertex(4));
		
		const Point3 eyeP = projectAndHomogenize4(eye, m_worldMatrix, viewMatrix, projMatrix);
		const Point3 c1P  = projectAndHomogenize4(c1, m_worldMatrix, viewMatrix, projMatrix);
		const Point3 c2P  = projectAndHomogenize4(c2, m_worldMatrix, viewMatrix, projMatrix);
		const Point3 c3P  = projectAndHomogenize4(c3, m_worldMatrix, viewMatrix, projMatrix);
		const Point3 c4P  = projectAndHomogenize4(c4, m_worldMatrix, viewMatrix, projMatrix);
		
		if(eyeP.w < 0 && c1P.w < 0 && c2P.w < 0)
			if(pointInsideTriangle(p, eyeP.xy(), c1P.xy(), c2P.xy())){
				//cerr << "eyeP = " << eyeP << ", c1P = " << c1P << ", c2P = " << c2P << " t = " << p << endl;
				return -MAX3(eyeP.w, c1P.w, c2P.w);
			}
		if(eyeP.w < 0 && c1P.w < 0 && c3P.w < 0)
			if(pointInsideTriangle(p, eyeP.xy(), c1P.xy(), c3P.xy())){
				//cerr << "eyeP = " << eyeP << ", c1P = " << c1P << ", c2P = " << c2P << " t = " << p << endl;
				return -MAX3(eyeP.w, c1P.w, c3P.w);
			}
		if(eyeP.w < 0 && c3P.w < 0 && c4P.w < 0)
			if(pointInsideTriangle(p, eyeP.xy(), c3P.xy(), c4P.xy())){
				//cerr << "eyeP = " << eyeP << ", c1P = " << c1P << ", c2P = " << c2P << " t = " << p << endl;
				return -MAX3(eyeP.w, c3P.w, c4P.w);
			}
		if(eyeP.w < 0 && c2P.w < 0 && c4P.w < 0)
			if(pointInsideTriangle(p, eyeP.xy(), c2P.xy(), c4P.xy())){
				//cerr << "eyeP = " << eyeP << ", c1P = " << c1P << ", c2P = " << c2P << " t = " << p << endl;
				return -MAX3(eyeP.w, c2P.w, c4P.w);
			}
		
		return FLT_MAX;
    }
            
private:
        
	Vertex::XYZW	 m_corners[5];
	
};


inline void
CameraCone::draw(Drawer& d)
{	
	d.setVertexSource(m_vertices);
	//d.setWorldMatrix(m_worldMatrix);
	d.draw(16, m_indices->getIndices(), FMT_USHORT);
}




