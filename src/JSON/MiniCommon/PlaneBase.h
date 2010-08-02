#ifndef PlaneBase_h
#define PlaneBase_h

template <class T> class TPoint {
    bool operator !();
    operator bool();

  public:
    typedef T value_type;
    T x,y;

    TPoint() {}
    TPoint(T a, T b) : x(a), y(b) {}
    TPoint(const TPoint<T> & rhs) : x(rhs.x), y(rhs.y) {}

    const T & operator [](int i) const { return (&x)[i]; }
          T & operator [](int i)       { return (&x)[i]; }

    // the < operator is defined lexicographically, so that TPoints can be
    // used as keys in containers requiring strict weak ordering.
    bool operator < (const TPoint<T> & rhs) const {
        return (y==rhs.y) ? (x<rhs.x) : (y<rhs.y);
    }
};

#define cTPoint const TPoint

// C++ has no way of inheriting constructors, so this makes sure our
// constructors are consistent; also, we must perform some typedefs
// because otherwise parent typedefs will be active.  The same applies
// to TPointInst, TVectorHdr, and TVectorInst.

enum enumBoth { Both };

#define TPointCtor(name, parent)                      \
    name() {}                                          \
    name(enumBoth, T xy) : parent(xy,xy) {}             \
    name(T a, T b) : parent(a,b) {}                      \
    name(const TPointFDI<T> & p) : parent(p.x, p.y) {}

#define TPointHdr(name, parent) \
    typedef name<T> F;           \
    typedef const name<T> cF;     \
    TPointCtor(name, parent)

#define TPointInst(name, type, parent) \
    typedef type T;                     \
    typedef name F;                      \
    typedef const name cF;                \
    typedef type V;                        \
    TPointCtor(name, parent)

/// Extends TPoint with geometric behavior, suitable for integer and
/// floating-point types:

template <class T> class TPointFDI : public TPoint<T> {
  public:
    TPointHdr(TPointFDI, TPoint<T>);

    void set(T a, T b)        { TPoint<T>::x = a; TPoint<T>::y = b; }
    void clear()              { TPoint<T>::x = T(0), TPoint<T>::y = T(0); }

    T dot(cF & p)       const { return TPoint<T>::x*p.x + TPoint<T>::y*p.y; }
    T cross(cF & p)     const { return TPoint<T>::x*p.y - TPoint<T>::y*p.x; }
    T sqr()             const { return TPoint<T>::x*TPoint<T>::x + TPoint<T>::y*TPoint<T>::y; }

    T dot2d(cF & p)     const { return TPoint<T>::x*p.x + TPoint<T>::y*p.y; }
    T cross2d(cF & p)   const { return TPoint<T>::x*p.y - TPoint<T>::y*p.x; }
    T sqr2d()           const { return TPoint<T>::x*TPoint<T>::x + TPoint<T>::y*TPoint<T>::y; }

    void scale(cF & p)        { TPoint<T>::x *= p.x; TPoint<T>::y *= p.y; }
    cF & operator += (cF & p) { TPoint<T>::x += p.x; TPoint<T>::y += p.y; return *this; }
    cF & operator -= (cF & p) { TPoint<T>::x -= p.x; TPoint<T>::y -= p.y; return *this; }
    cF & operator *= (T f)    { TPoint<T>::x *= f; TPoint<T>::y *= f; return *this; }
    cF & operator /= (T f)    { TPoint<T>::x /= f; TPoint<T>::y /= f; return *this; }
    cF & operator *= (cF & p) { TPoint<T>::x *= p.x; TPoint<T>::y *= p.y; return *this; }
    cF & operator /= (cF & p) { TPoint<T>::x /= p.x; TPoint<T>::y /= p.y; return *this; }

    void updateMin(cF & p) { if (p.x < TPoint<T>::x) TPoint<T>::x = p.x; if (p.y < TPoint<T>::y) TPoint<T>::y = p.y; }
    void updateMax(cF & p) { if (p.x > TPoint<T>::x) TPoint<T>::x = p.x; if (p.y > TPoint<T>::y) TPoint<T>::y = p.y; }
};

#endif // PlaneBase_h
