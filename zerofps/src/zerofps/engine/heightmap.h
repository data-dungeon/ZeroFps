//#define HM_SIZE 200

#ifndef _BASIC_HEIGHTMAP_H_
#define _BASIC_HEIGHTMAP_H_

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "engine_x.h"
#include <vector>
#include <string>



#include "../basic/basic.pkg"
#include "../render/render.pkg"

#define HEIGHTMAP_SCALE 2

struct ENGINE_API HM_vert 
{
	float			height;	
	Vector3		normal;
	int			texture;	
	Vector3		color;
};

struct HM_fileheader {
	int m_iHmSize;
};

struct TileSet{
	char m_acTexture[256];
	char m_acDetailTexture[256];
	char m_acMask[256];
};

struct TerrainBlock
{
	Vector3 kAABB_Min;
	Vector3 kAABB_Max;
};

class ENGINE_API HeightMap: public ZFObject {
	private:			
		TextureManager*	m_pkTexMan;
		FileIo*				m_pkFile;	
		ZFBasicFS*			m_pkBasicFS;
		Uint32 GetPixel(SDL_Surface* surface,int x,int y);
		bool	AllocHMMemory(int iSize);

		HM_vert* verts;	

	public:
		vector<TileSet>		m_kSets;
		vector<TerrainBlock>	m_kTerrainBlocks;
	
			void CreateBlocks();


		
		Vector3* m_pkVertex;	// Precalc vertex coo. Created at load time.
		void RebuildVertex();

		int m_iHmSize;
		int m_iHmScaleSize;
		char m_acTileSet[256];
		Vector3 m_kPosition;			// Position of Center of HMAP
		Vector3 m_kCornerPos;		// Position for corner for rendering.
		int m_iError;
	
//		FileIo *m_pkFile;
		
		HM_vert* GetHMVertex()	{	return verts;	}
		bool	IsIndexOutOfMap(int iIndex);

		HeightMap();		
		~HeightMap();		
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
		void GetMapXZ(float& x,float& z);
		
		void Smooth(int fStartx,int fStartz,int fWidth,int fHeight);
		void Flatten(int iPosx,int iPosy,int iSize);
		void Raise(int iPosx,int iPosy,int iMode,int iSize,bool bSmooth);
		void DrawMask(int iPosX,int iPosy,int iMode,int iSize,int r,int g,int b,int a);
		
		int GetSize(){return m_iHmSize*HEIGHTMAP_SCALE;};
		void AddSet(const char* acTexture,const char* acDetailTexture,const char* acMask);
		void ClearSet();

		bool IsAllZero();


		HM_vert* LinePick(Vector3 kPos,Vector3 kDir,Vector3 kCenterPos,int iWidth,Vector3& kHitPos);		
		bool LineVSPolygon(Vector3* pkVerts,Vector3 kPos1,Vector3 kPos2,Vector3& kColPos);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos);

		friend class Render;
};


#endif




