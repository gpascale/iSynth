#ifdef WINDOWS

#ifndef _D3DDRAWER_H_
#define _D3DDRAWER_H_

#include "Drawer.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <map>
#include <set>
#include <vector>
#include <string>

class D3DDrawer : public Drawer
{

public:
	D3DDrawer();
	~D3DDrawer();

	void				drawQuad(const Quad *quad);	
	void				setD3DDevice(LPDIRECT3DDEVICE9 d);
	void				setCameraOrientation(const Matrix &viewMatrix);
	void				setAspectRatio(float r);
    void                setFocalLength(float f);
	void				setNearClipPlane(float d);
	void				setFarClipPlane(float d);    
    void                setActiveTexture(string textureFile);

	void	begin(const Color4F &clearColor = Color4F(0.0f, 0.0f, 0.0f, 1.0f)) const;
	void	end() const;    

private:

    LPDIRECT3DVERTEXDECLARATION9    m_vDecl;

	D3DXCOLOR			m_color;
	D3DXCOLOR			m_inc;
	LPDIRECT3DDEVICE9	m_device;

    LPDIRECT3DTEXTURE9  m_texture; 
	D3DXMATRIX			m_worldMatD3D;
	D3DXMATRIX			m_viewMatD3D;
	D3DXMATRIX			m_projMatD3D;
	D3DXVECTOR3			m_cameraPosD3D;
	D3DXVECTOR3			m_cameraLookD3D;
	D3DXVECTOR3			m_cameraUpD3D;

	void				updateProjectionMatrix();	
	void				updateWorldMatrix(const Drawable *Drawable);

    map<string, LPDIRECT3DTEXTURE9> m_textures;    

	// marshalling
	static void			toD3DXVECTOR3(D3DXVECTOR3 &out, const Vector3 &in);
    static void			toD3DXVECTOR3(D3DXVECTOR3 &out, const Vector4 &in);
    static void			toD3DXVECTOR4(D3DXVECTOR4 &out, const Vector4 &in);
	static void			toD3DXMATRIX(D3DXMATRIX &out, const Matrix &in);
	static void			toD3DXCOLOR(D3DXCOLOR &out, const Color4F &in);

    static D3DXVECTOR3  toD3DXVECTOR3(const Vector3 &in);
    static D3DXVECTOR3  toD3DXVECTOR3(const Vector4 &in);
    static D3DXVECTOR4  toD3DXVECTOR4(const Vector4 &in);
    static D3DXMATRIX   toD3DXMATRIX(const Matrix &in);
    static D3DXCOLOR    toD3DXCOLOR(const Color4F &in);
};


#endif

#endif