#ifndef Hyperspace_h
#define Hyperspace_h

#include "HyperspaceBase.h"
#include <math.h>

#define TPoint4Ctor(name, supertype)                                                                        \
    name() {}                                                                                                \
    name(enumAll4, T xyzw)   {supertype::x=xyzw, supertype::y=xyzw, supertype::z=xyzw, supertype::w=xyzw;}    \
    name(T a, T b, T c, T d) { supertype::x=a, supertype::y=b, supertype::z=c, supertype::w=d; }               \
    name(cF & p) { supertype::x=p.x, supertype::y=p.y, supertype::z=p.z, supertype::w=p.w; }

#define TPoint4Hdr(name, super) \
    typedef name<T> F;           \
    typedef const name<T> cF;     \
    TPoint4Ctor(name, super<T>)

#define TPoint4Inst(name, super, type)  \
    typedef type T;                      \
    typedef name F;                       \
    typedef const name cF;                 \
    typedef type V;                         \
    TPoint4Ctor(name, super<type>)

// TPoint4FD extends TPoint4FDI with behavior suitable only for floating-point.

template <class T> class TPoint4FD : public TPoint4FDI<T> {
  public:
    TPoint4Hdr(TPoint4FD, TPoint4FDI);

    bool operator == (cF & p) const { return TPoint4<T>::x==p.x && TPoint4<T>::y==p.y && TPoint4<T>::z==p.z && TPoint4<T>::w==p.w; }
    bool operator != (cF & p) const { return TPoint4<T>::x!=p.x || TPoint4<T>::y!=p.y || TPoint4<T>::z!=p.z || TPoint4<T>::w!=p.w; }
    bool operator ! ()        const { return !TPoint4<T>::x && !TPoint4<T>::y && !TPoint4<T>::z && !TPoint4<T>::w; }

    T len()                const { return sqrt(TPoint4FDI<T>::sqr()); }
    T len3()               const { return sqrt(TPoint4FDI<T>::sqr3()); }

    void normalize()    { *this /= TPoint4FDI<T>::len(); }
    void normalize(T f) { *this *= sqrt(f, TPoint4FDI<T>::sqr()); }
};

// Functionality specific to 'float' (normally float) Points: FPoint4.

class FPoint4 : public TPoint4FD<float> {
  public:
    TPoint4Inst(FPoint4, TPoint4FD, float);

    F operator - ()       const { return F(-x,-y,-z,-w); }
    F operator + (cF & b) const { return F(x+b.x, y+b.y, z+b.z, w+b.w); }
    F operator - (cF & b) const { return F(x-b.x, y-b.y, z-b.z, w-b.w); }
    F operator * (T f)    const { return F(x*f, y*f, z*f, w*f); }
    F operator / (T f)    const { return F(x/f, y/f, z/f, w/f); }

    friend F operator * (T f, const F & q) { return q*f; }

    double dSqr()         const { return double(x)*x +
                                         double(y)*y +
                                         double(z)*z +
                                         double(w)*w; }
    double dLen()         const { return sqrt(dSqr()); }
    double dDot(cF & p)   const { return double(x)*p.x +
                                         double(y)*p.y +
                                         double(z)*p.z +
                                         double(w)*p.w; }

    F inv()               const { return F(T(1./x), T(1./y), T(1./z), T(1./w)); }
    F scaled(cF & p)      const { return F(x*p.x, y*p.y, z*p.z, w*p.w); }

    F normalized()          const { return *this / len(); }

    FPoint3 to3() const { return FPoint3(x,y,z); }
};

typedef const FPoint4 cFPoint4;

// Functionality specific to 'double' (normally double) Points: DPoint4.

class DPoint4 : public TPoint4FD<double> {
  public:
    TPoint4Inst(DPoint4, TPoint4FD, double);

    typedef double QuadraticT;
    typedef double CubicT;
    typedef double QuarticT;

    F operator - ()       const { return F(-x,-y,-z,-w); }
    F operator + (cF & b) const { return F(x+b.x, y+b.y, z+b.z, w+b.w); }
    F operator - (cF & b) const { return F(x-b.x, y-b.y, z-b.z, w-b.w); }
    F operator * (T f)    const { return F(x*f, y*f, z*f, w*f); }
    F operator / (T f)    const { return F(x/f, y/f, z/f, w/f); }

    friend F operator * (T f, const F & q) { return q*f; }

    F inv()          const { return F(1./x, 1./y, 1./z, 1./w); }
    F scaled(cF & p) const { return F(x*p.x, y*p.y, z*p.z, w*p.w); }

    F normalized(const T f) const { return *this * sqrt(f / sqr()); }
    F normalized()          const { return *this / len(); }

    DPoint3 to3() const { return DPoint3(x,y,z); }
};

typedef const DPoint4 cDPoint4;

#undef TPoint4Inst
#undef TPoint4Hdr
#undef TPoint4Ctor

inline void highest(FPoint4 & p) { p.x=p.y=p.z=p.w= 7e37f; }
inline void  lowest(FPoint4 & p) { p.x=p.y=p.z=p.w=-7e37f; }
inline void highest(DPoint4 & p) { p.x=p.y=p.z=p.w= 9e99; }
inline void  lowest(DPoint4 & p) { p.x=p.y=p.z=p.w=-9e99; }

template <class T> inline void zero(TPoint4FDI<T> & p) { p.x=p.y=p.z=0; }

inline FPoint4  sqr(cFPoint4 & p) { return FPoint4(p.x*p.x, p.y*p.y, p.z*p.z, p.w*p.w); }
inline DPoint4  sqr(cDPoint4 & p) { return DPoint4(p.x*p.x, p.y*p.y, p.z*p.z, p.w*p.w); }
inline DPoint4 dSqr(cFPoint4 & p) { return DPoint4(double(p.x)*p.x,
                                                   double(p.y)*p.y,
                                                   double(p.z)*p.z,
                                                   double(p.w)*p.w); }

// project: returns vector in direction of b with length equal to
// projection of a onto b.

#define RPROJ return b*(a.dot(b)/b.sqr());
inline FPoint4 project(cFPoint4 & a, cFPoint4 & b) { RPROJ }
inline DPoint4 project(cDPoint4 & a, cDPoint4 & b) { RPROJ }
#undef RPROJ

inline DPoint3 toDPoint3(const DPoint4 & p) {
    return DPoint3(p.x, p.y, p.z);
}

inline double dot3(const DPoint4 & lhs, const DPoint4 & rhs) {
    return lhs.x*rhs.x +
           lhs.y*rhs.y +
           lhs.z*rhs.z;
}

inline double dot3(const DPoint3 & lhs, const DPoint4 & rhs) {
    return lhs.x*rhs.x +
           lhs.y*rhs.y +
           lhs.z*rhs.z;
}

inline double dot3(const DPoint4 & lhs, const DPoint3 & rhs) {
    return lhs.x*rhs.x +
           lhs.y*rhs.y +
           lhs.z*rhs.z;
}

inline double dot(const DPoint3 & lhs, const DPoint4 & rhs) {
    return lhs.x*rhs.x +
           lhs.y*rhs.y +
           lhs.z*rhs.z +
                 rhs.w;
}

inline double dot(const DPoint4 & lhs, const DPoint3 & rhs) {
    return lhs.x*rhs.x +
           lhs.y*rhs.y +
           lhs.z*rhs.z +
           lhs.w;
}

#endif // Hyperspace_h
