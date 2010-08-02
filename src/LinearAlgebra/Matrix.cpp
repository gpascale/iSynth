
#include "Matrix.h"
#include "Quaternion.h"


// basic constructor creates an Identity matrix. I think this makes more sense than a zero matrix
Matrix::Matrix() :
    A(1.0f), B(0.0f), C(0.0f), D(0.0f),
    E(0.0f), F(1.0f), G(0.0f), H(0.0f),
    I(0.0f), J(0.0f), K(1.0f), L(0.0f),
    M(0.0f), N(0.0f), O(0.0f), P(1.0f)
{
}


Matrix::Matrix(const float* data)
{
    memcpy(m_data, data, 16 * sizeof(float));
}


Matrix::Matrix(float a, float b, float c, float d,
               float e, float f, float g, float h,
               float i, float j, float k, float l,
               float m, float n, float o, float p)
{    
    A = a; B = b; C = c; D = d; 
    E = e; F = f; G = g; H = h; 
    I = i; J = j; K = k; L = l; 
    M = m; N = n; O = o; P = p;
}


Matrix::Matrix(const Matrix &m)
{    
	copyMatrix(*this, m);
}


Matrix::Matrix(const Vector4 &v1, const Vector4 &v2, 
               const Vector4 &v3, const Vector4 &v4, bool useAsRowVectors)
{
    if(useAsRowVectors)
        setRowVectors(v1, v2, v3, v4);
    else
        setColumnVectors(v1, v2, v3, v4);
}


void
Matrix::setRowVectors(const Vector4 &v1, 
                      const Vector4 &v2,
                      const Vector4 &v3,
                      const Vector4 &v4)
{    
    for(int i = 0; i < 4; i++){
        m_data[0][i] = v1[i];
        m_data[1][i] = v2[i];
        m_data[2][i] = v3[i];
        m_data[3][i] = v4[i];
    } 
}


void
Matrix::setColumnVectors(const Vector4 &v1, 
                         const Vector4 &v2,
                         const Vector4 &v3,
                         const Vector4 &v4)
{    
    for(int i = 0; i < 4; i++){
        m_data[i][0] = v1[i];
        m_data[i][1] = v2[i];
        m_data[i][2] = v3[i];
        m_data[i][3] = v4[i];
    } 
}



Quaternion
Matrix::toQuaternion() const
{
    Quaternion ret;    
	
    const float  &m00=m_data[0][0], &m01=m_data[0][1], &m02=m_data[0][2],
                 &m10=m_data[1][0], &m11=m_data[1][1], &m12=m_data[1][2],
                 &m20=m_data[2][0], &m21=m_data[2][1], &m22=m_data[2][2];
	
    const double tr = m00 + m11 + m22 + 1.; //m4.trace();
	
    if (tr>1e-8) {
        float s = 0.5f / (float) sqrt(tr);
        ret.w = 0.25f / s;
        ret.x = (m21 - m12) * s;
        ret.y = (m02 - m20) * s;
        ret.z = (m10 - m01) * s;
    } 
    else {
        if (m00 > m11 && m00 > m22 ) {
            float s = sqrt(1.0 + m00 - m11 - m22);
            ret.x = 0.5f * s;
            s = 1.0f / (s+s);
            ret.y = (m01 + m10) * s;
            ret.z = (m02 + m20) * s;
            ret.w = (m21 - m12) * s; // negative
        }
        else if (m11 > m22) {
            float s = sqrt(1.0f + m11 - m00 - m22);
            ret.y = 0.5f * s;
            s = 1.0f / (s+s);
            ret.x = (m01 + m10) * s;
            ret.z = (m12 + m21) * s;
            ret.w = (m02 - m20) * s;
        } 
        else {
            float s = sqrt(1.0f + m22 - m00 - m11);
            ret.z = 0.5f * s;
            s = 1.0f / (s+s);
            ret.x = (m02 + m20) * s;
            ret.y = (m12 + m21) * s;
            ret.w = (m10 - m01) * s; // negative
        }
    }

    return ret;
}



// constructs a view matrix given a camera position and rotation
void
Matrix::ViewMatrix(Matrix &ret, const Vector3 &pos, const Quaternion &rot)
{
    IdentityMatrix(ret);
    rot.toMatrix(ret);
	
    Vector3 side, up, look;
    ret.getColumnVector(side, 0);
    ret.getColumnVector(up, 1);
    ret.getColumnVector(look, 2);
    
    ret(3,0) = -pos.dot(side);
    ret(3,1) = -pos.dot(up);
    ret(3,2) = -pos.dot(look);
}



