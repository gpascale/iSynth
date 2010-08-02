#ifndef Matrix_h
#define Matrix_h

#include "Plane.h"
#include "Space.h"
#include "Hyperspace.h"

// ENABLE_PROJECTIVE_SHORTCUT, when defined, allows DMatrix4 * DPoint3 type
// operators, which assume w==1.

#define ENABLE_PROJECTIVE_SHORTCUT 0

#define MatrixT(NAME,TYPE,D,R)      \
  public:                            \
    typedef       TYPE             T; \
    typedef const TYPE            cT;  \
    typedef       NAME##Point##D   F;   \
    typedef const NAME##Point##D  cF;    \
    typedef       NAME##Matrix##D  M;     \
    typedef const NAME##Matrix##D cM;      \
    F v[D];                                 \
    NAME##Matrix##D() { Identity(); }        \
    NAME##Matrix##D(cM & rhs)               { for (int i=0; i<D; i++) v[i]=rhs.v[i]; }              \
    NAME##Matrix##D & operator = (cM & rhs) { for (int i=0; i<D; i++) v[i]=rhs.v[i]; return *this; } \
    const F & operator [] (int i) const { return v[i]; }                                              \
          F & operator [] (int i)       { return v[i]; }                                               \
    bool operator == (const M & rhs) const { for (int i=0; i<D; i++) { if (!(v[i]==rhs.v[i])) return false; } return  true; } \
    bool operator != (const M & rhs) const { for (int i=0; i<D; i++) { if (  v[i]!=rhs.v[i] ) return  true; } return false; }  \
    void Identity() {                     \
        int i,j;                           \
        for (i=0; i<D; i++)                 \
            for (j=0; j<D; j++)              \
                v[i][j] = i==j ? T(1) : T(0); \
    }                                          \
    void transpose() {                  \
        int i,j;                         \
        for (i=0; i<D; i++)               \
            for (j=0; j<i; j++)            \
                matrix_Swap(v[i][j], v[j][i]);     \
    }                                        \
    void premultiply(cM & lhs) {              \
        M tmp;                                 \
        for (int j=0; j<D; j++) {               \
            for (int i=0; i<D; i++) {            \
                T f = T(0);                       \
                for (int k=0; k<D; k++)            \
                    f += lhs[i][k] * (*this)[k][j]; \
                tmp[i][j] = f;                       \
            }                                         \
        }                                              \
        *this = tmp;                                    \
    }                                                    \
    void postmultiply(cM & rhs) {             \
        M tmp;                                 \
        for (int j=0; j<D; j++) {               \
            for (int i=0; i<D; i++) {            \
                T f = T(0);                       \
                for (int k=0; k<D; k++)            \
                    f += (*this)[i][k] * rhs[k][j]; \
                tmp[i][j] = f;                       \
            }                                         \
        }                                              \
        *this = tmp;                                    \
    }                                                    \
    void rotate(cM & rhs) {                  \
        M tmp;                                \
        int i,j;                               \
        for (j=0; j<R; j++) {                   \
            for (i=0; i<R; i++) {                \
                T f = T(0);                       \
                for (int k=0; k<R; k++)            \
                    f += (*this)[i][k] * rhs[k][j]; \
                tmp[i][j] = f;                       \
            }                                         \
        }                                              \
        for (j=0; j<R; j++) {                           \
            for (i=0; i<R; i++)                          \
                (*this)[i][j] = tmp[i][j];                \
        }                                                  \
    }                                                       \
    void unrotate(cM & rhs) {                \
        M tmp;                                \
        int i,j,k;                             \
        for (j=0; j<R; j++) {                   \
            for (i=0; i<R; i++) {                \
                T f = T(0);                       \
                for (k=0; k<R; k++)                \
                    f += (*this)[i][k] * rhs[j][k]; \
                tmp[i][j] = f;                       \
            }                                         \
        }                                              \
        for (j=0; j<R; j++) {                           \
            for (i=0; i<R; i++)                          \
                (*this)[i][j] = tmp[i][j];                \
        }                                                  \
    }                                                       \
    T trace() const {                        \
        int i;                                \
        T t = T(0);                            \
        for (i=0; i<D; i++) t += (*this)[i][i]; \
        return t;                                \
    }

template <class T> inline void matrix_Swap(T & a, T & b) {
    T c = a;
    a = b;
    b = c;
}

