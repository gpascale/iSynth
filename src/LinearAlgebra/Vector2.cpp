/*
 *  Vector2.cpp
 *  Phonosynth
 *
 *  Created by Greg on 11/23/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#include "Vector2.h"

Vector2::Vector2() :
    x(0.0f), y(0.0f)
{
}


Vector2::Vector2(float _x, float _y) :
    x(_x), y(_y)
{    
}


Vector2::Vector2(const float *v)
{
    ASSERT(v != NULL);
    memcpy(&x, v, 2 * sizeof(float));
}


Vector2::Vector2(const Vector2 &v) :
    x(v.x), y(v.y)
{    
}


ostream& operator << (ostream& os, const Vector2& v)
{
    return os << "(" << v.x << ", " << v.y << ")";
}
