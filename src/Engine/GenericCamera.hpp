/*
 *  GenericCamera.hpp
 *  Phonosynth
 *
 *  Created by Greg on 10/27/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */


inline
GenericCamera::GenericCamera() :
    m_drawer(NULL),
    m_aspectRatio(1.0f),
    m_focalLength(1.0f),
    m_zNear(0.1f),
    m_zFar(10000.0f),
    m_position(0.0f, 0.0f, 0.0f, 1.0f),
    m_arcballCenter(0.0f, 0.0f, 0.0f, 1.0f),
    m_arcballRadius(0.0f)
{
	
}


inline
GenericCamera::~GenericCamera(void)
{
    
}

// basic movement controls

inline void 
GenericCamera::translate(const Vector3 &dir){ 	
    setPosition(m_position + dir);
}

inline void
GenericCamera::moveForward(float amt)
{ 
    static Vector3 look; 
	m_viewMatrix.getColumnVector(look, 2);
    translate((amt) * look); // negative of look is stored in view matrix
}

inline void
GenericCamera::moveBackward(float amt)
{ 
    static Vector3 look; 
	m_viewMatrix.getColumnVector(look, 2);
    translate(-amt * look);
}

inline void
GenericCamera::moveLeft(float amt)
{ 
    static Vector3 side; 
	m_viewMatrix.getColumnVector(side, 0);
    translate(-amt * side);
}

inline void
GenericCamera::moveRight(float amt)
{ 
    static Vector3 side; 
	m_viewMatrix.getColumnVector(side, 0);
    translate(amt * side);
}

inline void
GenericCamera::moveUp(float amt)
{ 
    static Vector3 up;
    m_viewMatrix.getColumnVector(up, 1);
    translate(amt * up);
}

inline void
GenericCamera::moveDown(float amt)
{ 
    static Vector3 up;
    m_viewMatrix.getColumnVector(up, 1);
    translate((-amt) * up);
}


inline void
GenericCamera::rotateLeft(float amt)
{
    //	ENGINE_PRINTF("rotateLeft(%f)\n", amt);
	rotateLeft(m_viewMatrix, m_viewMatrix, amt, m_position);
}


inline void
GenericCamera::rotateRight(float amt)
{
    //	ENGINE_PRINTF("rotateRight(%f)\n", amt);
	rotateRight(m_viewMatrix, m_viewMatrix, amt, m_position);
}


inline void
GenericCamera::rotateUp(float amt)
{
    //	ENGINE_PRINTF("rotateUp(%f)\n", amt);
	rotateUp(m_viewMatrix, m_viewMatrix, amt, m_position);	
}


inline void
GenericCamera::rotateDown(float amt)
{
    //	ENGINE_PRINTF("rotateDown(%f)\n", amt);
	rotateDown(m_viewMatrix, m_viewMatrix, amt, m_position);
}


inline void
GenericCamera::rotateLeft(Matrix &newViewMat, const Matrix &oldViewMat, float amt, const Point3& p)
{	
	static Vector3 up;    oldViewMat.getColumnVector(up, 1);         
    static Matrix rot;
	
    Matrix::RotationMatrix(rot, amt, Vector3(0.0f, 0.0f, 1.0f));    	
    Matrix::upper3x3Mult(newViewMat, rot, oldViewMat);
	
    newViewMat.setRowVector(3, -(p.xyz() * newViewMat));
}


inline void
GenericCamera::rotateRight(Matrix &newViewMat, const Matrix &oldViewMat, float amt, const Point3& p)
{
	static Vector3 up;    oldViewMat.getColumnVector(up, 1);         
    static Matrix rot;
	
    Matrix::RotationMatrix(rot, -amt, Vector3(0.0f, 0.0f, 1.0f));    	
    Matrix::upper3x3Mult(newViewMat, rot, oldViewMat);
	
    newViewMat.setRowVector(3, -(p.xyz() * newViewMat));
}


inline void
GenericCamera::rotateUp(Matrix &newViewMat, const Matrix &oldViewMat, float amt, const Point3& p)
{
	static Vector3 side;    oldViewMat.getColumnVector(side, 0);         
    static Matrix rot;
	
    Matrix::RotationMatrix(rot, amt, side);    	
    Matrix::upper3x3Mult(newViewMat, rot, oldViewMat);	
    
    newViewMat.setRowVector(3, -(p.xyz() * newViewMat));
	
}