class DMatrix2 { MatrixT(D,double, 2,2)
    DPoint2 diag() const { return DPoint2(v[0][0], v[1][1]); }
};

class DMatrix3 { MatrixT(D,double, 3,2)

    DPoint3 diag() const { return DPoint3(v[0][0], v[1][1], v[2][2]); }

    void rotate(double theta) {
        M m;
        const double c = cos(theta);
        const double s = sin(theta);
        m[0][0] = c; m[0][1] = -s; m[0][2] = 0;
        m[1][0] = s; m[1][1] =  c; m[1][2] = 0;
        m[2][0] = 0; m[2][1] =  0; m[2][2] = 1;
        postmultiply(m);
    }
    void translate(double x, double y) {
        M tmp;
        tmp[0][2] = x;
        tmp[1][2] = y;
        postmultiply(tmp);
    }
    void preTranslate(double x, double y) {
        M tmp;
        tmp[0][2] = x;
        tmp[1][2] = y;
        premultiply(tmp);
    }
    void translate(const DPoint & p) { translate(p.x, p.y); }
    void preTranslate(const DPoint & p) { preTranslate(p.x, p.y); }
};
class DMatrix4 { MatrixT(D,double, 4,3)

    DPoint4 diag() const { return DPoint4(v[0][0], v[1][1], v[2][2], v[3][3]); }

    /// homogeneous assignment-- assumes rhs is for 2D homogeneous transforms!
    DMatrix4 & operator = (const DMatrix3 & rhs) {
        v[0].set(rhs[0].x, rhs[0].y, 0., rhs[0].z);
        v[1].set(rhs[1].x, rhs[1].y, 0., rhs[1].z);
        v[2].set(     0. ,      0. , 1.,      0. );
        v[3].set(     0. ,      0. , 0.,      1. );
        return *this;
    }
    DMatrix4(const DMatrix3 & rhs) { operator = (rhs); }
    /// form taking a linear 3D transform on the rhs
    DMatrix4(const DMatrix3 & rhs, bool) {
        v[0].set(rhs[0].x, rhs[0].y, rhs[0].z, 0.);
        v[1].set(rhs[1].x, rhs[1].y, rhs[1].z, 0.);
        v[2].set(rhs[2].x, rhs[2].y, rhs[2].z, 0.);
        v[3].set(     0. ,      0. ,      0.,  1.);
    }
    DMatrix4(T * p) {
        // OpenGL constructor-- OpenGL stores translation in entries 12,13,14.
        v[0].set(p[0], p[4], p[ 8], p[12]);
        v[1].set(p[1], p[5], p[ 9], p[13]);
        v[2].set(p[2], p[6], p[10], p[14]);
        v[3].set(p[3], p[7], p[11], p[15]);
    }
    void translate(T x, T y, T z) {
        M tmp;
        tmp[0][3] = x;
        tmp[1][3] = y;
        tmp[2][3] = z;
        postmultiply(tmp);
    }
    void preTranslate(T x, T y, T z) {
        M tmp;
        tmp[0][3] = x;
        tmp[1][3] = y;
        tmp[2][3] = z;
        premultiply(tmp);
    }
    void translate(const DPoint3 & p) { translate(p.x, p.y, p.z); }
    void preTranslate(const DPoint3 & p) { preTranslate(p.x, p.y, p.z); }
};

#undef MatrixT

inline DPoint2 diag(const DMatrix2 & M) { return M.diag(); }
inline DPoint3 diag(const DMatrix3 & M) { return M.diag(); }
inline DPoint4 diag(const DMatrix4 & M) { return M.diag(); }

#define TransformM(D)                     \
inline void transform(DPoint##D & lhs,     \
                      const DMatrix##D & M, \
                      const DPoint##D & p) { \
    for (int i=0; i<D; i++) {                 \
        double f = 0.;                         \
        for (int j=0; j<D; j++)                 \
            f += M[i][j] * p[j];                 \
        lhs[i] = f;                               \
    }                                              \
}                                                   \
inline DPoint##D transform(const DMatrix##D & M,     \
                           const DPoint##D & p) {     \
    DPoint##D lhs;                                     \
    transform(lhs, M, p);                               \
    return lhs;                                          \
}

TransformM(2)
TransformM(3)
TransformM(4)

