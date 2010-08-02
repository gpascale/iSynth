
#include "Transformation.h"
#include "Matrix.h"
#include "Vector3.h"

#include <ctime>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;

const int NUM_VEC3S = 20;
const int NUM_VEC4S = 20;
const int NUM_MATRICES = 20;

vector<Vector3> randVec3s;
vector<Vector4> randVec4s;
vector<Matrix> randMatrices;
ofstream fout;

void testVector3Addition(int numTests)
{
    //cout << "Beginning test of Vector3 addition ... " << endl;
    fout << "Beginning test of Vector3 addition ... " << endl;
    FOR(test,0,numTests){
        int r1 = rand() % (int) randVec3s.size();
        int r2 = rand() % (int) randVec3s.size();
        Vector3 &v1 = randVec3s[r1];
        Vector3 &v2 = randVec3s[r2];
        fout << v1 << " + " << v2 << " = " << v1 + v2 << endl;
        //cout << v1 << " + " << v2 << " = " << v1 + v2 << endl;
    }
    //cout << "Vector3 addition tests complete" << endl << endl;
    fout << "Vector3 addition tests complete" << endl << endl;
}

void testVector3Subtraction(int numTests)
{
    //cout << "Beginning test of Vector3 addition ... " << endl;
    fout << "Beginning test of Vector3 addition ... " << endl;
    FOR(test,0,numTests){
        int r1 = rand() % (int) randVec3s.size();
        int r2 = rand() % (int) randVec3s.size();
        Vector3 &v1 = randVec3s[r1];
        Vector3 &v2 = randVec3s[r2];
        fout << v1 << " - " << v2 << " = " << v1 - v2 << endl;
        //cout << v1 << " - " << v2 << " = " << v1 - v2 << endl;
    }
    //cout << "Vector3 subtraction tests complete" << endl << endl;
    fout << "Vector3 subtraction tests complete" << endl << endl;
}

void testVector3ScalarMultiplication(int numTests)
{
    //cout << "Beginning test of Vector3 scalar multiplication" << endl;
    fout << "Beginning test of Vector3 scalar multiplication" << endl;
    FOR(test, 0, numTests){
        int r = rand() % (int) randVec3s.size();
        Vector3 &v = randVec3s[r];
        float m = (float) (rand() % 20) - 10;
        if(test < numTests / 2){
            //cout << m << " * " << v << " = " << m * v << endl;
            fout << m << " * " << v << " = " << m * v << endl;
        }
        else{
            //cout << m << " * " << v << " = " << m * v << endl;
            fout << m << " * " << v << " = " << m * v << endl;
        }
    }
    //cout << "Vector3 scalar multiplication tests complete" << endl << endl;
    fout << "Vector3 scalar multiplication tests complete" << endl << endl;    
}


void testVector3DotProduct(int numTests)
{
    //cout << "Beginning test of Vector3 dot product ... " << endl;
    fout << "Beginning test of Vector3 dot prodcut ... " << endl;
    FOR(test,0,numTests){
        int r1 = rand() % (int) randVec3s.size();
        int r2 = rand() % (int) randVec3s.size();
        Vector3 &v1 = randVec3s[r1];
        Vector3 &v2 = randVec3s[r2];
        fout << v1 << " dot " << v2 << " = " << v1.dot(v2) << endl;
        //cout << v1 << " dot " << v2 << " = " << v1.dot(v2) << endl;
    }
    //cout << "Vector3 dot product test complete" << endl << endl;
    fout << "Vector3 dot product test complete" << endl << endl;
}


void testVector3Magnitude(int numTests)
{
    //cout << "Beginning test of Vector3 magnitude ... " << endl;
    fout << "Beginning test of Vector3 magnitude ... " << endl;
    FOR(test,0,numTests){
        int r1 = rand() % (int) randVec3s.size();
        int r2 = rand() % (int) randVec3s.size();
        Vector3 &v1 = randVec3s[r1];        
        fout << "Magnitude(" << v1 << ") = " << v1.magnitude() << endl;
        //cout << "Magnitude(" << v1 << ") = " << v1.magnitude() << endl;
        fout << "MagnitudeSq(" << v1 << ") = " << v1.magnitudeSq() << endl;
        //cout << "MagnitudeSq(" << v1 << ") = " << v1.magnitudeSq() << endl;
    }
    //cout << "Vector3 magnitude test complete" << endl << endl;
    fout << "Vector3 magnitude test complete" << endl << endl;
}