inline void
GenericCamera::rotateDown(Matrix &newViewMat, const Matrix &oldViewMat, float amt, const Point3& p)
{
	static Vector3 side;    oldViewMat.getColumnVector(side, 0);
    static Matrix rot;
    
    Matrix::RotationMatrix(rot, -amt, side);
    Matrix::upper3x3Mult(newViewMat, rot, oldViewMat);

    newViewMat.setRowVector(3, -(p.xyz() * newViewMat));
}


inline void
GenericCamera::rotateLeftArcball(float amt)
{

}


inline void
GenericCamera::rotateRightArcball(float amt)
{
	
}


inline void
GenericCamera::rotateUpArcball(float amt)
{

}


inline void
GenericCamera::rotateDownArcball(float amt)
{
    
}


inline void
GenericCamera::moveForwardArcball(float amt)
{
    
}


inline void
GenericCamera::moveBackwardArcball(float amt)
{
	
}


inline void
GenericCamera::moveLeftArcball(float amt)
{
    
}


inline void
GenericCamera::moveRightArcball(float amt)
{
	
}


inline void
GenericCamera::moveUpArcball(float amt)
{
    
}


inline void
GenericCamera::moveDownArcball(float amt)
{
    
}


inline void
GenericCamera::rollLeft(float amt)
{
	static Vector3 side;    m_viewMatrix.getColumnVector(side, 0);
    static Vector3 up;      m_viewMatrix.getColumnVector(up, 1);
    static Vector3 look;    m_viewMatrix.getColumnVector(look, 2);         
    static Matrix rot;
    static Point3 campos;
    
    Matrix::RotationMatrix(rot, amt, look);    	
    Matrix::upper3x3Mult(m_viewMatrix, rot, m_viewMatrix);
    
    campos = (m_position) * m_viewMatrix;
    m_viewMatrix.setRowVector(3, -campos.xyz());
}


inline void
GenericCamera::rollRight(float amt)
{
	static Vector3 side;    m_viewMatrix.getColumnVector(side, 0);
    static Vector3 up;      m_viewMatrix.getColumnVector(up, 1);
    static Vector3 look;    m_viewMatrix.getColumnVector(look, 2);          
    static Matrix rot;
    static Point3 campos;
    
    Matrix::RotationMatrix(rot, -amt, look);    	
    Matrix::upper3x3Mult(m_viewMatrix, rot, m_viewMatrix);
    
    campos = (m_position) * m_viewMatrix;
    m_viewMatrix.setRowVector(3, -campos.xyz());
}


inline void
GenericCamera::zoomIn(float amt)
{
    ASSERT(amt >= 0.0f && amt <= 100.0f);
	setLens(m_aspectRatio, m_focalLength * (1.0f + amt), m_zNear, m_zFar, PIOVER2);	
}


inline void
GenericCamera::zoomOut(float amt)
{
    ASSERT(amt >= 0.0f && amt <= 1.0f);
	setLens(m_aspectRatio, m_focalLength * (1.0f - amt), m_zNear, m_zFar, PIOVER2);
}


// arcball rotation

inline void
GenericCamera::setOrientation(const Point3 &pos,
                              const Quaternion &rot,
                              bool adjustUpVector)
{    
    setRotation(rot);
    setPosition(pos);
}


inline void
GenericCamera::setRotation(const Quaternion &q, bool adjustUpVector)
{
    m_quaternion = q;
    q.toMatrix(m_viewMatrix);
    Quaternion q2 = m_viewMatrix.toQuaternion();

    setPosition(m_position);
}


inline void
GenericCamera::setOrientation(const Point3 &pos, const Vector3 &look, const Vector3 &up)
{	
    m_position = pos;
	Matrix::ViewMatrix(m_viewMatrix, pos.xyz(), look, up);
}


inline void
GenericCamera::setPosition(const Point3 &pos)
{
    // //cout << "matrix before = " << endl << m_viewMatrix << endl;
	m_position = pos;
    Point3 camPos;
    
    Vector3 side, up, look;
    m_viewMatrix.getColumnVector(side, 0);
    m_viewMatrix.getColumnVector(up, 1);
    m_viewMatrix.getColumnVector(look, 2);
    
    m_viewMatrix(3,0) = -(m_position.xyz().dot(side));
    m_viewMatrix(3,1) = -(m_position.xyz().dot(up));
    m_viewMatrix(3,2) = -(m_position.xyz().dot(look));
}


inline void
GenericCamera::setArcballRadius(float r)
{
    m_arcballRadius = r;
}


inline void
GenericCamera::setArcballCenter(const Point3& p)
{
    m_arcballCenter = p;
}


