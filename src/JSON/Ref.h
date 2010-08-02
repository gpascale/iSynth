#ifndef Ref_h
#define Ref_h

/// Define these atomically for some measure of thread-safety.

#define AtomicRefIncrement(XX) (++(XX))
#define AtomicRefDecrement(XX) (--(XX))

class RefpBase;

/// Base class for reference-counted objects. Derived classes may be
/// addressed using Refp and LivePtr.
class RefpObj {
  public:
    /// refCount is public because of problems making template
    /// friend classes; not normally for public consumption!
    volatile mutable long refCount;

    RefpObj() : refCount(0) {}
    RefpObj(const RefpObj &) : refCount(0) {}
    RefpObj & operator = (const RefpObj &) { return *this; }
    virtual ~RefpObj() {}
};

class RefpBase {
  protected:
    RefpObj * ptr_;

    explicit RefpBase() : ptr_(0) {}
    explicit RefpBase(RefpObj * ptr) : ptr_(ptr) {}
    RefpBase(const RefpBase & rhs) : ptr_(rhs.ptr_) {}

  public:
    RefpObj *    get() const { return ptr_; }
    bool        test() const { return ptr_ != 0; }
    bool operator  !() const { return ptr_ == 0; }

    long    getCount() const { return ptr_ ? ptr_->refCount : -1; } // HACK ALERT!

  protected:
    void assign(const RefpBase & rhs) {
        if (ptr_ != rhs.get()) {
            release();
            ptr_ = rhs.get();
            addRef();
        }
    }

    void assign(RefpObj * ptr) {
        if (ptr_ != ptr) {
            release();
            ptr_ = ptr;
            addRef();
        }
    }

    void addRef()  {
        if (ptr_) {
            addRefPtr(ptr_);
        }
    }

    void release() {
        if (ptr_) {
            releasePtr(ptr_);
        }
    }

    static inline void releasePtr(RefpObj *& t) {
        if (AtomicRefDecrement(t->refCount) == 0) {
            RefpObj * t1 = t;
            t = 0;
            delete t1;
        }
    }

    static inline void addRefPtr(RefpObj * t) {
        AtomicRefIncrement(t->refCount);
    }
};

/// Reference-counted pointer for addressing a RefpObj.
template <class T>
class Refp : public RefpBase {
  public:
    Refp() : RefpBase() {}
    Refp(T * t) : RefpBase(t) { addRef(); }
    Refp(const Refp<T> & rhs) : RefpBase(rhs.get()) { addRef(); }
    ~Refp() { release(); }

    /// When one Refp is assigned to another, the lhs
    /// releases its hold on its RefpObj and takes hold of
    /// the RefpObj addressed by the rhs.
    Refp<T> & operator = (const Refp<T> & rhs) {
        assign(rhs);
        return *this;
    }

    void reset(T * t = 0) { assign(t); }

    operator T *    () const { return  (T *)ptr_; }
    T & operator   *() const { return *(T *)ptr_; }
    T * operator  ->() const { return  (T *)ptr_; }
    T *          get() const { return  (T *)ptr_; }

    bool operator == (const Refp<T> & rhs) const { return ptr_ == rhs.get(); }
    bool operator != (const Refp<T> & rhs) const { return ptr_ != rhs.get(); }
    bool operator <  (const Refp<T> & rhs) const { return ptr_ <  rhs.get(); }
};

#endif // Ref_h
