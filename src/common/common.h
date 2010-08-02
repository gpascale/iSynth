


#ifndef _COMMON_H_
#define _COMMON_H_

#include <assert.h>
#include <iostream>
#include <cmath>
#include <sstream>
#include <string>

//random stupid macros

#ifndef FOR
	#define FOR(a,b,c)          	for(int a = (int)(b); a < (int)(c); a++)
#endif
#ifndef RFOR
	#define RFOR(a,b,c)         	for(int a = (int)(b); a > (int)(c); a--)
#endif

#define SQR(a)					((a)*(a))
#define CUBE(a)             	((a) * (a) * (a))
#define ALL(a)              	(a).begin(), (a).end()
#define SORT(a)             	std::sort(ALL(a))
#define REVERSE(a)          	std::reverse(ALL(a))
#define LL                  	long long         
#define EPS                 	1e-5
#define TWOPI                   6.28318531
#define PI						3.14159265
#define PIOVER2                 1.57079633
#define PIOVER3                 1.04719755
#define PIOVER4                 0.78539816


#undef ASSERT

#ifdef DEBUG
	#define ASSERT(x)               assert(x)
#else
	#define ASSERT(x)
#endif


 
// Simple wrappers around delete/free
#ifndef SAFE_DELETE
	#define SAFE_DELETE(x)			if((x) != NULL) delete(x);
#endif
#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(x)	if((x) != NULL) delete[](x);
#endif

#ifndef SAFE_DELETE_AND_CLEAR
	#define SAFE_DELETE_AND_CLEAR(x)	if((x) != NULL) delete(x), x = NULL;
#endif
#ifndef SAFE_DELETE_AND_CLEAR_ARRAY
	#define SAFE_DELETE_AND_CLEAR_ARRAY(x)	if((x) != NULL) delete[](x), x = NULL;
#endif

#define SAFE_FREE(x)            if ((x) != NULL) free(x);
#define SAFE_FREE_AND_CLEAR(x)  if ((x) != NULL) free(x), x = NULL;


// domain-specific print routines. Makes it a little easier to sort through all the printouts
// and also turn certain channels on/off

#define JSON_DEBUG(x)			//x
#define NETWORK_DEBUG(x)		//x
#define TEXTURE_DEBUG(x)		//x
#define APP_DEBUG(x)			//x
#define GUI_DEBUG(x)			//x
#define THREAD_DEBUG(x)			//x
#define ENGINE_DEBUG(x)			//x
#define BINARY_DEBUG(x)			//x
#define XML_DEBUG(x)			//x

#define JSON_PRINTF(...)		//{printf("JSON:    ");	  printf(__VA_ARGS__);}
#define NETWORK_PRINTF(...)		//{printf("NETWORK:    ");  printf(__VA_ARGS__);}
#define TEXTURE_PRINTF(...)		//{printf("TEXTURE:    ");  printf(__VA_ARGS__);}
#define APP_PRINTF(...)			{printf("APP:    ");      printf(__VA_ARGS__);}
#define GUI_PRINTF(...)			//{printf("GUI:    ");	  printf(__VA_ARGS__);}
#define THREAD_PRINTF(...)		//{printf("THREAD:	");   printf(__VA_ARGS__);}
#define ENGINE_PRINTF(...)		//{printf("ENGINE:    ");	  printf(__VA_ARGS__);}
#define BINARY_PRINTF(...)		//{printf("BINARY:    ");	  printf(__VA_ARGS__);}

// OUT is used to signify out-arguments. It has no meaning to the compiler
#define OUT

// Macros to write getters and setters with less typing
#define GETTER(type, name, var)			type getname()    { return var; }
#define C_GETTER(type, name, var)		type name() const { return var; }
#define SETTER(type, name, var)			void name(type x) { var = x;	}

#define GETTERSETTER(type, getterName, setterName, var)		\
GETTER(type, getterName, var)								\
SETTER(type, setterName, var)								

#define C_GETTERSETTER(type, getterName, setterName, var)	\
C_GETTER(type, getterName, var)								\
SETTER(type, setterName, var)

// Useful for stubbing out large classes
#define STUB(class, function)           printf("STUB: %s::%s\n", class, function)

#define BREAK							doBreakpoint();
#define BREAK_WITH_MESSAGE(MSG)		    doBreakpoint(MSG);
#define BREAK_IF_CONTEXT(c)		if(ContextManager::getCurrentContext() == (c)) breakpoint();

template <typename T>
struct GPoint
{
	GPoint<T>(T _x, T _y) : x(_x), y(_y) { }	
	T x;
	T y;
};

using namespace std;