void testVector3Normalize(int numTests)
{
    //cout << "Beginning test of Vector3 normalize ... " << endl;
    fout << "Beginning test of Vector3 normalize ... " << endl;
    FOR(test,0,numTests){
        int r1 = rand() % (int) randVec3s.size();
        int r2 = rand() % (int) randVec3s.size();
        Vector3 &v1 = randVec3s[r1];  
        Vector3 v2 = v1;
        v2.normalize();
        //cout << v1 << " normalized = " << v2 << endl;        
        fout << v1 << " normalized = " << v2 << endl;
    }
    //cout << "Vector3 normalize test complete" << endl << endl;
    fout << "Vector3 normalize test complete" << endl << endl;
}


void
testMatrixAddition(int numTests)
{
    //cout << "Beginning test of Matrix addition ... " << endl;
    fout << "Beginning test of Matrix addition ... " << endl;
    FOR(test, 0, numTests){
        int r1 = rand() % (int) randMatrices.size();
        int r2 = rand() % (int) randMatrices.size();
        Matrix &m1 = randMatrices[r1];
        Matrix &m2 = randMatrices[r2];
        //cout << randMatrices[0] << endl;
        //cout << m1 << " + " << endl << m2 << " = " << endl << m1 + m2 << endl;
        fout << m1 << " + " << endl << m2 << " = " << endl << m1 + m2 << endl;
    }
    //cout << "Matrix addition test complete" << endl;
    fout << "Matrix addition test complete" << endl;
}


void
testMatrixScalarMultiplication(int numTests)
{
    //cout << "Beginning test of Matrix scalar multiplication ... " << endl;
    fout << "Beginning test of Matrix scalar multiplication ... " << endl;
    FOR(test, 0, numTests){
        int r1 = rand() % (int) randMatrices.size();        
        Matrix &m1 = randMatrices[r1];        
        float f = (float) (rand() % 10) - 5;
        if(test < numTests / 2){
            //cout << m1 << " * " << endl << f << " = " << endl << m1 * f << endl;
            fout << m1 << " * " << endl << f << " = " << endl << m1 * f << endl;
        }
        else{
            //cout << f << " * " << endl << m1 << " = " << endl << f * m1 << endl;
            fout << f << " * " << endl << m1 << " = " << endl << f * m1 << endl;
        }
    }
    //cout << "Matrix scalar multiplication test complete" << endl;
    fout << "Matrix scalar multiplication test complete" << endl;
}


void
testMatrixVector3Multiplication(int numTests)
{
    //cout << "Beginning test of Matrix-Vector4 multiplication ... " << endl;
    fout << "Beginning test of Matrix-Vector4 multiplication ... " << endl;
    FOR(test, 0, numTests){
        int r1 = rand() % (int) randMatrices.size();
        int r2 = rand() % (int) randVec3s.size();
        Matrix &m1 = randMatrices[r1];        
        const Vector3 &v1 = randVec3s[r2];
        if(test < numTests / 2){
            //cout << m1 << " * " << endl << v1 << " = " << endl << m1 * v1 << endl << endl;
            fout << m1 << " * " << endl << v1 << " = " << endl << m1 * v1 << endl << endl;
        }
        else{
            //cout << v1 << " * " << endl << m1 << " = " << endl << v1 * m1 << endl << endl;
            fout << v1 << " * " << endl << m1 << " = " << endl << v1 * m1 << endl << endl;
        }
    }
    //cout << "Matrix-Vector3 multiplication test complete" << endl;
    fout << "Matrix-Vector3 multiplication test complete" << endl;
}


void
testMatrixVector4Multiplication(int numTests)
{
    //cout << "Beginning test of Matrix-Vector4 multiplication ... " << endl;
    fout << "Beginning test of Matrix-Vector4 multiplication ... " << endl;
    FOR(test, 0, numTests){
        int r1 = rand() % (int) randMatrices.size();
        int r2 = rand() % (int) randVec4s.size();
        Matrix &m1 = randMatrices[r1];        
        const Vector4 &v1 = randVec4s[r2];
        if(test < numTests / 2){
            //cout << m1 << " * " << endl << v1 << " = " << endl << m1 * v1 << endl << endl;
            fout << m1 << " * " << endl << v1 << " = " << endl << m1 * v1 << endl << endl;
        }
        else{
            //cout << v1 << " * " << endl << m1 << " = " << endl << v1 * m1 << endl << endl;
            fout << v1 << " * " << endl << m1 << " = " << endl << v1 * m1 << endl << endl;
        }
    }
    //cout << "Matrix-Vector4 multiplication test complete" << endl;
    fout << "Matrix-Vector4 multiplication test complete" << endl;
}


