#include "Vector3.h"

Vector3::Vector3() :
    x(0.0f), y(0.0f), z(0.0f)
{
}


Vector3::Vector3(float _x, float _y, float _z) :
    x(_x), y(_y), z(_z)
{        
}


Vector3::Vector3(const float *v)
{
    ASSERT(v != NULL);
    x = v[0];
    y = v[1];
    z = v[2];
}


Vector3::Vector3(const Vector3 &v) :
    x(v.x), y(v.y), z(v.z)
{    
}


void
Vector3::rotateAround(Vector3 &target, const Vector3 &axis, float amt)
{
	float u = axis.x;
	float v = axis.y;
	float w = axis.z;
	float x = target.x;
	float y = target.y;
	float z = target.z;

	float ux = axis.x * target.x;
	float uy = axis.x * target.y;
	float uz = axis.x * target.z;
	float vx = axis.y * target.x;
	float vy = axis.y * target.y;
	float vz = axis.y * target.z;
	float wx = axis.z * target.x;
	float wy = axis.z * target.y;
	float wz = axis.z * target.z;

	float sa = sin(amt);
	float ca = cos(amt);

	target.x = u * (ux+vy+wz) + (x * (v*v+w*w) - u * (vy+wz)) * ca + (-wy+vz) * sa;
	target.y = v * (ux+vy+wz) + (y * (u*u+w*w) - v * (ux+wz)) * ca + (wx-uz)  * sa;
	target.z = w * (ux+vy+wz) + (z * (u*u+v*v) - w * (ux+vy)) * ca + (-vx+uy) * sa;
}