/*
 *  GenericCamera.h
 *  Phonosynth
 *
 *  Created by Greg on 10/27/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */


// An interface for a basic camera. Implementations include SpringyCamera and ArcballCamera

#ifndef _GenericCamera_H_
#define _GenericCamera_H_

#include <GMath.h>

class Drawer;

class GenericCamera
{
    
public:
    
    GenericCamera();
    virtual ~GenericCamera();
    
    void setDrawer(Drawer* d);
    
    // simple getters
    float               getAspectRatio()	  const { return m_aspectRatio;                    }
    float               getFocalLength()	  const { return m_focalLength;                    }
    float				getZNear()            const { return m_zNear;                          }
	float				getZFar()             const { return m_zFar;                           }
    float               getArcballRadius()    const { return m_arcballRadius;                  }
    const Point3&       getPosition()		  const { return m_position;                       }
    const Point3&       getArcballCenter()    const { return m_arcballCenter;                  }    
    const Matrix&       getViewMatrix()		  const { return m_viewMatrix;                     }
	const Matrix&		getProjectionMatrix() const { return m_projectionMatrix;               }
    Quaternion          getQuaternion()       const { return m_viewMatrix.toQuaternion();      }    		
	Vector3             getLookVector()       const { return m_viewMatrix.getColumnVector3(2); }
	Vector3             getUpVector()         const { return m_viewMatrix.getColumnVector3(1); }
    Vector3             getSideVector()       const { return m_viewMatrix.getColumnVector3(0); }    


    virtual void        setArcballCenter(const Point3& p);
    virtual void        setArcballRadius(float r);
    
    virtual void        setPosition(const Point3 &p);
    
    virtual void        setRotation(const Quaternion &rot, bool adjustUpVector = false);    
    
	virtual void		setOrientation(const Point3 &pos, const Vector3 &look, const Vector3 &up);
    
    virtual void        setOrientation(const Point3 &pos, 
                                       const Quaternion &rot, 
                                       bool adjustUpVector = false);
    
    // lens adjustment
    virtual void			setLens(float aspect, 
                                    float focalLength,
                                    float zNear,
                                    float zFar,
                                    float screenOrientation);	// 0.0f = portrait
    
	
    // standard movement controls
    virtual void            moveForward(float amt);
    virtual void            moveBackward(float amt);
    virtual void            moveLeft(float amt);
    virtual void            moveRight(float amt);
    virtual void            moveUp(float amt);
    virtual void            moveDown(float amt);
    virtual void            rotateUp(float amt);	
    virtual void            rotateDown(float amt);
    virtual void            rotateLeft(float amt);
    virtual void            rotateRight(float amt);
	virtual void			rollRight(float amt);
	virtual void			rollLeft(float amt);
	virtual void			zoomIn(float amt);
    virtual void			zoomOut(float amt);
    
    // arcball rotation
    virtual void            rotateLeftArcball(float amt);
    virtual void            rotateRightArcball(float amt);
    virtual void            rotateUpArcball(float amt);
    virtual void            rotateDownArcball(float amt);
    
    virtual void            moveForwardArcball(float amt);
    virtual void            moveBackwardArcball(float amt);
    virtual void            moveLeftArcball(float amt);
    virtual void            moveRightArcball(float amt);
    virtual void            moveUpArcball(float amt);
    virtual void            moveDownArcball(float amt);
    
    virtual void            translate(const Vector3 &dir);
    //virtual void            rotate(const Vector3& axis, const float amt, const Vector3& p = Vector3(0,0,0));

    
protected:
    
    static void rotateLeft(OUT Matrix &newViewMat,
                           const Matrix &oldViewMat, 
                           float amt, 
                           const Point3& aroundPt = Point3::Origin());	
    
	static void rotateRight(OUT Matrix &newViewMat, 
                            const Matrix &oldViewMat,
                            float amt,
                            const Point3& aroundPt = Point3::Origin());	
    
	static void rotateUp(OUT Matrix &newViewMat,
                         const Matrix &oldViewMat,
                         float amt,
                         const Point3& aroundPt = Point3::Origin());
    
	static void rotateDown(OUT Matrix &newViewMat,
                           const Matrix &oldViewMat,
                           float amt,
                           const Point3& aroundPt = Point3::Origin());
    
    // lens parameters
    float           m_aspectRatio;
    float           m_focalLength;       
	float			m_zNear;
	float			m_zFar;    
    
    // orientatio info
    Quaternion      m_quaternion;
	Matrix			m_viewMatrix;
	Matrix			m_projectionMatrix;
    
    Point3          m_position;
        
    // arcball-mode stuff
    Point3          m_arcballCenter;
    float           m_arcballRadius;

	
    Drawer*         m_drawer;

};

#include "GenericCamera.hpp"

#endif // _GENERICCAMERA_H_