#if ENABLE_PROJECTIVE_SHORTCUT

// transform and untransform assume unitary D-1 matrices with offsets,
// as for rotation with offset; this won't invert the matrix in the
// general case.

inline void transform(DPoint2 & lhs,
                      const DMatrix3 & M,
                      DPoint2 p) {
    lhs.x = M[0][0] * p.x
          + M[0][1] * p.y
          + M[0][2];
    lhs.y = M[1][0] * p.x
          + M[1][1] * p.y
          + M[1][2];
}

inline DPoint2 transform(const DMatrix3 & M,
                         const DPoint2 & p) {
    DPoint2 lhs;
    transform(lhs, M, p);
    return lhs;
}

inline void untransform(DPoint2 & lhs,
                        const DMatrix3 & M,
                        DPoint2 p) {
    p.x  -= M[0][2];
    p.y  -= M[1][2];
    lhs.x = M[0][0] * p.x
          + M[1][0] * p.y;
    lhs.y = M[0][1] * p.x
          + M[1][1] * p.y;
}

inline DPoint2 untransform(const DMatrix3 & M,
                           const DPoint2 & p) {
    DPoint2 lhs;
    untransform(lhs, M, p);
    return lhs;
}

#endif // ENABLE_PROJECTIVE_SHORTCUT

inline DMatrix2 operator - (const DMatrix2 & A, const DMatrix2 & B) {
    DMatrix2 C;
    C.v[0] = A.v[0] - B.v[0];
    C.v[1] = A.v[1] - B.v[1];
    return C;
}

inline DMatrix2 operator + (const DMatrix2 & A, const DMatrix2 & B) {
    DMatrix2 C;
    C.v[0] = A.v[0] + B.v[0];
    C.v[1] = A.v[1] + B.v[1];
    return C;
}

inline DMatrix3 operator - (const DMatrix3 & A, const DMatrix3 & B) {
    DMatrix3 C;
    C.v[0] = A.v[0] - B.v[0];
    C.v[1] = A.v[1] - B.v[1];
    C.v[2] = A.v[2] - B.v[2];
    return C;
}

inline DMatrix3 operator + (const DMatrix3 & A, const DMatrix3 & B) {
    DMatrix3 C;
    C.v[0] = A.v[0] + B.v[0];
    C.v[1] = A.v[1] + B.v[1];
    C.v[2] = A.v[2] + B.v[2];
    return C;
}

inline DMatrix4 operator - (const DMatrix4 & A, const DMatrix4 & B) {
    DMatrix4 C;
    C.v[0] = A.v[0] - B.v[0];
    C.v[1] = A.v[1] - B.v[1];
    C.v[2] = A.v[2] - B.v[2];
    C.v[3] = A.v[3] - B.v[3];
    return C;
}

inline DMatrix4 operator + (const DMatrix4 & A, const DMatrix4 & B) {
    DMatrix4 C;
    C.v[0] = A.v[0] + B.v[0];
    C.v[1] = A.v[1] + B.v[1];
    C.v[2] = A.v[2] + B.v[2];
    C.v[3] = A.v[3] + B.v[3];
    return C;
}

inline DMatrix2 operator * (const DMatrix2 & lhs, double f) {
    DMatrix2 M;
    M.v[0] = lhs[0]*f;
    M.v[1] = lhs[1]*f;
    return M;
}

inline DMatrix3 operator * (const DMatrix3 & lhs, double f) {
    DMatrix3 M;
    M.v[0] = lhs[0]*f;
    M.v[1] = lhs[1]*f;
    M.v[2] = lhs[2]*f;
    return M;
}

inline DMatrix4 operator * (const DMatrix4 & lhs, double f) {
    DMatrix4 M;
    M.v[0] = lhs[0]*f;
    M.v[1] = lhs[1]*f;
    M.v[2] = lhs[2]*f;
    M.v[3] = lhs[3]*f;
    return M;
}

// determinants
double det (const DMatrix2 & M);
double det2(const DMatrix3 & M); /// homogeneous
double det (const DMatrix3 & M);
double det2(const DMatrix4 & M); /// homogeneous
double det3(const DMatrix4 & M); /// homogeneous
double det (const DMatrix4 & M);

