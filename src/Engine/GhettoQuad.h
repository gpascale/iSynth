/*
 *  GhettoQuad.h
 *  Phonosynth
 *
 *  Created by Greg on 10/13/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _GHETTOQUAD_H_
#define _GHETTOQUAD_H_

#include <Drawable.h>
#include <Color.h>
#include <SceneGraph.h>

class Texture;

class GhettoQuad : public Drawable
{
	
public:
	
    // constructors
	GhettoQuad();
	GhettoQuad(const GhettoQuad& gq);	
	~GhettoQuad() { }	    
    
    // location
    int  getX()                      const { return m_x;       }
    int  getY()                      const { return m_y;       }
    void getLocation(int& x, int &y) const { x = m_x, y = m_y; }
    
    void setX(int x);
    void setY(int y);
    void setLocation(int x, int y);
    
    // size
    int  getWidth()                        const { return m_width;                     }
    int  getHeight()                       const { return m_height;                    }
    void getSize(int& width, int& height)  const { width = m_width, height = m_height; }
    
    void setWidth(float width);
    void setHeight(float height);
    void setSize(float width, float height);

	void setRotation(float angle);

	// sets the GhettoQuad's texture with the contents of an image
    bool           setTextureWithImage(const char*   textureFile, short width = -1, short height = -1);
    bool           setTextureWithImage(const string& textureFile, short width = -1, short height = -1);
	
	// sets the GhettoQuad's texture with the specified text
	bool           setTextureWithText(const char*   text, short width = -1, short height = -1,
									  const Color4F& textColor = Color4F::White(),
									  const Color4F& backgroundColor = Color4F::None());
    bool           setTextureWithText(const string& text, short width = -1, short height = -1,
									  const Color4F& textColor = Color4F::White(),
									  const Color4F& backgroundColor = Color4F::None());
	
	// sets the GhettoQuad's texture explicitly
    void           setTexture(Texture* t);    
	
    const Texture* getTexture();
    
    // color
    void  setColor(const Color4UC& cAll);
    void  setColor(const Color4UC& c1, const Color4UC& c2, const Color4UC& c3, const Color4UC& c4);
    void  getColor(OUT Color4UC& c1, OUT Color4UC& c2, OUT Color4UC& c3, OUT Color4UC& c4) const;    
    
    void  setOpacity(float newOpacity);
    float getOpacity() const;
    
    // creates a texture from the given file and applies it to the quad. Returns true on success    
	
	bool clickIntersects(int clickX, int clickY, int err = 3) const;		
	
    void draw(Drawer& d);      

    
    
private:
	
    void correctTexCoords();
    
    // location and position
    int             m_x;
    int             m_y;
    int             m_width;
    int             m_height;
    
    // color array
	Color4UC        m_colors[4];
    
    // texture (stored by GhettoQuad itself since they're all perloaded)
    Texture*        m_texture;
    
    // the projection matrix the GhettoQuad uses to draw itself. This is going to be
    // an orthographic projection matrix
    Matrix          m_projectionMatrix;
	
	Matrix			m_rotationMatrix;
	Matrix			m_scaleMatrix;
	Matrix			m_translationMatrix;
    
};

#endif // _GHETTOQUAD_H_

