#include "Drawer.h"

#include "TextureManager.h"

Drawer::Drawer(void) :
	m_aspectRatio(1.333333f),
    m_focalLength(1.0f),
	m_nearClipPlane(0.001f),
	m_farClipPlane(10000.0f),
	m_textureManager(NULL),
    m_doWorldTransform(false)
{
	//m_textureManager = new TextureManager();
}






