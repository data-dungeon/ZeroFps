//#define HM_SIZE 200

#ifndef _HEIGHTMAP_H_
#define _HEIGHTMAP_H_

#include "vector3.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <cstdio>
#include "basic.pkg"

using namespace std;

struct HM_vert;

struct BASIC_API HM_vert {
	float height;	//land height
	Vector3 normal;
	int texture;	//what texture to use
//	HM_vert *childs[8];
};

struct HM_fileheader {
	int m_iHmSize;
};



class BASIC_API HeightMap {

	public:
		HM_vert* verts;		
		int m_iHmSize;
		int m_iBoxTresh;
		int m_iMaxSteps;
		char m_acTileSet[256];
		HM_vert *m_kCenter;
		Vector3 m_kPosition;
		int m_iError;
		
		HeightMap();		
		void Zero();
		void Random();
		bool Load(char* acFile);
		bool Save(char* avFile);
		void GenerateNormals();
		void GenerateTextures();
		float Height(int x,int z);
		void SetTileSet(char* acTileSet);
		void SetPosition(Vector3 kNewPos);

		HM_vert* GetVert(int x,int z);		
		
//		void MakeQuadTree();
//		HM_vert* CreateQuad(int x,int z,int width,int step,bool more);

//		bool BoxTest(int x,int z,int width);
};


#endif




