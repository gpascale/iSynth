#include <iomanip>

inline
float&
Matrix::operator () (int row, int col)
{
    return (float&) m_data[row][col];
}


inline
const float&
Matrix::operator () (int row, int col) const
{
    return (const float&) m_data[row][col];
}


inline
Matrix
Matrix::operator + (const Matrix &m) const
{
    Matrix ret;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            ret(i, j) = m_data[i][j] + m(i, j);
    return ret;
}


inline
Matrix&
Matrix::operator = (const Matrix &m)
{
	copyMatrix(*this, m);
	return *this;
}


inline
Matrix
Matrix::operator * (const Matrix &m) const
{       
    Matrix ret;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){            
            float &c = (ret(i, j) = 0.0f);
            for(int k = 0; k < 4; k++)
                c += m_data[i][k] * m(k, j);
        }
    }
    return ret;
}


inline
Matrix
Matrix::operator*(float f) const
{
    Matrix ret;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            ret(i, j) = m_data[i][j] * f;
    return ret;
}


inline
Vector3
Matrix::operator*(const Vector3 &v) const
{
    Vector3 ret(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 3; i++){
        float &c = ret[i];
        for(int j = 0; j < 3; j++)
            c += v[j] * m_data[i][j];
    }
    return ret;
}


inline
Vector4
Matrix::operator*(const Vector4 &v) const
{
    Vector4 ret(0.0f, 0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; i++){
        float &c = ret[i];
        for(int j = 0; j < 4; j++)
            c += v[j] * m_data[i][j];
    }
    return ret;
}


inline
Matrix&
Matrix::operator*=(const float f)
{
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            m_data[i][j] *= f;
    return (*this);
}


inline
Matrix&
Matrix::operator*=(const Matrix& m)
{

	Matrix ret = operator * (m);
	memcpy(&A, ret.getConstDataPointer(), 16 * sizeof(float));
	return (Matrix&) *this;
}


inline
Matrix&
Matrix::operator+=(const Matrix &m)
{
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            m_data[i][j] += m(i, j);
    return (*this);
}

inline
Matrix
Matrix::transpose() const
{        
    Matrix ret;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            ret(i, j) = m_data[j][i];
    return ret;
}


inline
bool
Matrix::inverse(Matrix &ret) const
{
    float det = D*G*J*M - C*H*J*M - D*F*K*M + B*H*K*M + 
                C*F*L*M - B*G*L*M + D*E*K*N - A*H*K*N - 
                C*E*L*N + A*G*L*N - D*E*J*O + A*H*J*O + 
                B*E*L*O - A*F*L*O + C*E*J*P - A*G*J*P - 
                B*E*K*P + A*F*K*P - D*G*N*I + C*H*N*I + 
                D*F*O*I - B*H*O*I - C*F*P*I + B*G*P*I;

    if (FUZZY_EQ0(det)) return false;

    float detI = 1.0f / det;
    ret(0, 0) = detI * (-H*K*N + G*L*N + H*J*O - F*L*O - G*J*P + F*K*P);
    ret(0, 1) = detI * ( D*K*N - C*L*N - D*J*O + B*L*O + C*J*P - B*K*P);
    ret(0, 2) = detI * (-D*G*N + C*H*N + D*F*O - B*H*O - C*F*P + B*G*P);
    ret(0, 3) = detI * ( D*G*J - C*H*J - D*F*K + B*H*K + C*F*L - B*G*L);
    ret(1, 0) = detI * ( H*K*M - G*L*M + E*L*O - E*K*P - H*O*I + G*P*I);
    ret(1, 1) = detI * (-D*K*M + C*L*M - A*L*O + A*K*P + D*O*I - C*P*I);
    ret(1, 2) = detI * ( D*G*M - C*H*M - D*E*O + A*H*O + C*E*P - A*G*P);
    ret(1, 3) = detI * ( D*E*K - A*H*K - C*E*L + A*G*L - D*G*I + C*H*I);
    ret(2, 0) = detI * (-H*J*M + F*L*M - E*L*N + E*J*P + H*N*I - F*P*I);
    ret(2, 1) = detI * ( D*J*M - B*L*M + A*L*N - A*J*P - D*N*I + B*P*I);
    ret(2, 2) = detI * (-D*F*M + B*H*M + D*E*N - A*H*N - B*E*P + A*F*P);
    ret(2, 3) = detI * (-D*E*J + A*H*J + B*E*L - A*F*L + D*F*I - B*H*I);
    ret(3, 0) = detI * ( G*J*M - F*K*M + E*K*N - E*J*O - G*N*I + F*O*I);
    ret(3, 1) = detI * (-C*J*M + B*K*M - A*K*N + A*J*O + C*N*I - B*O*I);
    ret(3, 2) = detI * ( C*F*M - B*G*M - C*E*N + A*G*N + B*E*O - A*F*O);
    ret(3, 3) = detI * ( C*E*J - A*G*J - B*E*K + A*F*K - C*F*I + B*G*I);
       
    return true;
}


