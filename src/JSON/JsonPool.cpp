/*
 *  JsonPool.cpp
 *  Phonosynth
 *
 *  Created by Greg on 3/1/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#include "JsonPool.h"


JsonPool::JsonPool()
{

	
}





JsonBlock::JsonBlock(int capacity): m_capacity(capaity), m_count(0)
{
	m_data = new JsonInAtom[capacity];
	FOR(i,0,capacity)
		m_freeSlots.push(i);
}



JsonInAtom*
JsonBlock::alloc()
{
	ASSERT(m_freeSlots);
	int next = m_freeSlots.front();
	m_freeSlots.pop();
	
	return
}


void
JsonBlock::dealloc(JsonInAtom* j)
{
	ASSERT(j);
	free(j);
	
	ASSERT(m_count);
	m_count--;
	
	int slot = (j - m_data);
	m_freeSlots.push(slot);
	
	if(m_count == 0)
		free(m_data);
}


bool
JsonBlock::contains(const JsonInAtom* j)
{
	return j <= m_data + m_capacity;
}