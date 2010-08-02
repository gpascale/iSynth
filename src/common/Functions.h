/*
 *  Functions.h
 *  Phonosynth
 *
 *  Created by Greg on 1/5/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

// My own implementation of Generic function objects. I haven't spent that much time making
// this as robust as possible, but I think it's pretty cool and enables me to work with 
// threads very cleanly and without subclassing as much as you'd normally have to

// The "Function" classes are simply wrappers around function objects. I opted not to 
// create a method called "call" rather than overload the () operator for clarity

// "Functor" objects encapsulate a function as well as an object to call on (in the case
// of MemberFunctors and optionally arguments. Currently Functors only support functions
// which return void. That way they can all inherit a base "call" function and this allows
// for powerful generality. e.g. GThread has a constructor which accepts any type of Functor.
// I plan on writing a separate group of Functors with templated return type but they will
// have to have a different inheritance hierarchy


struct StaticFunction
{
	StaticFunction(void (*fn)() = NULL) : m_fn(fn) { }
	void	call() { (*m_fn)(); }
private:
	void	(*m_fn)();
};


template <class R>
struct StaticFunctionR
{
	StaticFunctionR(R (*fn)() = NULL) : m_fn(fn) { }
	R    call() { return (*m_fn)(); }
private:
	R     (*m_fn)( );	// the function pointer
};



// encapsulates a static function which takes one argument and has no return value
template <class A>
struct StaticFunctionA : public StaticFunction
{
	StaticFunctionA(void (*fn)(A) = NULL) : m_fn(fn) { }
	void	call(A a){ (*m_fn)(a); }
private:	
	void	(*m_fn)(A);
};

	
// encapsulates a static function which takes two arguments and has no return value
template <class A1, class A2>
struct StaticFunctionA2 : public StaticFunction
{
	StaticFunctionA2(void (*fn)(A1, A2) = NULL) : m_fn(fn) { }
	void	call(A1 a1, A2 a2){ (*m_fn)(a1, a2); }
private:	
	void	(*m_fn)(A1, A2);
};

	
// encapsulates a static function which takes three arguments and has no return value	
template <class A1, class A2, class A3>
struct StaticFunctionA3 : public StaticFunction
{
	StaticFunctionA3(void (*fn)(A1, A2, A3) = NULL) : m_fn(fn) { }
	void	call(A1 a1, A2 a2, A3 a3){ (*m_fn)(a1, a2, a3); }
private:	
	void	(*m_fn)(A1, A2, A3);
};

	
// encapsulates a static function which takes four argumenst and has no return value	
template <class A1, class A2, class A3, class A4>
struct StaticFunctionA4 : public StaticFunction
{
	StaticFunctionA4(void (*fn)(A1, A2, A3, A4) = NULL) : m_fn(fn) { }
	void	call(A1 a1, A2 a2, A3 a3, A4 a4){ (*m_fn)(a1, a2, a3, a4); }
private:	
	void	(*m_fn)(A1, A2, A3, A4);
};
	

// encapsulates a static function which takes one argument and returns an R
template <class R, class A>
struct StaticFunctionRA : public StaticFunctionR<R>
{
	StaticFunctionRA(void (*fn)(A) = NULL) : m_fn(fn) { }
	R		call(A a){ (*m_fn)(a); }
private:	
	R		(*m_fn)(A);
};


// encapsulates a static function which takes two arguments and returns an R
template <class R, class A1, class A2>
struct StaticFunctionRA2 : public StaticFunctionR<R>
{
	StaticFunctionRA2(void (*fn)(A1, A2) = NULL) : m_fn(fn) { }
	R		call(A1 a1, A2 a2){ (*m_fn)(a1, a2); }
private:	
	R		(*m_fn)(A1, A2);
};


// encapsulates a static function which takes three arguments and returns an R	
template <class R, class A1, class A2, class A3>
struct StaticFunctionRA3 : public StaticFunctionR<R>
{
	StaticFunctionRA3(void (*fn)(A1, A2, A3) = NULL) : m_fn(fn) { }
	R		call(A1 a1, A2 a2, A3 a3){ (*m_fn)(a1, a2, a3); }
private:	
	R		(*m_fn)(A1, A2, A3);
};


// encapsulates a static function which takes four argumenst and returns an R	
template <class R, class A1, class A2, class A3, class A4>
struct StaticFunctionRA4 : public StaticFunctionR<R>
{
	StaticFunctionRA4(void (*fn)(A1, A2, A3, A4) = NULL) : m_fn(fn) { }
	R		call(A1 a1, A2 a2, A3 a3, A4 a4){ (*m_fn)(a1, a2, a3, a4); }
private:	
	R		(*m_fn)(A1, A2, A3, A4);
};


// wrapper around a C++ member function with no arguments
template <class T>
struct MemberFunction
{
	MemberFunction(void (T::*memberFn)() = NULL) : m_memberFn(memberFn) { }
	void call(T* target) { return (target->*m_memberFn)(); }
private:
	void			(T::*m_memberFn)(); // the function pointer
};


template <class T, class R>
struct MemberFunctionR
{
	MemberFunctionR(R (T::*fn)() = NULL) : m_fn(fn) { }
	R    call(T* target) { return (target->*m_fn)(); }
private:
	R     (T::*m_fn)();	// the function pointer
};


// wrapper around a C++ member function with one argument
template <class T, class A1>
struct MemberFunctionA : public MemberFunction<T>
{
	MemberFunctionA(void (T::*memberFn)(A1) = NULL) : m_memberFn(memberFn) { }
	void call(T* target, A1 a1) { return (target->*m_memberFn)(a1); }	
private:
	void			(T::*m_memberFn)(A1); // the function pointer
};


// wrapper around a C++ member function with two arguments
template <class T, class A1, class A2>
struct MemberFunctionA2 : public MemberFunction<T>
{
	MemberFunctionA2(void (T::*memberFn)(A1, A2) = NULL) : m_memberFn(memberFn) { }
	void call(T* target, A1 a1, A2 a2) { return (target->*m_memberFn)(a1, a2); }	
private:
	void			(T::*m_memberFn)(A1, A2); // the function pointer
};


// wrapper around a C++ member function with arguments
template <class T, class A1, class A2, class A3>
struct MemberFunctionA3 : public MemberFunction<T>
{
	MemberFunctionA3(void (T::*memberFn)(A1, A2, A3) = NULL) : m_memberFn(memberFn) { }
	void call(T* target, A1 a1, A2 a2, A3 a3) { return (target->*m_memberFn)(a1, a2, a3); }	
private:
	void			(T::*m_memberFn)(A1, A2, A3); // the function pointer
};


// wrapper around a C++ member function with arguments
template <class T, class A1, class A2, class A3, class A4>
struct MemberFunctionA4 : public MemberFunction<T>
{
	MemberFunctionA4(void (T::*memberFn)(A1, A2, A3, A4) = NULL) : m_memberFn(memberFn) { }
	void call(T* target, A1 a1, A2 a2, A3 a3, A4 a4) { return (target->*m_memberFn)(a1, a2, a3, a4); }	
private:
	void			(T::*m_memberFn)(A1, A2, A3, A4); // the function pointer
};


// encapsulates a static function which takes one argument and returns an R
template <class T, class R, class A>
struct MemberFunctionRA : public MemberFunctionR<T, R>
{
	MemberFunctionRA(R (T::*fn)(A) = NULL) : m_fn(fn) { }
	R		call(T* target, A a){ return (target->*m_fn)(a); }
private:	
	R		(T::*m_fn)(A);
};



class StaticFunctor;

	

class Functor { 
public:	
	virtual void	 call()			  = 0;
	virtual Functor* copy()     const = 0;
	
	// returns a functor which does nothing. I decided it's cleaner and clearer
	// to use this rather than, say, passing a NULL Functor* to indicate that
	// nothing should be done
	static const StaticFunctor& Identity();
};


template <class A1>
class FunctorA : public Functor { 
public:
	virtual void call(A1) = 0;
};


template <class A1, class A2>
class FunctorA2 : public Functor { 
public:	
	virtual void call(A1, A2) = 0;
};


template <class A1, class A2, class A3>
class FunctorA3 : public Functor {
public:
	virtual void call(A1, A2, A3) = 0;
};


template <class A1, class A2, class A3, class A4>
class FunctorA4 : public Functor { 
public:
	virtual void call(A1, A2, A3, A4) = 0;
};


class StaticFunctor : public Functor
{
public:	
	
	StaticFunctor() { }
	StaticFunctor(StaticFunction fn) : m_fn(fn) { }
	StaticFunctor(const StaticFunctor& rhs) : m_fn(rhs.m_fn) { }
	
	~StaticFunctor() { }
	
	void call()		  { m_fn.call(); }
	
	Functor* copy() const { Functor* ret = new StaticFunctor(*this); return ret; }
	
private:
	
	StaticFunction		m_fn;
	
};


template <class A1>
class StaticFunctorA : public FunctorA<A1>
{
public:	
	
	StaticFunctorA();
	StaticFunctorA(StaticFunctionA<A1> fn) : m_fn(fn) { }
	StaticFunctorA(StaticFunctionA<A1> fn, A1 a1) : m_fn(fn), m_a1(a1) { }
	StaticFunctorA(const StaticFunctorA<A1>& rhs) :
		m_fn(rhs.m_fn), m_a1(rhs.m_a1)
	{ }
	
	~StaticFunctorA() { }
	
	void setFunction(void (*fn)(A1)) { m_fn = fn;       }
	void setArgs(A1 a1)			     { m_a1 = a1;       }
	void call()						 { m_fn.call(m_a1); }
	void call(A1 a1)				 { m_fn.call(a1);	}
	
	Functor* copy() const            { Functor* ret = new StaticFunctorA<A1>(*this); return ret; }
	
private:
	StaticFunctionA<A1>		m_fn;	// the function pointer
	A1						m_a1;	// the arguments to pass to the function

};


template <class A1, class A2>
class StaticFunctorA2 : public FunctorA2<A1, A2>
{
public:
	
	StaticFunctorA2();
	StaticFunctorA2(StaticFunctionA<A1> fn) : m_fn(fn) { }
	StaticFunctorA2(StaticFunctionA2<A1, A2> fn, A1 a1, A2 a2) :
		m_fn(fn), m_a1(a1), m_a2(a2)
	{ }
	StaticFunctorA2(const StaticFunctorA2<A1, A2>& rhs) :
		m_fn(rhs.m_fn), m_a1(rhs.m_a1), m_a2(rhs.m_a2)
	{ }
	
	~StaticFunctorA2() { }
	
	void setFunction(void (*fn)(A1, A2)) { m_fn = fn;             }
	void setArgs(A1 a1, A2 a2)		     { m_a1 = a1; m_a2 = a2;  }
	void call()						     { m_fn.call(m_a1, m_a2); }
	void call(A1 a1, A2 a2)				 { m_fn.call(a1, a2);     }
	
	Functor* copy() const				 { Functor* ret = new StaticFunctorA2<A1, A2>(*this); return ret; }
	
private:
	StaticFunctionA2<A1, A2> m_fn;	// the function pointer
	A1						 m_a1;  // the arguments to pass to the function
	A2						 m_a2;	
	
};


template <class A1, class A2, class A3>
class StaticFunctorA3 : public FunctorA3<A1, A2, A3>
{
public:	
	
	StaticFunctorA3();
	StaticFunctorA3(StaticFunctionA<A1> fn) : m_fn(fn) { }
	StaticFunctorA3(StaticFunctionA3<A1, A2, A3> fn, A1 a1, A2 a2, A3 a3) :
		m_fn(fn),  m_a1(a1), m_a2(a2), m_a3(a3) 
	{ }
	StaticFunctorA3(const StaticFunctorA3<A1, A2, A3>& rhs) :
		m_fn(rhs.m_fn), m_a1(rhs.m_a1), m_a2(rhs.m_a2), m_a3(rhs.m_a3)
	{ }
	
	~StaticFunctorA3() { }
	
	void setFunction(void (*fn)(A1, A2, A3)) { m_fn = fn;					    }
	void setArgs(A1 a1, A2 a2, A3 a3)		 { m_a1 = a1, m_a2 = a2, m_a3 = a3; }
	void call()							     { m_fn.call(m_a1, m_a2, m_a3);		}
	void call(A1 a1, A2 a2, A3 a3)			 { m_fn.call(a1, a2, a3);		    }
	
	Functor* copy() const					 { Functor* ret = new StaticFunctorA3<A1, A2, A3>(*this); return ret; }
	
private:
	
	StaticFunctionA3<A1, A2, A3>	m_fn;	// the function pointer
	A1								m_a1;	// the auments to pass to the function
	A2								m_a2;
	A3								m_a3;
	
};
 


template <class T>
class MemberFunctor : public Functor
{
public:
	
	MemberFunctor() : m_target(NULL), m_fn(NULL)								        { }
	MemberFunctor(T* target, MemberFunction<T> fn) : m_target(target), m_fn(fn)		    { }
	MemberFunctor(const MemberFunctor<T>& rhs) : m_fn(rhs.m_fn), m_target(rhs.m_target) { }
	
	~MemberFunctor() { }
	
	void setTarget(T* target)			   { m_target = target;   }
	void setFunction(MemberFunction<T> fn) { m_fn = fn;           }
	void call()							   { m_fn.call(m_target); }
	
	Functor* copy() const				   { Functor* ret = new MemberFunctor<T>(*this); return ret; }
	
private:
	
	T*				  m_target;
	MemberFunction<T> m_fn;
	
};


template <class T, class A1>
class MemberFunctorA : public FunctorA<A1>
{
public:
	
	MemberFunctorA() : m_target(NULL), m_fn(NULL) { }	
	MemberFunctorA(T* target, MemberFunctionA<T, A1> fn, A1 a1) :
		m_target(target), m_fn(fn),	m_a1(a1)
	{ }
	MemberFunctorA(const MemberFunctor<T>& rhs) : 
		m_target(rhs.m_target), m_fn(rhs.m_fn), m_a1(rhs.m_a1)
	{ }
	
	~MemberFunctorA() { }
	
	void setTarget(T* target)				    { m_target = target;		  }
	void setFunction(MemberFunctionA<T, A1> fn) { m_fn = fn;				  }
	void setArgs(A1 a1)						    { m_a1 = a1;				  }	
	void call()								    { m_fn.call(m_target, m_a1);  }
	void call(A1 a1)							{ m_fn.call(m_target, a1);	  }
	
	Functor* copy() const						{ Functor* ret = new MemberFunctorA<T, A1>(*this); return ret; }
	
private:
	
	T*							m_target;
	MemberFunctionA<T, A1>		m_fn;
	A1							m_a1;
	
};


template <class T, class A1, class A2>
class MemberFunctorA2 : public FunctorA2<A1, A2>
{
public:
	
	MemberFunctorA2() : m_target(NULL), m_fn(NULL) { }
	MemberFunctorA2(T* target, MemberFunctionA2<T, A1, A2> fn, A1 a1, A2 a2) :
		m_target(target), m_fn(fn),	m_a1(a1), m_a2(a2)
	{ }
	MemberFunctorA2(const MemberFunctor<T>& rhs) : 
		m_target(rhs.m_target), m_fn(rhs.m_fn), m_a1(rhs.m_a1), m_a2(rhs.m_a2)
	{ }
	
	~MemberFunctorA2() { }
	
	void setTarget(T* target)				         { m_target = target;				}
	void setFunction(MemberFunctionA2<T, A1, A2> fn) { m_fn = fn;						}
	void setArgs(A1 a1, A2 a2)						 { m_a1 = a1; m_a2 = a2;			}	
	void call()								         { m_fn.call(m_target, m_a1, m_a2); }
	void call(A1 a1, A2 a2)							 { m_fn.call(m_target, a1, a2);	    }
	
	Functor* copy() const { Functor* ret = new MemberFunctorA2<T, A1, A2>(*this); return ret; }
	
private:
	
	T*							m_target;
	MemberFunctionA2<T, A1, A2>	m_fn;
	A1							m_a1;
	A2							m_a2;
	
};


class ObjCFunctor : public Functor
{
public:
	
	ObjCFunctor() : m_target(nil), m_fn(nil), m_arg(nil) { }	
	ObjCFunctor(NSObject* target, SEL fn, id arg) : m_target(target), m_fn(fn), m_arg(arg) { }
	ObjCFunctor(const ObjCFunctor& rhs) : m_target(rhs.m_target), m_fn(rhs.m_fn), m_arg(rhs.m_arg) { }
	~ObjCFunctor() { }
	
			
	void setTarget(NSObject* target) { m_target = target;				  }
	void setFunction(SEL fn)		 { m_fn = fn;						  }
	void setArg(id arg)				 { m_arg = arg;						  }	
	void call()						 { [m_target performSelector: m_fn
													  withObject: m_arg]; }

	Functor* copy() const			 { Functor* ret = new ObjCFunctor(*this); return ret; }
	
private:
	
	NSObject*	m_target;
	id			m_arg;
	SEL			m_fn;
		
};
	
	
// a function which does nothing
inline void __identityFunction() { }


inline
const StaticFunctor&
Functor::Identity()
{
	static const StaticFunctor identityFunctor(&__identityFunction);
	return identityFunctor;
}



class Predicate
{
public:
	virtual bool evaluate()         = 0;
	
	virtual Predicate* copy() const = 0;
};


template <class A1>
class StaticPredicate1 : public Predicate
{
public:
	
	StaticPredicate1()													      { }
	StaticPredicate1(StaticFunctionRA<bool, A1> fn) : m_fn(fn)			      { }
	StaticPredicate1(StaticFunctionRA<bool, A1> fn, A1 a1) : m_fn(fn), m_a1(a1) { }
	StaticPredicate1(const StaticPredicate1& rhs) : m_fn(rhs.m_fn), m_a1(rhs.m_a1)    { }
	
	bool evaluate()		 { return m_fn.call(m_a1); }
	bool evaluate(A1 a1) { return m_fn.call(a1);   }
	
	Predicate* copy() const { Predicate* ret = new StaticPredicate1<A1>(*this); return ret; }
	
private:
	
	StaticFunctionRA<bool, A1> m_fn;
	A1						   m_a1;
	
};


template <class A1, class A2>
class StaticPredicate2 : public Predicate
{
public:
	
	StaticPredicate2()																				{ }
	StaticPredicate2(StaticFunctionRA2<bool, A1, A2> fn) : m_fn(fn)								    { }
	StaticPredicate2(StaticFunctionRA2<bool, A1, A2> fn, A1 a1, A2 a2) : m_fn(fn), m_a1(a1), m_a2(a2) { }
	StaticPredicate2(const StaticPredicate2& rhs) : m_fn(rhs.m_fn), m_a1(rhs.m_a1), m_a2(rhs.m_a2)			{ }
	
	bool evaluate()				{ return m_fn.call(m_a1, m_a2); }
	bool evaluate(A1 a1, A2 a2) { return m_fn.call(a1, a2);     }
	
	Predicate* copy() const { Predicate* ret = new StaticPredicate2<A1, A2>(*this); return ret; }
	
private:
	
	StaticFunctionRA2<bool, A1, A2> m_fn;
	A1								m_a1;
	A2								m_a2;

};



template <class T, class A1>
class MemberPredicate1 : public Predicate
{
public:
	
	MemberPredicate1()													      { }
	MemberPredicate1(T* target, MemberFunctionRA<T, bool, A1> fn) : m_target(target), m_fn(fn)			      { }
	MemberPredicate1(T* target, MemberFunctionRA<T, bool, A1> fn, A1 a1) : m_target(target), m_fn(fn), m_a1(a1) { }
	MemberPredicate1(const MemberPredicate1& rhs) : m_target(rhs.m_target), m_fn(rhs.m_fn), m_a1(rhs.m_a1)    { }
	
	bool evaluate()		 { return m_fn.call(m_target, m_a1); }
	bool evaluate(A1 a1) { return m_fn.call(m_target, a1);   }
	
	Predicate* copy() const { Predicate* ret = new MemberPredicate1<T, A1>(*this); return ret; }
	
private:
	
	T*						   m_target;
	MemberFunctionRA<T, bool, A1> m_fn;
	A1						   m_a1;
	
};


/* TODO
template <class T, class A1, class A2>
class MemberPredicate2 : public Predicate
{
public:
	
	MemberPredicate2()																				{ }
	MemberPredicate2(T* target, MemberFunctionRA2<T, bool, A1, A2> fn) : m_target(target),  m_fn(fn)								    { }
	MemberPredicate2(T* target, MemberFunctionRA2<T, bool, A1, A2> fn, A1 a1, A2 a2) : m_target, m_fn(fn), m_a1(a1), m_a2(a2) { }
	MemberPredicate2(const MemberPredicate2& rhs) : m_target(rhs.m_target), m_fn(rhs.m_fn), m_a1(rhs.m_a1), m_a2(rhs.m_a2)			{ }
	
	bool evaluate()				{ return m_fn.call(m_target, m_a1, m_a2); }
	bool evaluate(A1 a1, A2 a2) { return m_fn.call(m_target, a1, a2);     }
	
	Predicate* copy() const { Predicate* ret = new MemberPredicate2<T, A1, A2>(*this); return ret; }
	
private:
	
	T*									m_target;
	MemberFunctionRA2<T, bool, A1, A2>  m_fn;
	A1									m_a1;
	A2									m_a2;
	
};
 
 */

#endif // _FUNCTIONS_H_

