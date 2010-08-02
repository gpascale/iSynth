#ifndef Space_h
#define Space_h

#include "SpaceBase.h"
#include "Plane.h"

// TPoint3FD extends TPoint3FDI with behavior suitable only for floating-point.

template <class T> class TPoint3FD : public TPoint3FDI<T> {
  public:
    typedef TPoint3FD<T> F;
    typedef const TPoint3FD<T> cF;
    TPoint3FD() {}
    TPoint3FD(enumAll3, T xyz) : TPoint3FDI<T>(xyz,xyz,xyz) {}
    TPoint3FD(T a, T b, T c) : TPoint3FDI<T>(a,b,c) {}
    TPoint3FD(const TPoint3<T> & p) : TPoint3FDI<T>(p.x, p.y, p.z) {}
    explicit TPoint3FD(const TPoint<T> & rhs) : TPoint3FDI<T>(rhs.x, rhs.y, 0) {}
    F & operator = (const TPoint<T> & rhs) {
        TPoint3FDI<T>::x=rhs.x;  TPoint3FDI<T>::y=rhs.y;  TPoint3FDI<T>::z=0;
        return *this; }

    bool operator == (cF & p) const { return TPoint3FDI<T>::x==p.x && TPoint3FDI<T>::y==p.y && TPoint3FDI<T>::z==p.z; }
    bool operator != (cF & p) const { return TPoint3FDI<T>::x!=p.x || TPoint3FDI<T>::y!=p.y || TPoint3FDI<T>::z!=p.z; }
    bool operator ! ()        const { return !TPoint3FDI<T>::x && !TPoint3FDI<T>::y && !TPoint3FDI<T>::z; }

    void normalize()    { *this /= TPoint3FDI<T>::len(); }
    void normalize(T f) { *this *= sqrt(f, TPoint3FDI<T>::sqr()); }

    void rotate(cF & n, T c, T s) { // rotate about n with this cosine and sine
        const T t = 1-c;
        const T xx=n.x*n.x, xy=n.x*n.y, xz=n.x*n.z;
        const T yy=n.y*n.y, yz=n.y*n.z, zz=n.z*n.z;
        const T newX = TPoint3FDI<T>::x*(t*xx+c    ) + TPoint3FDI<T>::y*(t*xy-s*n.z) + TPoint3FDI<T>::z*(t*xz+s*n.y);
        const T newY = TPoint3FDI<T>::x*(t*xy+s*n.z) + TPoint3FDI<T>::y*(t*yy+c    ) + TPoint3FDI<T>::z*(t*yz-s*n.x);
        const T newZ = TPoint3FDI<T>::x*(t*xz-s*n.y) + TPoint3FDI<T>::y*(t*yz+s*n.x) + TPoint3FDI<T>::z*(t*zz+c    );
        TPoint3FDI<T>::x=newX, TPoint3FDI<T>::y=newY, TPoint3FDI<T>::z=newZ;
    }
    void rotate(cF & n, T ang) {
        T c = cos(ang), s = sin(ang);
        rotate(n,c,s);
    }
};

// Functionality specific to 'float' (normally float) Points: FPoint3.

class FPoint3 : public TPoint3FD<float> {
  public:
    //TPoint3Inst(FPoint3, float, TPoint3FD<float>);
    //#define TPoint3Inst(name, type, parent)
    typedef float T;
    typedef FPoint3 F;
    typedef const FPoint3 cF;
    typedef float V;
    //TPoint3Ctor(name, parent, TPoint<type>)
    //#define TPoint3Ctor(name, parent, twod)
    FPoint3() {}
    FPoint3(enumAll3, T xyz) : TPoint3FD<T>(xyz,xyz,xyz) {}
    FPoint3(T a, T b, T c) : TPoint3FD<T>(a,b,c) {}
    FPoint3(const TPoint3<T> & p) : TPoint3FD<T>(p.x, p.y, p.z) {}
    explicit FPoint3(const TPoint<T> & rhs) : TPoint3FD<T>(rhs.x, rhs.y, 0) {}
    F & operator = (const TPoint<T> & rhs) {
        x=rhs.x;  y=rhs.y;  z=0;
        return *this; }

