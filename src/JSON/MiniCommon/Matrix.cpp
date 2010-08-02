#include "Matrix.h"

#ifndef b_assert
#include <assert.h>
#define b_assert(XX) assert(XX)
#endif // b_assert

static const double invDetEpsilon = 1e-13;
static const double entryEpsilon = 1e-10;

double det(const DMatrix2 & M) {
    return M[0][0]*M[1][1] - M[0][1]*M[1][0];
}

double det2(const DMatrix3 & M) {
    return M[0][0]*M[1][1] - M[0][1]*M[1][0];
}

double det2(const DMatrix4 & M) {
    return M[0][0]*M[1][1] - M[0][1]*M[1][0];
}

double det(const DMatrix3 & M) {
    #define D2(R1,C1,R2,C2) (M[R1][C1]*M[R2][C2] - M[R1][C2]*M[R2][C1])
    return   M[0][0] * D2(1,1,2,2)
           - M[0][1] * D2(1,0,2,2)
           + M[0][2] * D2(1,0,2,1);
    #undef D2
}


double det3(const DMatrix4 & M) {
    #define D2(R1,C1,R2,C2) (M[R1][C1]*M[R2][C2] - M[R1][C2]*M[R2][C1])
    return   M[0][0] * D2(1,1,2,2)
           - M[0][1] * D2(1,0,2,2)
           + M[0][2] * D2(1,0,2,1);
    #undef D2
}

double det(const DMatrix4 & M) {
    #define D2(R1,C1,R2,C2) (M[R1][C1]*M[R2][C2] - M[R1][C2]*M[R2][C1])
    #define D3(R1,C1,R2,C2,R3,C3) (M[R1][C1] * D2(R2,C2,R3,C3) - \
                                   M[R1][C2] * D2(R2,C1,R3,C3) +  \
                                   M[R1][C3] * D2(R2,C1,R3,C2))
    return   M[0][0] * D3(1,1,2,2,3,3)
           - M[0][1] * D3(1,0,2,2,3,3)
           + M[0][2] * D3(1,0,2,1,3,3)
           - M[0][3] * D3(1,0,2,1,3,2);
    #undef D3
    #undef D2
}

inline DMatrix3 invH(const DMatrix3 & M) {
    b_assert( //"3x3 matrix inhomogeneous",
        fabs(M[2][0]     ) < entryEpsilon &&
        fabs(M[2][1]     ) < entryEpsilon &&
        fabs(M[2][2] - 1.) < entryEpsilon);

    DMatrix3 Mi;
    const double d  = 1. / (M[0][0]*M[1][1] - M[0][1]*M[1][0]);
    Mi[0][0] =  M[1][1] * d;
    Mi[0][1] = -M[0][1] * d;
    Mi[0][2] = (M[0][1] * M[1][2] - M[0][2] * M[1][1]) * d;
    Mi[1][0] = -M[1][0] * d;
    Mi[1][1] =  M[0][0] * d;
    Mi[1][2] = (M[0][2] * M[1][0] - M[0][0] * M[1][2]) * d;
    return Mi;
}

DMatrix2 inv(const DMatrix2 & M) {
    const double &a=M[0][0], &b=M[0][1],
                 &e=M[1][0], &f=M[1][1];

    double de = a*f - b*e;
    b_assert(fabs(de) > invDetEpsilon);
    de = 1./de;

    DMatrix2 Mi;
    Mi[0][0] = de *  f;
    Mi[0][1] = de * -b;
    Mi[1][0] = de * -e;
    Mi[1][1] = de *  a;
    return Mi;
}

