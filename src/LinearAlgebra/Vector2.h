/*
 *  Vector2.h
 *  Phonosynth
 *
 *  Created by Greg on 11/23/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _Vector2_H_
#define _Vector2_H_
#include <common.h>

//class Vector2;

using namespace std;

class Vector2
    {
    public:
        
        // constructors
        Vector2();
        Vector2(float _x, float _y);
        Vector2(const float *v);
        Vector2(const Vector2 &v);
        ~Vector2(void){ /* everything's on the stack */ }
        

        float&          operator [] (int n);
        const float&    operator [] (int n)         const;		
        
        operator const float* () const { return (const float*) &x; }
        operator float*		  ()       { return (      float*) &x; }
        
        // operator overloads
        Vector2         operator + (const Vector2 &v)     const;
        Vector2         operator - (const Vector2 &v)     const; 
        Vector2         operator * (float f)			  const;
        Vector2         operator / (float f)			  const;
        
        Vector2&		operator = (const Vector2 &v);
        Vector2&        operator += (const Vector2 &v);
        Vector2&        operator -= (const Vector2 &v);
        Vector2&        operator *= (const float f);
        Vector2&        operator /= (const float f);
/*        
        // in the future, this code could be made faster by using this
        // function in tandem with memcpy for things like copy constructors
        // for now, I just do that stuff with loop and assign
        float*          getDataPointer(){ return &x; }
        void			setData(const float *f);
        
        // this is kind of a lie, need to only use this for read access
        const float*    getConstDataPointer() const { return &x; }
        void            zero(){ memset(m_data, 0x00, sizeof(m_data)); }
*/
		

        // other math
        float           magnitudeSq()                   const;
        float           magnitude()                     const;
        float           dot(const Vector2 &v)           const;
        float           cross(const Vector2 &v)         const;
        Vector2&        normalize();
        Vector2			normalized()					const;
        						
        static void     rotateAround(Vector2 &target, const Vector2 &axis, float amt);
        static void     copyVector2(Vector2 &out, const Vector2 &in);
    
 
		static const Vector2& UnitX() { static const Vector2 ret(1.0f, 0.0f); return ret; }
		static const Vector2& UnitY() { static const Vector2 ret(0.0f, 1.0f); return ret; }
		static const Vector2& Ones()  { static const Vector2 ret(1.0f, 1.0f); return ret; }
		static const Vector2& Zeros() { static const Vector2 ret(0.0f, 0.0f); return ret; }
		
        
        // members
        static inline Vector2 interpolate(const Vector2& v1, const Vector2& v2, float factor);
        
        union{
            float m_data[2];
            struct{
                float x;
                float y;   
            };
        };
        
    };

ostream& 
operator<<(ostream &os, const Vector2 &v);

// multiplication is commutative
Vector2
operator*(const float f, const Vector2 &v);

Vector2
operator-(const Vector2 &v);

#include "Vector2.hpp"

#endif