#ifndef _OPACITYSPRING_H_
#define _OPACITYSPRING_H_

#include <Drawable.h>
#include <GTime.h>
#include <GMutex.h>
#include <Animator.h>

class Functor;
class Drawer;

// won't need this when I remove that big hack
#include <Drawer.h>


// TODO: rewrite this class a bit when have some time

using namespace std;

class OpacitySpring : public Animator
{
public:

    OpacitySpring();
    OpacitySpring(AdjustableOpacity *e);
    ~OpacitySpring();

    // pure virtual functions inherited from Animator
    
    // probably take these out to be more consistent with TextureAnimator
    void    start();
    void    stop();
    
    // overrides of pure virtual Animator functions
    void    setTarget(SceneGraphNode *e) { m_target = dynamic_cast<AdjustableOpacity*>(e); }
    void    update();
    void    update(const GTime& now);
    
    // TODO: fix this big hack, it makes code compile for now
    void    apply(Drawer& d) { update(); }
    
    // this function should really only be used once per spring to match up the spring's
    // starting opacity with the opacity of the target. More generally, anytime the target's
    // opacity changes by some means other than this spring, this function should be called
    // to keep the two on the same page
    void    setFromOpacity(float opacity);
    
    // this function is the means by which the spring is used to actually change the target's
    // opacity. Calling it has the effect of activating the spring
    void    setToOpacity(float opacity, float duration = 1.0f);
    
    // probably shouldn't be necessary before long
	float	getToOpacity() const { return m_toOpacity; }        
    	
	// allows the user to specify a function to be called when the spring finishes
	void    setEndFunctor(const Functor& f);

private:

    
    float           m_fromOpacity;      // the opacity the spring is modulating from
    float           m_toOpacity;        // the opacity the spring is modulating to        
    float           m_currentOpacity;   // the current opacity (as of the last spring update) This
                                        // could potentially cause problems if the spring has for 
                                        // some reason not updated in a while, but its cheaper to
                                        // use this value than recalculate the current opacity when
                                        // we need to update the starting opacity
	
    float           m_duration;
	GTime			m_startTime;
    bool            m_active;
	
    AdjustableOpacity*    m_target;
	
	Functor*		m_endFunctor;

	GMutex			m_mutex;
	
};



#endif

