#ifndef _VECTOR4_H_
#define _VECTOR4_H_

#include <common.h>

class Vector2;
class Vector3;

class Vector4
{
public:

    // constructors
    Vector4(float _x, float _y, float _z, float _w);
    Vector4(const float *v);
    Vector4(const Vector4 &v);
	// w is intentionally not given a default parameter
	// do not want implicit conversion
    Vector4(const Vector2 &v, const float _z, const float _w);
    Vector4(const Vector3 &v, const float _w);    
    Vector4();
    ~Vector4(){ /* everything's on the stack */ }

    // access
    float&          operator [] (const int n);
    const float&    operator [] (const int n)         const;
	
    operator const float* () const { return (const float*) &x; }
    operator float* ()			   { return (      float*) &x; }
    
    // Vector4 - Vector4 operations
    Vector4         operator + (const Vector4 &v)     const;
    Vector4         operator - (const Vector4 &v)     const; 
    Vector4&		operator =  (const Vector4 &v);
    Vector4&        operator += (const Vector4 &v);
    Vector4&        operator -= (const Vector4 &v);
    
    // Vector4 - scalar operations
    Vector4         operator * (const float f)        const;
    Vector4         operator / (const float f)        const;	    
    Vector4&        operator *= (const float f);
    Vector4&        operator /= (const float f);
    
    // Vector4 - Vector3 operations. These exist solely for purposes of adding a 
    // Vector3 to a Point3, which is stored as a Vector 4, which is why they are
    // only defined for the case where the Vector4 (point) is on the left. Be 
    // careful using these
    
    Vector4         operator +  (const Vector3& v)     const;
    Vector4         operator -  (const Vector3& v)     const;
    Vector4&        operator += (const Vector3& v);
    Vector4&        operator -= (const Vector3& v);
   
    
	// treat a Vector4 as a Vector3 or Vector2 by ignoring elements
    const Vector3& xyz() const { 
        return reinterpret_cast<const Vector3&> (*this);
    }
	const Vector3& yzw() const {
		return reinterpret_cast<const Vector3&> (*(&y));
	}		
	const Vector2& xy()  const {
		return reinterpret_cast<const Vector2&> (*this);
	}
	const Vector2& yz()  const {
		return reinterpret_cast<const Vector2&> (*(&y));
	}
	const Vector2& zw()  const {
		return reinterpret_cast<const Vector2&> (*(&z));
	}

    // in the future, this code could be made faster by using this
    // function in tandem with memcpy for things like copy constructors
    // for now, I just do that stuff with loop and assign
    float*          getDataPointer(){ return &x; } 
	void			setData(const float *f);
	
    // this is kind of a lie, need to only use this for read access
    const float*    getConstDataPointer() const { return &x; }
    void            zero(){ memset(m_data, 0x00, sizeof(m_data)); }

    // other math
    float           magnitudeSq()                     const;
    float           magnitude()                       const;
    float           dot(const Vector4 &v)             const;
    Vector4         cross(const Vector4 &v)           const;    
    Vector3         homogenized3()                    const;    
    Vector4         homogenized4()                    const;
    Vector4			normalized()					  const;
    Vector4&        homogenize4();
    Vector4&        normalize();	    

    static void     copyVector4(Vector4 &out, const Vector4 &in);
	static void		copyVector4ToVector3(Vector3 &out, const Vector4 &in);

    // some common Vector4s
    static const Vector4& Zeros() { static Vector4 v(0.0f, 0.0f, 0.0f, 0.0f); return v; }
    static const Vector4& Ones() { static Vector4 v(1.0f, 1.0f, 1.0f, 0.0f); return v; }
    static const Vector4& UnitX() { static Vector4 v(1.0f, 0.0f, 0.0f, 0.0f); return v; }
    static const Vector4& UnitY() { static Vector4 v(0.0f, 1.0f, 0.0f, 0.0f); return v; }
    static const Vector4& UnitZ() { static Vector4 v(0.0f, 0.0f, 1.0f, 0.0f); return v; }
    
    // the origin is used quite often. Be careful though as this is a point (w = 1)
    // Use as Point3::Origin() for clarity
    static const Vector4& Origin() { static Vector4 p(0.0f, 0.0f, 0.0f, 1.0f); return p; }

    
    static inline Vector4  interpolate(const Vector4& v1, const Vector4& v2, float factor);
    
    // union to hold the actual data
    union{
        float m_data[4];
        struct{
            float x;
            float y;
            float z;
            float w;
        };        
    };

};

Vector4
operator - (const Vector4 &v);

#include "Vector4.hpp"

#endif