//useful functions

extern void doBreakpoint(const string& message = "");

struct URL
{
	
	URL(string s) : m_string(s) { }
	const string& getString() const { return m_string; }
	
private:
	
	string m_string;
	
};
	

class Exception
{
public:
	
	Exception(const string& why = "Unknown exception",
			  int line = -1) : 
		m_why(why), m_line(line) 
	{ }
	
	Exception(int line = -1) :
		m_line(line) 
	{ }
	
	const string& why()  const { return m_why;  }
	int			  line() const { return m_line; }
	
private:
	
	string  m_why;
	int		m_line;
};


enum DestructBehavior
{
    DESTRUCT_DELETE,
    DESTRUCT_DELETE_ARRAY,
    DESTRUCT_FREE,
    DESTRUCT_DO_NOTHING
};


struct GRect
{
    int x;
    int y;
    int w;
    int h;
    
    GRect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) { }    
    static const GRect& negOnes() { static GRect ret(-1, -1, -1, -1); return ret; }
};


template <typename T>
class PtrWithLen
{
public:
	
	PtrWithLen<T>(T* data = NULL, 
				  int len = 0,
                  DestructBehavior dbh = DESTRUCT_DO_NOTHING,
				  bool zeroOut = false) : 
		m_data(data), m_length(len), m_destructBehavior(dbh)
	{
		if(zeroOut)
			memset(m_data, 0x00, sizeof(T) * len);
	}
		
	PtrWithLen<T>(const PtrWithLen<T> &p) : 
		m_data(p.m_data), 
		m_length(p.m_length),
        m_destructBehavior(p.m_destructBehavior)
	{
        if(m_destructBehavior != DESTRUCT_DO_NOTHING)
            ASSERT(0 && "Cannot copy a pointer with destruct behavior");
    }
    
    ~PtrWithLen<T>(){
        release(m_destructBehavior);        
    }
	
    T& operator *     ()        { return *m_data;     }
    
    operator T&       ()        { return *m_data;     }
    operator const T& () const  { return *m_data;     }
	operator T*       ()        { return m_data;      }
    operator const T* () const  { return m_data;      }
	T*	data()           const  { return m_data;      }		
	int	length()         const  { return m_length;    }
	
	const T& operator [] (int n) const { return m_data[n]; }
		  T& operator [] (int n)	   { return m_data[n]; }
        
    void setDestructBehavior(const DestructBehavior behavior){ m_destructBehavior = behavior; }
    DestructBehavior getDestructBehavior() const { return m_destructBehavior; }
    
    void setData(const T* data) { m_data = data;     }
    void setLength(int length)  { m_length = length; }
    
    void release(DestructBehavior releaseType)
    {
        switch(releaseType){
            case DESTRUCT_DELETE:
                SAFE_DELETE(m_data);
                break;
            case DESTRUCT_DELETE_ARRAY:
                SAFE_DELETE_ARRAY(m_data);
                break;
            case DESTRUCT_FREE:
                SAFE_FREE(m_data);
                break;
            case DESTRUCT_DO_NOTHING:
                break;
            default:
                printf("invalid enumerant passed to PtrWithLen::release()\n");
				int x = 4;
                ASSERT(0);
        }
    }
    
    bool operator == (const PtrWithLen<T>& rhs) const { return m_data == rhs.m_data; }
	bool operator != (const PtrWithLen<T>& rhs) const { return m_data != rhs.m_data; }
    
private:	
	
    DestructBehavior m_destructBehavior;
	T*               m_data;
	int              m_length;
	 
};


typedef PtrWithLen<unsigned char> ByteArray;

// created this specifically to be able to compare two pointers
// using the operator < function of their dereferenced type
template<typename T>
class GPtr
{
public:
	
	GPtr<T>(T* data) : m_data(data){ ASSERT(data != NULL); }	
	GPtr<T>(const GPtr<T> &rhs) : m_data(rhs.getData()){ }
	
	T* getData(){ return m_data; }	
	
	bool operator == (const T* ptr)   const { return m_data == ptr; }	
	bool operator != (const T* ptr)   const { return m_data != ptr; }
	bool operator == (unsigned int n) const { return m_data == n;   }
	bool operator != (unsigned int n) const { return m_data != n;   }
	bool operator !  ()				  const { return !m_data;		}
	
	bool operator < (const GPtr<T> &rhs){
		ASSERT(rhs.getData() != NULL);
		ASSERT(m_data != NULL);
		return *m_data < *rhs;
	}
	
	GPtr<T> operator = (const GPtr<T> &rhs){
		m_data = rhs.getData();
	}
	