inline DMatrix2 operator * (const DMatrix2 & lhs,
                            const DMatrix2 & rhs) {
    DMatrix2 M;
    M[0][0] = lhs[0][0] * rhs[0][0] + lhs[0][1] * rhs[1][0];
    M[1][0] = lhs[1][0] * rhs[0][0] + lhs[1][1] * rhs[1][0];
    M[0][1] = lhs[0][0] * rhs[0][1] + lhs[0][1] * rhs[1][1];
    M[1][1] = lhs[1][0] * rhs[0][1] + lhs[1][1] * rhs[1][1];
    return M;
}

/// matrix inverse for a general homogeneous 2D matrix (6 deg of m_maxConcurrentConnectiondom)
DMatrix3 invH(const DMatrix3 & M);

inline DMatrix3 operator * (const DMatrix3 & lhs,
                            const DMatrix3 & rhs) {
    DMatrix3 M;
    const int D = 3;
    for (int j=0; j<D; j++) {
        for (int i=0; i<D; i++) {
            double f = 0.;
            for (int k=0; k<D; k++)
                f += lhs[i][k] * rhs[k][j];
            M[i][j] = f;
        }
    }
    return M;
}

// left matrix inverse for a 2x2 matrix:
// inv(M)*M == Identity
DMatrix2 inv(const DMatrix2 & M);

// left matrix inverse for a general 3x3 matrix:
// inv(M)*M == Identity
DMatrix3 inv(const DMatrix3 & M);

// left matrix inverse for a general 4x4 matrix:
// inv(M)*M == Identity
DMatrix4 inv(const DMatrix4 & M);

/// version of the 4D matrix inverse that assumes a homogeneous transform,
/// i.e. bottom row all zeroes except M[3][3]==1.

DMatrix4 invH(const DMatrix4 & M);

/// invIH inverts homogeneous 4x4 matrices with no off-diagonal elements
/// in the top left 3x3 block-- much easier than the general case!
/// This form is encountered in NDC <--> viewport matrices.
DMatrix4 invIH(const DMatrix4 & M);

inline DMatrix4 operator * (const DMatrix4 & lhs,
                            const DMatrix4 & rhs) {
    DMatrix4 M;
    const int D = 4;
    for (int j=0; j<D; j++) {
        for (int i=0; i<D; i++) {
            double f = 0.;
            for (int k=0; k<D; k++)
                f += lhs[i][k] * rhs[k][j];
            M[i][j] = f;
        }
    }
    return M;
}

inline DPoint multiply(const DMatrix3 & M,
                       const DPoint & p) {
    return DPoint(M[0].x * p.x + M[0].y * p.y + M[0].z,
                  M[1].x * p.x + M[1].y * p.y + M[1].z);
}

inline DPoint project(const DMatrix3 & M,
                      const DPoint & p) {

    DPoint3 q(M[0].x * p.x + M[0].y * p.y + M[0].z,
              M[1].x * p.x + M[1].y * p.y + M[1].z,
              M[2].x * p.x + M[2].y * p.y + M[2].z);
    const double w1 = 1./q.z;
    return DPoint(q.x * w1,
                  q.y * w1);
}

#if ENABLE_PROJECTIVE_SHORTCUT
inline DPoint operator * (const DMatrix3 & M, const DPoint & p) { return multiply(M,p); }
inline DPoint operator * (const DPoint & p, const DMatrix3 & M) { return multiply(M,p); }
#else // !ENABLE_PROJECTIVE_SHORTCUT
inline DPoint operator * (const DMatrix3 & M, const DPoint & p) { return  project(M,p); }
inline DPoint operator * (const DPoint & p, const DMatrix3 & M) { return  project(M,p); }
#endif // ENABLE_PROJECTIVE_SHORTCUT

inline DPoint2 multiply(const DMatrix2 & M, const DPoint2 & p) {
    return DPoint2(M[0].x * p.x + M[0].y * p.y,
                   M[1].x * p.x + M[1].y * p.y);
}

inline DPoint3 multiply(const DMatrix3 & M, const DPoint3 & p) {
    return DPoint3(M[0].x * p.x + M[0].y * p.y + M[0].z * p.z,
                   M[1].x * p.x + M[1].y * p.y + M[1].z * p.z,
                   M[2].x * p.x + M[2].y * p.y + M[2].z * p.z);
}


