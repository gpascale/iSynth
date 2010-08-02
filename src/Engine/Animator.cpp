/*
 *  Animator.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/18/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "Animator.h"
#include <SceneGraph.h>
#include <Drawer.h>
#include <Texture.h>
#import  <QuartzCore/QuartzCore.h>
#import  <OpenGLES/EAGLDrawable.h>
#import  <OpenGLES/EAGL.h>
#import  <OpenGLES/ES1/gl.h>
#import  <OpenGLES/ES1/glext.h>

TextureAnimator::TextureAnimator(Texture* startingTexture, bool projective) :
    m_texture(startingTexture),
    m_oldTexture(NULL),
    m_projective(projective),
    m_effectStart(GTime::Zero()),
    m_effectEnd(GTime::Zero()),
    m_effectActive(false)
{
    
}


TextureAnimator::TextureAnimator(bool projective) : 
    m_texture(NULL),
    m_oldTexture(NULL),
    m_projective(projective),
    m_effectStart(GTime::Zero()),
    m_effectEnd(GTime::Zero()),
    m_effectActive(false)
{

}


void
TextureAnimator::setTarget(SceneGraphNode* target)
{
    m_target = dynamic_cast<GeometryNode*>(target);
}


void
TextureAnimator::setProjectiveTextureMatrix(const Matrix& mat)
{
    m_textureMatrix = mat; // store this for reference only, it isn't needed

    // matrix used to shift projected texture coordinates from [-1, 1] to [0, 1] 
    
    static const Matrix projModulate(0.5f, 0.0f, 0.0f, 0.0f, 
                                     0.0f, 0.5f, 0.0f, 0.0f,
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.5f, 0.5f, 0.0f, 1.0f);
    
    m_textureMatrix = mat * projModulate;
}
    

void
TextureAnimator::apply(Drawer& d)
{    
	m_mutex.lock();
	
	GTime now = GTime::Now();
	
    if(m_effectActive = (now < m_effectEnd)){
		float w1 = (now - m_effectStart) / (m_effectEnd - m_effectStart);
		
		d.setActiveTexture(m_oldTexture, 0);
		if(m_oldTexture){
			d.setTextureMatrix(m_oldTexture->getMatrix(), 0);
			////cerr << "applying GL texture " << m_oldTexture->getGLTexture() << endl;
		}
		
		d.setActiveTexture(m_texture, 1);
		if(m_texture){			
			d.setTextureMatrix(m_texture->getMatrix(), 1);
			////cerr << "applying GL texture " << m_texture->getGLTexture() << endl;
		}
		d.enableTextureUnit(1);							
				
		d.setMultitextureInterpolationParameter(w1);
    }
    else{        
		
		d.setActiveTexture(m_texture, 0);
		d.disableTextureUnit(1);        
		
		if(m_texture != NULL){
			if(m_projective){
				/*
				static const Matrix projModulate(0.5f, 0.0f, 0.0f, 0.0f, 
												 0.0f, 0.5f, 0.0f, 0.0f,
												 0.0f, 0.0f, 1.0f, 0.0f,
												 0.5f, 0.5f, 0.0f, 1.0f);
				
				d.setTextureMatrix(d.getViewMatrix() * d.getProjectionMatrix() * projModulate);
				*/
				const float xS = (m_texture ? m_texture->getHRatio() : 1.0f);
				const float yS = (m_texture ? m_texture->getVRatio() : 1.0f);
				Matrix scaleMat;
				Matrix::ScaleMatrix(scaleMat, xS * 0.99f, yS * 0.99f, 1.0f);
				d.setTextureMatrix(m_textureMatrix * scaleMat);
			}
			else{				
				d.setTextureMatrix(m_texture->getMatrix());
				////cerr << "applying GL texture " << m_texture->getGLTexture() << endl;				
			}
		}
    }
	m_mutex.unlock();
}


void
TextureAnimator::setNewTexture(Texture* newTex, float fadeInDuration)
{       
	m_mutex.lock();
    if(fadeInDuration > 0.01f){
        m_effectActive = true;
        m_effectStart = GTime::Now();
        m_effectEnd = m_effectStart + fadeInDuration;
        m_oldTexture = m_texture;
    }
    else{
        m_oldTexture = NULL;
        m_effectActive = false;
    }        
    m_texture = newTex;
	m_mutex.unlock();
}