	T& operator * (){ ASSERT(m_data); return *m_data; }
		
private:
	
	T*	m_data;
	
};
		

template <typename T>	
struct FuzzyLessThan
{
	bool operator () (const T& lhs, const T& rhs)
	{
		return lhs.fuzzyLessThan(rhs);
	}		
};


template<class T> inline
bool isPow2(T n)
{
	bool found1 = 0;
	FOR(i,0,sizeof(n) * 8){
		if (n & (1 << i)){
			if(found1) return 0;
			found1 = 1;
		}
	}
	return found1;
}


inline int
smallestPow2GEQ(int n)
{
	if(isPow2(n)) return n;
	if(n & 0xF000)
		RFOR(i,31,-1) if(n & (1 << i)) return (1 << (i+1));
	if(n & 0x0F00)
		RFOR(i,23,-1) if(n & (1 << i)) return (1 << (i+1));
	if(n & 0x00F0)
		RFOR(i,15,-1) if(n & (1 << i)) return (1 << (i+1));
	RFOR(i,31,-1) if(n & (1 << i)) return (1 << (i+1));
	return 1;
}



inline short
hiBits(int target)
{
	return *((short*) &target);
}

inline short&
loBits(int target)
{
	return *((short*) ((unsigned char*) &target + 2));
}




inline float 
FOVY(float aspectRatio, float focalLength)
{    
    if (aspectRatio <= 1.0f)
        return 2.0f * atan2(0.5f, focalLength * (1.0f / aspectRatio));
    return 2.0f * atan2(0.5f, focalLength);
}


inline bool isURL(const string &s)
{
	printf("isURL s = %s\n", s.c_str());
	if(s.length() < 4) return false;
	return s.substr(0, 4) == "http";
}



inline float heronArea(float a, float b, float c)
{
	const float p = 0.5f * (a + b + c);
	return sqrt(p * (p - a) * (p - b) * (p - c));
}

#define NSSTRING(a) [NSString stringWithUTF8String: a]
#define CSTRING(a) [a UTF8String]

inline
float degToRad(int deg)
{
	return PI * (deg / 180.f);
}

inline
int radToDeg(float rad)
{
	return rad * 180.0f / PI;
}


inline
bool endsWith(const string &a, const string &b)
{
    if(b.length() > a.length()) return false;
    return a.substr(a.length() - b.length()) == b;
}


inline
bool startsWith(const string &a, const string &b)
{
    if(b.length() > a.length()) return false;
    return a.substr(0, b.length()) == b;
}


template<class T> inline
T interpolate(float t, T a, T b){ return (t * a) + (b / t); }

template<class T> inline
T MAX3(T a, T b, T c){ return max(a, max(b, c)); }

template<class T> inline
T MAX4(T a, T b, T c, T d){ return max(max(a, b), max(c, d)); }

template<class T> inline
T MIN3(T a, T b, T c){ return min(a, min(b, c)); }

template<class T> inline
T MIN4(T a, T b, T c, T d){ return min(min(a, b), min(c, d)); }

inline
bool FUZZY_EQ(float a, float b, float eps = EPS){ return fabs(a - b) <= eps; }

inline
bool FUZZY_EQ0(float a, float eps = EPS){ return fabs(a) <= eps; }

inline
bool FUZZY_NEQ(float a, float b, float eps = EPS){ return fabs(a - b) > eps; }

inline
bool FUZZY_NEQ0(float a, float eps = EPS){ return fabs(a) > eps; }

inline 
bool FUZZY_GEQ(float a, float b, float eps = EPS){ return a >= b - eps; }

inline 
bool FUZZY_GEQ0(float a, float eps = EPS){ return a >= -eps; }

inline 
bool FUZZY_LEQ(float a, float b, float eps = EPS){ return a <= b + eps; }

inline 
bool FUZZY_LEQ0(float a, float eps = EPS){ return a <= eps; }

inline
bool FUZZY_IN_RNG(float val, float a, float b){ return FUZZY_GEQ(val, a) && FUZZY_LEQ(val, b); }



inline
float randFloat(float lo, float hi)
{
    float r = (float) rand() / RAND_MAX;
    return lo + r * (hi - lo);
}

inline
int randInt(int lo, int hi)
{
    return (rand() % (hi - lo + 1)) + lo;
}

inline
string i2s(int n, int pad = 0)
{
    string ret;
    ostringstream oss; oss << n; ret = oss.str();    
    return string(max(pad - (int)ret.length(), 0), '0') + ret;
}


inline int s2i(string s)
{ 
    return atoi(s.c_str()); 
} 


#endif                        