#define SIZE 100

#ifndef _HEIGHTMAP_H_
#define _HEIGHTMAP_H_

#include "vector3.h"

using namespace std;

struct HM_vert {
	float height;	//land height
	int texture;	//what texture to use
};

class HeightMap {
	private:
		HM_vert verts[SIZE*SIZE];
		
	public:
		HeightMap();		
		void Zero();
		bool Load(char* acFile);
};


#endif




