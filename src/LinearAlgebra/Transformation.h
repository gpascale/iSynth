#ifndef _TRANSFORMATION_H_
#define _TRANSFORMATION_H_

#include <common.h>
#include "Vector3.h"
#include "Matrix.h"

// this class models the world transformation for an object in the scene
// it may not be necessary and is perhaps slower than just using matrices,
// but I like it because it keeps the code a lot cleaner

class Transformation
{
public:

    Transformation(void);
    ~Transformation(void);

    void        	applyIdentity();
    void        	applyAbsoluteTranslation(float x, float y, float z);
	void        	applyRelativeTranslation(float x, float y, float z);
    void        	applyAbsoluteTranslation(const Vector3 &v);
	void        	applyRelativeTranslation(const Vector3 &v);
	void        	applyAbsoluteRotation(float x, float y, float z);
	void        	applyRelativeRotation(float x, float y, float z);
    void        	applyAbsoluteRotation(const Vector3 &v);
	void        	applyRelativeRotation(const Vector3 &v);
	void        	applyAbsoluteScale(float x, float y, float z);
	void        	applyRelativeScale(float x, float y, float z);
    void        	applyAbsoluteScale(const Vector3 &v);
	void        	applyRelativeScale(const Vector3 &v);

    const Matrix&   getMatrix() const;	
    void			getPosition( Vector3 &out ) const;
    void			getScale( Vector3 &out ) const;
    void			getRotation( Vector3 &out ) const;

private:

    Vector3         m_rotation;

    Matrix          m_translate;
    Matrix          m_rotate;
    Matrix          m_scale;
    Matrix          m_composite;

};

#endif
