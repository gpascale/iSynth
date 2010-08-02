#include "Quaternion.h"

Quaternion::Quaternion(float _x, float _y, float _z, float _w) :
    Vector4(_x, _y, _z, _w)
{
}

Quaternion::Quaternion(float *v) :
    Vector4(v)
{
}

Quaternion::Quaternion(const Quaternion &q) :
    Vector4(q)
{    
}

Quaternion::Quaternion(const Vector4 &v) :
    Vector4(v)
{
}    


Quaternion::Quaternion(const Vector3 &v, const float w) :
    Vector4(v, w)
{
}

Quaternion::Quaternion() : Vector4()
{
}

