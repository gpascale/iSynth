#ifndef Plane_h
#define Plane_h

#include "PlaneBase.h"
#include <math.h>

/// TPointFD extends TPointFDI with behavior suitable only for floating-point.

template <class T> class TPointFD : public TPointFDI<T> {
  public:
    TPointHdr(TPointFD, TPointFDI<T>);

    bool operator == (cF & p) const { return TPointFDI<T>::x == p.x && TPointFDI<T>::y == p.y; }
    bool operator != (cF & p) const { return TPointFDI<T>::x != p.x || TPointFDI<T>::y != p.y; }
    bool operator ! ()        const { return !TPointFDI<T>::x && !TPointFDI<T>::y; }

    T L1distTo(cF & p)  const { return fabs(TPoint<T>::x-p.x) +
                                       fabs(TPoint<T>::y-p.y); }
    T len()             const { return T(sqrt(double(TPointFDI<T>::sqr()))); }
    void normalize()    { *this /= len(); }
    void normalize(T f) { *this *= sqrt(f / TPointFDI<T>::sqr()); }
    void rotate(T c, T s) { // rotate with this cosine and sine
        T xx = c*TPointFDI<T>::x + s*TPointFDI<T>::y;
        TPointFDI<T>::y = c*TPointFDI<T>::y - s*TPointFDI<T>::x;
        TPointFDI<T>::x = xx;
    }
    void rotate(T ang) {
        T c = cos(ang), s = sin(ang);
        rotate(c,s);
    }
    double theta() const { return atan2(double(TPoint<T>::y),double(TPoint<T>::x)); }
};

/// Functionality specific to float Points: FPoint.

class FPoint : public TPointFD<float> {
  public:
    TPointInst(FPoint, float, TPointFD<float>);

    F operator - ()       const { return F(-x, -y); }
    F operator + (cF & b) const { return F(x+b.x, y+b.y); }
    F operator - (cF & b) const { return F(x-b.x, y-b.y); }
    F operator * (T f)    const { return F(x*f, y*f); }
    F operator / (T f)    const { return F(x/f, y/f); }

    friend F operator * (T f, const F & q) { return q*f; }

    double dSqr()       const { return double(x)*x + double(y)*y; }
    double dLen()       const { return sqrt(double(x)*x + double(y)*y); }
    double dDot(cF & p) const { return double(x)*p.x + double(y)*p.y; }

    F inv()          const { return F(1.f/x, 1.f/y); }
    F dual()         const { return F(-y, x); }
    F scaled(cF & p) const { return F(x*p.x, y*p.y); }
    F div(cF & p)    const { return F(x/p.x, y/p.y); }

    F normalized(const T f) const { return *this * sqrt(f / sqr()); }
    F normalized()          const { return *this / len(); }

    F rotated(T c, T s) const { return F(c*x+s*y, c*y-s*x); }
    F rotated(T ang)    const {
        T c = cos(ang), s = sin(ang);
        return rotated(c,s);
    }

    typedef double QuadraticT;
    typedef double QuarticT;
};

typedef const FPoint cFPoint;

/// Functionality specific to 'double' (normally double) Points: DPoint.

class DPoint : public TPointFD<double> {
  public:
    TPointInst(DPoint, double, TPointFD<double>);

    typedef double QuadraticT;
    typedef double CubicT;
    typedef double QuarticT;

    F operator -()         const { return F(-x, -y); }
    F operator + (cF & b)  const { return F(x+b.x, y+b.y); }
    F operator - (cF & b)  const { return F(x-b.x, y-b.y); }
    F operator * (T f)     const { return F(x*f, y*f); }
    F operator * (float f) const { return F(x*f, y*f); }
    F operator / (T f)     const { return F(x/f, y/f); }

    friend F operator * (T f, const F & q) { return q*f; }

    F inv()          const { return F(1./x, 1./y); }
    F dual()         const { return F(-y, x); }
    F scaled(cF & p) const { return F(x*p.x, y*p.y); }
    F div(cF & p)    const { return F(x/p.x, y/p.y); }

    F normalized(const T f) const { return *this * sqrt(f / sqr()); }
    F normalized()          const { return *this / len(); }

    F rotated(T c, T s) const { return F(c*x+s*y, c*y-s*x); }
    F rotated(T ang)    const {
        T c = cos(ang), s = sin(ang);
        return rotated(c,s);
    }
};

typedef const DPoint cDPoint;

//#undef TPointInst
//#undef TPointHdr
//#undef TPointCtor

/// Arithmetic for TPoint
inline void highest(FPoint & p) { p.x= 7e37f; p.y= 7e37f; }
inline void  lowest(FPoint & p) { p.x=-7e37f; p.y=-7e37f; }
inline void highest(DPoint & p) { p.x= 9e99;  p.y= 9e99; }
inline void  lowest(DPoint & p) { p.x=-9e99;  p.y=-9e99; }

inline void zero(FPoint & p) { p.x=0; p.y=0; }
inline void zero(DPoint & p) { p.x=0; p.y=0; }

typedef FPoint FPoint2;
typedef DPoint DPoint2;

#endif // Plane_h
