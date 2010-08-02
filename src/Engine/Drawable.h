#ifndef _Drawable_H_
#define _Drawable_H_

#include <GMath.h>
#include <string>
#include <EngineTypes.h>
#include <VertexArray.h>
#include <Color.h>
#include <SceneGraph.h>

class Drawer;

using namespace std;

class AdjustableOpacity
{	
public:		
	virtual void	setOpacity(float f) = 0;
	virtual float	getOpacity() const  = 0;	
};



class AdjustableColor
{
		
public:	
	virtual void		   setColor(const Color4F& color) = 0;
	virtual const Color4F& getColor() const			      = 0;
};



class Drawable : public GeometryNode, public AdjustableColor, public AdjustableOpacity
{
public:

    // constructors and destructors
	Drawable(VertexArray* vertices = NULL, bool deepCopyVertices = false,
             IndexArray* indices = NULL, bool deepCopyIndices = false,
             const Matrix& mat = Matrix::IdentityCRef());    
    Drawable(VertexArray* vertices, bool deepCopyVertice = false,
             const Matrix& mat = Matrix::IdentityCRef());
	
	Drawable(const Drawable& rhs);
   
    virtual ~Drawable();	
	    
    // vertices
    void           setVertexArray(VertexArray* vertArray, bool deepCopy = false);
    void           setIndexArray(IndexArray* indexArray, bool deepCopy = false);
            
    // transform
    void           setWorldMatrix(const Matrix& m)       { m_worldMatrix = m;    }
	const Matrix&  getWorldMatrix()                const { return m_worldMatrix; }
    
    // basic properties for convenience
    void           setColor(const Color4F& c);
    const Color4F& getColor()                      const { return m_color;       }
	
	float		   getOpacity()					   const { return m_color.a;	 }
	void		   setOpacity(float f);
    
    void           setDrawMode(DrawMode mode)            { m_drawMode = mode;    }
    DrawMode       getDrawMode()                   const { return m_drawMode;    }
    
	
    // draw
    virtual void   draw(Drawer& d) = 0;	
	
	// override getActive to avoid ever visiting invisible quads when walking the scenegraph
	// should result in a pretty big speedup
	// bool   getActive() const { //cerr << "Drawable::getActive()" << endl; return m_color.a >= 0.01f && m_active; }

protected:
    
	
    // vertices
    VertexArray*        m_vertices;
    IndexArray*         m_indices;

    // matrices
    Matrix              m_textureMatrix;
    Matrix              m_worldMatrix;

    DrawMode            m_drawMode;
    
    // color
    Color4F             m_color;

	bool				m_deepCopiedVertices;
	bool				m_deepCopiedIndices;
	
private:
	
	 
	
};


#endif



#if 0 // oldness

virtual void setPosition(const Vector3 &v);
virtual void setRotation(const Vector3 &v);
virtual void setScale(const Vector3 &v);

virtual void getPosition(Vector3 &ret){ m_transform.getPosition(ret); }
virtual void getRotation(Vector3 &ret){ m_transform.getRotation(ret); }
virtual void getScale(Vector3 &ret)   { m_transform.getScale(ret);    }

virtual void translate(const Vector3 &v);
virtual void rotate(const Vector3 &v);
virtual void scale(const Vector3 &v);

inline void
Drawable::setPosition(const Vector3 &v)
{
	m_transform.applyAbsoluteTranslation(v);
}

inline void
Drawable::setRotation(const Vector3 &v)
{
	m_transform.applyAbsoluteRotation(v);
}

inline void
Drawable::setScale(const Vector3 &v)
{
	m_transform.applyAbsoluteScale(v);
}

inline void
Drawable::translate(const Vector3 &v)
{
	m_transform.applyRelativeTranslation(v);
}

inline void
Drawable::rotate(const Vector3 &v)
{
	m_transform.applyRelativeRotation(v);
}

inline void
Drawable::scale(const Vector3 &v)
{
	m_transform.applyRelativeScale(v);
}

#endif // oldness


