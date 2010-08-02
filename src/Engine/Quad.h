/*
 *  Quad.h
 *  Phonosynth
 *
 *  Created by Greg on 12/18/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _QUAD_H_
#define _QUAD_H_

#include <Drawable.h>
#include <GMath.h>
#include "Vertex.h"
#include <Subdivision.h>
#include <vector>
#include <map>
#include <GregJsonProjector.h>
#include "SceneGraph.h"

using namespace std;

class JsonImage;
class Drawer;
class Quad;
class PhonosynthImage;
class OpacitySpring;
class TextureAnimator;
class TextureManager;
class GenericCamera;


class Quad : public Drawable
{
	
	typedef map<Vertex::XYZ, int, FuzzyLessThan<Vertex::XYZ> > PositionMap;
	typedef map<Vertex::UV, int, FuzzyLessThan<Vertex::UV> > TexCoordMap;	
	
public:
    
    friend void computeSubdivisionVertices(Quad& q, int numSubdivides,
                                           OUT Vertex::XYZWUV** vertices, 
                                           OUT int& numVertices);
	
	Quad(GenericCamera* camera);
	Quad(const Quad &q);	
	~Quad();		
    
    void        initializeWithProjector(JsonProjector* proj); 
    
	
    bool        shouldRender() const { return m_color.a > .01f; }
    
    void                 draw(Drawer& d);
	
    const Vector4&       getPoint(int iPoint) const;
	const Vector4&       getCenter() const { return m_center; }    	
	
    void                 correctTexCoords(float uFactor, float vFactor);
     
	float				 getUCorrectionFactor() const { return m_uCorrectionFactor; }
	float				 getVCorrectionFactor() const { return m_vCorrectionFactor; }

    
    // Photosynth Specific stuff
    
    void        setWorldCoords(const Vector4& corner1,
                               const Vector4& corner2,
                               const Vector4& corner3,
                               const Vector4& corner4,
							   const Vector4& center);
	
	const Vector4& getDominantPlane()    const { return m_projector->getDominantPlane();    }
	const Point3&  getEyePosition()      const { return m_projector->getPosition();         }
    float		   getFOVY()		     const { return m_projector->getFOVY();             }
	float		   getAspectRatio()      const { return m_projector->getAspectRatio();      }
    const Matrix&  getViewMatrix()       const { return m_projector->getViewMatrix();       }
    const Matrix&  getProjectionMatrix() const { return m_projector->getProjectionMatrix(); }
     
    const string&			getTextureURL(int level) const;                                 
	void					getTextureURL(int level, OUT string& ret) const;
    
    int						getImageIndex() const { return m_imageIndex; }
    void					setImageIndex(const int n) { m_imageIndex = n; }
	
    void					setProjector(const JsonProjector* projector) { m_projector = projector; }
    const JsonProjector*	getProjector() const                         { return m_projector;      }
    
    void					setJsonImage(const JsonImage* image) { m_image = image; }
    const JsonImage*		getJsonImage() const                 { return m_image;  }
    
    void					setTextureFile(const string& textureFile);

    void                    setTextureAnimator(TextureAnimator* tAnim);
    void                    setOpacitySpring(OpacitySpring* spr);
	
	void					fadeOut(float duration);
	void					fadeIn(float toOpacity, float duration);
    
    TextureAnimator*        getTextureAnimator();
    const TextureAnimator*  getTextureAnimator() const;
    
    OpacitySpring*          getSpring();
    const OpacitySpring*    getSpring() const;        
    
    void                    set3DRendering(bool b);
    
		
	
private:
        
    // quads can render using either a 2d or 3d method. This function pointer points to
    // the appropriate function to use (either draw2D or draw3D)
    void (Quad::*m_drawFnPtr)(Drawer& d);    
    
    void                 draw2D(Drawer& d);
    void                 draw3D(Drawer& d);
    
    const JsonProjector* m_projector;
    const JsonImage*     m_image;
	
	void				 computeVertices(int numSubdivides = 0);	
    
    Point3               m_corners[4];
	Point3				 m_center;	
	
	float				 m_uCorrectionFactor;
	float				 m_vCorrectionFactor;    
    
    int                  m_imageIndex;
    
    TextureAnimator*     m_textureAnimator;
    OpacitySpring*       m_opacitySpring;
    
    // Quad needs to know about the camera so it can manually project itself into clip space for 2D drawing
    GenericCamera*       m_camera;
    
    /*    
     // this is a duplicate of the m_animator member of the base SceneGraphNode.
     // It exists so as not to have to do a dynamic cast on m_animator every time
     // we want to use OpacitySpring-specific functionality
     OpacitySpring*       m_spring;
     */
    
};

#endif
