/*
 *  BinaryDataReader.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 7/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "BinaryDataReader.h"
#include <common.h>

BinaryDataReader::BinaryDataReader() :
	m_curPosition(0),
	m_totalSize(0)
{ 

}


BinaryDataReader::~BinaryDataReader()
{ 
	
}


// Behold, the most robust exception handler of all time
void
BinaryDataReader::throwException(BinaryDataError e)
{
	switch(e){		
		case E_EOF:
			printf("Binary Data Error: End of file hit unexpectedly\n");
			ASSERT(0);
		case E_CINT2BIG:
			printf("Binary Data Error: End of file hit unexpectedly\n");
			ASSERT(0);
		default:
			printf("Binary Data Error: Unspecified Error\n");
			ASSERT(0);
	}
}



void
BinaryDataReader::setDataToRead(const PtrWithLen<unsigned char> data)
{
	m_dataToRead = (const unsigned char*) data;
	m_totalSize = data.length();
}


void
BinaryDataReader::readUncompressedInt(int &ret)
{
	if(bytesLeft() < 4) 
		throwException(E_EOF);	
	
	char c[4];
	c[0] = m_dataToRead[m_curPosition + 3];
	c[1] = m_dataToRead[m_curPosition + 2];
	c[2] = m_dataToRead[m_curPosition + 1];
	c[3] = m_dataToRead[m_curPosition    ];	
	ret = *((int*) c);
	
	m_curPosition += 4;
}


void
BinaryDataReader::readUncompressedFloat(float &ret)
{
	if(bytesLeft() < 4) 
		throwException(E_EOF);
	
	char c[4];
	c[0] = m_dataToRead[m_curPosition + 3];
	c[1] = m_dataToRead[m_curPosition + 2];
	c[2] = m_dataToRead[m_curPosition + 1];
	c[3] = m_dataToRead[m_curPosition + 0];	
	ret = *((float*) c);
	
	m_curPosition += 4;
}
	

void
BinaryDataReader::readUncompressedShort(short &ret)
{
	if(bytesLeft() < 2) 
		throwException(E_EOF);
	
	char c[2];
	c[0] = m_dataToRead[m_curPosition + 1];
	c[1] = m_dataToRead[m_curPosition    ];
	
	ret = *((short*) c);
	
	m_curPosition += 2;
}



void
BinaryDataReader::readUncompressedBytes(void* ret, int numToRead)
{
	ASSERT(numToRead > 0);
	ASSERT(ret);
	
	if(bytesLeft() < numToRead) 
		throwException(E_EOF);
	
	memcpy(ret, &m_dataToRead[m_curPosition], numToRead);
	
	m_curPosition += numToRead;
}




void
BinaryDataReader::readCompressedInt(int &ret)
{
	int numBytes = 0;
	int val = 0;
	
	while(bytesLeft() > 0){		
		char cur = m_dataToRead[m_curPosition++];
		if(cur == 0){
			ret = -1;
			return;
		}		
		val = (val << 7) | (cur & 0x7F);
		ret = val;
		if((cur & 0x80) != 0)
			return;
		numBytes++;
		if (numBytes > 4)
			throwException(E_CINT2BIG);		
	}
	throwException(E_EOF);
}