void
Matrix::ViewMatrixEuler(Matrix &ret, const Vector3 &pos, const Vector3 &rots)
{
	RotationMatrixXYZ(ret, pos.x, pos.y, pos.z);
	
	const Vector3 side = ret.getColumnVector3(0);
	const Vector3 up   = ret.getColumnVector3(1);
	const Vector3 look = ret.getColumnVector3(2);
	
	ret(3,0) = -pos.dot(side);
    ret(3,1) = -pos.dot(up);
    ret(3,2) = -pos.dot(look);
}


// builds a perspective projection matrix using the given field of view and aspect ratio
void
Matrix::ProjectionMatrixPerspRH(Matrix &ret, float fovy, float aspect, float zNear, float zFar)
{
    ASSERT(zFar > zNear);
    ASSERT(zNear >= 0.0f);
    
    ZeroMatrix(ret);    
	float f = 1.0f / tan(0.5f * fovy);
    ret(0, 0) = f;
    ret(1, 1) = f * aspect;
    ret(2, 2) = (zFar+zNear) / (zNear - zFar);
    ret(2, 3) = -1.0f;
    ret(3, 2) = 2.0f * zFar * zNear / (zNear - zFar);
 
}


void
Matrix::ProjectionMatrixOrthoRH(Matrix& ret, 
                                float xLeft, float xRight, 
                                float yBottom, float yTop, 
                                float zNear, float zFar)
{
    IdentityMatrix(ret);
    
    ret(0, 0) =  2.0f / (xRight - xLeft);
    ret(1, 1) =  2.0f / (yTop - yBottom);
    ret(2, 2) =  -2.0f / (zFar - zNear);
    
    ret(3, 0) = (xRight + xLeft) / (xLeft - xRight);
    ret(3, 1) = (yTop + yBottom) / (yBottom - yTop);
    ret(3, 2) = (zFar + zNear)   / (zFar - zNear);
}


// constructs a view matrix given a camera basis
void
Matrix::ViewMatrix(Matrix &ret, 
				   const Vector3 &eye,
				   const Vector3 &look,
				   const Vector3 &up)
{
	IdentityMatrix(ret);
    
	// make sure look and up are unit-length
	ASSERT( FUZZY_EQ(look.magnitudeSq(), 1.0f, 1e-4) );
	ASSERT( FUZZY_EQ(up.magnitudeSq(), 1.0f, 1e-4) );
    
	
	//compute the side vector and make sure it is unit-length
	const Vector3 side    = up.cross(look).normalized();
    const Vector3 orthoUp = side.cross(look);
    
	// take the standard basis to the new view
	ret.setColumnVector(0, side);
	ret.setColumnVector(1, orthoUp);
	ret.setColumnVector(2, -look);
    
    
    ret(3,0) = -eye.dot(side);
    ret(3,1) = -eye.dot(orthoUp);
    ret(3,2) = -eye.dot(-look);	
}


void
Matrix::toGLMatrix(OUT float glMat[16], bool switchHandedness) const
{
    memcpy(glMat, (const float*) this, 16 * sizeof(float));
    
    // switch coordinate system-handedness
    if(switchHandedness){
        glMat[2] *= -1.0f;
        glMat[6] *= -1.0f;
        glMat[10] *= -1.0f;
        glMat[14] *= -1.0f;
    }
}


