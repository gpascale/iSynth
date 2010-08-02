#include <time.h>
#include <math.h>
#include "Random.h"

long & RandomNumberStream_() {
    static long randomNumberStream = 0;
    return randomNumberStream;
}

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

double Ran0(long & i) {
    i ^= MASK;
    long k = i / IQ;
    i = IA * (i-k*IQ) - IR*k;
    if (i<0)
        i += IM;
    double ans = AM * i;
    i ^= MASK;
    return ans;
}

#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS  1.2e-7
#define RNMX (1.-EPS)

double Ran1(long & i) {
    static long iy = 0;
    static long iv[NTAB];

    if (i <= 0 || !iy) {
        i = -i;
        if (i < 1)
            i = 1;
        for (int j = NTAB+7; j >= NTAB; j--) {
            long k = i / IQ;
            i = IA * (i-k*IQ) - IR*k;
            if (i < 0)
                i += IM;
        }
        for (int j = NTAB-1; j >= 0; j--) {
            long k = i / IQ;
            i = IA * (i-k*IQ) - IR*k;
            if (i < 0)
                i += IM;
            iv[j] = i;
        }
        iy = *iv;
    }
    long k = i / IQ;
    i = IA * (i-k*IQ) - IR*k;
    if (i < 0)
        i += IM;
    int j = iy / NDIV;
    iy = iv[j];
    iv[j] = i;
    double ans = AM * iy;
    return ans > RNMX ? RNMX : ans;
}

double Gaussian0(long & i) {
    static bool extraAvailable = false;
    static double extra;
    if (extraAvailable) {
        extraAvailable = false;
        return extra;
    }
    double x, y, r2;
    do {
        x = (Ran0(i) * 2.) - 1.;
        y = (Ran0(i) * 2.) - 1.;
        r2 = x*x + y*y;
    } while (r2 >= 1. || r2 == 0.);
    double f = sqrt(-2.*log(r2)/r2);
    extra = x*f;
    extraAvailable = true;
    return y*f;
}

double Gaussian1(long & i) {
    static bool extraAvailable = false;
    static double extra;
    if (extraAvailable) {
        extraAvailable = false;
        return extra;
    }
    double x, y, r2;
    do {
        x = (Ran1(i) * 2.) - 1.;
        y = (Ran1(i) * 2.) - 1.;
        r2 = x*x + y*y;
    } while (r2 >= 1. || r2 == 0.);
    double f = sqrt(-2.*log(r2)/r2);
    extra = x*f;
    extraAvailable = true;
    return y*f;
}

void GaussianPair0(long & i, double & g1, double & g2) {
    double x, y, r2;
    do {
        x = (Ran0(i) * 2.) - 1.;
        y = (Ran0(i) * 2.) - 1.;
        r2 = x*x + y*y;
    } while (r2 >= 1. || r2 == 0.);
    double f = sqrt(-2.*log(r2)/r2);
    g1 = x*f;
    g2 = y*f;
}

void GaussianPair1(long & i, double & g1, double & g2) {
    double x, y, r2;
    do {
        x = (Ran1(i) * 2.) - 1.;
        y = (Ran1(i) * 2.) - 1.;
        r2 = x*x + y*y;
    } while (r2 >= 1. || r2 == 0.);
    double f = sqrt(-2.*log(r2)/r2);
    g1 = x*f;
    g2 = y*f;
}

int RanBit(unsigned long & i) {
    unsigned long newbit;
    newbit = (i >> 17) & 1
           ^ (i >>  4) & 1
           ^ (i >>  1) & 1
           ^ (i      ) & 1;
    i = (i << 1) | newbit;
    return newbit;
}

unsigned TimeSeed() {
    double cps = CLOCKS_PER_SEC;
    double ms = fmod((cps*time(0) + (unsigned(clock())%1000)), double(0x7FFFFFFF));
    return unsigned(ms); // truncates as necessary
}
