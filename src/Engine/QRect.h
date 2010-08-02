/*
 *  QRect.h
 *  Phonosynth
 *
 *  Created by Greg on 1/28/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#ifndef _QRECT_H_
#define _QRECT_H_

#include <GMath.h>
#include <Drawable.h>
#include <Vertex.h>
#include <Drawer.h>
#include <SceneGraph.h>
#include <PSTypes.h>
#include <System.h>
#include <GenericCamera.h>
#include <Functions.h>

class QRect : public Drawable
{
	
public:
	
	QRect(const JsonProjector* projector) :
		Drawable(),
		m_dominantPlane(0.0f, 0.0f, 0.0f, 0.0f),
		m_on(false)
	{
		projector->getQuadCorners(m_corners[0], m_corners[1], m_corners[3], m_corners[2]);
		setVertexArray(new VertexArray(Vertex::XYZW::getDesc(), (float*) m_corners, 4, false), false);
		
		setDrawPriority(PRIORITY_QRECT);
		setDrawMode(DRAWMODE_LINE_LOOP);
		
		setActive(false);
		setOpacity(0.0f);
		
		m_dominantPlane = planeFrom3Points(m_corners[0], m_corners[1], m_corners[2]);
		
		// configure the opacity spring
		m_spring = new OpacitySpring();
		m_spring->setTarget(this);
		Drawable::addAnimator(m_spring);
	}
	
	~QRect()
	{
		// vertex array is not deep-copied, so we delete it here in the subclass
		SAFE_DELETE(m_vertices);
	}
		
		
	float hitTest(const Vector2& p, const GenericCamera& camera) const
	{						
		const Vector3 camDir(p.x, p.y, camera.getFocalLength());
		const Vector3 worldDir = (camDir * camera.getViewMatrix()) - camera.getPosition().xyz();		
		Ray ray(camera.getPosition(), worldDir);
		
		Point3 planeIsectPt;
		float dist = rayPlaneIntersect(ray, m_dominantPlane, planeIsectPt);
		
		if(dist >= 0.0f){
		
			// temporary, use screen space intersection testing. World space will be more efficient so I
			// need to implement that soon
			const Matrix viewMatrix = camera.getViewMatrix();
			const Matrix projMatrix = camera.getProjectionMatrix();
			
			const Point3 c1P = projectAndHomogenize4(m_corners[0], viewMatrix, projMatrix);
			const Point3 c2P = projectAndHomogenize4(m_corners[1], viewMatrix, projMatrix);
			const Point3 c3P = projectAndHomogenize4(m_corners[2], viewMatrix, projMatrix);
			const Point3 c4P = projectAndHomogenize4(m_corners[3], viewMatrix, projMatrix);			
			
			if((fabs(c1P.x) > 1.0f || fabs(c1P.y) > 1.0f) &&
			   (fabs(c2P.x) > 1.0f || fabs(c2P.y) > 1.0f) &&
			   (fabs(c3P.x) > 1.0f || fabs(c3P.y) > 1.0f) &&
			   (fabs(c4P.x) > 1.0f || fabs(c4P.y) > 1.0f))
				return FLT_MAX;
				
				
			if(c1P.w < 0 && c2P.w < 0 && c3P.w < 0)
				if(pointInsideTriangle(p, c1P.xy(), c2P.xy(), c3P.xy())){
					return triangleArea(c1P.xy(), c2P.xy(), c3P.xy());
				}
			
			if(c3P.w < 0 && c4P.w < 0 && c1P.w < 0)
				if(pointInsideTriangle(p, c3P.xy(), c4P.xy(), c1P.xy())){
					return triangleArea(c3P.xy(), c4P.xy(), c1P.xy());
				}
			
			
		}
		
		// no intersection
		return FLT_MAX;
	}
	
	
	inline const Plane& getDominantPlane()
	{		
		return m_dominantPlane;
	}
	
	
	// makes a QRect potentially visible (so it can show up if a finger is dragged over it)
	inline void turnOn()
	{
		m_on = true;
	}
	
	
	inline void turnOff()
	{
		m_on = false;
	}
	
	
	inline bool isOn() const
	{
		return m_on;
	}
	
	
	inline void show(float duration = 0.1f)
	{
		setActive(true);
		setOpacity(1.0f); // was fading in, but this is more responsive
		m_spring->setToOpacity(1.0f, 0.001f);
		m_spring->setEndFunctor(Functor::Identity());
	}
	
	
	inline void hide(float duration = 0.5f)
	{
		m_spring->setToOpacity(0.0f, duration);
		m_spring->setEndFunctor(MemberFunctorA<Drawable, bool>(this, &Drawable::setActive, false));
	}
	
	
	inline void draw(Drawer& d)
	{
		d.setColor(m_color);
		d.setVertexSource(m_vertices);
		d.draw(4, NULL);
	}
	
	
private:
	
	OpacitySpring*	 m_spring;
	
	Plane			 m_dominantPlane;
	Vertex::XYZW	 m_corners[4];
	
	bool			 m_on;
	
};


#endif // _QRECT_H_


