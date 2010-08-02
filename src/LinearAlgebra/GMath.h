
#ifndef _GMATH_H_
#define _GMATH_H_

#include "Matrix.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Transformation.h"
#include "Quaternion.h"

typedef Vector4 Point3;
typedef Vector3 Point2;
typedef Vector4 Plane;



struct Ray
{
public:

    Ray() : o(0.0f, 0.0f, 0.0f, 1.0f), d(1.0f, 0.0f, 0.0f) { }
    Ray(const Vector4& origin, const Vector3& direction) : o(origin), d(direction) { }
            
    Point3 shootFixedDistance(float t) const { return o + t * d; }
    
    Point3  o;
    Vector3 d;        
};

// tests whether or not a 2-dimensional point falls inside a 2-dimensional triangle

//simply returns true if t is positive, false otherwise
template<typename T>
inline bool sign(T t) { return t > 0; }

extern 
Plane planeFrom3Points(const Point3& a, const Point3& b, const Point3& c);

extern
bool  pointInsideTriangle(const Vector2& pt, const Vector2& t1, const Vector2& t2, const Vector2& t3);

extern
float pointPlaneDistance(const Plane& plane, const Point3& point);

extern
float rayPlaneIntersect(const Ray& ray, const Plane& plane, OUT Point3& ret, double eps = 1e-12);

extern 
Vector3 projectAndHomogenize3(const Vector4 &v, const Matrix &viewMatrix, const Matrix &projectionMatrix);

extern 
Vector3 projectAndHomogenize3(const Vector4 &v, const Matrix& worldMatrix, const Matrix &viewMatrix, const Matrix &projectionMatrix);

extern
Vector4 projectAndHomogenize4(const Vector4 &v, const Matrix& viewMatrix, const Matrix& projectionMatrix);

extern
Vector4 projectAndHomogenize4(const Vector4 &v, const Matrix& worldMatrix, const Matrix& viewMatrix, const Matrix& projectionMatrix);

extern
Vector4 project(const Vector4 &v, const Matrix &viewMatrix, const Matrix &projectionMatrix);

extern
Vector4 project(const Vector4 &v, const Matrix& worldMatrix, const Matrix &viewMatrix, const Matrix &projectionMatrix);

extern 
float triangleArea(const Vector2& a, const Vector2& b, const Vector2& c);

#endif // _GMATH_H_