    F operator - ()       const { return F(-x,-y,-z); }
    F operator + (cF & b) const { return F(x+b.x, y+b.y, z+b.z); }
    F operator - (cF & b) const { return F(x-b.x, y-b.y, z-b.z); }
    F operator * (T f)    const { return F(x*f, y*f, z*f); }
    F operator / (T f)    const { return F(x/f, y/f, z/f); }
    F cross(cF & p)       const { return F(y*p.z - z*p.y,
                                           z*p.x - x*p.z,
                                           x*p.y - y*p.x); }
    T cross2d(cF & p)     const { return x*p.y - y*p.x; }

    friend F operator * (T f, const F & q) { return q*f; }

    double dSqr()       const { return double(x)*x +
                                       double(y)*y +
                                       double(z)*z; }
    double dLen()       const { return sqrt(dSqr()); }
    double dDot(cF & p) const { return double(x)*p.x +
                                       double(y)*p.y +
                                       double(z)*p.z; }

    F inv()          const { return F(T(1./x), T(1./y), T(1./z)); }
    F scaled(cF & p) const { return F(x*p.x, y*p.y, z*p.z); }

    F normalized()          const { return *this / len(); }

    F rotated(cF & n, T c, T s) const { F f(*this); f.rotate(n,c,s); return f; }
    F rotated(cF & n, T ang)    const { F f(*this); f.rotate(n,ang); return f; }
};

typedef const FPoint3 cFPoint3;

// Functionality specific to 'double' (normally double) Points: DPoint3.

class DPoint3 : public TPoint3FD<double> {
  public:
    typedef double T;
    typedef DPoint3 F;
    typedef const DPoint3 cF;
    typedef double V;
    DPoint3() {}
    DPoint3(enumAll3, T xyz) : TPoint3FD<T>(xyz,xyz,xyz) {}
    DPoint3(T a, T b, T c) : TPoint3FD<T>(a,b,c) {}
    DPoint3(const TPoint3<T> & p) : TPoint3FD<T>(p.x, p.y, p.z) {}
    explicit DPoint3(const TPoint<T> & rhs) : TPoint3FD<T>(rhs.x, rhs.y, 0) {}

    F & operator = (const TPoint<T> & rhs) {
        x=rhs.x;  y=rhs.y;  z=0;
        return *this; }

    typedef double QuadraticT;
    typedef double CubicT;
    typedef double QuarticT;

    F operator - ()       const { return F(-x,-y,-z); }
    F operator + (cF & b) const { return F(x+b.x, y+b.y, z+b.z); }
    F operator - (cF & b) const { return F(x-b.x, y-b.y, z-b.z); }
    F operator * (T f)    const { return F(x*f, y*f, z*f); }
    F operator / (T f)    const { return F(x/f, y/f, z/f); }
    F cross(cF & p)       const { return F(y*p.z - z*p.y,
                                           z*p.x - x*p.z,
                                           x*p.y - y*p.x); }
    T cross2d(cF & p)     const { return x*p.y - y*p.x; }

    friend F operator * (T f, const F & q) { return q*f; }

    F inv()          const { return F(1./x, 1./y, 1./z); }
    F scaled(cF & p) const { return F(x*p.x, y*p.y, z*p.z); }

    F normalized(const T f) const { return *this * sqrt(f / sqr()); }
    F normalized()          const { T f = 1./len(); return *this * f; }

    F rotated(cF & n, T c, T s) const { F f(*this); f.rotate(n,c,s); return f; }
    F rotated(cF & n, T ang)    const { F f(*this); f.rotate(n,ang); return f; }
};

typedef const DPoint3 cDPoint3;

#undef TPoint3Inst
#undef TPoint3Hdr
#undef TPoint3Ctor

inline void highest(FPoint3 & p) { p.x= 7e37f; p.y= 7e37f; p.z= 7e37f; }
inline void  lowest(FPoint3 & p) { p.x=-7e37f; p.y=-7e37f; p.z=-7e37f; }
inline void highest(DPoint3 & p) { p.x= 9e99;  p.y= 9e99;  p.z= 9e99; }
inline void  lowest(DPoint3 & p) { p.x=-9e99;  p.y=-9e99;  p.z=-9e99; }

inline void zero(FPoint3 & p) { p.x=p.y=p.z=0; }
inline void zero(DPoint3 & p) { p.x=p.y=p.z=0; }

#endif // Space_h
