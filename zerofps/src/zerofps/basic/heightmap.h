//#define HM_SIZE 200

#ifndef _HEIGHTMAP_H_
#define _HEIGHTMAP_H_

#include "vector3.h"
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

struct BASIC_API HM_vert {
	float height;	//land height
	Vector3 normal;
	int texture;	//what texture to use
};

class BASIC_API HeightMap {
	public:
		HM_vert* verts;		
		int m_iHmSize;
		char m_acTileSet[256];
		
		HeightMap();		
		void Zero();
		void Random();
		bool Load(char* acFile);
		void GenerateNormals();
		float Height(int x,int z);
		void SetTileSet(char* acTileSet);
};


#endif




