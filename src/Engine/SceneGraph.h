/*
 *  SceneGraph.h
 *  Phonosynth
 *
 *  Created by Greg on 11/23/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include <common.h>
#include <vector>

class Texture;
class Drawer;
class GTime;

template<class T>
class GrowablePtrArray
{
public:
    
    GrowablePtrArray<T>(int initialCapacity = 8, int growBy = -1) : 
        m_capacity(initialCapacity),
        m_growBy(growBy),
        m_sz(0)
    { 
        m_array = new T*[m_capacity];
        memset(m_array, 0x00, m_capacity * sizeof(T*));
    }
    
    ~GrowablePtrArray(){
		// we free the array but not the pointers it contains. Since those may or may not have
		// been allocated on the heap, the responsibility of cleaning them up is left to the
		// client, exactly how it is in vector
        SAFE_DELETE_ARRAY(m_array);
    }
    
    int size() const { return m_sz; }
    
    void setGrowBy(int n) { m_growBy = n; }
    
    int insert(T* t){
        FOR(i,0,m_capacity){
            if(m_array[i] == NULL){
                m_array[i] = t;
                m_sz = max(m_sz, i + 1);
                return i;
            }
        }
        
        // array is filled to capacity, need to grow it
        const int newCapacity = (m_growBy > 0) ? (m_capacity + m_growBy) : (m_capacity << 1);                
        int ret = m_capacity;
        // grow the array
        growArray(newCapacity);        
        //insert the item
        m_array[ret] = t;
        m_sz = max(m_sz, ret + 1);
        return ret;
    }
    
    // inserts t at the specified index. Returns the previous occupier of the index, or NULL
    // if it was empty. 0
    T* insert(T* t, int index)
    {
        if(index >= m_capacity){
            growArray(index + 1);
            m_array[index] = t;
            m_sz = index + 1;
            return NULL;     
        }
        else{
            T* ret = m_array[index];
            m_array[index] = t;
            m_sz = max(m_sz, index + 1);
            return ret;
        }
    }
    
    T* remove(int index){
        if(index < 0 || index >= m_capacity){
            ASSERT("trying to remove element from outside array bounds" && 0);
            // silently return null in release mode
            return NULL;
        }
        T* ret = m_array[index];
        m_array[index] = NULL;
        return ret;
    }
    
    bool contains(const T* t) const
    {
        FOR(i,0,m_sz)
            if(m_array[i] == t)
                return true;
        return false;
    }
    
    const T* operator [] (int n) const { return m_array[n]; }
          T* operator [] (int n)       { return m_array[n]; }
    
private:
        
    void growArray(int newCapacity)
    {
        ASSERT(newCapacity > m_capacity);
        T** tArray = new T*[newCapacity];
        memset(tArray, 0x00, newCapacity * sizeof(T*));
        memcpy(tArray, m_array, m_capacity * sizeof(T*));
        m_array = tArray;
        m_capacity = newCapacity;
    }    
    
    
    T** m_array;
    
    int m_sz;
    int m_capacity;
    int m_growBy;
    
};
 

class Animator;
class SceneGraph;


class SceneGraphNode
{

public:
    
    SceneGraphNode(SceneGraphNode* parent = NULL);
    virtual ~SceneGraphNode();
    
    void setParent(SceneGraphNode* p) { m_parent = p; }
        
    const SceneGraphNode* getParent() const { return m_parent; }
          SceneGraphNode* getParent()       { return m_parent; }
    
    void visit(Drawer& drawer);
    virtual void doVisit(Drawer& drawer) = 0;
    virtual void doDraw(Drawer& drawer) = 0;
    
    virtual void writeToStream(ostream& stream, const string& tab);
    
    void setSceneGraph(SceneGraph* sg) { m_scenegraph = sg; }
    
    // draw priority is needed for proper rendering order    
    virtual int  setDrawPriority(int pri)       { m_drawPriority = pri;  }
    virtual int  getDrawPriority()        const { return m_drawPriority; }
    
    const vector<Animator*> getAnimators() const { return m_animators; }
          vector<Animator*> getAnimators()       { return m_animators; }
    
    void  addAnimator(Animator* a)               { m_animators.push_back(a); }
    
    // use to activate or deactivate the node
    virtual void    setActive(bool b)   { m_active = b;    }
    virtual bool    getActive() const   { return m_active; }
	
	
	// used by children to tell parent when they become visible/invisible
	// declared in generic Node but only implemented by GroupNode
	virtual void incVisibleCount() { }
	virtual void decVisibleCount() { }
    
protected:
    
    // a node knows which scenegraph it's a member of. Currently the only use for this is
    // so that GeometryNodes can add themselves to the scenegraph's render queue, but it
    // doesn't seem like a bad idea to have this here regardless
    SceneGraph*         m_scenegraph;                                          
    bool                m_active;      // necessary?
    SceneGraphNode*     m_parent;
    vector<Animator*>   m_animators;   // should this really be in abstract SceneGraphNode?
    float               m_drawPriority;

};


class GroupNode : public SceneGraphNode
{
public:
    
    GroupNode(SceneGraphNode* parent = NULL, int initialChildCapacity = 8);
    GroupNode(int initialChildCapacity);
    
    virtual ~GroupNode();            
    
    const SceneGraphNode*   getChild(int index) const;
          SceneGraphNode*   getChild(int index);	
    
    int                     numChildren() const { return m_children.size(); }
    
    bool                    addChild(SceneGraphNode* child, int index = -1);
    SceneGraphNode*         removeChild(int index);
    
    void doVisit(Drawer& drawer);
    void doDraw(Drawer& drawer);    
    
    // by default, GroupNodes don't do anything when told to draw. Subclasses may
    // make changes to render state which affect children
    virtual void draw(Drawer& drawer) { };
    
    // debug
    virtual void writeToStream(ostream& stream, const string& tab);
    
private:
	
	/*
	// called by a child when it becomes visible
	void incVisibleCount();
	// called by a child when it becomes invisible
	void decVisibleCount();
	 */
    
	int									  m_visibleCount;
    int                                   m_numChildren;    
    GrowablePtrArray<SceneGraphNode>      m_children;
        
};


