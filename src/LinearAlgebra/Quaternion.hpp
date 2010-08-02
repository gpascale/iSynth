
inline
Matrix
Quaternion::toMatrix() const
{
    Matrix m;
    toMatrix(m);
    return m;
}



inline
void
Quaternion::toMatrix(Matrix &m) const
{
    float Nq = SQR(x) + SQR(y) + SQR(z) + SQR(w);
    float s = (Nq > 0.0f) ? (2.0f / Nq) : (0.0f);
    
    const float xs = x*s,  ys = y*s,  zs = z*s;
    const float wx = w*xs, wy = w*ys, wz = w*zs;
    const float xx = x*xs, xy = x*ys, xz = x*zs;
    const float yy = y*ys, yz = y*zs, zz = z*zs;
 
    m(0, 3) = m(1, 3) = m(2, 3) = m(3, 2) = m(3, 1) = m(3, 0) = 0.0f;
    m(3, 3) = 1.0f;

    m(0, 0) = 1.0f - (yy+zz);   m(0, 1) =        (xy-wz);    m(0, 2) =        (xz+wy);
    m(1, 0) =        (xy+wz);   m(1, 1) = 1.0f - (xx+zz);    m(1, 2) =        (yz-wx);
    m(2, 0) =        (xz-wy);   m(2, 1) =        (yz+wx);    m(2, 2) = 1.0f - (xx+yy);

}


inline
void
Quaternion::toAxisAngle(OUT Vector3& axis, OUT float& angle)
{
	angle = 2 * acos(w);
	const float n = 1.0f / sqrt(1-SQR(w));
	axis.x = x * n;
	axis.y = y * n;
	axis.z = z * n;
}


inline
void
Quaternion::SlerpQuaternion(float alpha, 
                            const Quaternion &a, const Quaternion &b,
                            OUT Quaternion &ret,
                            int spin)
{
    bool bFlip;
    float ct = a.dot(b);
    if (ct < 0.0f) {
        ct = -ct;
        bFlip = true;
    } 
    else
        bFlip = false;
    float beta;
    if (1.0f - ct < 1e-6) {
        beta = 1.0f - alpha;
    } 
	else {
        const float theta = acos(ct);
		const float phi	  = theta + 3.14159f * spin;
		const float st    = sin(theta);
				    beta  = sin(theta - alpha * phi) / st;
        const float alpha = sin(alpha * phi) / st;
	}
	
    if (bFlip) alpha *= -1.0f;
	
	ret = Quaternion(((beta) * a) + ((alpha) * b));
}




