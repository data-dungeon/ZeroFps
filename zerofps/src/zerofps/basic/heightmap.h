//#define HM_SIZE 200

#ifndef _BASIC_HEIGHTMAP_H_
#define _BASIC_HEIGHTMAP_H_

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "basic_x.h"
#include "os.h"
#include "vector3.h"
#include "fileio.h"
#include "globals.h"

struct BASIC_API HM_vert {
	float height;	//land height
	Vector3 normal;
	int texture;	//what texture to use
	Vector3 color;

//	HM_vert *childs[8];
};

struct HM_fileheader {
	int m_iHmSize;
};

#include "zfobject.h"

class BASIC_API HeightMap: public ZFObject {
	private:		
		Uint32 GetPixel(SDL_Surface* surface,int x,int y);
		
	public:
		void RunCommand(int cmdid, const CmdArgument* kCommand);

		HM_vert* verts;		
		int m_iHmSize;
//		int m_iBoxTresh;
//		int m_iMaxSteps;
		char m_acTileSet[256];
//		HM_vert *m_kCenter;
		Vector3 m_kPosition;
		int m_iError;
		FileIo *m_pkFile;
		
		HeightMap(FileIo* pkFile);		
		void Zero();
		void Random();
		bool Load(char* acFile);
		bool Save(char* acFile);
		bool LoadImageHmap(char* acFile);
		void GenerateNormals();
		void GenerateTextures();
		float Height(float x,float z);
		Vector3 Tilt(float x,float z);		
		void SetTileSet(char* acTileSet);
		void SetPosition(Vector3 kNewPos);

		HM_vert* GetVert(int x,int z);		

//		void MakeQuadTree();
//		HM_vert* CreateQuad(int x,int z,int width,int step,bool more);

//		bool BoxTest(int x,int z,int width);
};


#endif




