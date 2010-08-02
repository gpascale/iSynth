/*
 *  Vertex.h
 *  Phonosynth
 *
 *  Created by Greg on 12/19/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <GMath.h>
#include <Color.h>

class VertexDesc
{
public:
    
    VertexDesc(int nPos, int nTex, int nCol, int nNorm) :
        m_nPosition(nPos),
        m_positionOffset(0),
        m_nTexture(nTex),
        m_texCoordOffset(nPos * sizeof(float)),
        m_nColor(nCol),
        m_colorOffset((nPos + nTex) * sizeof(float)),
        m_nNormal(nNorm),
        m_normalOffset((nPos + nTex + nCol) * sizeof(float)),
        m_size((nPos + nTex + nCol + nNorm) * sizeof(float))
    {
    
    }
    
    inline int getNumPositionComponents() const { return m_nPosition;      }
    inline int getNumTexCoordComponents() const { return m_nTexture;       }
    inline int getNumColorComponents()    const { return m_nColor;         }
    inline int getNumNormalComponents()   const { return m_nNormal;        }
    
    inline int getPositionOffset()        const { return m_positionOffset; }
    inline int getTexCoordOffset()        const { return m_texCoordOffset; }
    inline int getColorOffset()           const { return m_colorOffset;    }
    inline int getNormalOffset()          const { return m_normalOffset;   }
    
    inline int getSize()                  const { return m_size;           }
    
private:
    
    int m_nPosition;
    int m_nTexture;
    int m_nColor;
    int m_nNormal;
    
    int m_positionOffset;
    int m_texCoordOffset;
    int m_colorOffset;
    int m_normalOffset;
    
    int m_size; // in bytes
    
};


namespace Vertex{


// vertex with only position
struct XYZ
{
    // constructors
    XYZ()                                                { }
    XYZ(const Vector3& v) : m_position(v)                { }
	XYZ(const Point3& v) : m_position(v.xyz())			 { }
    XYZ(const float* v) : m_position(v)                  { }
    XYZ(float x, float y, float z) : m_position(x, y, z) { }      
    XYZ(const XYZ& rhs) : m_position(rhs.m_position)     { }
    
    // conversions
    inline operator Vector3&       ()       { return m_position;          }
    inline operator const Vector3& () const { return m_position;          }
    inline operator float*         ()       { return (float*) this;       }
    inline operator const float*   () const { return (const float*) this; }                
    
    // Vertex Description
    static const VertexDesc& getDesc()
    {
        static VertexDesc desc(3, 0, 0, 0);
        return desc;
    }        
    
private:
    Vector3		m_position;        
};


inline
ostream& operator << (ostream& os, const XYZ& rhs)
{
    return os << "Vertex::XYZ: Position - " << (const Vector3&) rhs;
}

    
// vertex with 4-d position
struct XYZW
{
    // constructors
    XYZW()															  { }
	XYZW(const Vector3& v) : m_position(v, 1.0f)				      { }
    XYZW(const Point3& v) : m_position(v)                             { }
    XYZW(const float* v) : m_position(v)                              { }
    XYZW(float x, float y, float z, float w) : m_position(x, y, z, w) { }      
    XYZW(const XYZW& rhs) : m_position(rhs.m_position)                { }
    
    // conversions
    inline operator Vector4&       ()       { return m_position;          }
    inline operator const Vector4& () const { return m_position;          }
    inline operator float*         ()       { return (float*) this;       }
    inline operator const float*   () const { return (const float*) this; }                
    
    // Vertex Description
    static const VertexDesc& getDesc()
    {
        static VertexDesc desc(4, 0, 0, 0);
        return desc;
    }
    
private:
    Vector4		m_position;        
}; 

inline
ostream& operator << (ostream& os, const XYZW& rhs)
{
    return os << "Vertex::XYZW: Position - " << (const Vector4&) rhs;
}    
    
    
// vertex with only texture coordinates
struct UV
{
    // constructors
    UV()                                             { }        
    UV(const Vector2& v) : m_texCoords(v)            { }
    UV(const float* v) : m_texCoords(v)              { }
    UV(float u, float v) : m_texCoords(u, v)         { }                
    UV(const UV& rhs) :	m_texCoords(rhs.m_texCoords) { }
            
    // conversions
    inline operator Vector2&       ()       { return m_texCoords;         }
    inline operator const Vector2& () const { return m_texCoords;         }
    inline operator float*         ()       { return (float*) this;       }
    inline operator const float*   () const { return (const float*) this; }
    
    // Vertex Description
    static const VertexDesc& getDesc()
    {
        static VertexDesc desc(0, 2, 0, 0);
        return desc;
    }
    
private:            
    Vector2 m_texCoords;        
};	

inline
ostream& operator << (ostream& os, const UV& rhs)
{
    return os << "Vertex::XYZ: TexCoords - " << (const Vector2&) rhs;
}



// vertex with position and color
struct XYZRGB
{
    // constructors
    XYZRGB()                                                                     { }                
    XYZRGB(const Vector3 &p, const Color3F &c) : m_position(p), m_color(c)       { }
    XYZRGB(const float* p, const float* c) : m_position(p), m_color(c)           { }
    XYZRGB(float x, float y, float z, float r, float g, float b) :
        m_position(x, y, z), m_color(r, g, b)                                    { }
    XYZRGB(const XYZRGB& rhs) : m_position(rhs.m_position), m_color(rhs.m_color) { }
    
    // conversions
    inline operator float*         ()       { return (float*) this;       }
    inline operator const float*   () const { return (const float*) this; }
    
    // access to components
    inline const Vector3& position() const { return m_position; }
    inline       Vector3& position()       { return m_position; }
    inline const Vector3& color()    const { return m_color; }
    inline       Vector3& color()          { return m_color; }
    
    // vertex description        
    static const VertexDesc& getDesc()
    {
        static VertexDesc desc(3, 0, 3, 0);
        return desc;
    }
    
private:        
    Vector3		m_position;
    Vector3		m_color;        
};

inline
ostream& operator << (ostream& os, const XYZRGB& rhs)
{
    return os << "Vertex::XYZW: Position - " << rhs.position() 
              << " Color - " << rhs.color();
}


// vertex with position, color including alpha, and texture coordinates
// vertex with position and color
struct XYZRGBAUV
{
    // constructors
    XYZRGBAUV()                                                                       { }                
    XYZRGBAUV(const Vector3 &p, const Color4F& c, const Vector2& t) : 
        m_position(p), m_color(c), m_texCoords(t)                                     { }
    XYZRGBAUV(const float* p, const float* c, const float* t) : 
        m_position(p), m_color(c), m_texCoords(t)                                     { }
    XYZRGBAUV(float x, float y, float z, 
              float r, float g, float b, float a,
              float u, float v) :
        m_position(x, y, z), m_color(r, g, b), m_texCoords(u, v)                      { }
    XYZRGBAUV(const XYZRGBAUV& rhs) : 
        m_position(rhs.m_position), m_color(rhs.m_color), m_texCoords(rhs.m_texCoords) { }
    
    // conversions
    inline operator float*         ()       { return (float*) this;       }
    inline operator const float*   () const { return (const float*) this; }
    
    // access to components
    inline const Vector3& position()  const { return m_position;  }
    inline       Vector3& position()        { return m_position;  }
    inline const Color4F& color()     const { return m_color;     }
    inline       Color4F& color()           { return m_color;     }
    inline const Vector2& texCoords() const { return m_texCoords; }
    inline       Vector2& texCoords()       { return m_texCoords; }
    
    
    // vertex description        
    static const VertexDesc& getDesc()
    {
        static VertexDesc desc(3, 2, 4, 0);
        return desc;
    }
    
private:        
    Vector3		m_position;
    Color4F		m_color;
    Vector2     m_texCoords;
};

inline
ostream& operator << (ostream& os, const XYZRGBAUV& rhs)
{
    return os << "Vertex::XYZRGBAUV: Position - " << rhs.position() 
              << " Color - " << rhs.color() 
              << " TexCoords - " << rhs.texCoords();
}


// vertex with 2d-position and texture coordinates
struct XYUV
{
    XYUV()                                                          { }
    XYUV(const Vector2& p, const Vector2& t) :
        m_position(p), m_texCoords(t)                               { }
    XYUV(float x, float y, float u, float v) :
        m_position(x, y), m_texCoords(u, v)                         { }
    XYUV(const float* p, const float* t) :
        m_position(p), m_texCoords(t)                               { }        
    XYUV(const XYUV& rhs) :
        m_position(rhs.m_position), m_texCoords(rhs.m_texCoords)    { }
    
    // conversions
    inline operator float*         ()       { return (float*) this;       }
    inline operator const float*   () const { return (const float*) this; }
    
    // access to components
    inline const Vector2& position()  const { return m_position;  }
    inline       Vector2& position()        { return m_position;  }
    inline const Vector2& texCoords() const { return m_texCoords; }
    inline       Vector2& texCoords()       { return m_texCoords; }
    
    // Vertex Description
    static const VertexDesc& getDesc()
    {
        static VertexDesc desc(2, 2, 0, 0);
        return desc;
    }
    
private:        
    Vector2 m_position;
    Vector2 m_texCoords;        
};

inline
ostream& operator << (ostream& os, const XYUV& rhs)
{
    return os << "Vertex::XYUV: Position - " << rhs.position() 
              << " TexCoords - " << rhs.texCoords();
}


// vertex with position and texture coordinates
struct XYZUV
{        
    // constructors
    XYZUV()                                                         { }
    XYZUV(const Vector3& p, const Vector2& t) :
        m_position(p), m_texCoords(t)                               { }
    XYZUV(float x, float y, float z, float u, float v) :
        m_position(x, y, z), m_texCoords(u, v)                      { }
    XYZUV(const float* p, const float* t) :
        m_position(p), m_texCoords(t)                               { }        
    XYZUV(const XYZUV& rhs) : 
        m_position(rhs.m_position), m_texCoords(rhs.m_texCoords)    { }
    
    // conversions
    inline operator float*         ()       { return (float*) this;       }
    inline operator const float*   () const { return (const float*) this; }
    
    // access to components
    inline const Vector3& position()  const { return m_position;  }
    inline       Vector3& position()        { return m_position;  }
    inline const Vector2& texCoords() const { return m_texCoords; }
    inline       Vector2& texCoords()       { return m_texCoords; }
    
    // Vertex Description
    static const VertexDesc& getDesc()
    {
        static VertexDesc desc(3, 2, 0, 0);
        return desc;
    }
    
private:        
    Vector3 m_position;
    Vector2 m_texCoords;        
};

inline
ostream& operator << (ostream& os, const XYZUV& rhs)
{
    return os << "Vertex::XYZUV: Position - " << rhs.position() 
              << " TexCoords - " << rhs.texCoords();
}


// vertex with homegeneous position and texture coordinates
struct XYZWUV
{        
    // constructors
    XYZWUV()                                                        { }
    XYZWUV(const Vector4& p, const Vector2& t) :
        m_position(p), m_texCoords(t)                               { }
    XYZWUV(float x, float y, float z, float w, float u, float v) :
        m_position(x, y, z, w), m_texCoords(u, v)                   { }
    XYZWUV(const float* p, const float* t) :
        m_position(p), m_texCoords(t)                               { }        
    XYZWUV(const XYZWUV& rhs) : 
        m_position(rhs.m_position), m_texCoords(rhs.m_texCoords)    { }
    
    // conversions
    inline operator float*         ()       { return (float*) this;       }
    inline operator const float*   () const { return (const float*) this; }
    
    // access to components
    inline const Vector4& position()  const { return m_position;  }
    inline       Vector4& position()        { return m_position;  }
    inline const Vector2& texCoords() const { return m_texCoords; }
    inline       Vector2& texCoords()       { return m_texCoords; }
    
    // Vertex Description
    static const VertexDesc& getDesc()
    {
        static VertexDesc desc(4, 2, 0, 0);
        return desc;
    }
    
private:        
    Vector4 m_position;
    Vector2 m_texCoords;        
};

inline
ostream& operator << (ostream& os, const XYZWUV& rhs)
{
    return os << "Vertex::XYZWUV: Position - " << rhs.position() 
              << " TexCoords - " << rhs.texCoords();
}


} // end namespace


#endif




/*
 inline
 bool
 UV::fuzzyLessThan(const UV &rhs)
 {
 if(u < rhs.u) return true;
 else if (u > rhs.u) return false;
 if(v < rhs.v) return true;
 else if(v > rhs.v) return false;
 return false;
 }
 */


