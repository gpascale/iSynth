

inline
float&
Vector4::operator[](const int n)
{
    ASSERT(n >= 0 && n <= 3);
    return (float&) m_data[n];
}


inline
const float&
Vector4::operator[](const int n) const
{
    ASSERT(n >= 0 && n <= 3);
    return (const float&) m_data[n];
}


inline
Vector4
Vector4::operator+(const Vector4 &v) const
{
    return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}


inline
Vector4
Vector4::operator-(const Vector4 &v) const
{
    return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}


inline
Vector4
Vector4::operator*(const float f) const
{
    return Vector4(x * f, y * f, z * f, w * f);
}


inline
Vector4
Vector4::operator/(const float f) const
{
    static float ff = 1.0f / f;
    return Vector4(x * ff, y * ff, z * ff, w * ff);
}



inline
Vector4&
Vector4::operator=(const Vector4 &v)
{
	copyVector4(*this, v);
	return (Vector4&) *this;
}



inline
Vector4&
Vector4::operator+=(const Vector4 &v)
{
    x += v.x; y += v.y; z += v.z; w += v.w;
    return *this;
}


inline
Vector4&
Vector4::operator-=(const Vector4 &v)
{
    x -= v.x; y -= v.y; z -= v.z; w -= v.z;
    return *this;
}


inline
Vector4&
Vector4::operator*=(const float f)
{
    x *= f; y *= f; z *= f; w *= f;
    return *this;
}


inline
Vector4&
Vector4::operator/=(const float f)
{
    static float ff = 1.0f / f;
    x *= ff; y *= ff; z *= ff; w *= ff;
    return *this;
}



inline
void
Vector4::setData(const float *f)
{
	ASSERT(f != NULL);
	memcpy(&x, f, 4 * sizeof(float));
}


inline
float
Vector4::dot(const Vector4 &v) const
{
    return x * v.x + y * v.y + z * v.z + w * v.w;
}


inline
Vector4
Vector4::cross(const Vector4 &v) const
{
    return Vector4((y * v.z - z * v.y),
                   (z * v.x - x * v.z),
                   (x * v.y - y * v.x),
                   v.w);    //TODO: investigate what is best behavior here
}


inline
float
Vector4::magnitudeSq() const
{
    return SQR(x) + SQR(y) + SQR(z);
}


inline
float
Vector4::magnitude() const
{
    return sqrt(SQR(x) + SQR(y) + SQR(z));
}


inline
Vector4&
Vector4::normalize()
{
    if(x == 0.0f && y == 0.0f && z == 0.0f)
		return *this;
    const float imag = 1.0f / magnitude();
    x *= imag; y *= imag; z *= imag;
    return *this;
}


inline
Vector4
Vector4::normalized() const
{
	Vector4 ret(*this);
	ret.normalize();
	return ret;
}
	



inline
Vector4
operator*(const float f, const Vector4 &v)
{    
    return Vector4(v.x * f, v.y * f, v.z * f, v.w * f);
}


inline
Vector4
operator-(Vector4 &v)
{
	Vector4 ret(v);
    ret.x *= -1;
    ret.y *= -1;
    ret.z *= -1;
    ret.w *= -1;
	return ret;
}


inline
void
Vector4::copyVector4(Vector4 &out, const Vector4 &in)
{
    memcpy(out.getDataPointer(), in.getConstDataPointer(), 4 * sizeof(float));
}


// debug print
inline
ostream&
operator<<(ostream &os, const Vector4 &v)
{
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}


inline
Vector4
operator-(const Vector4 &v)
{
    return Vector4(-v.x, -v.y, -v.z, -v.w);
}


inline
Vector4
Vector4::interpolate(const Vector4& v1, const Vector4& v2, float factor)
{
    return v1 + (factor * (v2 - v1));
}

