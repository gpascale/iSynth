// Quaternion.h implementation

typedef DMatrix4 M;
typedef const M cM;

const DQuaternion DQuaternionOne(0.0, 0.0, 0.0, 1.0);
const FQuaternion FQuaternionOne(0.f, 0.f, 0.f, 1.f);

template <class T>
void TQuaternion<T>::fromAxis(const TPoint3FDI<T> & axis, double theta) {
    theta *= -0.5;
    const double
        st = sin(theta),
        ct = cos(theta);
    x = T(st * axis.x);
    y = T(st * axis.y);
    z = T(st * axis.z);
    w = T(ct);
}

template <class T>
void TQuaternion<T>::toAxis(TPoint3FDI<T> * axis, double * theta) const {
    double w1 = w<1.0 ? (w>-1.0 ? w : -1.0) : 1.0;
    *theta = -2. * acos(w1);
    double st = sqrt(1.-w1*w1); // = |sin(theta/2)|
    if (st < 1e-6) {
        st = 1.;
    } else {
        st = 1./st;
    }
    axis->x = T(st * x);
    axis->y = T(st * y);
    axis->z = T(st * z);
    double l = axis->len();
    if (l > 0.) {
        *axis /= l;
    }
}

template <class T>
void TQuaternion<T>::fromBallPoints(const TPoint3FDI<T> & from,
                                    const TPoint3FDI<T> & to) {
    x = from.y*to.z - from.z*to.y;
    y = from.z*to.x - from.x*to.z;
    z = from.x*to.y - from.y*to.x;
    w = from.x*to.x + from.y*to.y + from.z*to.z;
}

template <class T>
void TQuaternion<T>::toBallPoints(TPoint3FDI<T> & from,
                                  TPoint3FDI<T> & to) const {
    double s = sqrt(x*x + y*y);
    from = s==0. ? V(0.,1.,0.,1.) : V(-y/s, x/s, 0.,1.);
    to.x = w*from.x - z*from.y;
    to.y = w*from.y + z*from.x;
    to.z = x*from.y - y*from.x;
    if (w<0.) from.set(-from.x, -from.y, 0., 1.);
}

template <class T>
inline void fromMatrixInternal(TQuaternion<T> & q,
                               const DPoint3 & m0,
                               const DPoint3 & m1,
                               const DPoint3 & m2) {

    const double &m00=m0.x, &m01=m0.y, &m02=m0.z,
                 &m10=m1.x, &m11=m1.y, &m12=m1.z,
                 &m20=m2.x, &m21=m2.y, &m22=m2.z;

    const double tr = m00 + m11 + m22 + 1.; //m4.trace();

    if (tr>1e-8) {
        double s = 0.5 / sqrt(tr);
        q.w = T(0.25 / s);
        q.x = T((m21 - m12) * s);
        q.y = T((m02 - m20) * s);
        q.z = T((m10 - m01) * s);

    } else {
        if (m00 > m11 && m00 > m22 ) {
            double s = sqrt(1.0 + m00 - m11 - m22);
            q.x = T(0.5 * s);
            s = 1.0 / (s+s);
            q.y = T((m01 + m10) * s);
            q.z = T((m02 + m20) * s);
            q.w = T((m21 - m12) * s); // negative
        } else if (m11 > m22) {
            double s = sqrt(1.0 + m11 - m00 - m22);
            q.y = T(0.5 * s);
            s = 1.0 / (s+s);
            q.x = T((m01 + m10) * s);
            q.z = T((m12 + m21) * s);
            q.w = T((m02 - m20) * s);

        } else {
            double s = sqrt(1.0 + m22 - m00 - m11);
            q.z = T(0.5 * s);
            s = 1.0 / (s+s);
            q.x = T((m02 + m20) * s);
            q.y = T((m12 + m21) * s);
            q.w = T((m10 - m01) * s); // negative
        }
    }
}

template <class T>
void TQuaternion<T>::fromMatrix(const DMatrix4 & m) {
    const DPoint3 * m0 = (DPoint3 *)(&(m[0].x));
    const DPoint3 * m1 = (DPoint3 *)(&(m[1].x));
    const DPoint3 * m2 = (DPoint3 *)(&(m[2].x));
    fromMatrixInternal(*this, *m0,*m1,*m2);
}

template <class T>
void TQuaternion<T>::fromMatrix(const DMatrix3 & m) {
    fromMatrixInternal(*this, m[0],m[1],m[2]);
}

