#ifndef _TESTCODE_H_
#define _TESTCODE_H_
    
    void testVector3Addition(int numTests);
    void testVector3Subtraction(int numTests);
    void testVector3ScalarMultiplication(int numTests);
    void testVector3DotProduct(int numTests);
    void testVector3Magnitude(int numTests);
    void testVector3Normalize(int numTests);

    void testMatrixAddition(int numTests);
    void testMatrixScalarMultiplication(int numTests); 
    void testMatrixTranspose(int numTests);
    void testMatrixInverse(int numTests);
    void testMatrixMatrixMultiplication(int numTests);    
    void testMatrixVector3Multiplication(int numTests);
    void testMatrixVector4Multiplication(int numTests);

    void initTests();
    void doTests();

#endif