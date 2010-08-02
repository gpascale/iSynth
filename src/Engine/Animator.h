/*
 *  Animator.h
 *  Phonosynth
 *
 *  Created by Greg on 12/18/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include <GTime.h>
#include <GMath.h>
#include <GMutex.h>

class SceneGraphNode;
class GeometryNode;
class Texture;
class Drawer;


class Animator
{
public:
    
    Animator(){ }
    virtual ~Animator(){ }
    
    virtual void apply(Drawer& d) = 0;       
    virtual void setTarget(SceneGraphNode* target) = 0;
    
protected:
	
	// animators are used by the render thread but typically manipulated by other threads 
	// so there is a need for synchronization
	GMutex		 m_mutex;
	
};


// an animator responsible for texturing objects. Handles projective texturing and smooth
// transitions between different textures
class TextureAnimator : public Animator
{
public:
    
    TextureAnimator(Texture* startingTexture = NULL, bool projective = false);
    TextureAnimator(bool projective);
    
    ~TextureAnimator() { /* fill in later */ }    	
	
    void apply(Drawer& d);
    void setTarget(SceneGraphNode* target);

    // change the texture this animator textures its target with. If fadeInDuration is nonzero,
    // the new texture will smoothly
    void setNewTexture(Texture* newTex, float fadeInDuration = 0.0f);
	
	const Texture* getCurrentTexture() const { return m_texture; }
    
    // set the texture matrix to be used in the case of projective texturing
    void setProjectiveTextureMatrix(const Matrix& matrix);
	
	bool effectActive() const { return m_effectActive; }
    
private:
    
    Texture*      m_texture;          // the currently applied texture
    Texture*      m_oldTexture;       // the old texture, used during texture transitions
    
    GTime         m_effectStart;      // the time at which the last/current transition effect was started
    GTime         m_effectEnd;        // the duration of the last/current transition effect    
    bool          m_effectActive;
            
    GeometryNode* m_target;           // can only apply texturing to GeometryNodes    
    
    bool          m_projective;       // whether or not this effect does projective texturing
    Matrix        m_textureMatrix;    // The matrix to be used for projective texturing
    
};

#endif // _ANIMATOR_H_