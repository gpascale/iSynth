#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "Vector4.h"
#include "Matrix.h"

class Quaternion : public Vector4
{
public:
    Quaternion(float _x, float _y, float _z, float _w = 1.0f);
    Quaternion(float *v);
    Quaternion(const Quaternion &v);
	// w is intentionally not given a default parameter
	// do not want implicit conversion
    Quaternion(const Vector3 &v, const float w);
    Quaternion(const Vector4 &v);
    Quaternion();
    ~Quaternion(){ /* everything's on the stack */ }

    void          toMatrix(Matrix &m) const;
    Matrix        toMatrix() const;
    void		  toAxisAngle(OUT Vector3& axis, OUT float& angle);
	
    static
    void SlerpQuaternion(float alpha, 
                         const Quaternion &a, const Quaternion &b,
                         Quaternion &ret,
                         int spin);
    
    float magSq() const { return SQR(x) + SQR(y) + SQR(z) + SQR(w); }
    float mag()   const { return sqrt(magSq()); }
    
    Quaternion getConjugate() const { return Quaternion(-x, -y, -z, w); }
    
};


// not really sure why this works but it makes intuitive sense. Don't think I 
// really care about normalizing
inline
Quaternion operator - (const Quaternion& q)
{
    return 1.0f / q.magSq() * q.getConjugate();
}

 
#include "Quaternion.hpp"

#endif