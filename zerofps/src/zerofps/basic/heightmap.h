#define HM_SIZE 100

#ifndef _HEIGHTMAP_H_
#define _HEIGHTMAP_H_

#include "vector3.h"
#include <cstdlib>
#include <ctime>

using namespace std;

struct BASIC_API HM_vert {
	float height;	//land height
	int texture;	//what texture to use
};

class BASIC_API HeightMap {
	public:
		HM_vert verts[HM_SIZE*HM_SIZE];		
		
		HeightMap();		
		void Zero();
		void Random();
		bool Load(char* acFile);
		
};


#endif