template <class T>
inline void toMatrixInternal(DPoint3 & m0,
                             DPoint3 & m1,
                             DPoint3 & m2,
                             const TQuaternion<T> & q) {
    double
        Nq = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w,
        s = Nq>0. ? 2./Nq : 0.,
        xs = q.x*s,  ys = q.y*s,  zs = q.z*s,
        wx = q.w*xs, wy = q.w*ys, wz = q.w*zs,
        xx = q.x*xs, xy = q.x*ys, xz = q.x*zs,
        yy = q.y*ys, yz = q.y*zs, zz = q.z*zs;
    m0.x = 1.-(yy+zz); m1.x =     xy+wz ; m2.x =     xz-wy ;
    m0.y =     xy-wz ; m1.y = 1.-(xx+zz); m2.y =     yz+wx ;
    m0.z =     xz+wy ; m1.z =     yz-wx ; m2.z = 1.-(xx+yy);
}

template <class T>
void TQuaternion<T>::toMatrix(M & m) const {
    DPoint3 * m0 = (DPoint3 *)(&(m[0].x));
    DPoint3 * m1 = (DPoint3 *)(&(m[1].x));
    DPoint3 * m2 = (DPoint3 *)(&(m[2].x));
    toMatrixInternal(*m0,*m1,*m2, *this);
    m[0][3] = m[1][3] = m[2][3] = m[3][0] =
    m[3][1] = m[3][2] = 0.;
    m[3][3] = 1.;
}

template <class T>
void TQuaternion<T>::toMatrix(M3 & m) const {
    toMatrixInternal(m[0],m[1],m[2], *this);
}

template <class T>
DMatrix4 TQuaternion<T>::toMatrix() const {
    M m;
    toMatrix(m);
    return m;
}

template <class T>
DMatrix3 TQuaternion<T>::toMatrix3() const {
    M3 m;
    toMatrix(m);
    return m;
}

template <class T>
void TQuaternion<T>::fromAngularMomentum(const TPoint3FDI<T> & L) {
    T len = L.len();
    if (len>0.) {
        T li = T(1./len);
        TPoint3FDI<T> Ln = L;
        Ln *= li;
        fromAxis(Ln, len);
    } else {
        Identity();
    }
}

template <class T>
void TQuaternion<T>::fromAngularMomentum(T lx, T ly, T lz) {
    fromAngularMomentum(TPoint3FDI<T>(lx,ly,lz));
}

template <class T>
DPoint3 TQuaternion<T>::toAngularMomentum() const {
    DPoint3 axis;
    if (w>=1.) {
        axis.set(0.,0.,0.);
    } else {
        double theta;
        toAxis(&axis, &theta);
        axis *= theta;
    }
    return axis;
}

template <class T>
void TQuaternion<T>::toAngularMomentum(T * lx, T * ly, T * lz) const {
    V axis = toAngularMomentum();
    *lx = axis.x;
    *ly = axis.y;
    *lz = axis.z;
}

template <class T>
void TQuaternion<T>::random() {
    double l;
    do {
        GaussianPair0(x,y);
        GaussianPair0(z,w);
        l = sqr();
    } while (l<.0001);
    T f = T(1./sqrt(l));
    x*=f, y*=f, z*=f, w*=f;
}

template <class T>
void SlerpQuaternion(double alpha,
                     const TQuaternion<T> & a,
                     const TQuaternion<T> & b,
                     TQuaternion<T> & q,
                     int spin) {
    bool bFlip;
    double ct = a.dot(b);
    if (ct<0.) {
        ct = -ct;
        bFlip = true;
    } else
        bFlip = false;
    double beta;
    if (1.-ct < 1.e-6) {
        beta = 1.-alpha;
    } else {
        const double
            theta = acos(ct),
            phi = theta + 3.14159265359*spin,
            st = sin(theta);
        beta = sin(theta - alpha*phi) / st;
        alpha = sin(alpha*phi) / st;
    }
    if (bFlip)
        alpha = -alpha;
    q = a*T(beta) + b*T(alpha);
}

template <class T>
void SlerpQuaternion3(double alpha,
                      double beta,
                      double gamma,
                      const TQuaternion<T> & a,
                      const TQuaternion<T> & b,
                      const TQuaternion<T> & c,
                      TQuaternion<T> & q) {
    b_assert(alpha>=0 && beta>=0 && gamma>=0);
    b_assert(fabs(alpha+beta+gamma - 1.0) < 1e-10);
    // we're using a somewhat silly method for the time being...
    // barycentrically interpolate the 'momentum representation', then
    // convert back to a quaternion.  This avoids the elaborate pairwise sign
    // logic induced by the double-covering of the sphere.
    const DPoint3 am = a.toAngularMomentum(),
                  bm = b.toAngularMomentum(),
                  cm = c.toAngularMomentum(),
                  qm = am*alpha + bm*beta + cm*gamma;
    q.fromAngularMomentum(qm);
}