void
testMatrixTranspose(int numTests)
{
    //cout << "Beginning test of Matrix transpose ... " << endl;
    fout << "Beginning test of Matrix transpose ... " << endl;
    FOR(test, 0, numTests){
        int r1 = rand() % (int) randMatrices.size();        
        Matrix &m1 = randMatrices[r1];
        Matrix m2 = m1.transpose();        
        //cout << m1 << " transposed = " << endl << m2 << endl;
        fout << m1 << " transposed = " << endl << m2 << endl;
    }
    //cout << "Matrix transpose test complete" << endl;
    fout << "Matrix transpose test complete" << endl;
}



void
testMatrixInverse(int numTests)
{
    //cout << "Beginning test of Matrix inverse ... " << endl;
    fout << "Beginning test of Matrix inverse ... " << endl;
    FOR(test, 0, numTests){
        int r1 = rand() % (int) randMatrices.size();        
        Matrix &m1 = randMatrices[r1];
        Matrix m1Inv;
        bool hasInverse = m1.inverse( m1Inv );
        if(!hasInverse){
            //cout << m1 << " has no inverse" << endl;
            fout << m1 << " has no inverse" << endl;
        }
        else{
            //cout << m1 << " inverse = " << endl << m1Inv << endl;
            fout << m1 << " inverse = " << endl << m1Inv << endl;
        }
    }
    Matrix iden; 
    Matrix::IdentityMatrix(iden);
    Matrix inverseIden;
    iden.inverse( inverseIden );
    //cout << "inverse of Identity = " << endl << inverseIden << endl;
    //cout << "Matrix transpose test complete" << endl;
    fout << "Matrix transpose test complete" << endl;
}

void testMatrixMatrixMultiplication(int numTests)
{
    //cout << "Beginning test of Matrix-Matrix multiplication" << endl;
    fout << "Beginning test of Matrix-Matrix multiplication" << endl;
    for(int test = 0; test < numTests; test++){
        int r1 = rand() % (int) randMatrices.size();
        int r2 = rand() % (int) randMatrices.size();
        const Matrix &m1 = randMatrices[r1];
        const Matrix &m2 = randMatrices[r2];
        //cout << m1 << " * " << endl << m2 << " = " << endl << m1 * m2 << endl << endl;
        fout << m1 << " * " << endl << m2 << " = " << endl << m1 * m2 << endl << endl;
    }
    //cout << "Matrix-Matrix multiplication test complete" << endl;
    fout << "Matrix-Matrix multiplication test complete" << endl;
}

void initTests()
{
    randVec3s.clear();
    randVec4s.clear();
    randMatrices.clear();

    srand((unsigned int) time(NULL));

    fout.open("TestResults.txt");

    // generate random Vector3s
    FOR(i,0,NUM_VEC3S){               
        float r1 = (rand() % 50) - 25.0f;
        float r2 = (rand() % 50) - 25.0f;
        float r3 = (rand() % 50) - 25.0f;
        randVec3s.push_back(Vector3(r1, r2, r3));
    }

    FOR(i,0,NUM_VEC4S){               
        float r1 = (rand() % 50) - 25.0f;
        float r2 = (rand() % 50) - 25.0f;
        float r3 = (rand() % 50) - 25.0f;       
        float r4 = (rand() % 50) - 25.0f;
        randVec4s.push_back(Vector4(r1, r2, r3, r4));
    }

    FOR(i,0,NUM_MATRICES){               
        Matrix m;
        FOR(i,0,4) FOR(j,0,4){
            m(i, j) = (rand() % 50) - 25.0f;
        }        
        randMatrices.push_back(m);
    }
}

void doTests()
{
    initTests();

    testVector3Addition(2);
    testVector3Subtraction(2);
    testVector3ScalarMultiplication(2);
    testVector3DotProduct(2);
    testVector3Magnitude(2);
    testVector3Normalize(2);

    testMatrixAddition(2);
    testMatrixScalarMultiplication(2); 
    testMatrixTranspose(2);
    testMatrixInverse(2);
    testMatrixMatrixMultiplication(2);    
    testMatrixVector3Multiplication(2);
    testMatrixVector4Multiplication(2);
}

