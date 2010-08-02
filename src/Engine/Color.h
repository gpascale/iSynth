/*
 *  Color.h
 *  Phonosynth
 *
 *  Created by Greg on 10/17/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _COLOR_H_
#define _COLOR_H_

/*
 * A couple of simple structures used to represent colors in Phonosynth. Since the only colors I ever need
 * are RGB or RGBA, Color3 and Color4 are implemented separately and templated only on the underlying
 * data type used to store the color. Most commonly used is Color4UC which consists of 4 unsigned chars
 * representing red, green, blue and alpha respectively
 */


template<class T>
struct ColorInfo
{
	// dummy implementations, overridden with template specialization
	static T maxColorVal() { return 1.0; }
	static T minColorVal() { return 0.0; }
	static T midColorVal() { return 0.5; }
};

template<> inline float		    ColorInfo<float>::maxColorVal()		    { return 1.0f; }
template<> inline float		    ColorInfo<float>::minColorVal()		    { return 0.0f; }
template<> inline float		    ColorInfo<float>::midColorVal()		    { return 0.5f; }
template<> inline unsigned char ColorInfo<unsigned char>::maxColorVal() { return 255;  }
template<> inline unsigned char ColorInfo<unsigned char>::minColorVal() { return 0;    }
template<> inline unsigned char ColorInfo<unsigned char>::midColorVal() { return 127;  }

// so things fit on one line neatly
#define MAXCV ColorInfo<T>::maxColorVal()
#define MINCV ColorInfo<T>::minColorVal()
#define MIDCV ColorInfo<T>::minColorVal()


template<class T>
class Color3
{
public:

    union{
        struct{ T r, g, b; };
        struct{ T x, y, z; };
        T data[3];
    };
    
    Color3<T>(T _r = MAXCV, T _g = MAXCV, T _b = MAXCV) : r(_r), g(_g), b(_b) { }
    Color3<T>(const T* t) { memcpy(data, t, 3 * sizeof(T)); }
    ~Color3<T>(){ } 
    
    Color3<T>& operator = (const Color3<T>& rhs) { r = rhs.r, g = rhs.g, b = rhs.b; return *this;}
    
    T&         operator [] (const int n)       { return data[n]; }
    const T&   operator [] (const int n) const { return data[n]; }
    
    operator const T* () const { return (const T*) &r; }
    operator	   T* ()       { return (      T*) &r; }	
	
	// common colors
	static const Color3<T>& White()  { static const Color3<T> ret(MAXCV, MAXCV, MAXCV); return ret; }
	static const Color3<T>& Black()  { static const Color3<T> ret(MINCV, MINCV, MINCV); return ret; }
	static const Color3<T>& Red()    { static const Color3<T> ret(MAXCV, MINCV, MINCV); return ret; }
	static const Color3<T>& Green()  { static const Color3<T> ret(MINCV, MAXCV, MINCV); return ret; }
	static const Color3<T>& Blue()   { static const Color3<T> ret(MINCV, MINCV, MAXCV); return ret; }
	static const Color3<T>& Gray()   { static const Color3<T> ret(MIDCV, MIDCV, MIDCV); return ret; }
	static const Color3<T>& Yellow() { static const Color3<T> ret(MAXCV, MAXCV, MINCV); return ret; }
            
};


template<class T>
class Color4
{
public:
        
    union{
        struct{ T r, g, b, a; };
        struct{ T x, y, z, w; };
        T data[4];
    };                  
    
    Color4<T>(T _r = MAXCV, T _g = MAXCV, T _b = MAXCV, T _w = MAXCV) : r(_r), g(_g), b(_b), w(_w) { }
    Color4<T>(const T* t) { memcpy(data, t, 4 * sizeof(T)); }
    Color4<T>(const Color3<T>& c3) : r(c3.r), g(c3.g), b(c3.b), a(MAXCV) { }
    ~Color4<T>(){ }
    
    Color4<T>& operator = (const Color4<T>& rhs) { r = rhs.r, g = rhs.g, b = rhs.b, a = rhs.a; return *this; }
    
    T&         operator [] (const int n)       { return data[n]; }
    const T&   operator [] (const int n) const { return data[n]; }
    
    operator const T* () const { return (const T*) &r; }
    operator	   T* ()       { return (      T*) &r; }
        
	// common colors
	static const Color4<T>& White()  { static const Color4<T> ret(MAXCV, MAXCV, MAXCV, MAXCV); return ret; }
	static const Color4<T>& Black()  { static const Color4<T> ret(MINCV, MINCV, MINCV, MAXCV); return ret; }
	static const Color4<T>& Red()    { static const Color4<T> ret(MAXCV, MINCV, MINCV, MAXCV); return ret; }
	static const Color4<T>& Green()  { static const Color4<T> ret(MINCV, MAXCV, MINCV, MAXCV); return ret; }
	static const Color4<T>& Blue()   { static const Color4<T> ret(MINCV, MINCV, MAXCV, MAXCV); return ret; }
	static const Color4<T>& Gray()   { static const Color4<T> ret(MIDCV, MIDCV, MIDCV, MAXCV); return ret; }
	static const Color4<T>& Yellow() { static const Color4<T> ret(MAXCV, MAXCV, MINCV, MAXCV); return ret; }
	static const Color4<T>& None()   { static const Color4<T> ret(MINCV, MINCV, MINCV, MINCV); return ret; }	
	
};

#undef MAXCV
#undef MINCV
#undef MIDCV

typedef Color3<float> Color3F;
typedef Color4<float> Color4F;
typedef Color3<unsigned char> Color3UC;
typedef Color4<unsigned char> Color4UC;


inline
Color3UC convertColor3FToUC(const Color3F& rhs)
{
	Color3UC ret;
	const float scale = 1.0f / ColorInfo<float>::maxColorVal() * ColorInfo<unsigned char>::maxColorVal();
	FOR(i,0,3)
		ret[i] = (float) rhs[i] * scale;
	return ret;
}


inline
Color3F convertColor3UCToF(const Color3UC& rhs)
{
	Color3F ret;
	const float scale = 1.0f / ColorInfo<unsigned char>::maxColorVal() * ColorInfo<float>::maxColorVal();
	FOR(i,0,3)
	ret[i] = (float) rhs[i] * scale;
	return ret;
}


inline
Color4UC convertColor4FToUC(const Color4F& rhs)
{
	Color4UC ret;
	const float scale = 1.0f / ColorInfo<float>::maxColorVal() * ColorInfo<unsigned char>::maxColorVal();
	FOR(i,0,4)
	ret[i] = (float) rhs[i] * scale;
	return ret;
}


inline
Color4F convertColor4UCToF(const Color4UC& rhs)
{
	Color4F ret;
	const float scale = 1.0f / ColorInfo<unsigned char>::maxColorVal() * ColorInfo<float>::maxColorVal();
	FOR(i,0,4)
	ret[i] = (float) rhs[i] * scale;
	return ret;
}


#endif