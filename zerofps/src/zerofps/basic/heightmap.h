#define HM_SIZE 100

#ifndef _HEIGHTMAP_H_
#define _HEIGHTMAP_H_

#include "vector3.h"

using namespace std;

struct HM_vert {
	float height;	//land height
	int texture;	//what texture to use
};

class HeightMap {
	public:
		HM_vert verts[HM_SIZE*HM_SIZE];		
		
		HeightMap();		
		void Zero();
		bool Load(char* acFile);
		
};


#endif