inline DPoint3 multiply(const DMatrix4 & M, const DPoint & p) {
    return DPoint3(M[0].x * p.x + M[0].y * p.y + M[0].w,
                   M[1].x * p.x + M[1].y * p.y + M[1].w,
                   M[2].x * p.x + M[2].y * p.y + M[2].w);
}

inline DPoint3 multiply(const DMatrix4 & M, const DPoint3 & p) {
    return DPoint3(M[0].x * p.x + M[0].y * p.y + M[0].z * p.z + M[0].w,
                   M[1].x * p.x + M[1].y * p.y + M[1].z * p.z + M[1].w,
                   M[2].x * p.x + M[2].y * p.y + M[2].z * p.z + M[2].w);
}

inline DPoint4 multiply(const DMatrix4 & M, const DPoint4 & p) {
    return DPoint4(M[0].x * p.x + M[0].y * p.y + M[0].z * p.z + M[0].w * p.w,
                   M[1].x * p.x + M[1].y * p.y + M[1].z * p.z + M[1].w * p.w,
                   M[2].x * p.x + M[2].y * p.y + M[2].z * p.z + M[2].w * p.w,
                   M[3].x * p.x + M[3].y * p.y + M[3].z * p.z + M[3].w * p.w);
}

inline DPoint3 project(const DMatrix4 & M, const DPoint & p) {
    DPoint4 q(M[0].x * p.x + M[0].y * p.y + M[0].w,
              M[1].x * p.x + M[1].y * p.y + M[1].w,
              M[2].x * p.x + M[2].y * p.y + M[2].w,
              M[3].x * p.x + M[3].y * p.y + M[3].w);
    const double w1 = 1./q.w;
    return DPoint3(q.x * w1,
                   q.y * w1,
                   q.z * w1);
}

inline DPoint3 project(const DMatrix4 & M, const DPoint3 & p) {
    DPoint4 q(M[0].x * p.x + M[0].y * p.y + M[0].z * p.z + M[0].w,
              M[1].x * p.x + M[1].y * p.y + M[1].z * p.z + M[1].w,
              M[2].x * p.x + M[2].y * p.y + M[2].z * p.z + M[2].w,
              M[3].x * p.x + M[3].y * p.y + M[3].z * p.z + M[3].w);
    const double w1 = 1./q.w;
    return DPoint3(q.x * w1,
                   q.y * w1,
                   q.z * w1);
}

inline DPoint3 project(const DMatrix4 & M, const DPoint4 & p) {
    DPoint4 q(M[0].x * p.x + M[0].y * p.y + M[0].z * p.z + M[0].w * p.w,
              M[1].x * p.x + M[1].y * p.y + M[1].z * p.z + M[1].w * p.w,
              M[2].x * p.x + M[2].y * p.y + M[2].z * p.z + M[2].w * p.w,
              M[3].x * p.x + M[3].y * p.y + M[3].z * p.z + M[3].w * p.w);
    const double w1 = 1./q.w;
    return DPoint3(q.x * w1,
                   q.y * w1,
                   q.z * w1);
}

inline DPoint3 project3(const DMatrix4 & M, const DPoint4 & p) {
    DPoint4 q(M[0].x * p.x + M[0].y * p.y + M[0].z * p.z + M[0].w,
              M[1].x * p.x + M[1].y * p.y + M[1].z * p.z + M[1].w,
              M[2].x * p.x + M[2].y * p.y + M[2].z * p.z + M[2].w,
              M[3].x * p.x + M[3].y * p.y + M[3].z * p.z + M[3].w);
    const double w1 = 1./q.w;
    return DPoint3(q.x * w1,
                   q.y * w1,
                   q.z * w1);
}


inline DPoint3 operator * (const DMatrix3 & M, const DPoint3  & p) { return multiply(M,p); }
inline DPoint4 operator * (const DMatrix4 & M, const DPoint4  & p) { return multiply(M,p); }
inline DPoint3 operator * (const DPoint3  & p, const DMatrix3 & M) { return multiply(M,p); }
inline DPoint4 operator * (const DPoint4  & p, const DMatrix4 & M) { return multiply(M,p); }