inline
Matrix
Matrix::inverse() const
{
	Matrix ret;
	ASSERT(inverse(ret));
	return ret;
}


inline
Vector3
operator*(const Vector3 &v, const Matrix &m)
{
    Vector3 ret(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 3; i++){
        float &c = ret[i];
        for(int j = 0; j < 3; j++)
            c += v[j] * m(j, i);
    }
    return ret;
}


inline
Vector4
operator*(const Vector4 &v, const Matrix &m)
{
    Vector4 ret(0.0f, 0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; i++){
        float &c = ret[i];
        for(int j = 0; j < 4; j++)
            c += v[j] * m(j, i);
    }
    return ret;
}


inline
Matrix
operator*(const float f, const Matrix &m)
{
    Matrix ret;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            ret(i, j) = f * m(i, j);
    return ret;
}


inline
Matrix
operator-(Matrix &m)
{
	Matrix ret(m);
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            ret(i, j) *= -1;
	return ret;
}


inline
ostream&
operator << (ostream &os, const Matrix &m)
{
    os << setiosflags(ios::fixed) << setprecision(2);
    for(int i = 0; i < 4; i++){        
        os << "( ";
        os << m(i, 0) << ", " << m(i, 1) << ", " << m(i, 2) << ", " << m(i, 3) << " )" << endl;
    }
    return os;
}


inline
void
Matrix::setRowVector(int row, const Vector4 &v)
{
	memcpy(&m_data[row][0], v.getConstDataPointer(), 4 * sizeof(float));
}


inline
void
Matrix::setColumnVector(int column, const Vector4 &v)
{
	m_data[0][column] = v[0];
	m_data[1][column] = v[1];
	m_data[2][column] = v[2];
	m_data[3][column] = v[3];
}



// these guys set the first 3 elements of the row/column
// leaving the 4th untouched. useful especially for creating
// a view matrix

inline
void
Matrix::setRowVector(int row, const Vector3 &v)
{
	memcpy(&m_data[row][0], v.getConstDataPointer(), 3 * sizeof(float));
}


inline
void
Matrix::setColumnVector(int column, const Vector3 &v)
{
	m_data[0][column] = v[0];
	m_data[1][column] = v[1];
	m_data[2][column] = v[2];
}


inline 
void
Matrix::getRowVector(Vector4 &out, int row) const
{
	assert(row >= 0 && row <= 3);
	out.setData((const float*) &(m_data[row]));
}


inline
void
Matrix::getRowVector(Vector3 &out, int row) const
{
	assert(row >= 0 && row <= 3);
	out.setData((const float*) &(m_data[row]));
}


inline
void
Matrix::getColumnVector(Vector4 &out, int col) const
{
	out[0] = m_data[0][col];
	out[1] = m_data[1][col];
	out[2] = m_data[2][col];
	out[3] = m_data[3][col];
}


inline
void
Matrix::getColumnVector(Vector3 &out, int col) const
{
	out[0] = m_data[0][col];
	out[1] = m_data[1][col];
	out[2] = m_data[2][col];
}



inline
Vector3
Matrix::getRowVector3(int col) const
{
	Vector3 ret;
	getRowVector(ret, col);
	return ret;
}


inline
Vector4
Matrix::getRowVector4(int col) const
{
	Vector4 ret;
	getRowVector(ret, col);
	return ret;
}



inline
Vector3
Matrix::getColumnVector3(int col) const
{
	Vector3 ret;
	getColumnVector(ret, col);
	return ret;
}


inline
Vector4
Matrix::getColumnVector4(int col) const
{
	Vector4 ret;
	getColumnVector(ret, col);
	return ret;
}



inline
void
Matrix::copyMatrix(Matrix &to, const Matrix &from)
{
	memcpy(to.getDataPointer(), from.getConstDataPointer(), 16 * sizeof(float));
}


