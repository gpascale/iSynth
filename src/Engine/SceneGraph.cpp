/*
 *  SceneGraph.cpp
 *  Phonosynth
 *
 *  Created by Greg on 11/24/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "SceneGraph.h"
#include <PSTypes.h>
#include <GTime.h>
#include <Animator.h>



/**********************************************************************************************/
/*                          SceneGraphNode implementation                                     */
/**********************************************************************************************/

SceneGraphNode::SceneGraphNode(SceneGraphNode* parent) :
    m_parent(parent),
    m_active(true)
{
    
}


SceneGraphNode::~SceneGraphNode()
{
	// only thing to do here is delete the node's animators
	const int nAnimators = (const int) m_animators.size();
	FOR(i,0,nAnimators)
		SAFE_DELETE(m_animators[i]);
}


void
SceneGraphNode::writeToStream(ostream& stream, const string& tab)
{    
}


void
SceneGraphNode::visit(Drawer& drawer)
{
	doVisit(drawer);
}




/**********************************************************************************************/
/*                          GroupNode implementation                                          */
/**********************************************************************************************/

GroupNode::GroupNode(SceneGraphNode* parent, int initialChildCapacity) :
    SceneGraphNode(parent),
    m_numChildren(0),
    m_children(initialChildCapacity, -1),
	m_visibleCount(0)
{
    
}


GroupNode::GroupNode(int initialChildCapacity) :
    SceneGraphNode(NULL),
    m_numChildren(0),
    m_children(initialChildCapacity, -1),
	m_visibleCount(0)
{
    
}


GroupNode::~GroupNode()
{
	// group nodes own their children, so forward the delete call down to them
    FOR(i,0,m_children.size())
		SAFE_DELETE(m_children[i]);
}


const SceneGraphNode*
GroupNode::getChild(int index) const
{
    if(index >= m_children.size()){
        return NULL;
    }
    return (const SceneGraphNode*) m_children[index];
}


SceneGraphNode*
GroupNode::getChild(int index)
{
    if(index >= m_children.size()){
        return NULL;
    }
    return m_children[index];
}


// TODO: need to return bool?
bool
GroupNode::addChild(SceneGraphNode* toAdd, int index)
{
    toAdd->setSceneGraph(m_scenegraph);
    
    if(index < 0)
        m_children.insert(toAdd);
    else
        m_children.insert(toAdd, index);
    return true;
}


SceneGraphNode*
GroupNode::removeChild(int index)
{
    ASSERT(index >= 0);
    return m_children.remove(index);
}


void
GroupNode::doVisit(Drawer& drawer)
{
	m_scenegraph->pushOntoRenderQueue(this);
	FOR(i,0,m_children.size())
		if(m_children[i]->getActive())
			m_children[i]->visit(drawer);    
}


void
GroupNode::doDraw(Drawer& drawer)
{
	FOR(i,0,m_animators.size())
		m_animators[i]->apply(drawer);
    draw(drawer);
}


void
GroupNode::writeToStream(ostream& stream, const string& tab)
{
    stream << tab << "GroupNode" << endl;
    FOR(i,0,m_children.size())
        m_children[i]->writeToStream(stream, tab + "\t");
}
 


/**********************************************************************************************/
/*                          GeometryNode implementation                                       */
/**********************************************************************************************/
    

GeometryNode::GeometryNode(SceneGraphNode* parent) :
    SceneGraphNode(parent)
{    
    setDrawPriority(PRIORITY_LAST);
}


GeometryNode::~GeometryNode()
{
    // nothing to delete in the base class
}


void
GeometryNode::doVisit(Drawer& drawer)
{   
    if(m_active)
        m_scenegraph->pushOntoRenderQueue(this);    
}


void
GeometryNode::doDraw(Drawer& drawer)
{
    FOR(i,0,m_animators.size())
        m_animators[i]->apply(drawer);
    
    draw(drawer);
}


void
GeometryNode::writeToStream(ostream& stream, const string& tab)
{
    stream << tab << "GeometryNode" << endl;
}


/**********************************************************************************************/
/*                          SceneGraph implementation                                         */
/**********************************************************************************************/


SceneGraph::SceneGraph(SceneGraphNode* root) :
    m_root(root),
	m_invalidated(true)
{
    if(m_root != NULL)
        m_root->setSceneGraph(this);
}


void
SceneGraph::cleanup()
{
	m_renderQueue.clear();
	SAFE_DELETE_AND_CLEAR(m_root);
}


void
SceneGraph::setRoot(SceneGraphNode* root)
{
    m_root = root;
    if(m_root != NULL)
        m_root->setSceneGraph(this);
}


void
SceneGraph::render(Drawer& drawer, SceneGraphNode* root)
{
	if(root == NULL)
		root = m_root;
	
    if(!root){
        //cout << "tried to render the scenegraph but root is null!" << endl;
        return;
    }

	if(m_invalidated || root != m_root){
		////cerr << "walking scenegraph" << endl;
		m_invalidated = false;
		// walk the scenegraph, objects to be rendered will be thrown into the render queue
		m_renderQueue.clear();
		m_renderQueue.reserve(16);
		//m_root->visit(drawer);
		root->visit(drawer);
		// sort the render queue and render its contents
		sort(m_renderQueue.begin(), m_renderQueue.end(), RenderPriorityComparator());
	}
	
    const int nToRender = (const int) m_renderQueue.size();
	
    FOR(i,0,nToRender)
        m_renderQueue[i]->doDraw(drawer);
}


void
SceneGraph::writeToStream(ostream& stream)
{
    m_root->writeToStream(stream, "");
}


void
SceneGraph::pushOntoRenderQueue(SceneGraphNode* node)
{   
    m_renderQueue.push_back(node);
}

