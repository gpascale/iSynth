/*
 *  Math.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/21/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "GMath.h"



bool pointInsideTriangle(const Vector2& pt, const Vector2& t1, const Vector2& t2, const Vector2& t3)
{
	Vector2 u1 = pt - t1, v1 = t2 - t1;	
	Vector2 u2 = pt - t2, v2 = t3 - t2;	
	Vector2 u3 = pt - t3, v3 = t1 - t3;
		
	const float s1 = sign(u1.cross(v1)), s2 = sign(u2.cross(v2)), s3 = sign(u3.cross(v3));
	if(s1 == s2 && s2 == s3)
		return true;
		
	return false;
}


Plane planeFrom3Points(const Point3& a, const Point3& b, const Point3& c)
{
	const Vector3 n = -(b-a).xyz().cross((c-a).xyz()).normalized();
	return Plane(n, -n.dot(a.xyz()));
}


float pointPlaneDistance(const Plane& plane, const Point3& point)
{
    return plane.x*point.x + plane.y*point.y + plane.z*point.z + plane.w;    
}


// computes the intersection point of the ray 'ray' with the plane 'plane'. Returns that
// point in 'ret' and the t-value as return value.

// if t comes out negative, for now it is just being flipped which seems to work.
// TODO: Figure out what's really going on



float rayPlaneIntersect(const Ray& ray, const Plane& plane, OUT Point3& ret, double eps)
{
    const Point3&  o = ray.o;
    const Vector3& d = ray.d;
    
    //float dist = plane.xyz().dot(d);
    float dist = (plane.x * d.x) + (plane.y * d.y) + (plane.z * d.z);
    if (dist > eps || dist < -eps) {
        float t = -(plane.x * o.x + plane.y * o.y + plane.z * o.z + plane.w) / dist;
        if (t < 0.0f) 
            t *= -1;
        if (t > 0.0f) {
            ret = o + d * t;
            return t;
        }
		// ray is parallel to plane
		ret = Point3(0.0f, 0.0f, 0.0f, 1.0f);
        return FLT_MAX;
    }    
	// point must be on plane
	ret = ray.o;
	return 0.0f;
}


Vector3 projectAndHomogenize3(const Vector4 &v,
                              const Matrix &viewMatrix,
                              const Matrix &projectionMatrix)
{    
	Vector4 ret = project(v, viewMatrix, projectionMatrix);
    return ret.homogenized3();
}


Vector4 projectAndHomogenize4(const Vector4& v,
                              const Matrix& viewMatrix,
                              const Matrix& projectionMatrix)
{
    Vector4 ret = project(v, viewMatrix, projectionMatrix);
    return ret.homogenize4();
}


Vector4 projectAndHomogenize4(const Vector4& v,
							  const Matrix& worldMatrix,
                              const Matrix& viewMatrix,
                              const Matrix& projectionMatrix)
{
    Vector4 ret = project(v, worldMatrix, viewMatrix, projectionMatrix);
    return ret.homogenize4();
}


Vector4 project(const Vector4 &v,
				const Matrix &viewMatrix,
				const Matrix &projectionMatrix)
{
    return v * viewMatrix * projectionMatrix;
}


Vector4 project(const Vector4 &v,
				const Matrix& worldMatrix,
				const Matrix& viewMatrix,
				const Matrix& projectionMatrix)
{
    return v * worldMatrix * viewMatrix * projectionMatrix;
}


float triangleArea(const Vector2& a, const Vector2& b, const Vector2& c)
{
	return heronArea(fabs((b-a).magnitude()), fabs((c-b).magnitude()), fabs((c-a).magnitude()));
}