inline
void
Matrix::upper3x3Mult(Matrix &ret, const Matrix &a, const Matrix &b)
{   
    // deal with the case where one of the input args is also the output arg.
    // in that case, make a copy to use for calculation
    const Matrix &A = (&ret == &a) ? Matrix(a) : a;
    const Matrix &B = (&ret == &b) ? Matrix(b) : b;   
        
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){            
            ret(i, j) = 0.0f;
            for(int k = 0; k < 3; k++)
                ret(i, j) += A(i, k) * B(k, j);
        }
    }       
}


inline
void
Matrix::copyUpper3x3(Matrix &to, const Matrix &from)
{
    memcpy(&to(0, 0), &from(0, 0), 3 * sizeof(float));
    memcpy(&to(1, 0), &from(1, 0), 3 * sizeof(float));
    memcpy(&to(2, 0), &from(2, 0), 3 * sizeof(float));
}


// special matrices


inline
void
Matrix::ZeroMatrix(OUT Matrix &ret)
{
    ret.zero();
}


inline
void
Matrix::IdentityMatrix(OUT Matrix &ret)
{    
    ret.zero();
    ret(0, 0) = ret(1, 1) = ret(2, 2) = ret(3, 3) = 1.0f;
}


inline
void
Matrix::ScaleMatrix(OUT Matrix &ret, float x, float y, float z)
{
    ret.zero();
    ret(0, 0) = x;
    ret(1, 1) = y;
    ret(2, 2) = z;
    ret(3, 3) = 1.0f;
}


inline
void
Matrix::TranslationMatrix(OUT Matrix& ret, const Vector3& v)
{
	IdentityMatrix(ret);
	ret(3, 0) = v.x;
	ret(3, 1) = v.y;
	ret(3, 2) = v.z;
}
	

inline
void
Matrix::TranslationMatrix(OUT Matrix &ret, float x, float y, float z)
{
    IdentityMatrix(ret);
    ret(3, 0) = x;
    ret(3, 1) = y;
    ret(3, 2) = z;    
}


inline
void
Matrix::RotationMatrixX(OUT Matrix &ret, float amt)
{
    IdentityMatrix(ret);
    ret(1, 1) = ret(2, 2) = cos(amt);
    ret(1, 2) = -(ret(2, 1) = sin(amt));
}

inline
void
Matrix::RotationMatrixY(OUT Matrix &ret, float amt)
{
    IdentityMatrix(ret);
    ret(0, 0) = ret(2, 2) = cos(amt);
    ret(0, 2) = -(ret(2, 0) = sin(amt));
}


inline
void
Matrix::RotationMatrixZ(OUT Matrix &ret, float amt)
{
    IdentityMatrix(ret);
    ret(0, 0) = ret(1, 1) = cos(amt);
    ret(0, 1) = -(ret(1, 0) = sin(amt));
}


inline
void
Matrix::RotationMatrixXYZ(OUT Matrix &ret, float x, float y, float z)
{
	IdentityMatrix(ret);
	Matrix cur;
	RotationMatrixX(cur, x); 
	ret *= cur;
	RotationMatrixY(cur, y);
	ret *= cur;
	RotationMatrixZ(cur, z);
	ret *= cur;
}


inline
const Matrix&
Matrix::IdentityCRef()
{
    // don't rely on the fact that default constructor creates an Identity matrix, in case that changes
    static const Matrix iden(1.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 1.0f);
    return iden;
}


inline
const Matrix&
Matrix::ZeroCRef()
{
    static const Matrix iden(0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f);
    return iden;
}


// constructs a rotation matrix around the axis (x, y, z)
inline
void
Matrix::RotationMatrix(OUT Matrix &ret, float amt, float x, float y, float z)
{
    Matrix iden;
	IdentityMatrix(iden);
	Matrix S(0.0f, -z,  y, 0.0f,
			  z, 0.0f, -x, 0.0f,
			 -y,  x, 0.0f, 0.0f,
			 0.0f, 0.0f, 0.0f, 0.0f);
	
	ret = iden + (sin(amt) * S) + ((1-cos(amt)) * (S*S));
}


inline
void
Matrix::RotationMatrix(OUT Matrix &ret, float amt, const Vector3 &axis)
{
	RotationMatrix(ret, amt, axis.x, axis.y, axis.z);
}




