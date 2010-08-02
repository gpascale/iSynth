
// access

inline
float&
Vector3::operator[](const int n)
{
    ASSERT(n >= 0 && n <= 2);
    return (float&) m_data[n];
}


inline
const float&
Vector3::operator[](const int n) const
{
    ASSERT(n >= 0 && n <= 2);
    return (const float&) m_data[n];
}


// operator overloads
inline
Vector3
Vector3::operator+(const Vector3 &v) const
{
    return Vector3(x + v.x, y + v.y, z + v.z);
}


inline
Vector3
Vector3::operator-(const Vector3 &v) const
{
    return Vector3(x - v.x, y - v.y, z - v.z);
}


inline
Vector3
Vector3::operator*(const float f) const
{
    return Vector3(x * f, y * f, z * f);
}


inline
Vector3
Vector3::operator/(const float f) const
{
    static float ff = 1.0f / f;
    return Vector3(x * ff, y * ff, z * ff);
}


inline
Vector3&
Vector3::operator=(const Vector3 &v)
{
	copyVector3(*this, v);
	return (Vector3&) *this;
}


inline
Vector3&
Vector3::operator+=(const Vector3 &v)
{
    x += v.x; y += v.y; z += v.z;
    return *this;
}


inline
Vector3&
Vector3::operator-=(const Vector3 &v)
{
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}


inline
Vector3&
Vector3::operator*=(const float f)
{
    x *= f; y *= f; z *= f;
    return *this;
}


inline
Vector3&
Vector3::operator/=(const float f)
{
    static float ff = 1.0f / f;
    x *= ff; y *= ff; z *= ff;
    return *this;
}


inline
void
Vector3::setData(const float *f)
{
	ASSERT(f != NULL);
	memcpy(&x, f, 3 * sizeof(float));
}


inline
float
Vector3::dot(const Vector3 &v) const
{
    return x * v.x + y * v.y + z * v.z;
}


inline
Vector3
Vector3::cross(const Vector3 &v) const
{
    return Vector3((y * v.z - z * v.y),
                   (z * v.x - x * v.z),
                   (x * v.y - y * v.x));
}


inline
float
Vector3::magnitudeSq() const
{
    return SQR(x) + SQR(y) + SQR(z);
}


inline
float
Vector3::magnitude() const
{
	const float mag = magnitudeSq();
	if(mag == 0.0f)
		return 0.0f;
    return sqrt(mag);
}


inline
Vector3&
Vector3::normalize()
{
    if(x == 0.0f && y == 0.0f && z == 0.0f)
		return *this;
    float imag = 1.0f / magnitude();
    x *= imag; y *= imag; z *= imag;
    return *this;
}



inline
Vector3
Vector3::normalized() const
{
	Vector3 ret(*this);
	return ret.normalize();
}



inline
Vector3
operator*(const float f, const Vector3 &v)
{    
    return Vector3(v.x * f, v.y * f, v.z * f);
}


inline
Vector3
operator-(const Vector3 &v)
{
    Vector3 ret(v);
    ret.x *= -1;
    ret.y *= -1;
    ret.z *= -1;
    return ret;
}


inline
Vector3
Vector3::interpolate(const Vector3& v1, const Vector3& v2, float f)
{
    return v1 + (f * (v2 - v1));
}


inline
void
Vector3::copyVector3(Vector3 &out, const Vector3 &in)
{
    memcpy(out.getDataPointer(), in.getConstDataPointer(), 3 * sizeof(float));
}



// debug print
inline
ostream&
operator<<(ostream &os, const Vector3 &v)
{
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

