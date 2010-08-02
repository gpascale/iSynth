#ifndef _VECTOR3_H_
#define _VECTOR3_H_
#include <common.h>

class Vector2;

using namespace std;

class Vector3
{
public:

    // constructors
    Vector3();
    Vector3(float _x, float _y, float _z);
    Vector3(const float *v);
    Vector3(const Vector3 &v);
    //Vector3(const Vector4 &v);
    ~Vector3(void){ /* everything's on the stack */ }

    // access
    float&          operator [] (const int n);
    const float&    operator [] (const int n)         const;		

    operator const float* () const { return (const float*) &x; }
    operator float*		  ()       { return (      float*) &x; }
    
    // operator overloads
    Vector3         operator + (const Vector3 &v)     const;
    Vector3         operator - (const Vector3 &v)     const; 
    Vector3         operator * (const float f)        const;
    Vector3         operator / (const float f)        const;

	Vector3&		operator = (const Vector3 &v);
    Vector3&        operator += (const Vector3 &v);
    Vector3&        operator -= (const Vector3 &v);
    Vector3&        operator *= (const float f);
    Vector3&        operator /= (const float f);

    // in the future, this code could be made faster by using this
    // function in tandem with memcpy for things like copy constructors
    // for now, I just do that stuff with loop and assign
    float*          getDataPointer(){ return &x; }
	void			setData(const float *f);

    // this is kind of a lie, need to only use this for read access
    const float*    getConstDataPointer() const { return &x; }
    void            zero(){ memset(m_data, 0x00, sizeof(m_data)); }
   
    // other math
    float           magnitudeSq()                   const;
    float           magnitude()                     const;
    float           dot(const Vector3 &v)           const;
    Vector3         cross(const Vector3 &v)         const;
    Vector3&        normalize();
	Vector3			normalized()					const;
    
	const Vector2& xy() const { 
        return reinterpret_cast<const Vector2&> (*this);
    }
	
	const Vector2& yz() const {
		return reinterpret_cast<const Vector2&> (*(&y));
	}
	
    // const references to some commonly used vectors for read-access only
    static const Vector3& UnitX() { static const Vector3 ret(1.0f, 0.0f, 0.0f); return ret; }
    static const Vector3& UnitY() { static const Vector3 ret(0.0f, 1.0f, 0.0f); return ret; }
    static const Vector3& UnitZ() { static const Vector3 ret(0.0f, 0.0f, 1.0f); return ret; }
    static const Vector3& Zeros() { static const Vector3 ret(0.0f, 0.0f, 0.0f); return ret; }
    static const Vector3& Ones()  { static const Vector3 ret(1.0f, 1.0f, 1.0f); return ret; }

    static void     rotateAround(Vector3 &target, const Vector3 &axis, float amt);
    static void     copyVector3(Vector3 &out, const Vector3 &in);
	
    static inline Vector3 interpolate(const Vector3& v1, const Vector3& v2, float factor);

    // members
    union{
        float m_data[3];
        struct{
            float x;
            float y;
            float z;
        };
    };

};

ostream& 
operator<<(ostream &os, const Vector3 &v);

// multiplication is commutative
Vector3
operator*(const float f, const Vector3 &v);

Vector3
operator-(const Vector3 &v);

#include "Vector3.hpp"

#endif