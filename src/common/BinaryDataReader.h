/*
 *  BinaryDataReader.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BINARYDATAREADER_H_
#define _BINARYDATAREADER_H_

#include <common.h>

enum BinaryDataError
{
	E_EOF,
	E_CINT2BIG
};

class BinaryDataReader
{

public:
	
	BinaryDataReader();
	~BinaryDataReader();
	
	void		readUncompressedInt(int &ret);
	void		readUncompressedFloat(float &ret);
	void		readUncompressedShort(short &ret);
	void		readUncompressedBytes(void *ret, int numToRead);
	
	void		readCompressedInt(int &num);	
	
	void		setDataToRead(const PtrWithLen<unsigned char> data);
	
	void		throwException(BinaryDataError);

private:
	
	int                     bytesLeft() const { return m_totalSize - m_curPosition; }
	
	const unsigned char*	m_dataToRead;
	int                     m_totalSize;
	int                     m_curPosition;
	
};

#endif _BINARYDATAREADER_H_








