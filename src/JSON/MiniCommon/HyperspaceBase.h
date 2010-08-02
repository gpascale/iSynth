#ifndef HyperspaceBase_h
#define HyperspaceBase_h

#include "Space.h"

template <class T> class TPoint4 {
    bool operator !();
    operator bool();

  public:
    typedef T value_type;
    T x,y,z,w;

    const T & operator [](int i) const { return (&x)[i]; }
          T & operator [](int i)       { return (&x)[i]; }
};

#define cTPoint4 const TPoint4

// C++ has no way of inheriting constructors, so this makes sure our
// constructors are consistent; also, we must perform some typedefs
// because otherwise parent typedefs will be active.  The same applies
// to TPoint4Inst, TVectorHdr, and TVectorInst.

enum enumAll4 { All4 };

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

// Extends TPoint4 with geometric behavior, suitable for integer and
// floating-point types:

template <class T> class TPoint4FDI : public TPoint4<T> {
  public:
    TPoint4Hdr(TPoint4FDI, TPoint4);

    void set(T a, T b, T c, T d) { TPoint4<T>::x=a; TPoint4<T>::y=b, TPoint4<T>::z=c, TPoint4<T>::w=d; }
    void clear()                 { TPoint4<T>::x=TPoint4<T>::y=TPoint4<T>::z=TPoint4<T>::w=T(0); }

    T dot(cF & p)          const { return TPoint4<T>::x*p.x + TPoint4<T>::y*p.y + TPoint4<T>::z*p.z + TPoint4<T>::w*p.w; }
    T sqr()                const { return TPoint4<T>::x*TPoint4<T>::x +
                                          TPoint4<T>::y*TPoint4<T>::y +
                                          TPoint4<T>::z*TPoint4<T>::z +
                                          TPoint4<T>::w*TPoint4<T>::w; }
    T sqr3()               const { return TPoint4<T>::x*TPoint4<T>::x +
                                          TPoint4<T>::y*TPoint4<T>::y +
                                          TPoint4<T>::z*TPoint4<T>::z; }

    void scale(cF & p)           { TPoint4<T>::x*=p.x; TPoint4<T>::y*=p.y; TPoint4<T>::z*=p.z; TPoint4<T>::w*=p.w;               }
    cF & operator += (cF & p)    { TPoint4<T>::x+=p.x; TPoint4<T>::y+=p.y; TPoint4<T>::z+=p.z; TPoint4<T>::w+=p.w; return *this; }
    cF & operator -= (cF & p)    { TPoint4<T>::x-=p.x; TPoint4<T>::y-=p.y; TPoint4<T>::z-=p.z; TPoint4<T>::w-=p.w; return *this; }
    cF & operator *= (T f)       { TPoint4<T>::x*=f;   TPoint4<T>::y*=f;   TPoint4<T>::z*=f;   TPoint4<T>::w*=f;   return *this; }
    cF & operator /= (T f)       { TPoint4<T>::x/=f;   TPoint4<T>::y/=f;   TPoint4<T>::z/=f;   TPoint4<T>::w/=f;   return *this; }
    cF & operator *= (cF p)      { TPoint4<T>::x*=p.x; TPoint4<T>::y*=p.y; TPoint4<T>::z*=p.z; TPoint4<T>::w*=p.w; return *this; }
    cF & operator /= (cF p)      { TPoint4<T>::x/=p.x; TPoint4<T>::y/=p.y; TPoint4<T>::z/=p.z; TPoint4<T>::w/=p.w; return *this; }

    void updateMin(cF & p) { if (p.x<TPoint4<T>::x) TPoint4<T>::x=p.x;
                             if (p.y<TPoint4<T>::y) TPoint4<T>::y=p.y;
                             if (p.z<TPoint4<T>::z) TPoint4<T>::z=p.z;
                             if (p.w<TPoint4<T>::w) TPoint4<T>::w=p.w; }
    void updateMax(cF & p) { if (p.x>TPoint4<T>::x) TPoint4<T>::x=p.x;
                             if (p.y>TPoint4<T>::y) TPoint4<T>::y=p.y;
                             if (p.z>TPoint4<T>::z) TPoint4<T>::z=p.z;
                             if (p.w>TPoint4<T>::w) TPoint4<T>::w=p.w; }
};

#undef TPoint4Inst
#undef TPoint4Hdr
#undef TPoint4Ctor

#endif // HyperspaceBase_h
