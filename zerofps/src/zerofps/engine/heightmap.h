//#define HM_SIZE 200

#ifndef _BASIC_HEIGHTMAP_H_
#define _BASIC_HEIGHTMAP_H_

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "engine_x.h"
//#include "os.h"
//#include "vector3.h"
//#include "fileio.h"
//#include "globals.h"
//#include "zffile.h"
//#include "zfobject.h"
//#include "zfobjectmanger.h"
#include <vector>
#include <string>

#include "../basic/basic.pkg"


#define HEIGHTMAP_SCALE 2

struct ENGINE_API HM_vert {
	float			height;		//land height
	Vector3		normal;
	int			texture;		//what texture to use
	Vector3		color;


//	HM_vert *childs[8];
};

struct HM_fileheader {
	int m_iHmSize;
};

struct TileSet{
	char m_acTexture[256];
	char m_acDetailTexture[256];
	char m_acMask[256];
};


class ENGINE_API HeightMap: public ZFObject {
	private:		
		Uint32 GetPixel(SDL_Surface* surface,int x,int y);
		
	public:
		vector<TileSet> m_kSets;
		
		HM_vert* verts;	
		
		Vector3* m_pkVertex;	// Precalc vertex coo. Created at load time.
		void RebuildVertex();

		int m_iHmSize;
		int m_iHmScaleSize;
		char m_acTileSet[256];
		Vector3 m_kPosition;
		int m_iError;
		FileIo *m_pkFile;
		
		HeightMap();		
		void RunCommand(int cmdid, const CmdArgument* kCommand);		
		void Create(int iHmSize);
		void Zero();
		void Random();
		bool Load(const char* acFile);
		bool Save(const char* acFile);
		bool LoadImageHmap(const char* acFile);
		void GenerateNormals();
		void GenerateTextures();
		void GenerateNormals(int iPosX,int iPosZ,int iWidth,int iHeight);		
		float Height(float x,float z);
		Vector3 Tilt(float x,float z);		
		void SetTileSet(char* acTileSet);
		void SetPosition(Vector3 kNewPos);
		Vector3 &GetPos(){return m_kPosition;};
		HM_vert* GetVert(int x,int z);		
		
		int GetSize(){return m_iHmSize*HEIGHTMAP_SCALE;};
		void AddSet(const char* acTexture,const char* acDetailTexture,const char* acMask);
		void ClearSet();
};


#endif




