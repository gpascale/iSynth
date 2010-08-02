/*
 *  JsonPool.h
 *  Phonosynth
 *
 *  Created by Greg on 3/1/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#include <vector>
#include <common.h>

class JsonInAtom;

using namespace std;

class JsonPool
{
public:

	// singleton, provides access to the one global copy
	static JsonPool& sharedPool()
	
	static JsonInAtom* alloc();
	static void		dealloc(JsonInAtom* j);
	
private:

	JsonPool();
	
	vector<JsonBlock> m_blocks;
	
};


class JsonBlock
{
	
public:
	
	JsonBlock(int capacity);
	
	JsonInAtom* alloc();
	void		dealloc(JsonInAtom* j);	
	bool		contains(const JsonInAtom* j);
	
private:
	
	JsonInAtom*		m_data;
	int				m_count
	int				m_capacity;
	
	queue<short>	m_freeSlots;
	
}
