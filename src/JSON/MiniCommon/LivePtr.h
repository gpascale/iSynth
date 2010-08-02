#ifndef LivePtr_h
#define LivePtr_h

/// @file
/// Live pointers have two-way communication between pointer and object pointed
/// to.  If the object is deleted, then all live pointers pointing to it are
/// set to zero.  Hence live pointers, unlike RefPtrs, do not control lifetime.
/// To accomplish this functionality, each LiveObj has a list of live pointers that
/// reference it.
///
/// It is often useful for a complex object to be both a LiveObj and a RefpObj;
/// then the RefPtr(s) to it control its lifetime.  When the last RefPtr
/// pointing to it is zeroed or destroyed, all dangling LivePtrs are zeroed too.

class LiveObjPtr;

/// Base class for objects that are addressed by live pointers.
class LiveObj {
    LiveObjPtr * head;
    friend class LiveObjPtr;
  public:
    LiveObj() : head(0) {}
    virtual ~LiveObj();
};

/// A live pointer for addressing a LiveObj. This class is responsible for maintaining
/// its inclusion in the list of live pointers associated with the LiveObj that it addresses.
/// In order to facilitate the two-way communication with LiveObj and prevent code bloat 
/// from the template expansion of a large number of types of live pointers, LiveObjPtr 
/// is not templatized. Instead, its derived class LivePtr provides template functionality.
class LiveObjPtr {
  protected:
    LiveObj * ptr;
    LiveObjPtr * prev;
    LiveObjPtr * next;
    friend class LiveObj;

    void addLiveRef() {
        if (ptr) {
            prev = 0;
            next = ptr->head;
            ptr->head = this;
            if (next)
                next->prev = this;
        }
    }
    void releaseLiveRef() {
        if (ptr) {
            if (prev)
                prev->next = next;
            else
                ptr->head = next;
            if (next)
                next->prev = prev;
        }
    }

  public:
    LiveObjPtr() : ptr(0) {}
    LiveObjPtr(LiveObj * p) : ptr(p) { addLiveRef(); }
    LiveObjPtr(const LiveObjPtr & rhs) : ptr(rhs.ptr) { addLiveRef(); }
    ~LiveObjPtr() { releaseLiveRef(); }

    /// If one live pointer is assigned to another, the lhs releases
    /// the reference to its target LiveObj, then adds a reference
    /// to the LiveObj addressed by the rhs.
    LiveObjPtr & operator = (const LiveObjPtr & rhs) {
        if (ptr != rhs.ptr) {
            releaseLiveRef();
            ptr = rhs.ptr;
            addLiveRef();
        }
        return *this;
    }
    operator LiveObj *    () const { return  ptr; }
    LiveObj & operator   *() const { return *ptr; }
    LiveObj * operator  ->() const { return  ptr; }
    LiveObj *          get() const { return  ptr; }
    operator bool         () const { return  ptr != 0; }
    bool              test() const { return  ptr != 0; }
    bool operator        !() const { return  ptr == 0; }
    bool operator == (const LiveObjPtr & rhs) const { return ptr == rhs.ptr; }
    bool operator != (const LiveObjPtr & rhs) const { return ptr != rhs.ptr; }
    bool operator <  (const LiveObjPtr & rhs) const { return ptr <  rhs.ptr; }
};

/// A templatized version of LiveObjPtr that casts the target LiveObj to the
/// template class. See LiveObj for an explanation of the motivation behind this
/// approach.
template <class T>
class LivePtr
: public LiveObjPtr {
  public:
    LivePtr() : LiveObjPtr() {}
    LivePtr(T * rhs) : LiveObjPtr(rhs) {}
    LivePtr(const LivePtr<T> & rhs) : LiveObjPtr(rhs) {}
    LivePtr<T> & operator = (const LivePtr<T> & rhs) {
        *((LiveObjPtr *)this) = *((const LiveObjPtr *)(&rhs));
        return *this;
    }
    operator T *        () const { return   (T *) ptr ; }
    T & operator       *() const { return *((T *) ptr); }
    T * operator      ->() const { return   (T *) ptr ; }
    T * get() const { return   (T *) ptr ; }
    bool operator == (const LivePtr<T> & rhs) const { return ptr == rhs.ptr; }
    bool operator != (const LivePtr<T> & rhs) const { return ptr != rhs.ptr; }
    bool operator <  (const LivePtr<T> & rhs) const { return ptr <  rhs.ptr; }
};

#endif // LivePtr_h