/*
 inline
 bool 
 XYZ::fuzzyLessThan(const XYZ &rhs)
 {
 const Vector3& rhsV = rhs;
 if(m_position.x < rhsV.x) return true;
 else if (m_position.x > rhsV.x) return false;	
 if(m_position.y < rhsV.y) return true;
 else if (m_position.y > rhsV.y) return false;
 if(m_position.z < rhsV.z) return true;
 else if (m_position.z > rhsV.z) return false;
 return false;
 }
 */


/*
 
 struct RGBA
 {
 RGBA(const Color4F &color = Color4F(1.0f, 1.0f, 1.0f, 1.0f)) :
 m_color(color)
 { }
 
 RGBA(float r, float g, float b, float a) :
 m_color(r, g, b, a)
 { }
 
 RGBA& operator = (const Color4F& c){ m_color = c; return *this; }
 
 operator const Color4F& () const { return m_color; }
 operator const float*  () const { return m_color.getConstDataPointer(); }
 
 operator Color4F& () { return m_color; }
 operator float*  () { return (float*) m_color; }
 
 private:
 
 Color4F		m_color;
 
 };
 
 
 struct RGBA32
 {
 RGBA32(){ memset(m_color, 0x00, sizeof(m_color)); }	
 
 RGBA32(unsigned char *c) 
 { 
 memcpy(m_color, c, 4 * sizeof(unsigned char));
 }
 
 RGBA32(unsigned char r, unsigned char g, unsigned char b, unsigned char a) 
 { 
 m_color[0] = r;
 m_color[1] = g;
 m_color[2] = b;
 m_color[3] = a;
 }
 
 RGBA32& operator = (int n){ *m_color = n; return *this; }
 
 operator const unsigned char* () const { return m_color;        }
 operator unsigned char*		  ()	   { return	m_color;        }
 operator int				  () const { return (int) *m_color; }
 
 private:
 
 unsigned char		m_color[4];
 
 };
 
 */

