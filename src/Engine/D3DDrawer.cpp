#ifdef WINDOWS

#include "D3DDrawer.h"
#include <GMath.h>
#include <iostream>

const D3DVERTEXELEMENT9 decl[4] =
{
    {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    {0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    D3DDECL_END()
};

using namespace std;

D3DDrawer::D3DDrawer() : Drawer()    
{    
    m_textures.clear();
	m_color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f);
	m_inc = D3DXCOLOR(0.0f, 0.0005f, 0.0007f, 0.0012f);     
}

D3DDrawer::~D3DDrawer()
{
	//cout << "STUB: D3DDrawer::~D3DDrawer()" << endl;
}


void
D3DDrawer::setD3DDevice(LPDIRECT3DDEVICE9 d)
{
    m_device = d;
    m_device->CreateVertexDeclaration(decl, &m_vDecl);
    
    m_device->SetRenderState(D3DRS_ZENABLE,  FALSE); // Z-Buffer (Depth Buffer)
    m_device->SetRenderState(D3DRS_CULLMODE, FALSE); // Disable Backface Culling
    m_device->SetRenderState(D3DRS_LIGHTING, FALSE); // Disable Light

    
    m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    
    m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);          
    m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    
    m_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
    m_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, 
                                      D3DTTFF_PROJECTED | D3DTTFF_COUNT4);
    
}


void
D3DDrawer::toD3DXVECTOR3(D3DXVECTOR3 &out, const Vector3 &in)
{
	memcpy((float*) out, ((Vector3&) in).getDataPointer(), 3 * sizeof(float));
}


void
D3DDrawer::toD3DXVECTOR3(D3DXVECTOR3 &out, const Vector4 &in)
{
	memcpy((float*) out, ((Vector3&) in).getDataPointer(), 3 * sizeof(float));
}


void
D3DDrawer::toD3DXVECTOR4(D3DXVECTOR4 &out, const Vector4 &in)
{
	memcpy((float*) out, ((Vector3&) in).getDataPointer(), 4 * sizeof(float));
}


void
D3DDrawer::toD3DXMATRIX(D3DXMATRIX &out, const Matrix &in)
{
	memcpy((float*) out, ((Vector3&) in).getDataPointer(), 16 * sizeof(float));
}



D3DXCOLOR
D3DDrawer::toD3DXCOLOR(const Color4F &in)
{
    D3DXCOLOR ret;
	toD3DXCOLOR(ret, in);
    return ret;
}

D3DXVECTOR3
D3DDrawer::toD3DXVECTOR3(const Vector3 &in)
{
    D3DXVECTOR3 ret;
    toD3DXVECTOR3(ret, in);
    return ret;
}


D3DXVECTOR3
D3DDrawer::toD3DXVECTOR3(const Vector4 &in)
{
	D3DXVECTOR3 ret;
    toD3DXVECTOR3(ret, in);
    return ret;
}


D3DXVECTOR4
D3DDrawer::toD3DXVECTOR4(const Vector4 &in)
{
	D3DXVECTOR4 ret;
    toD3DXVECTOR4(ret, in);
    return ret;
}


D3DXMATRIX
D3DDrawer::toD3DXMATRIX(const Matrix &in)
{
	D3DXMATRIX ret;
    toD3DXMATRIX(ret, in);
    return ret;
}



void
D3DDrawer::toD3DXCOLOR(D3DXCOLOR &out, const Color4F &in)
{
	memcpy((float*) out, ((Color4F&) in).getDataPointer(), 4 * sizeof(float));
}



// this method is overloaded so we can fill the D3D structures in 
// straight away rather than having to do an extra copy
void
D3DDrawer::setCameraOrientation(const Matrix &viewMatrix)
{	
	Matrix::copyMatrix(m_viewMatrix, viewMatrix);	
	toD3DXMATRIX(m_viewMatD3D, m_viewMatrix);    

	// not sure why this is necessary ... could point to a bigger problem

	m_device->SetTransform(D3DTS_VIEW, &m_viewMatD3D);    
    updateProjectionMatrix();
}



