#include "Transformation.h"

Transformation::Transformation(void) :
    m_rotation(0.0f, 0.0f, 0.0f)
{
	applyIdentity();
}

Transformation::~Transformation(void)
{
}



void
Transformation::applyIdentity()
{
	Matrix::IdentityMatrix(m_translate);
	Matrix::IdentityMatrix(m_rotate);
	Matrix::IdentityMatrix(m_scale);
	Matrix::IdentityMatrix(m_composite);	
}



void 
Transformation::applyAbsoluteTranslation(float x, float y, float z)
{
    Matrix::TranslationMatrix( m_translate, x, y, z );
	m_composite = m_scale * m_rotate * m_translate;
}



void
Transformation::applyRelativeTranslation(float x, float y, float z)
{
	m_translate(3, 0) += x;
	m_translate(3, 1) += y;
	m_translate(3, 2) += z;
	m_composite = m_scale * m_rotate * m_translate;
}



void
Transformation::applyAbsoluteTranslation(const Vector3 &v)
{
    applyAbsoluteTranslation(v.x, v.y, v.z);	
}



void
Transformation::applyRelativeTranslation(const Vector3 &v)
{
    applyRelativeTranslation(v.x, v.y, v.z);	
}



void
Transformation::applyAbsoluteRotation(float x, float y, float z)
{
    /*
    m_rotation[0] = fmod(x, 6.28f);
    m_rotation[1] = fmod(y, 6.28f);
    m_rotation[2] = fmod(z, 6.28f);
    */
    
    m_rotation[0] = x; 
    m_rotation[1] = y; 
    m_rotation[2] = z; 

    Matrix::RotationMatrixXYZ(m_rotate, m_rotation.x, m_rotation.y, m_rotation.z);
	m_composite = m_scale * m_rotate * m_translate;
}



void
Transformation::applyRelativeRotation(float x, float y, float z)
{   
    m_rotation[0] += x; 
    m_rotation[1] += y; 
    m_rotation[2] += z; 

    Matrix::RotationMatrixXYZ(m_rotate, m_rotation.x, m_rotation.y, m_rotation.z);
	m_composite = m_scale * m_rotate * m_translate;
}



void
Transformation::applyAbsoluteRotation(const Vector3 &v)
{
    applyAbsoluteRotation(v.x, v.y, v.z);
	m_composite = m_scale * m_rotate * m_translate;
}



void
Transformation::applyRelativeRotation(const Vector3 &v)
{
    applyRelativeRotation(v.x, v.y, v.z);
	m_composite = m_scale * m_rotate * m_translate;
}



void
Transformation::applyAbsoluteScale(float x, float y, float z)
{
	Matrix::ScaleMatrix(m_scale, x, y, z);
	m_composite = m_scale * m_rotate * m_translate;
}



void
Transformation::applyRelativeScale(float x, float y, float z)
{
	m_scale(0, 0) *= x;
	m_scale(1, 1) *= y;
	m_scale(2, 2) *= z;
	m_composite = m_scale * m_rotate * m_translate;
}



void
Transformation::applyAbsoluteScale(const Vector3 &v)
{
    applyAbsoluteScale(v.x, v.y, v.z);
	m_composite = m_scale * m_rotate * m_translate;
}



void
Transformation::applyRelativeScale(const Vector3 &v)
{
    applyRelativeScale(v.x , v.y, v.z);
	m_composite = m_scale * m_rotate * m_translate;
}



const Matrix&
Transformation::getMatrix() const
{
	//get rid of this when done debugging
	//m_composite = m_scale * m_rotate * m_translate;	
	return (const Matrix&) m_composite;
}


void
Transformation::getPosition(Vector3 &out) const
{
    out.x = m_translate(3, 0);
    out.y = m_translate(3, 1);
    out.z = m_translate(3, 2);
}


void
Transformation::getRotation(Vector3 &out) const
{
    Vector3::copyVector3(out, m_rotation);
}


void
Transformation::getScale(Vector3 &out) const
{
    out.x = m_scale(0, 0);
    out.y = m_scale(1, 1);
    out.z = m_scale(2, 2);
}