class GeometryNode : public SceneGraphNode
{
    
public:
    GeometryNode(SceneGraphNode* parent = NULL);
    virtual ~GeometryNode();
    
    // doVisit pushes this node onto the scenegraph's render queue if it needs to be rendered
    void doVisit(Drawer& drawer);    
    // doDraw activates all of this node's animators and then calls draw.
    void doDraw(Drawer& drawer);
    
    // All geometry nodes must override draw
    virtual void draw(Drawer& drawer) = 0;
    
    // debug
    virtual void writeToStream(ostream& stream, const string& tab);
            
protected:    		
	
	/*
	inline setVisible()   { if(m_visible) return; m_parent->incVisibleCount(); m_visible = true;   }
	inline setInvisible() { if(!m_visible) return; m_parent->decVisibleCount(); m_visible = false; }
	 */
	
private:
    
};



typedef vector<SceneGraphNode*> RenderQueue;

struct RenderPriorityComparator : public binary_function<SceneGraphNode*, SceneGraphNode*, bool>
{
    bool operator () (const SceneGraphNode* lhs, const SceneGraphNode* rhs)
    {        
        return lhs->getDrawPriority() < rhs->getDrawPriority(); 
    }
};
    

class SceneGraph
{
public:
    
    SceneGraph(SceneGraphNode* root = NULL);
    ~SceneGraph();
    
    void  setRoot(SceneGraphNode* root);
	
	void  cleanup();
	
    const SceneGraphNode* getRoot() const { return m_root; }
          SceneGraphNode* getRoot()       { return m_root; }
    
    void render(Drawer& drawer, SceneGraphNode* root = NULL);
    
    void writeToStream(ostream& stream);
    void writeToFile(const char* filename);
    
    void setTextureAnimator(int coordSystem, int index, Texture* texAnim);
    
    void pushOntoRenderQueue(SceneGraphNode* node);
	
	void invalidate() { m_invalidated = true; }
    
private:
    
    RenderQueue     m_renderQueue;
    SceneGraphNode* m_root;
    
    bool			m_invalidated;
};

#endif // _SCENEGRAPH_H_