/*
 // vertex with position, color (as 4 chars), and texture coordinates
 struct XYZCUV
 {
 XYZCUV() : m_position(), r(0), g(0), b(0), a(1), u(0.0f), v(0.0f)
 {}
 
 XYZCUV(const Vector3 &p, const unsigned char *color, float _u, float _v) :
 m_position(p), u(_u), v(_v)
 { 
 ASSERT(color != NULL);
 r = color[0];
 g = color[1];
 b = color[2];
 a = color[3];
 }
 
 XYZCUV(const Vector3 &p, const Color4F& color, float _u, float _v) :
 m_position(p), u(_u), v(_v)
 { 
 r = color[0];
 g = color[1];
 b = color[2];
 a = color[3];
 }
 
 XYZCUV(float x, float y, float z, 
 const unsigned char *color,
 float _u, float _v) :
 m_position(x, y, z), u(_u), v(_v)
 { 
 ASSERT(color != NULL);
 r = color[0];
 g = color[1];
 b = color[2];
 a = color[3];
 }            
 
 XYZCUV(float x, float y, float z, 
 unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a,
 float _u, float _v) :
 m_position(x, y, z), u(_u), v(_v)
 { 
 r = _r;
 g = _g;
 b = _b;
 a = _a;
 }
 
 bool operator < (const XYZCUV &rhs) const {
 if(m_position.x < rhs.getPosition().x) return true;
 if(m_position.x > rhs.getPosition().x) return false;
 if(m_position.y < rhs.getPosition().y) return true;
 if(m_position.y > rhs.getPosition().y) return false;
 if(m_position.z < rhs.getPosition().z) return true;
 return false;
 }
 
 bool operator > (const XYZCUV &rhs) const {
 if(m_position.x > rhs.getPosition().x) return true;
 if(m_position.x < rhs.getPosition().x) return false;
 if(m_position.y > rhs.getPosition().y) return true;
 if(m_position.y < rhs.getPosition().y) return false;
 if(m_position.z > rhs.getPosition().z) return true;
 return false;
 }
 
 const Vector3&	getPosition() const { return m_position; }
 const float		getU() const { return u; }
 const float		getV() const { return v; }
 void			getTexCoords(float &uRet, float &vRet) const { uRet = u; vRet = v; }
 
 Vector3		    m_position;
 unsigned char   r, g, b, a;
 float           u, v;    
 
 static const VertexDesc& getDesc()
 {
 static VertexDesc desc(3, 2, 4, 0);
 return desc;
 }
 
 };
 */