DMatrix3 inv(const DMatrix3 & M) {
    const double &a=M[0][0], &b=M[0][1], &c=M[0][2],
                 &e=M[1][0], &f=M[1][1], &g=M[1][2],
                 &i=M[2][0], &j=M[2][1], &k=M[2][2];

    double de = c*e*j - a*g*j - b*e*k + a*f*k - c*f*i + b*g*i;
    b_assert(fabs(de) > invDetEpsilon);
    de = 1./de;

    DMatrix3 Mi;
    Mi[0][0] = de * (-g*j + f*k);
    Mi[0][1] = de * ( c*j - b*k);
    Mi[0][2] = de * (-c*f + b*g);
    Mi[1][0] = de * (-e*k + g*i);
    Mi[1][1] = de * ( a*k - c*i);
    Mi[1][2] = de * ( c*e - a*g);
    Mi[2][0] = de * ( e*j - f*i);
    Mi[2][1] = de * (-a*j + b*i);
    Mi[2][2] = de * (-b*e + a*f);
    return Mi;
}

DMatrix4 inv(const DMatrix4 & M) {
#if 1
    // brute force algorithm (validation)
    const double &a=M[0][0], &b=M[0][1], &c=M[0][2], &d=M[0][3],
                 &e=M[1][0], &f=M[1][1], &g=M[1][2], &h=M[1][3],
                 &i=M[2][0], &j=M[2][1], &k=M[2][2], &l=M[2][3],
                 &m=M[3][0], &n=M[3][1], &o=M[3][2], &p=M[3][3];

    double de =
        d*g*j*m - c*h*j*m - d*f*k*m + b*h*k*m + c*f*l*m - b*g*l*m + d*e*k*n - 
        a*h*k*n - c*e*l*n + a*g*l*n - d*e*j*o + a*h*j*o + b*e*l*o - a*f*l*o + 
        c*e*j*p - a*g*j*p - b*e*k*p + a*f*k*p - d*g*n*i + c*h*n*i + d*f*o*i - 
        b*h*o*i - c*f*p*i + b*g*p*i;
    b_assert(fabs(de) > invDetEpsilon);
    de = 1./de;

    DMatrix4 Mi;
    Mi[0][0] = de * (-h*k*n + g*l*n + h*j*o - f*l*o - g*j*p + f*k*p);
    Mi[0][1] = de * ( d*k*n - c*l*n - d*j*o + b*l*o + c*j*p - b*k*p);
    Mi[0][2] = de * (-d*g*n + c*h*n + d*f*o - b*h*o - c*f*p + b*g*p);
    Mi[0][3] = de * ( d*g*j - c*h*j - d*f*k + b*h*k + c*f*l - b*g*l);
    Mi[1][0] = de * ( h*k*m - g*l*m + e*l*o - e*k*p - h*o*i + g*p*i);
    Mi[1][1] = de * (-d*k*m + c*l*m - a*l*o + a*k*p + d*o*i - c*p*i);
    Mi[1][2] = de * ( d*g*m - c*h*m - d*e*o + a*h*o + c*e*p - a*g*p);
    Mi[1][3] = de * ( d*e*k - a*h*k - c*e*l + a*g*l - d*g*i + c*h*i);
    Mi[2][0] = de * (-h*j*m + f*l*m - e*l*n + e*j*p + h*n*i - f*p*i);
    Mi[2][1] = de * ( d*j*m - b*l*m + a*l*n - a*j*p - d*n*i + b*p*i);
    Mi[2][2] = de * (-d*f*m + b*h*m + d*e*n - a*h*n - b*e*p + a*f*p);
    Mi[2][3] = de * (-d*e*j + a*h*j + b*e*l - a*f*l + d*f*i - b*h*i);
    Mi[3][0] = de * ( g*j*m - f*k*m + e*k*n - e*j*o - g*n*i + f*o*i);
    Mi[3][1] = de * (-c*j*m + b*k*m - a*k*n + a*j*o + c*n*i - b*o*i);
    Mi[3][2] = de * ( c*f*m - b*g*m - c*e*n + a*g*n + b*e*o - a*f*o);
    Mi[3][3] = de * ( c*e*j - a*g*j - b*e*k + a*f*k - c*f*i + b*g*i);
#else
    // Strassen algorithm-- 60 multiplies, 2 divides instead of 280 multiplies, 1 divide!
    // see http://en.wikipedia.org/wiki/Matrix_inversion
    const double &Aa=M[0][0], &Ab=M[0][1], &Ba=M[0][2], &Bb=M[0][3], // M = {{A,B},{C,D}} 2x2 block matrices
                 &Ac=M[1][0], &Ad=M[1][1], &Bc=M[1][2], &Bd=M[1][3],
                 &Ca=M[2][0], &Cb=M[2][1], &Da=M[2][2], &Db=M[2][3],
                 &Cc=M[3][0], &Cd=M[3][1], &Dc=M[3][2], &Dd=M[3][3];
    double Fa,Fb,Fc,Fd;                                          // F = A^(-1)
    {
        double detF = Aa*Ad-Ab*Ac;
        b_assert(fabs(detF) > invDetEpsilon);
        detF = 1./detF;
        Fa= Ad*detF; Fb=-Ab*detF;
        Fc=-Ac*detF; Fd= Aa*detF;
    }
    const double Ga=-(Ca*Fa+Cb*Fc), Gb=-(Ca*Fb+Cb*Fd),           // G = -CF
                 Gc=-(Cc*Fa+Cd*Fc), Gd=-(Cc*Fb+Cd*Fd);
    DMatrix4 Mi;
    double &Ea=Mi[2][2], &Eb=Mi[2][3],
           &Ec=Mi[3][2], &Ed=Mi[3][3];
    {
        const double Ja=Da+Ga*Ba+Gb*Bc, Jb=Db+Ga*Bb+Gb*Bd,       // J = D+GB
                     Jc=Dc+Gc*Ba+Gd*Bc, Jd=Dd+Gc*Bb+Gd*Bd;
        double detJ = Ja*Jd-Jb*Jc;
        b_assert(fabs(detJ) > invDetEpsilon);
        detJ = 1./detJ;
        Ea= Jd*detJ; Eb=-Jb*detJ;                                // E = J^(-1)
        Ec=-Jc*detJ; Ed= Ja*detJ;
    }
    double &Ha=Mi[0][2], &Hb=Mi[0][3],
           &Hc=Mi[1][2], &Hd=Mi[1][3];
    {
        const double Ka=-(Fa*Ba+Fb*Bc), Kb=-(Fa*Bb+Fb*Bd),       // K = -FB
                     Kc=-(Fc*Ba+Fd*Bc), Kd=-(Fc*Bb+Fd*Bd);
        Ha=Ka*Ea+Kb*Ec; Hb=Ka*Eb+Kb*Ed;                          // H = KE
        Hc=Kc*Ea+Kd*Ec; Hd=Kc*Eb+Kd*Ed;
    }
    double &La=Mi[0][0], &Lb=Mi[0][1],
           &Lc=Mi[1][0], &Ld=Mi[1][1];
    La=Fa+Ha*Ga+Hb*Gc; Lb=Fb+Ha*Gb+Hb*Gd,                        // L = F+HG
    Lc=Fc+Hc*Ga+Hd*Gc; Ld=Fd+Hc*Gb+Hd*Gd;
    double &Pa=Mi[2][0], &Pb=Mi[2][1],
           &Pc=Mi[3][0], &Pd=Mi[3][1];
    Pa=Ea*Ga+Eb*Gc; Pb=Ea*Gb+Eb*Gd;                              // P = EG
    Pc=Ec*Ga+Ed*Gc; Pd=Ec*Gb+Ed*Gd;
#endif
    return Mi;
}

