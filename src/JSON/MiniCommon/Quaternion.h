#ifndef Quaternion_h
#define Quaternion_h

#include "Space.h"
#include "Hyperspace.h"
#include "Matrix.h"
#include "Random.h"


template <class T>
class TQuaternion {
  public:
    typedef TQuaternion<T> Q;
    typedef TPoint3FDI<T> V;
    typedef const Q cQ;
    typedef const V cV;
    typedef DMatrix4 M;
    typedef const M cM;
    typedef DMatrix3 M3;
    typedef const M3 cM3;

    T x,y,z,w;

    enum { D=4, R=3 };

    const T & operator [] (int i) const { b_assert(i>=0 && i<D); return (&x)[i]; }
          T & operator [] (int i)       { b_assert(i>=0 && i<D); return (&x)[i]; }

    TQuaternion()                   : x(0), y(0), z(0), w(1) {}
    explicit TQuaternion(int)       : x(0), y(0), z(0), w(1) {}
    TQuaternion(T a, T b, T c, T d) : x(a), y(b), z(c), w(d) {}
    TQuaternion(T a, T b, T c     ) : x(a), y(b), z(c), w(1.) {}
    TQuaternion(T a, T b          ) : x(a), y(b), z(0), w(1.) {}
    TQuaternion(V & v, T d)         : x(v.x), y(v.y), z(v.z), w(d) {}
    TQuaternion(cQ & rhs)           : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

    template <class U> explicit TQuaternion(const TQuaternion<U> & rhs) :
        x(T(rhs.x)),
        y(T(rhs.y)),
        z(T(rhs.z)),
        w(T(rhs.w)) {}

    Q & operator = (cQ & v) { x=v.x; y=v.y; z=v.z; w=v.w; return *this; }
    Q & operator += (cQ & v) { x+=v.x; y+=v.y; z+=v.z; w+=v.w; return *this; }
    Q & operator -= (cQ & v) { x-=v.x; y-=v.y; z-=v.z; w-=v.w; return *this; }

    Q operator + (cQ & v) const { return Q(x+v.x, y+v.y, z+v.z, w+v.w); }
    Q operator - (cQ & v) const { return Q(x-v.x, y-v.y, z-v.z, w-v.w); }

    Q & operator *= (T f) { x*=f, y*=f, z*=f, w*=f; return *this; }
    Q & operator /= (T f) { x/=f, y/=f, z/=f, w/=f; return *this; }

    Q operator - () const { return Q(-x,-y,-z,w); }
    Q operator * (T f) const { return Q(x*f, y*f, z*f, w*f); }
    Q operator / (T f) const { return Q(x/f, y/f, z/f, w/f); }

    void Identity() { x=y=z=T(0); w=T(1); }
    DPoint3 vectorPart() const { return V(x,y,z); }

    M toMatrix() const;
    void toMatrix(M & m) const;
    void fromMatrix(cM & m);

    M3 toMatrix3() const;
    void toMatrix(M3 & m) const;
    void fromMatrix(cM3 & m);

    void fromAngularMomentum(T lx, T ly, T lz);
    DPoint3 toAngularMomentum() const;

    void fromAxis(cV & axis, double theta);
    void toAxis(V * axis, double * theta) const;
    void fromBallPoints(cV & from, cV & to);
    void toBallPoints(V & from, V & to) const;
    void fromAngularMomentum(cV & L);
    void toAngularMomentum(T * lx, T * ly, T * lz) const;
    void set(cV & v, T d) { x=v.x,y=v.y,z=v.z,w=d; }
    T dot(cQ & rhs) const { return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w; }

    void set(T a, T b, T c, T d) { x=a,y=b,z=c,w=d; }
    void set(T a, T b, T c     ) { x=a,y=b,z=c,w=1.; }
    void set(T a, T b          ) { x=a,y=b,z=0,w=1.; }

    double sqr() const { return x*x + y*y + z*z + w*w; }
    double len() const { return sqrt(sqr()); }
    void normalize() { *this /= len(); }
    void random();

    Q conj() const { return Q(-x,-y,-z,w); }
    Q inv()  const { return conj()/sqr(); }
};

typedef TQuaternion<double> DQuaternion;
typedef TQuaternion<float > FQuaternion;

template <class T> inline
TQuaternion<T> operator * (const TQuaternion<T> & lhs,
                           const TQuaternion<T> & rhs) {
    return TQuaternion<T>(lhs.w*rhs.x + lhs.x*rhs.w + lhs.y*rhs.z - lhs.z*rhs.y,
                          lhs.w*rhs.y + lhs.y*rhs.w + lhs.z*rhs.x - lhs.x*rhs.z,
                          lhs.w*rhs.z + lhs.z*rhs.w + lhs.x*rhs.y - lhs.y*rhs.x,
                          lhs.w*rhs.w - lhs.x*rhs.x - lhs.y*rhs.y - lhs.z*rhs.z);
}
template <class T> inline
TQuaternion<T> quaternionProduct(const TQuaternion<T> & lhs,
                                 const TQuaternion<T> & rhs) { return lhs*rhs; }

template <class T> inline
DPoint3 xyzCross(const TQuaternion<T> & a,
                 const TQuaternion<T> & b) {
    return DPoint3(a.y*b.z - a.z*b.y,
                   a.z*b.x - a.x*b.z,
                   a.x*b.y - a.y*b.x);
}

template <class T>
void SlerpQuaternion(double alpha,
                     const TQuaternion<T> & a,
                     const TQuaternion<T> & b,
                     TQuaternion<T> & q,
                     int spin =0);

template <class T>
void SlerpQuaternion3(double alpha,
                      double beta,
                      double gamma,
                      const TQuaternion<T> & a,
                      const TQuaternion<T> & b,
                      const TQuaternion<T> & c,
                      TQuaternion<T> & q);

template <class T>
bool operator==(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs) {
    return lhs.x == rhs.x
        && lhs.y == rhs.y
        && lhs.z == rhs.z
        && lhs.w == rhs.w;
}

template <class T>
bool operator!=(const TQuaternion<T>& lhs, const TQuaternion<T>& rhs) {
    return !(lhs == rhs);
}

#include "Quaternion.hpp"

#endif // Quaternion_h