// builds a projection matrix which takes coplanar points p1, p2, p3, p4 to coplanar points q1, q2, q3, q4
// returns false if the either set of points is not colinear or contains 3 colinear points
bool
Matrix::ProjectionMatrix(OUT Matrix& ret, 
                         const Vector4& p1, const Vector4& p2, const Vector4& p3, const Vector4& p4,
                         const Vector4& q1, const Vector4& q2, const Vector4& q3, const Vector4& q4)
{
    Matrix A, B, Ainv;
    
    A.setRowVector(0, p1);
    A.setRowVector(1, p2);
    A.setRowVector(2, p3 - p1 - p2);
    A.setRowVector(3, p4);
    
    //cout << "(1.0f, 0.0f, 0.0f 0.0f) * A = " << Vector4(1.0f, 0.0f, 0.0f, 0.0f) * A << endl;
    //cout << "(0.0f, 1.0f, 0.0f 0.0f) * A = " << Vector4(0.0f, 1.0f, 0.0f, 0.0f) * A << endl;
    //cout << "(1.0f, 1.0f, 1.0f 0.0f) * A = " << Vector4(1.0f, 1.0f, 1.0f, 0.0f) * A << endl;
    //cout << "(0.0f, 0.0f, 0.0f 1.0f) * A = " << Vector4(0.0f, 0.0f, 0.0f, 1.0f) * A << endl;
    
    ASSERT(A.inverse(Ainv));
    
    //cout << "A * Ainv = " << A * Ainv << endl;
    
    //cout << "p1 * Ainv = " << p1 * Ainv << endl;
    //cout << "p2 * Ainv = " << p2 * Ainv << endl;
    //cout << "p3 * Ainv = " << p3 * Ainv << endl;
    //cout << "p4 * Ainv = " << p4 * Ainv << endl;
    
    B.setRowVector(0, q1);
    B.setRowVector(1, q2);
    B.setRowVector(2, q3 - q1 - q2);
    B.setRowVector(3, q4);
    
    //cout << "(1.0f, 0.0f, 0.0f 0.0f) * B = " << Vector4(1.0f, 0.0f, 0.0f, 0.0f) * B << endl;
    //cout << "(0.0f, 1.0f, 0.0f 0.0f) * B = " << Vector4(0.0f, 1.0f, 0.0f, 0.0f) * B << endl;
    //cout << "(1.0f, 1.0f, 1.0f 0.0f) * B = " << Vector4(1.0f, 1.0f, 1.0f, 0.0f) * B << endl;
    //cout << "(0.0f, 0.0f, 0.0f 1.0f) * B = " << Vector4(0.0f, 0.0f, 0.0f, 1.0f) * B << endl;    
    
    ret = Ainv * B;
    
    return true;
}


bool
Matrix::AffineMatrix(OUT Matrix& ret,
                     const Vector3& p1, const Vector3& p2, const Vector3& p3,
                     const Vector3& q1, const Vector3& q2, const Vector3& q3)
{
    Matrix pToOrigin;
    pToOrigin.setRowVector(0, Vector4(p1, 0.0f));
    pToOrigin.setRowVector(1, Vector4(p2, 0.0f));
    pToOrigin.setRowVector(2, Vector4(p3, 0.0f));
    pToOrigin.setRowVector(3, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    
    //cout << "pToOrigin = " << pToOrigin << endl;
    
    if(!pToOrigin.inverse(pToOrigin))
        return false;
    
    ret = pToOrigin;
    
    Matrix originToQ;
    originToQ.setRowVector(0, Vector4(q1, 0.0f));
    originToQ.setRowVector(1, Vector4(q2, 0.0f));
    originToQ.setRowVector(2, Vector4(q3, 0.0f));
    originToQ.setRowVector(3, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    
    ret = pToOrigin * originToQ;
    
    return true;
}


bool
Matrix::InferAffineTransform(OUT Matrix& ret,
							 const Vector3& p1, const Quaternion& q1,
							 const Vector3& p2, const Quaternion& q2)
{					
	// rotation component
	const Matrix r1 = q1.toMatrix();
	const Matrix r2 = q2.toMatrix();
	// r1 * rot = r2  -->  rot = r1^-1 * r2	
	const Matrix rot(r2.transpose() * r1);
	
	// translation component
	Matrix trans;
	Matrix::TranslationMatrix(trans, p2 - p1);
	
	ret = rot * trans;
	
	return true;
}


bool
Matrix::InferAffineTransform(OUT Matrix& ret, const Matrix& m1, const Matrix& m2)
{
	// m1 * ret = m2   -->   m1^-1 * m2 = ret
	
	Matrix m1Inv;
	if(!m1.inverse(m1Inv))
		return false;
	ret = m1Inv * m2;
	return true;
}


bool
Matrix::isOrthogonal() const
{
    if (FUZZY_NEQ0(A*C + E*G + I*K)) return false;
    if (FUZZY_NEQ0(B*C + F*G + J*K)) return false;
    if (FUZZY_NEQ0(A*B + E*F + I*J)) return false;
    return true;
}


float
Matrix::trace() const
{
    return A + F + K + P;
}