DMatrix4 invH(const DMatrix4 & M) {
    b_assert( //"4x4 matrix inhomogeneous",
        fabs(M[3][0]     ) < entryEpsilon &&
        fabs(M[3][1]     ) < entryEpsilon &&
        fabs(M[3][2]     ) < entryEpsilon &&
        fabs(M[3][3] - 1.) < entryEpsilon);

    DMatrix4 Mi;
    #define D2(R1,C1,R2,C2) (M[R1][C1]*M[R2][C2] - M[R1][C2]*M[R2][C1])
    #define D3(R1,C1,R2,C2,R3,C3) (M[R1][C1] * D2(R2,C2,R3,C3) - \
                                   M[R1][C2] * D2(R2,C1,R3,C3) +  \
                                   M[R1][C3] * D2(R2,C1,R3,C2))
    const double d = 1. / D3(0,0,1,1,2,2);
    Mi[0][0] =  D2(1,1,2,2) * d; // _12
    Mi[1][0] = -D2(1,0,2,2) * d; // 0_2
    Mi[2][0] =  D2(1,0,2,1) * d; // 01_
    Mi[0][1] = -D2(0,1,2,2) * d;
    Mi[1][1] =  D2(0,0,2,2) * d;
    Mi[2][1] = -D2(0,0,2,1) * d;
    Mi[0][2] =  D2(0,1,1,2) * d;
    Mi[1][2] = -D2(0,0,1,2) * d;
    Mi[2][2] =  D2(0,0,1,1) * d;
    Mi[0][3] = -D3(0,1,1,2,2,3) * d; // _123
    Mi[1][3] =  D3(0,0,1,2,2,3) * d; // 0_23
    Mi[2][3] = -D3(0,0,1,1,2,3) * d; // 01_3
    Mi[3][3] =  D3(0,0,1,1,2,2) * d; // 012_
    #undef D3
    #undef D2
    return Mi;
}