void
D3DDrawer::updateProjectionMatrix()
{

	D3DXMatrixPerspectiveFovLH(&m_projMatD3D, 
							   FOVY(m_aspectRatio, m_focalLength), 
							   m_aspectRatio, 
							   m_nearClipPlane,
							   m_farClipPlane);	
    memcpy(m_projectionMatrix.getDataPointer(), (float*) m_projMatD3D, 16 * sizeof(float));    
    
    Matrix::ProjectionMatrixPerspRH(m_projectionMatrix,
                             FOVY(m_aspectRatio, m_focalLength / 2.0f),
                             m_aspectRatio,
                             m_nearClipPlane,
                             m_farClipPlane);
    m_projMatD3D = toD3DXMATRIX(m_projectionMatrix);
    m_device->SetTransform(D3DTS_PROJECTION, &m_projMatD3D);
    

    Matrix m;
    Matrix::IdentityMatrix(m);
    m(1,1) = -1.0f;
    m_projectionMatrix = m_projectionMatrix * m;
    m_projMatD3D = toD3DXMATRIX(m_projectionMatrix);
    m_device->SetTransform(D3DTS_PROJECTION, &m_projMatD3D);

}





void
D3DDrawer::updateWorldMatrix(const Drawable *Drawable)
{
    if(m_doWorldTransform){
	    toD3DXMATRIX(m_worldMatD3D, Drawable->getMatrix());	    
    }
    else{
        D3DXMatrixIdentity(&m_worldMatD3D);
    }
    m_device->SetTransform(D3DTS_WORLD, &m_worldMatD3D);
}



void
D3DDrawer::setAspectRatio(float r)
{
	Drawer::setAspectRatio(r);
	updateProjectionMatrix();
}



void
D3DDrawer::setNearClipPlane(float d)
{
	Drawer::setNearClipPlane(d);
	updateProjectionMatrix();
}



void
D3DDrawer::setFarClipPlane(float d)
{
	Drawer::setFarClipPlane(d);
	updateProjectionMatrix();
}


void
D3DDrawer::setFocalLength(float f)
{
    Drawer::setFocalLength(f);
    updateProjectionMatrix();
}



void
D3DDrawer::begin(const Color4F &clearColor) const
{
	D3DXCOLOR clearColorD3D;
	toD3DXCOLOR(clearColorD3D, clearColor);    
	m_device->Clear(0, 
                    NULL, 
                    D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
                    clearColorD3D, 
                    1.0f, 
                    0);
	m_device->BeginScene();
}



void
D3DDrawer::end() const
{
	m_device->EndScene();
	m_device->Present(0, 0, 0, 0);	
}



void
D3DDrawer::drawQuad(const Quad *quad)
{
    
    HRESULT hr;    

    if(m_wireframe)
        m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    else 
        m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);   

	updateWorldMatrix( quad );   

    Vertex::XYZCUV v = quad->getVertices()[4];

    // projective texturing
    D3DXMATRIX textureMatrix = m_projMatD3D;    
    Matrix camMat = m_viewMatrix;
    Matrix quadMat = quad->getViewMatrix();
    Matrix camI;
    Matrix trans; Matrix::TranslationMatrix(trans, 0.5f, -0.5f, 0.0f);
    Matrix scale; Matrix::ScaleMatrix(scale, 0.5f, 0.5f, 1.0f);
    
    ASSERT(camMat.inverse(camI)); // should definitely be invertible
    
    
    Vector4 p1 = quad->getPoint(0) * camMat;
    Vector4 p2 = quad->getPoint(1) * camMat;
    Vector4 p3 = quad->getPoint(2) * camMat;
    Vector4 p4 = quad->getPoint(3) * camMat;
    
    // camera space --> camI --> world space
    // world space --> quadMat --> projector space
    // projector space --> quad->getProjectionMatrix() --> screen space
    // screen space --> scale --> (-.5, .5) 
    // (-.5, .5) --> trans --> (0, 1) (desired texture coordinate range)
    Matrix textureMat = camI * quadMat * quad->getProjectionMatrix() * scale * trans;

    hr = m_device->SetTransform(D3DTS_TEXTURE0, &toD3DXMATRIX(textureMat));
    hr = m_device->SetVertexDeclaration(m_vDecl);
    hr = m_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
								   2,
								   quad->getVertices(),
								   sizeof(Vertex::XYZCUV));

}


void
D3DDrawer::setActiveTexture(string textureFile)
{    
    if(textureFile == ""){
        m_device->SetTexture(0, NULL);
        return;
    }
    
    ASSERT(m_device);

    map<string, LPDIRECT3DTEXTURE9>::iterator it;

    if((it = m_textures.find(textureFile)) != m_textures.end()){
        m_device->SetTexture(0, it->second);
        return;
    }
    
    HRESULT res = D3DXCreateTextureFromFile(m_device, textureFile.c_str(), &m_texture);
    if(SUCCEEDED(res)){
        m_textures[textureFile] = m_texture;
        m_device->SetTexture(0, m_textures[textureFile]);                    
    }
    else{
        //cout << "ERROR: Failed to create texture from file: " << textureFile << endl;        
        m_device->SetTexture(0, NULL);        
    }

}

#endif