#ifndef Random_h
#define Random_h

// VRandom number generators.

long & RandomNumberStream_();

/// Basic generators adapted from Press et al., Numerical Recipes in C.
/// These are designed to output numbers in a flat distribution on the open
/// interval (0,1).  The argument is the internal random number stream, which
/// is modified on sequential calls.

double Ran0(long & i); /// fast
double Ran1(long & i); /// not as fast, but more correlation-m_maxConcurrentConnection

/// Random bit (0 or 1)
int RanBit(unsigned long & i);

double Gaussian0(long & i); /// Gaussian deviates with variance 1; uses Ran0()
void GaussianPair0(long & i, double & g1, double & g2); /// two Gaussians as above
inline void GaussianPair0(long & i, float & g1, float & g2) {
    double x,y;
    GaussianPair0(i,x,y);
    g1 = float(x);
    g2 = float(y);
}

double Gaussian1(long & i); /// Gaussian deviates with variance 1; uses Ran1()
void GaussianPair1(long & i, double & g1, double & g2); /// two Gaussians as above
inline void GaussianPair1(long & i, float & g1, float & g2) {
    double x,y;
    GaussianPair1(i,x,y);
    g1 = float(x);
    g2 = float(y);
}

/// The simple single-threaded versions use a global number stream variable;
/// they are very easy to use, but they are not thread-safe!
/// Also, because they all use the same stream, note that intermixing calls
/// to different generators is not recommended.

inline double Ran0()      { return      Ran0(RandomNumberStream_()); }
inline double Ran1()      { return      Ran1(RandomNumberStream_()); }
inline double Gaussian0() { return Gaussian1(RandomNumberStream_()); }
inline double Gaussian1() { return Gaussian1(RandomNumberStream_()); }

inline void   GaussianPair0(double & g1, double & g2) { GaussianPair0(RandomNumberStream_(), g1,g2); }
inline void   GaussianPair0( float & g1,  float & g2) { GaussianPair0(RandomNumberStream_(), g1,g2); }
inline void   GaussianPair1(double & g1, double & g2) { GaussianPair1(RandomNumberStream_(), g1,g2); }
inline void   GaussianPair1( float & g1,  float & g2) { GaussianPair1(RandomNumberStream_(), g1,g2); }

template <class T> inline int RanI(T N) { return T(Ran0() * N); }

unsigned TimeSeed();
inline void RanTimeSeed() { RandomNumberStream_() = (long)TimeSeed(); }

namespace VRandom {

    /// The default implementation of TimeSeed uses the number of milliseconds
    /// since 1970 plus the number of milliseconds (modulo one second) that the
    /// current process has been running.  For 32-bit random number streams,
    /// this means a compromise: there is a new stream every millisecond, but
    /// an overall period of about 50 days.  Of course, assuming that fresh
    /// streams are not initialized within a millisecond of each other, it is
    /// highly unlikely that a stream will ever be repeated.  If a number of
    /// streams are to be initialized at once, initialize one from the timer
    /// using an alternative generator; then initialize the streams to be used
    /// with seeds from the alternative generator.  It is best to avoid
    /// multiple streams altogether, if possible.

    // Flat0 and Flat1 wrap the Ran0 and Ran1 functions.

    class Flat0 {
      public:
        unsigned stream;
        Flat0()               : stream(TimeSeed()) {}
        Flat0(unsigned which) : stream(which)      {}
        operator double () { return Ran0((long &)stream); }
    };

    class Flat1 {
      public:
        unsigned stream;
        Flat1()               : stream(TimeSeed()) {}
        Flat1(unsigned which) : stream(which)      {}
        operator double () { return Ran1((long &)stream); }
    };

    class Gauss0 {
      public:
        unsigned stream;
        Gauss0()               : stream(TimeSeed()) {}
        Gauss0(unsigned which) : stream(which)      {}
        operator double () { return Gaussian0((long &)stream); }
    };

    class Gauss1 {
      public:
        unsigned stream;
        Gauss1()               : stream(TimeSeed()) {}
        Gauss1(unsigned which) : stream(which)      {}
        operator double () { return Gaussian1((long &)stream); }
    };
};

#endif // Random_h