DMatrix4 invIH(const DMatrix4 & M) {
    b_assert( //"4x4 matrix not homogeneous with 3x3 diagonal",
        fabs(M[3][0]     ) < entryEpsilon &&
        fabs(M[3][1]     ) < entryEpsilon &&
        fabs(M[3][2]     ) < entryEpsilon &&
        fabs(M[3][3] - 1.) < entryEpsilon &&
        fabs(M[0][1]     ) < entryEpsilon &&
        fabs(M[1][0]     ) < entryEpsilon &&
        fabs(M[0][2]     ) < entryEpsilon &&
        fabs(M[2][0]     ) < entryEpsilon &&
        fabs(M[1][2]     ) < entryEpsilon &&
        fabs(M[2][1]     ) < entryEpsilon);

    DMatrix4 iM;
    const double
        a = 1./M[0][0],
        b = 1./M[1][1],
        c = 1./M[2][2];
    iM[0][0] = a;
    iM[1][1] = b;
    iM[2][2] = c;
    iM[3][3] = 1./M[3][3];
    iM[0][3] = -a*M[0][3];
    iM[1][3] = -b*M[1][3];
    iM[2][3] = -c*M[2][3];
    return iM;
}

DMatrix3 inferLinearTransform(const DPoint3 & p1, const DPoint3 & p1p,
                              const DPoint3 & p2, const DPoint3 & p2p,
                              const DPoint3 & p3, const DPoint3 & p3p) {
    const double &x1 = p1 .x, &y1 = p1 .y, &z1 = p1 .z,
                 &x2 = p2 .x, &y2 = p2 .y, &z2 = p2 .z,
                 &x3 = p3 .x, &y3 = p3 .y, &z3 = p3 .z,
                 &r1 = p1p.x, &s1 = p1p.y, &t1 = p1p.z,
                 &r2 = p2p.x, &s2 = p2p.y, &t2 = p2p.z,
                 &r3 = p3p.x, &s3 = p3p.y, &t3 = p3p.z;

    const double de = 1. / (x3*y2*z1 - x2*y3*z1 - x3*y1*z2 + x1*y3*z2 + x2*y1*z3 - x1*y2*z3),
                 dm = -de;

    DMatrix3 M;
    M[0][0] = (r3*y2*z1 - r2*y3*z1 - r3*y1*z2 + r1*y3*z2 + r2*y1*z3 - r1*y2*z3)*de;
    M[0][1] = (r3*x2*z1 - r2*x3*z1 - r3*x1*z2 + r1*x3*z2 + r2*x1*z3 - r1*x2*z3)*dm;
    M[0][2] = (r3*x2*y1 - r2*x3*y1 - r3*x1*y2 + r1*x3*y2 + r2*x1*y3 - r1*x2*y3)*de;
    M[1][0] = (s3*y2*z1 - s2*y3*z1 - s3*y1*z2 + s1*y3*z2 + s2*y1*z3 - s1*y2*z3)*de;
    M[1][1] = (s3*x2*z1 - s2*x3*z1 - s3*x1*z2 + s1*x3*z2 + s2*x1*z3 - s1*x2*z3)*dm;
    M[1][2] = (s3*x2*y1 - s2*x3*y1 - s3*x1*y2 + s1*x3*y2 + s2*x1*y3 - s1*x2*y3)*de;
    M[2][0] = (t3*y2*z1 - t2*y3*z1 - t3*y1*z2 + t1*y3*z2 + t2*y1*z3 - t1*y2*z3)*de;
    M[2][1] = (t3*x2*z1 - t2*x3*z1 - t3*x1*z2 + t1*x3*z2 + t2*x1*z3 - t1*x2*z3)*dm;
    M[2][2] = (t3*x2*y1 - t2*x3*y1 - t3*x1*y2 + t1*x3*y2 + t2*x1*y3 - t1*x2*y3)*de;

    return M;
}

