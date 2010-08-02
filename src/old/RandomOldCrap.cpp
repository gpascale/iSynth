/*
 *  RandomOldCrap.cpp
 *  Phonosynth
 *
 *  Created by Greg on 10/27/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */

#if 0

inline void
Camera::rotateEulerSpringy(const Vector3& amts)
{
	cout << "rotateEulerSpringy(" << amts << ")" << endl;
	const Matrix oldViewMat = m_spring->getEndRotation().toMatrix();
	Matrix newViewMat;
	Matrix::ViewMatrixEuler(newViewMat, m_position, amts);
	doSpringyRotate(newViewMat);
}


// code to pick a random font / print out all the supported fonts
	NSString* fontFamilyName = [[UIFont familyNames] randomElement];
	[[UIFont familyNames] printS];
	cout << endl;
	NSString* fontName       = [[UIFont fontNamesForFamilyName: fontFamilyName] randomElement];	
	FOR(i,0,[[UIFont familyNames] count]){
		NSString* famName = [[UIFont familyNames] objectAtIndex: i];
		cout << CSTRING(famName) << ":" << endl;
		NSArray* fn = [UIFont fontNamesForFamilyName: famName];
		[fn printS];
	}

#endif // 0