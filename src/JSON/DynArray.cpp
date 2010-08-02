//#include "Stable.h"
#include "SynthNavToolkit.h"
#include "DynArray.h"

#ifdef SYNTH_NAV_TOOLKIT_DEBUGGING
#define DYNARRAY_TEST_N 100

static int nAs = 0;

class TestA {
  public:
    int x_;
    TestA() : x_(5) { ++nAs; }
    TestA(const TestA & rhs) : x_(rhs.x_) {}
    TestA & operator = (const TestA & rhs) { x_=rhs.x_; return *this; }
    ~TestA() { b_assert(x_==5); --nAs; }
};

static int nBs = 0;

class TestB {
  public:
    DynArray<TestA> As_;
    TestB() : As_(DYNARRAY_TEST_N, true) { ++nBs; }
    ~TestB() { --nBs; }
};

static int nCs = 0;

class TestC {
  public:
    DynArray1<TestB> Bs_;
    TestC() : Bs_(DYNARRAY_TEST_N, true) { ++nCs; }
    ~TestC() { --nCs; }
};

void TestDynArray() {
    b_assert(!nAs && !nBs && !nCs);
    {
        TestC C;
        b_assert(nCs==1);
        b_assert(nBs==DYNARRAY_TEST_N);
        b_assert(nAs==DYNARRAY_TEST_N*DYNARRAY_TEST_N);

        const int n2 = DYNARRAY_TEST_N * 20;
        for (int i=0; i<n2; ++i) {
            C.Bs_.append(TestB());
            b_printf(".");
        }
    }
    b_printf("\nnAs=%d nBs=%d nCs=%d\n", nAs, nBs, nCs);
    b_assert(!nAs && !nBs && !nCs);
}

#endif // SYNTH_NAV_TOOLKIT_DEBUGGING
