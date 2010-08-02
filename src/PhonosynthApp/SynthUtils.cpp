/*
 *  SynthUtils.cpp
 *  Phonosynth
 *
 *  Created by Greg on 1/22/09.
 *  Copyright 2009 Brown University. All rights reserved.
 *
 */

#include "SynthUtils.h"
#include <GregJsonProjector.h>
#include <GregJsonCollection.h>
#include <JsonImage.h>
#include <PSGlobalState.h>
#include <queue>

void
SynthUtils::calcProjectorDistances(const JsonProjector* root, OUT map<const JsonProjector*, int>& ret)
{
	// initialize all projectors with a distance of infinity
	const JsonCollection* coll = PSGlobalState::getCurrentCollection();
	FOR(i,0,coll->getNumCoordSystems()){
		const JsonCoordSystem& cs = coll->getCoordSystem(i);
		FOR(j,0,cs.getNumProjectors()){
			const JsonProjector* p = &cs.getProjector(j);
			ret[p] = INT_MAX;
		}
	}
	
	// distance from the root to itself is 0
	ret[root] = 0;
	
	// initialize the queue for the BFS
	queue<const JsonProjector*> Q;	
	Q.push(root);
	
	// do the BFS
	while(!Q.empty()){
		const JsonProjector* cur = Q.front();
		const int curDist = ret[cur];
		Q.pop();		
		const JsonProjector * const * neighbors = cur->getAllNeighbors();
		FOR(i,0,6){
			const JsonProjector* newProj = neighbors[i];
			if(newProj){
				map<const JsonProjector*, int>::iterator iter = ret.find(newProj);
				const int newProjDist = ret[newProj];	
				if(curDist + 1 < newProjDist){
					ret[newProj] = curDist + 1;
					Q.push(newProj);				
				}
			}
		}
	}
}


void
SynthUtils::calcImageDistances(const JsonProjector* root, OUT map<int, int>& ret)
{
	map<const JsonProjector*, int> distances;
	calcProjectorDistances(root, distances);
	
	map<const JsonProjector*, int>::iterator iter;
	for(iter = distances.begin(); iter != distances.end(); iter++){
		const JsonProjector* p = iter->first;
		const int distance = iter->second;
		const int imgIndex = p->getImageIndex();
		
		map<int, int>::iterator iter = ret.find(imgIndex);
		if(iter == ret.end() || distance < iter->second)
			ret[imgIndex] = distance;
	}
}


void
SynthUtils::orderImagesByDistanceFromProjector(const JsonProjector* root, OUT vector<int>& ret)
{
	map<int, int> distances;
	calcImageDistances(root, distances);
	
	vector<pair<int, int> > v;
	
	map<int, int>::iterator iter;
	for(iter = distances.begin(); iter != distances.end(); iter++){
		v.push_back(make_pair(iter->second, iter->first));
	}
	SORT(v);
	
	RFOR(i, v.size() - 1, -1)
		ret.push_back(v[i].second);
}


void
SynthUtils::projectorBFS(const JsonProjector* root, OUT map<const JsonProjector*, int>& ret)
{
	
}

