#ifndef _BASIC_HEIGHTMAP_H_
#define _BASIC_HEIGHTMAP_H_

#include <SDL/SDL.h>
#include "../engine_systems_x.h"
#include <vector>
#include <string>
#include "../../basic/vector3.h"
#include "../../basic/rect.h"
#include "../../basic/zfsystem.h"
#include "../../render/texturemanager.h"

#define HEIGHTMAP_SCALE 1

struct Mad_Face;

struct ENGINE_SYSTEMS_API HM_vert 
{
	float			height;	
	Vector3		normal;
};

struct HM_fileheader 
{
	int	m_iHmSize;
};

struct TileSet
{
	char	m_acTexture[256];
	char	m_acDetailTexture[256];
	char	m_acMask[256];
};

struct TerrainBlock
{
	Vector3	kAABB_Min;
	Vector3	kAABB_Max;
};

class ENGINE_SYSTEMS_API HMSelectVertex
{
public:
	int		m_iIndex;
	float		m_fValue;
};

class ENGINE_SYSTEMS_API HeightMap
{
	private:			
		TextureManager*	m_pkTexMan;		
		ZFBasicFS*			m_pkBasicFS;

		int m_iTilesSide;					// The number of edges/tiles on each side.
		int m_iVertexSide;				// The number of vertex on each side (tiles + 1)
		int m_iNumOfHMVertex;			// Total number of HMVertex that are in this map.

		HM_vert* verts;	
		vector<TileSet>		m_kSets;
		//vector<TerrainBlock>	m_kTerrainBlocks;

		int		m_iHmScaleSize;
		Vector3	m_kPosition;			// Position of Center of HMAP

		Uint32 GetPixel(SDL_Surface* surface, int x, int y);
		bool AllocHMMemory(int iSize);

	public:
		Vector3	m_kCornerPos;			// Position for Corner for rendering.

		// Construct & Destruct
		HeightMap();		
		~HeightMap();		

		// Init
		void Create(int iTilesSide);
		void Zero();
		bool IsAllZero();
		void Random();
		void GenerateNormals();
		void GenerateNormals(int iPosX,int iPosZ,int iWidth,int iHeight);		

		// Load/Save
		bool Load(const char* acFile);
		bool Save(const char* acFile);
		//bool LoadImageHmap(const char* acFile);


		vector<HMSelectVertex> GetSelection(Vector3 kCenter, float fInRadius, float fOutRadius);
		//void CreateBlocks();
		
		Vector3* m_pkVertex;			// Precalc vertex coo. Created at load time.
		void RebuildVertex();
		HM_vert* GetHMVertex()	{	return verts;	}
		bool	IsIndexOutOfMap(int iIndex);

		float Height(float x,float z);
		Vector3 Tilt(float x,float z);		
		void SetPosition(Vector3 kNewPos);
		Vector3 &GetPos(){return m_kPosition;};
		HM_vert* GetVert(int x,int z);		
		void GetMapXZ(float& x,float& z);
		
		void Smooth(int fStartx,int fStartz,int fWidth,int fHeight);
		void Flatten(vector<HMSelectVertex> kSelected, Vector3 kPos);
		void Raise(vector<HMSelectVertex> kSelected, float fSize);		// Raise or lower a selection.
		
		void DrawMask(Vector3 kPos,int iMode,int iSize,int r,int g,int b,int a);
		
		int GetSize(){return m_iTilesSide*HEIGHTMAP_SCALE;};				// Return the size of one side of the Hm.
		void AddSet(const char* acTexture,const char* acDetailTexture,const char* acMask);
		void ClearSet();


		HM_vert* LinePick(Vector3 kPos,Vector3 kDir,Vector3 kCenterPos,int iWidth,Vector3& kHitPos);		
		bool LineVSPolygon(Vector3* pkVerts,Vector3 kPos1,Vector3 kPos2,Vector3& kColPos);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos);

		float GetAlpha(float x,float y,int iTexture);
		int GetMostVisibleTexture(float x,float y);
		
		void GenerateTextures();

		Point GetSqrFromPos(Vector3 pos);
		Vector3 GetPosFromSqr(Point square);
		
		void GetCollData(vector<Mad_Face>* pkFace,vector<Vector3>* pkVertex , vector<Vector3>* pkNormal);
		
		friend class Render;
};


#endif




