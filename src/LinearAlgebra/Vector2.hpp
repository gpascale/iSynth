/*
 *  Vector2.hpp
 *  Phonosynth
 *
 *  Created by Greg on 11/23/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */




// TODO: actually implement Vector2, at which point this will get fixed
inline
Vector2
Vector2::interpolate(const Vector2& v1, const Vector2& v2, float f)
{
    return Vector2((1.0 - f) * v1.x + f * v2.x, (1.0 - f) * v1.y + f * v2.y);    
}


inline
float&
Vector2::operator [] (int n)
{
	return m_data[n];
}


inline
const float&
Vector2::operator [] (int n) const
{
	return m_data[n];
}


inline
Vector2
Vector2::operator + (const Vector2& v) const
{
	return Vector2(x + v.x, y + v.y);
}


inline
Vector2
Vector2::operator - (const Vector2& v) const
{
	return Vector2(x - v.x, y - v.y);
}


inline
Vector2
Vector2::operator * (float f) const
{
	return Vector2(x * f, y * f);
}


inline
Vector2
Vector2::operator / (float f) const
{
	ASSERT(f != 0.0f);
	const float fInv = 1.0f / f;
	return Vector2(x * fInv, y * fInv);
}


inline
Vector2&
Vector2::operator = (const Vector2 &v)
{
	x = v.x, y = v.y;
	return *this;
}


inline
Vector2&
Vector2::operator += (const Vector2 &v)
{
	x += v.x, y += v.y;
	return *this;
}


inline
Vector2&
Vector2::operator -= (const Vector2 &v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}


inline
Vector2&
Vector2::operator *= (const float f)
{
	x *= f;
	y *= f;
	return *this;
}


inline
Vector2&
Vector2::operator /= (const float f)
{
	const float fInv = 1.0f / f;
	x *= fInv;
	y *= fInv;
	return *this;
}



inline
float
Vector2::magnitudeSq() const
{
	return SQR(x) +  SQR(y);
}


inline
float
Vector2::magnitude() const
{
	const float mag = magnitudeSq();
	if(mag == 0.0f)
		return 0.0f;
	return sqrt(mag);
}



inline
float
Vector2::dot(const Vector2& v) const
{
	return x * v.x + y * v.y;	
}


inline
float
Vector2::cross(const Vector2& v) const
{
	return x * v.y - y * v.x;
}


inline
Vector2&
Vector2::normalize()
{
	const float mag = magnitudeSq();
	if(mag == 0.0f)
		return *this;
	return operator /= (mag);
}


inline
Vector2
Vector2::normalized() const
{
	Vector2 ret(*this);
	return ret.normalize();
}