void PerspectiveTransform::calculate(const DMatrix4 & M) {
    M_ = M;
    const double &a=M[0][0], &b=M[0][1], &c=M[0][2], &d=M[0][3],
                 &e=M[1][0], &f=M[1][1], &g=M[1][2], &h=M[1][3],
                 &i=M[2][0], &j=M[2][1], &k=M[2][2], &l=M[2][3],
                 &m=M[3][0], &n=M[3][1], &p=M[3][2], &q=M[3][3];

    d0 = - c*f*i + b*g*i + c*e*j - a*g*j - b*e*k + a*f*k;
    dr =   g*j*m - f*k*m - g*i*n + e*k*n + f*i*p - e*j*p;
    ds = - c*j*m + b*k*m + c*i*n - a*k*n - b*i*p + a*j*p;
    dt =   c*f*m - b*g*m - c*e*n + a*g*n + b*e*p - a*f*p;

    x0 =   d*g*j - c*h*j - d*f*k + b*h*k + c*f*l - b*g*l;
    xr = - h*k*n + g*l*n + h*j*p - f*l*p - g*j*q + f*k*q;
    xs =   d*k*n - c*l*n - d*j*p + b*l*p + c*j*q - b*k*q;
    xt = - d*g*n + c*h*n + d*f*p - b*h*p - c*f*q + b*g*q;

    y0 = - d*g*i + c*h*i + d*e*k - a*h*k - c*e*l + a*g*l;
    yr =   h*k*m - g*l*m - h*i*p + e*l*p + g*i*q - e*k*q;
    ys = - d*k*m + c*l*m + d*i*p - a*l*p - c*i*q + a*k*q;
    yt =   d*g*m - c*h*m - d*e*p + a*h*p + c*e*q - a*g*q;

    z0 = - d*f*i + b*h*i + d*e*j - a*h*j - b*e*l + a*f*l;
    zr =   h*j*m - f*l*m - h*i*n + e*l*n + f*i*q - e*j*q;
    zs = - d*j*m + b*l*m + d*i*n - a*l*n - b*i*q + a*j*q;
    zt =   d*f*m - b*h*m - d*e*n + a*h*n + b*e*q - a*f*q;
}

DPoint3 PerspectiveTransform::NDCtoWorld(const double & r,
                                         const double & s,
                                         const double & t) const {
    const double de = 1. / (d0 + dr*r + ds*s + dt*t);
    return DPoint3(de*(x0 + xr*r + xs*s + xt*t),
                   de*(y0 + yr*r + ys*s + yt*t),
                  -de*(z0 + zr*r + zs*s + zt*t));
}