#if ENABLE_PROJECTIVE_SHORTCUT
inline DPoint3 operator * (const DMatrix4 & M, const DPoint   & p) { return multiply(M,p); }
inline DPoint3 operator * (const DMatrix4 & M, const DPoint3  & p) { return multiply(M,p); }
inline DPoint3 operator * (const DPoint   & p, const DMatrix4 & M) { return multiply(M,p); }
inline DPoint3 operator * (const DPoint3  & p, const DMatrix4 & M) { return multiply(M,p); }
#else // !ENABLE_PROJECTIVE_SHORTCUT
inline DPoint3 operator * (const DMatrix4 & M, const DPoint   & p) { return  project(M,p); }
inline DPoint3 operator * (const DMatrix4 & M, const DPoint3  & p) { return  project(M,p); }
inline DPoint3 operator * (const DPoint   & p, const DMatrix4 & M) { return  project(M,p); }
inline DPoint3 operator * (const DPoint3  & p, const DMatrix4 & M) { return  project(M,p); }
#endif // ENABLE_PROJECTIVE_SHORTCUT

inline DPoint3 vectorTransform(const DMatrix4 & M,
                               const DPoint3 & p) {
    return DPoint3(M[0].x * p.x + M[0].y * p.y + M[0].z * p.z,
                   M[1].x * p.x + M[1].y * p.y + M[1].z * p.z,
                   M[2].x * p.x + M[2].y * p.y + M[2].z * p.z);
}

inline DMatrix3 inferIsoTransform(const DPoint & a0, const DPoint & a1,
                                  const DPoint & b0, const DPoint & b1) {
    const DPoint
        p = a0 - b0,
        q = a1 - b1;
    const double
        det0 = 1. / -(p.x*p.x + p.y*p.y),
        deta = -(q.x*p.x + q.y*p.y),
        detb =   p.x*q.y - q.x*p.y;
    DMatrix3 M;
    M[0].x = M[1].y = deta * det0;
    M[0].y = detb * det0;
    M[1].x = -M[0].y;
    const DPoint o = a1 - DPoint(M[0].x*a0.x + M[0].y*a0.y + M[0].z,
                                 M[1].x*a0.x + M[1].y*a0.y + M[1].z); //M*a0;
    M[0].z = o.x;
    M[1].z = o.y;
    return M;
}

//-----------------------------------------------------------------------------

/// Infers a general 3x3 linear transform M by solving
///   M*p1 == p1p
///   M*p2 == p2p
///   M*p3 == p3p
/// could be substantially optimized by using efficient diagonalization techniques.

DMatrix3 inferLinearTransform(const DPoint3 & p1, const DPoint3 & p1p,
                              const DPoint3 & p2, const DPoint3 & p2p,
                              const DPoint3 & p3, const DPoint3 & p3p);

#ifdef _DEBUG
void testInferLinearTransform();
#endif

//-----------------------------------------------------------------------------

/// If M is projectionMatrix * modelViewMatrix, this class is a machine to
/// quickly convert between world coordinates from normalized device coordinates
/// (NDC).  It inverts the forward perspective transformation s=[x/w,y/w,z/w],
/// [x,y,z,w] = M * pWorld.
class PerspectiveTransform {
    double d0,dr,ds,dt, // denominator = d0 + dr*r + ds*s + dt*t
           x0,xr,xs,xt, // x numerator = w*(x0 + xr*r + xs*s + xt*t) with w==1
           y0,yr,ys,yt, // y numerator = w*(y0 + yr*r + ys*s + yt*t) with w==1
           z0,zr,zs,zt; // ditto, but z is over -denominator
    DMatrix4 M_;        // keeps forward transform

  public:
    PerspectiveTransform() {}
    void calculate(const DMatrix4 & M);

    DPoint3 NDCtoWorld(const double & r,
                       const double & s,
                       const double & t) const;
    DPoint3 NDCtoWorld(const DPoint3 & p) const {
        return NDCtoWorld(p.x,p.y,p.z);
    }
    DPoint3 worldToNDC(const DPoint4 & p) const {
        DPoint4 q = M_ * p;
        double d = 1. / q.w;
        return DPoint3(q.x*d, q.y*d, q.z*d);
    }
    DPoint3 worldToNDC(const double & x,
                       const double & y,
                       const double & z)  const { return worldToNDC(DPoint4(x,y,z,1.)); }
    DPoint3 worldToNDC(const DPoint3 & p) const { return worldToNDC(p.x, p.y, p.z); }
};

/// testing
#ifdef _DEBUG
void testPerspectiveInverse();
#endif

#endif // Matrix_h
