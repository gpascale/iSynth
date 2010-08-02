/*
 *  SynthUtils.h
 *  Phonosynth
 *
 *  Created by Greg on 1/22/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */


#ifndef _SYNTHUTILS_H_
#define _SYNTHUTILS_H_

#include <common.h>
#include <map>
#include <vector>

class JsonProjector;

// defined as a class simply so private methods can be hidden. It's just a collection of static functions.
class SynthUtils
{
public:
		
// calculates the distance of each projector in the synth from the specified "root" projector
static void calcProjectorDistances(const JsonProjector* root,
								   OUT map<const JsonProjector*, int>& ret);
	
// similar to calcProjectorDistances but operates on image indices. For each image, calculates
// the shortest path from any projector displaying that image to the root projector
static void calcImageDistances(const JsonProjector* root,
							   OUT map<int, int>& ret);

static void orderImagesByDistanceFromProjector(const JsonProjector* root,
											   OUT vector<int>& ret);
	
private:
	
static void projectorBFS(const JsonProjector* root, OUT map<const JsonProjector*, int>& ret);
		
};

#endif