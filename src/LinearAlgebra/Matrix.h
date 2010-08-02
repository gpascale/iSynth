#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <common.h>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

class Quaternion;

class Matrix
{
public:    

    // constructors
    Matrix();
    Matrix(const Matrix &m);
    Matrix(float a, float b, float c, float d,
           float e, float f, float g, float h,
           float i, float j, float k, float l,
           float m, float n, float o, float p);
    Matrix(const float *data);
    Matrix(const Vector4 &v1, const Vector4 &v2, 
           const Vector4 &v3, const Vector4 &v4, bool useAsRowVectors);

    ~Matrix(){ /* everything's on the stack */ }

    // access
    float&          operator () (int row, int col);
    const float&    operator () (int row, int col) const;    
    
    // cast operators
    operator const float* () const { return (const float*) &A; }
    operator float*       ()       { return (      float*) &A; }
    
    // operators
    Matrix          operator + (const Matrix &m) const;
    Matrix          operator * (const Matrix &m) const;
    Matrix          operator * (float f) const;    
    Vector3         operator * (const Vector3 &v) const;
    Vector4         operator * (const Vector4 &v) const;
    Matrix&         operator *= (const float f);
    Matrix&         operator *= (const Matrix &m);
    Matrix&         operator += (const Matrix &m);	
	Matrix&			operator =  (const Matrix &m);

    // random properties
    bool            isOrthogonal()  const;
    float           trace()         const;
    
    // other math        
    void    setRowVectors(const Vector4 &v1, 
                          const Vector4 &v2,
                          const Vector4 &v3,
                          const Vector4 &v4);
    void    setColumnVectors(const Vector4 &v1, 
                             const Vector4 &v2,
                             const Vector4 &v3,
                             const Vector4 &v4);
	void	setColumnVector(int column,
							const Vector4 &v);
	void	setRowVector(int row, 
						 const Vector4 &v);
	void	setColumnVector(int column,
							const Vector3 &v);
	void	setRowVector(int row, 
						 const Vector3 &v);

    static void		  copyMatrix(Matrix &to, const Matrix &from);
    static void       upper3x3Mult(Matrix &ret, const Matrix &A, const Matrix &B);
    static void       copyUpper3x3(Matrix &to, const Matrix &from);

    Quaternion        toQuaternion() const;

    // some useful matrices

    static void		  ViewMatrix(Matrix &ret,
								 const Vector3 &eye,
					  			 const Vector3 &target,
								 const Vector3 &up);

    static void       ViewMatrix(Matrix &ret,
                                 const Vector3 &pos,
                                 const Quaternion &rot);
	
	static void		  ViewMatrixEuler(Matrix &ret, 
									  const Vector3 &pos, 
									  const Vector3 &rots);

    static void       ProjectionMatrixPerspRH(Matrix &ret, 
                                              float fovy, 
                                              float aspect, 
                                              float zNear,
                                              float zFar);
    
    static void       ProjectionMatrixOrthoRH(Matrix& ret,
                                              float xLeft, float xRight,
                                              float yTop, float yBottom,
                                              float zNear, float zFar);
                                            
		
    static void       ZeroMatrix(OUT Matrix &ret);
    static void       IdentityMatrix(OUT Matrix &ret);
    static void       ScaleMatrix(OUT Matrix &ret, Vector3 &v);
    static void       ScaleMatrix(OUT Matrix &ret, float xAmt, float yAmt, float zAmt);
    static void       TranslationMatrix(OUT Matrix &ret, const Vector3 &v);
    static void       TranslationMatrix(OUT Matrix &ret, float xAmt, float yAmt, float zAmt);
    static void       RotationMatrixX(OUT Matrix &ret, float amt);
    static void       RotationMatrixY(OUT Matrix &ret, float amt);
    static void       RotationMatrixZ(OUT Matrix &ret, float amt);
	static void		  RotationMatrixXYZ(OUT Matrix &ret, float x, float y, float z);
	
	static bool       ProjectionMatrix(OUT Matrix& ret, 
                                       const Vector4& p1, const Vector4& p2, const Vector4& p3, const Vector4& p4,
                                       const Vector4& q1, const Vector4& q2, const Vector4& q3, const Vector4& q4);
    
    static bool       AffineMatrix(OUT Matrix& ret, 
                                   const Vector3& p1, const Vector3& p2, const Vector3& p3,
                                   const Vector3& q1, const Vector3& q2, const Vector3& q3);
    
	static bool		  InferAffineTransform(OUT Matrix& ret,
										   const Vector3& p1, const Quaternion& rot1,
										   const Vector3& p2, const Quaternion& rot2);
	
	static bool		  InferAffineTransform(OUT Matrix& ret, const Matrix& m1, const Matrix& m2);
	
    // rotate amt around axis
	static void       RotationMatrix(Matrix &ret, float amt, const Vector3 &axis);
    static void       RotationMatrix(Matrix &ret, float amt, float x, float y, float z);
    

    float             determinant() const;
    Matrix            transpose() const;    
	
    bool              inverse(Matrix &ret) const;
	Matrix			  inverse()			   const;
    
    
    // one way this code could be made faster is to have the linear 
    // algebra constructs use this function and then utilize memcpy 
    // for interacting with eachother, rather than looping and assigning
    float*            getDataPointer(){ return (float*) m_data; }
	const float*	  getConstDataPointer() const { return (const float*) m_data; };
    void              zero(){ memset(m_data, 0x00, sizeof(m_data)); }
	
	
	Vector3			  getRowVector3(int row) const;
	Vector4			  getRowVector4(int row) const;
	Vector3			  getColumnVector3(int col) const;
	Vector4			  getColumnVector4(int col) const;
	
	void			  getRowVector(Vector4 &v, int row) const;
	void			  getRowVector(Vector3 &v, int row) const;
	void			  getColumnVector(Vector4 &v, int col) const;
	void			  getColumnVector(Vector3 &v, int col) const;
	
    void              toGLMatrix(OUT float glMat[16], bool switchHandeness = false) const;
    
    // references to statically allocated common matrices. Saves constructor calls
    // if read access is all that is needed
    static const Matrix&     IdentityCRef();
    static const Matrix&     ZeroCRef();

private:    

    // actual matrix data
    union{
        float      m_data[4][4];    
        struct{
            float  A,B,C,D,
                   E,F,G,H,
                   I,J,K,L,
                   M,N,O,P;
        };
    };

};

// right-hand operators
Vector3      operator*(const Vector3 &v, const Matrix &m);
Vector4      operator*(const Vector4 &v, const Matrix &m);
Matrix       operator*(const float f, const Matrix &m);
Matrix       operator-(Matrix &m);
ostream&     operator<<(ostream &os, const Matrix &m);

#include "Matrix.hpp"

#endif
