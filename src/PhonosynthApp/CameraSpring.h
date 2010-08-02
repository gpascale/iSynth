#ifndef _CAMERASPRING_H_
#define _CAMERASPRING_H_


#include <GMath.h>
#include <GTime.h>

class SpringyCamera;
class GTime;
class PSButtonManager;

class CameraSpringUser
{
public:
	virtual void onCameraSpringStart() = 0;
	virtual void onCameraSpringStop()  = 0;	
};
	
enum{
	SPRING_FOCAL_LENGTH		  = (1 << 0),
	SPRING_POSITION			  = (1 << 1),
	SPRING_ROTATION			  = (1 << 2),
	SPRING_ZNEAR			  = (1 << 3),
	SPRING_ZFAR				  = (1 << 4),
	SPRING_ASPECT_RATIO		  = (1 << 5),
	SPRING_ARCBALL_ROTATION   = (1 << 6),
	SPRING_ARCBALL_CENTER     = (1 << 7),
    SPRING_ARCBALL_RADIUS     = (1 << 8),
	SPRING_SCREEN_ORIENTATION = (1 << 9),
    SPRING_ARCBALL_ALL        = (1 << 6) | (1 << 7) | (1 << 8),
    SPRING_LENS_ALL           = (1 << 0) | (1 << 3) | (1 << 4) | (1 << 5)
};

// CameraSprings are the mechanism through which all camera movement and manipulation happens. Rather than
// move or manipulate the camera explicitly each time update is called, the camera is governed by a spring
// which pulls it a little bit towards some destination state a little bit on each update. This has 3 major benefits
// 1. It is very clean and easy to work with
// 2. It separates the camera entirely from the frame rate
// 3. It allows for non-linear manipulation of camera parameters (notice how camera's speed varies as it moves)


class CameraSpring
{
public:    
	
    CameraSpring();
    ~CameraSpring();

    
    int     getFlags () const { return m_flags; }
    
    // sets the camera this spring is to manipulate
	void	setTarget(SpringyCamera* c);    
    
    // so that the camera spring user can be notified when the spring starts and stops
	void	setParent(CameraSpringUser *parent) { m_parent = parent; }
    
    void    start(float duration, int flags);
    void    stop(bool didFinish);
    void    update();
    void    update(const GTime &now);
	
	void	setDuration(float dur){ ASSERT(dur >= 0.0f); m_duration = dur; }
	
	bool	getActive()	const	{ return m_active; }

    // these functions should really never be used. If some parameter of the camera is modified
    // by somebody other than this spring, then the camera should call the appropriate setStart
    // function to make sure it is on the same page as the spring
    void    setStartPosition         (const Point3 &p    ) { m_startPosition = p;          }
    void    setStartRotation         (const Quaternion &q) { m_startRotation = q;          }
    void    setStartFocalLength      (float f            ) { m_startFocalLength = f;       }
    void    setStartAspectRatio      (float f            ) { m_startAspectRatio = f;       }
    void	setStartZNear            (float f			 ) { m_startZNear = f;             }
    void	setStartZFar             (float f			 ) { m_startZFar = f;              }
    void	setStartScreenOrientation(float f            ) { m_startScreenOrientation = f; }
    void	setStartArcballCenter    (const Point3& p    ) { m_startArcballCenter = p;     }
    void	setStartArcballRadius    (float f			 ) { m_startArcballRadius = f;     }
    
    
    // these function set target values for the spring. When released from rest, the spring
    // will gradually pull the camera toward these values
    void    setEndPosition           (const Point3 &p    ) { m_endPosition = p;            }    
    void    setEndRotation           (const Quaternion &q) { m_endRotation = q;            }
    void    setEndFocalLength        (float f            ) { m_endFocalLength = f;         }    
    void    setEndAspectRatio        (float f            ) { m_endAspectRatio = f;         }	
	void	setEndZNear              (float f			 ) { m_endZNear = f;               }	
	void	setEndZFar               (float f			 ) { m_endZFar = f;                }		
	void	setEndScreenOrientation  (float f            ) { m_endScreenOrientation = f;   }		
	void	setEndArcballCenter      (const Point3& p    ) { m_endArcballCenter = p;       }		
    void	setEndArcballRadius      (float f			 ) { m_endArcballRadius = f;       }
 	
    // a few of the spring parameters need to be readable for various reasons
	float				getEndFocalLength()   const { return m_endFocalLength;   }
	const Quaternion&	getEndRotation()      const { return m_endRotation;      }
    float               getEndArcballRadius() const { return m_endArcballRadius; }
    const Point3&       getEndArcballCenter() const { return m_endArcballCenter; }

    
private:    

    // returns the current spring time normalized to a 0-1 scale (faster than linear)
	inline float getTime01Fast(const GTime &now) const;
	// returns the current spring time normalized to a 0-1 scale (slower than linear)
	inline float getTime01Slow(const GTime &now) const;
    
    // The following functions compute current spring values by interpolating between start
    // and end according to the normalized time t01.
	inline void getCurRotation(Quaternion &ret, float t01) const;		
	inline void getCurScreenOrientation(float& ret, float t01) const;	
	inline void getCurPosition(Point3 &ret, float t01) const;
	inline void getCurArcballCenter(Point3 &ret, float t01) const;
    inline void getCurArcballRadius(float &ret, float t01) const;
	inline void getCurFocalLength(float &ret, float t01) const;
	inline void getCurZNear(float &ret, float t01) const;
	inline void getCurZFar(float &ret, float t01) const;	
	
	CameraSpringUser*		m_parent;
	
    // the camera to manipulate
    SpringyCamera*          m_target;

    // manipulation parameters
    Quaternion      m_startRotation;
    Quaternion      m_endRotation;	
    Point3          m_startPosition;
    Point3          m_endPosition;        
    float           m_startAspectRatio;
    float           m_endAspectRatio;	
    float           m_startFocalLength;
    float           m_endFocalLength;   	
	float			m_startZNear;
	float			m_endZNear;	
	float			m_startZFar;
	float			m_endZFar;
    Point3			m_startArcballCenter;
	Point3			m_endArcballCenter;
    float           m_startArcballRadius;
    float           m_endArcballRadius;
	float			m_startScreenOrientation;
	float			m_endScreenOrientation;    
    
    Quaternion      m_curRotation;	
    Point3          m_curPosition;      
    float           m_curAspectRatio;
    float           m_curFocalLength; 	
	float			m_curZNear;
	float			m_curZFar;
    Point3			m_curArcballCenter;
    float           m_curArcballRadius;
	float			m_curScreenOrientation;
        		
    // other shit
    float           m_duration;       // how long the spring will take to come to rest
    GTime			m_startTime;      // the time at which the spring was started
    bool            m_active;         // true if the spring is currently in effect, false if it is "at rest"
	int				m_flags;          // mask representing the attributes affected by this spring
    
    // kind of hacky, maybe a better place to keep this information, probably should rework Camera
	
	PSButtonManager*  m_buttonManager;
	
	
};

#include "CameraSpring.hpp"

#endif