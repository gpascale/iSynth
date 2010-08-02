#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"

Vector4::Vector4() :
    x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}


Vector4::Vector4(float _x, float _y, float _z, float _w) :
    x(_x), y(_y), z(_z), w(_w)
{    
}


Vector4::Vector4(const float *v)
{
    ASSERT(v != NULL);
    memcpy(m_data, v, 4 * sizeof(float));
}


Vector4::Vector4(const Vector4 &v) :
    x(v.x), y(v.y), z(v.z), w(v.w)
{    
}


Vector4::Vector4(const Vector3 &v, const float _w) :
    x(v.x), y(v.y), z(v.z), w(_w)
{    
}


Vector4::Vector4(const Vector2 &v, const float _z, const float _w) :
	x(v.x), y(v.y), z(_z), w(_w)
{    
}


Vector3
Vector4::homogenized3() const
{
	// if w is 0, simply return the same vector. This is better than crashing
	// Perhaps it would be cleaner to throw an exception. TODO: that
    if(w == 0.0f){
		//cerr << "Vector4::homogenized3() failed, w = 0" << endl;
		BREAK;
		return Vector3(x, y, z);
	}		
    const float wInv = 1.0f / w;
    return Vector3(x * wInv, y * wInv, z * wInv);
}


// homogenizes but leaves w in there
Vector4
Vector4::homogenized4() const
{
    if(w == 0.0f){
		//cerr << "Vector4::homogenized4() failed, w = 0" << endl;
		BREAK;
		return *this;
	}
    const float wInv = 1.0f / w;
    return Vector4(x * wInv, y * wInv, z * wInv, w);
}


Vector4&
Vector4::homogenize4() 
{
    if(w == 0.0f){
		//cerr << "Vector4::homogenized() failed, w = 0" << endl;
		BREAK;
		return *this;
	}
    const float wInv = 1.0f / w;
    x *= wInv; y*= wInv; z *= wInv;
    return *this;
}


Vector4
Vector4::operator + (const Vector3& v) const
{
    return Vector4(x + v.x, y + v.y, z + v.z, w);
}


Vector4
Vector4::operator - (const Vector3& v) const
{
    return Vector4(x - v.x, y - v.y, z - v.z, w);
}


Vector4&
Vector4::operator += (const Vector3& v)
{
    x += v.x, y += v.y, z += v.z;
    return *this;
}


Vector4&
Vector4::operator -= (const Vector3& v)
{
    x -= v.x, y -= v.y, z -= v.z;
    return *this;
}



