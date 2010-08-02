/*
 *  LinAlgTester.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 6/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <common.h>
#include "TestCode.h"
#include "LinAlgTester.h"


LinAlgTester::LinAlgTester()
{
}


void
LinAlgTester::run()
{        
    Vector3 p1(1.0f, 0.0f, 0.0f);
    Vector3 p2(0.0f, 1.0f, 0.0f);
    Vector3 p3(1.0f, 1.0f, 1.0f);
    
    Vector3 q1(1.0f, 0.0f, 0.0f);
    Vector3 q2(0.0f, 0.0f, 1.0f);
    Vector3 q3(1.0f, 0.0f, 1.0f);
    
    Matrix m;
    ASSERT(Matrix::AffineMatrix(m, p1, p2, p3, q1, q2, q3));
    
    cout << "m = " << m << endl;
    
    cout << "p1 * m = " << p1 * m << endl;
    cout << "p2 * m = " << p2 * m << endl;
    cout << "p3 * m = " << p3 * m << endl;    

    Vector4 P1(10.0f, 0.0f, 0.0f, 1.0f), 
            P2(0.0f, 7.0f, 0.0f, 1.0f), 
            P3(-2.0f, 3.0f, 0.0f, 1.0f), 
            P4(0.0f, 0.0f, 0.0f, 1.0f);
    Vector4 Q1(50.0f, 0.0f, 0.0f, 1.0f),
            Q2(0.0f, 0.0f, -12.0f, 1.0f),
            Q3(49.5f, 0.0f, 6.0f, 1.0f),
            Q4(0.0f, 0.0f, 0.0f, 1.0f);
    
    Matrix m2;
    ASSERT(Matrix::ProjectionMatrix(m2, P1, P2, P3, P4, Q1, Q2, Q3, Q4));
    
    cout << "m2 = " << m2 << endl;
    cout << "P1 * m2 = " << P1 * m2 << endl;
    cout << "P2 * m2 = " << P2 * m2 << endl;
    cout << "P3 * m2 = " << P3 * m2 << endl;
    cout << "P4 * m2 = " << P4 * m2 << endl;
    
    cout << "wee" << endl;
}
