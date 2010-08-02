#ifndef SpaceBase_h
#define SpaceBase_h

#include "PlaneBase.h"

template <class T> class TPoint3 {

public:
    
    bool operator !();
    operator bool();


    typedef T value_type;
    T x,y,z;

    TPoint3() {}
    TPoint3(T a, T b, T c) : x(a), y(b), z(c) {}
    TPoint3(const TPoint3<T> & rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}

    const T & operator [](int i) const { return (&x)[i]; }
          T & operator [](int i)       { return (&x)[i]; }

    // the < operator is defined lexicographically, so that TPoint3s can be
    // used as keys in containers requiring strict weak ordering.
    bool operator < (const TPoint3<T> & rhs) const {
        return (z==rhs.z) ? ((y==rhs.y) ? (x<rhs.x) : (y<rhs.y)) : (z<rhs.z);
    }
};

#define cTPoint3 const TPoint3

// C++ has no way of inheriting constructors, so this makes sure our
// constructors are consistent; also, we must perform some typedefs
// because otherwise parent typedefs will be active.

enum enumAll3 { All3 };

#define TPoint3Ctor(name, parent, twod)                    \
    name() {}                                               \
    name(enumAll3, T xyz) : parent(xyz,xyz,xyz) {}           \
    name(T a, T b, T c) : parent(a,b,c) {}                    \
    name(const TPoint3<T> & p) : parent(p.x, p.y, p.z) {}      \
    explicit name(const twod & rhs) : parent(rhs.x, rhs.y, 0) {}

#define TPoint3Hdr(name, parent, twod)               \
    typedef name<T> F;                                \
    typedef const name<T> cF;                          \
    TPoint3Ctor(name, parent, twod)                     \
    F & operator = (const twod & rhs) {                  \
        parent::x=rhs.x;  parent::y=rhs.y;  parent::z=0;  \
        return *this; }

#define TPoint3Inst(name, type, parent)         \
    typedef type T;                              \
    typedef name F;                               \
    typedef const name cF;                         \
    typedef type V;                                 \
    TPoint3Ctor(name, parent, TPoint<type>)          \
    F & operator = (const TPoint<type> & rhs) {       \
        x=rhs.x;  y=rhs.y;  z=0;                       \
        return *this; }

// Extends TPoint3 with geometric behavior, suitable for integer and
// floating-point types:

template <class T> class TPoint3FDI : public TPoint3<T> {
  public:
    TPoint3Hdr(TPoint3FDI, TPoint3<T>, TPoint<T>);

    void set(T a, T b)        { TPoint3<T>::x=a; TPoint3<T>::y=b, TPoint3<T>::z=0; }
    void set(T a, T b, T c)   { TPoint3<T>::x=a; TPoint3<T>::y=b, TPoint3<T>::z=c; }
    void clear()              { TPoint3<T>::x=TPoint3<T>::y=TPoint3<T>::z=T(0); }

    T dot(cF & p)       const { return TPoint3<T>::x*p.x + TPoint3<T>::y*p.y + TPoint3<T>::z*p.z; }
    T dot2d(cF & p)     const { return TPoint3<T>::x*p.x + TPoint3<T>::y*p.y; }
    T sqr()             const { return TPoint3<T>::x*TPoint3<T>::x + TPoint3<T>::y*TPoint3<T>::y + TPoint3<T>::z*TPoint3<T>::z; }
    T sqr2d()           const { return TPoint3<T>::x*TPoint3<T>::x + TPoint3<T>::y*TPoint3<T>::y; }
    T len()             const { return sqrt(sqr()); }
    T len2d()           const { return sqrt(sqr2d()); }

    void scale(cF & p)        { TPoint3<T>::x*=p.x; TPoint3<T>::y*=p.y; TPoint3<T>::z*=p.z;               }
    cF & operator += (cF & p) { TPoint3<T>::x+=p.x; TPoint3<T>::y+=p.y; TPoint3<T>::z+=p.z; return *this; }
    cF & operator -= (cF & p) { TPoint3<T>::x-=p.x; TPoint3<T>::y-=p.y; TPoint3<T>::z-=p.z; return *this; }
    cF & operator *= (T f)    { TPoint3<T>::x*=f;   TPoint3<T>::y*=f;   TPoint3<T>::z*=f;   return *this; }
    cF & operator /= (T f)    { TPoint3<T>::x/=f;   TPoint3<T>::y/=f;   TPoint3<T>::z/=f;   return *this; }
    cF & operator *= (cF p)   { TPoint3<T>::x*=p.x; TPoint3<T>::y*=p.y; TPoint3<T>::z*=p.z; return *this; }
    cF & operator /= (cF p)   { TPoint3<T>::x/=p.x; TPoint3<T>::y/=p.y; TPoint3<T>::z/=p.z; return *this; }

    void updateMin(cF & p) { if (p.x<TPoint3<T>::x) TPoint3<T>::x=p.x; if (p.y<TPoint3<T>::y) TPoint3<T>::y=p.y; if (p.z<TPoint3<T>::z) TPoint3<T>::z=p.z; }
    void updateMax(cF & p) { if (p.x>TPoint3<T>::x) TPoint3<T>::x=p.x; if (p.y>TPoint3<T>::y) TPoint3<T>::y=p.y; if (p.z>TPoint3<T>::z) TPoint3<T>::z=p.z; }
};

#endif // SpaceBase_h
