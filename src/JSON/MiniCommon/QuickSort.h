#ifndef Quicksort_h
#define Quicksort_h

#define TT template<class T>
#define TTi template<class T> inline
#define TTU template<class T, class U>
#define TTUV template<class T, class U, class V>

// This version uses cmp.Greater(a, b).

TTU void quicksortEx(T * t, int n, U & cmp);

// These versions uses the > operator and Swap()--
//  so don't sort large objects!

TT void quicksort(T * t, int n);

// Sort an array in parallel with another array.

TTU void quicksort2(T * f, U * l, int n);
TTUV void quicksort4(T * f, U * l, V * r, int n);

#undef TTUV
#undef TTU
#undef TTi
#undef TT



#include "QuickSort.hpp"

#endif // Quicksort_h
