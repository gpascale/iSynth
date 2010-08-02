#ifndef DynArray_h
#define DynArray_h

#include <new>
#include <memory.h>
#include "SynthNavToolKit.h"

///////////////////////////////////////////////////////////////////////////////
/// This is intended to store simple objects in a dynamic array.  As the array
/// is grown, the memory needs to be moved, which should be done in the virtual
/// deepCopy().
/// Note that, although the constructors for new elements will be called, no
/// destructors will be called.  Constructors won't be called on deepCopy(),
/// which is why deepCopy() needs to be smart about any pointers or other
/// nontrivial data in the faces or vertices.
/// For a generic container with these properties, use std::vector.  DynArray
/// is a lightweight alternative to be used when low-level access to the
/// memory and/or small code size is required.

template <class T> class DynArray {
    size_t n_;
    size_t max_;
    T * p_;

    void checkSize() const { b_assert(!(sizeof(T)&3)); }
    void checkAlign() const { b_assert(!((reinterpret_cast<long long>(p_))&3)); }

  protected:
    /// T-specific code to relocate n copies of T at base address @oldp to @p.
    /// The default version just moves the memory (assumes primitive T.)
    virtual void deepCopy(T * p, T * oldp, size_t n) {
        memcpy(p, oldp, n*sizeof(T));
    }

  public:
    DynArray(size_t initMax = 16,
             bool setSizeToMax = false) {
        if (sizeof(T) >= 4) {
            checkSize();
        }
        n_ = 0;
        max_ = initMax;
        p_ = new T[initMax];
        checkAlign();
        if (setSizeToMax)
            n_ = max_;
    }

    /// Deep copy constructor.
    DynArray(const DynArray<T> & rhs) {
        n_ = rhs.n_;
        max_ = rhs.max_;
        p_ = new T[max_];
        checkAlign();
        deepCopy(p_, rhs.p_, n_);
    }

    DynArray & operator = (const DynArray<T> & rhs) {
        if (rhs.p_ != p_) {
            delete [] p_;
            n_ = rhs.n_;
            max_ = rhs.max_;
            p_ = new T[max_];
            checkAlign();
            deepCopy(p_, rhs.p_, n_);
        }
        return *this;
    }

    const T * begin() const { return p_; }
          T * begin()       { return p_; }

    const T * end() const { return p_ + n_; }
          T * end()       { return p_ + n_; }

    const T & front() const { b_assert(n_>0); return *p_; }
          T & front()       { b_assert(n_>0); return *p_; }

    const T & back() const { b_assert(n_>0); return *(p_ + (n_-1)); }
          T & back()       { b_assert(n_>0); return *(p_ + (n_-1)); }

    size_t size() const { return n_; }

    T & get(size_t i) {
        b_assert(i < n_);
        return p_[i];
    }

    T & operator [] (size_t i) {
        b_assert(i < n_);
        return p_[i];
    }

    const T & get(size_t i) const {
        b_assert(i < n_);
        return p_[i];
    }

    const T & operator [] (size_t i) const {
        b_assert(i < n_);
        return p_[i];
    }

    /// Assigns a constant value to all entries.
    void assign(const T & t) {
        T * it;
        const T * it1 = end();
        for (it = begin(); it!=it1; ++it) {
            *it = t;
        }
    }

    /// Grows the array to a new maximum size of @newMax, if @newMax is
    /// greater than the current @max_.
    void growMaxTo(size_t newMax) {
        if (newMax > max_) { // we need to grow
            T * newp = new T[newMax];
            deepCopy(newp, p_, n_);
            T * oldp = p_;
            p_ = newp;
            max_ = newMax;
            delete [] oldp;
        }
    }

    /// Resizes the array, growing max_ if necessary (in doubling steps).
    void resize(size_t n) {
        size_t oldn = n_;
        if (n <= max_) { // no memory move necessary
            n_ = n;
        } else { // we need to grow
            size_t newMax = max_ + max_;
            if (newMax < n) {
                newMax = n;
            }
            growMaxTo(newMax);
            n_ = n;
        }
    }

    /// Adds a single element and returns a pointer to it.  Note that this
    /// pointer is only guaranteed to be valid until the next non-const method
    /// call.
    T * add() {
        resize(n_+1);
        b_assert(n_>0);
        return p_+(n_-1);
    }

    /// Adds n elements to the end and returns a pointer to the first added
    /// elementit.  Note that this pointer is only guaranteed to be valid until
    /// the next non-const method call.
    T * add(size_t n) {
        b_assert(n>0);
        const size_t n0 = n_;
        resize(n_+n);
        return p_+n0;
    }

    /// Adds a single element by value.  Don't use for heavyweight objects.
    void append(const T & t) { *(add()) = t; }

    /// Pops off the last element, returning a pointer to it and reducing the
    /// size by one.  Note that this pointer is only guaranteed to be valid
    /// until the next non-const method call.
    T * pop() {
        b_assert(n_>0);
        --n_;
        return p_+n_;
    }

    /// Pop (and discard) n elements off the end; returns a pointer to the
    /// first removed element.  Note that this pointer is only guaranteed to
    /// be valid until the next non-const method call.
    T * pop(size_t n) {
        b_assert(n_>=n);
        n_-=n;
        return p_+n_;
    }

    /// Reserve space for at least @n more elements (guarantees that add() @n
    /// more times won't invalidate pointers).
    void reserve(size_t n) {
        if (n_+n > max_) {
            size_t newMax = max_ + max_;
            if (newMax < n_+n) {
                newMax = n_+n;
            }
            growMaxTo(newMax);
        }
    }

    const T * find(const T & t) const {
        const T * it;
        const T * it1 = end();
        for (it = begin(); it!=it1; ++it) {
            if (*it==t) {
                break;
            }
        }
        return it;
    }

    T * find(const T & t) {
        T * it;
        const T * it1 = end();
        for (it = begin(); it!=it1; ++it) {
            if (*it==t) {
                break;
            }
        }
        return it;
    }

    virtual ~DynArray() {
        delete [] p_;
    }
};

/// Version of DynArray that avoids the memcpy-based deepCopy operator.
template <class T> class DynArray1 : public DynArray<T> {
    typedef DynArray<T> B;
  protected:
    virtual void deepCopy(T * p, T * oldp, size_t n) {
        for (size_t i=0; i<n; ++i) {
            *p++ = *oldp++;
        }
    }
  public:
    DynArray1(size_t initMax = 16, bool setSizeToMax = false) : B(initMax, setSizeToMax) {}
    DynArray1(const DynArray1<T> & rhs) : B(rhs.size(), true) {
        DynArray1<T>::deepCopy(DynArray<T>::begin(), rhs.begin(), DynArray<T>::size());
    }
    DynArray1 & operator = (const DynArray1 & rhs) {
        B::operator = (rhs);
        return *this;
    }
};

void TestDynArray();

#endif // DynArray_h
