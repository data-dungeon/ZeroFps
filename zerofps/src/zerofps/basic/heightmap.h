//#define HM_SIZE 200

#ifndef _HEIGHTMAP_H_
#define _HEIGHTMAP_H_

#include "vector3.h"
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

struct HM_vert;

struct BASIC_API HM_vert {
	float height;	//land height
	Vector3 normal;
	int texture;	//what texture to use
	HM_vert *childs[8];
};



class BASIC_API HeightMap {

	public:
		HM_vert* verts;		
		int m_iHmSize;
		int m_iBoxTresh;
		int m_iMaxSteps;
		char m_acTileSet[256];
		HM_vert *m_kCenter;
		
		HeightMap();		
		void Zero();
		void Random();
		bool Load(char* acFile);
		void GenerateNormals();
		float Height(int x,int z);
		void SetTileSet(char* acTileSet);
		
		void MakeQuadTree();
		HM_vert* CreateQuad(int x,int z,int width,int step,bool more);
		HM_vert* GetVert(int x,int z);
		bool BoxTest(int x,int z,int width);
};


#endif




