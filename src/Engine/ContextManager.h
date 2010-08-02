/*
 *  ContextManager.h
 *  Phonosynth
 *
 *  Created by Greg on 11/17/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

/* ContextManager is a singleton for managing all of the program's GL contexts */

#ifndef _CONTEXTMANAGER_H_
#define _CONTEXTMANAGER_H_

class ContextManager
{
public:    
    
	// Access to the contexts
	static EAGLContext* getMainThreadContext(){
		return get().mainContext;
	}
    static EAGLContext* getRenderContext(){
        return get().renderContext;
    }
    static EAGLContext* getTextureContext(){
        return get().textureContext;
    }	
		
	// Quickly activate a specific context
    static void activateRenderContext(){
        [EAGLContext setCurrentContext: get().renderContext];
    }
    static void activateTextureContext(){
        [EAGLContext setCurrentContext: get().textureContext];
    }
	static void activateMainThreadContext(){
		[EAGLContext setCurrentContext: get().mainContext];
	}
	
	// explicitly activate the specified context
	static void activateContext(EAGLContext* c){
		[EAGLContext setCurrentContext: c];
	}
	
	// returns the currently active context
	static EAGLContext* getCurrentContext(){
		return [EAGLContext currentContext];
	}		
	
	// releases the currently active context. A thread must do this before it finishes
	// execution or is cancelled. Failure to do so was causing some very unpredictable and
	// werid results after the context was used by enough threads.
	static void releaseCurrentContext(){
		[EAGLContext setCurrentContext: nil];
	}
        
    
private:        
    	
    ContextManager()
    {
		mainContext			   = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1
													   sharegroup: nil];
        renderContext		   = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1 
													   sharegroup: mainContext.sharegroup];
        textureContext		   = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1 
												       sharegroup: mainContext.sharegroup];
	}
	
    static ContextManager& get()
    {
        static ContextManager ret;
        return ret;
    }
    
    EAGLSharegroup* group;
	EAGLContext*	mainContext;
    EAGLContext*    renderContext;
    EAGLContext*    textureContext;	
    
};




#endif // _CONTEXTMANAGER_H_