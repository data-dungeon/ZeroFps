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
#include "../../engine/res_texture.h"

#define HM_FLAGVISIBLE1	1
#define HM_FLAGVISIBLE2	2

struct Mad_Face;

struct ENGINE_SYSTEMS_API HM_vert 
{
	float			height;					// Local Height at HM vertex.
	Vector3		normal;					// Normal at HM vertex
};

/* File Header for Hm files.*/
struct HM_fileheader 
{
	int	m_iHmSize;						// Num of vertices on a side.				
	int	m_iNumOfLayers;				// Num of layers in Hmap
	float	m_fTileSize;
	bool	m_bInverted;
};

// One rendering pass with a texture over the terrain. 
class HM_Layer
{
public:
	string	m_strName;					// Name of layer.
	string	m_strTexture;				// Name of texture to use.
	string	m_strDetailTexture;		// Detail texture name.
	string	m_strMask;					// Name of layer alpha mask.

	ZFResourceHandle	m_kMaskHandle;

	HM_Layer& operator=(const HM_Layer &kOther) {
		m_strName = kOther.m_strName;
		m_strTexture = kOther.m_strTexture;
		m_strDetailTexture = kOther.m_strDetailTexture;
		m_strMask = kOther.m_strMask;
		m_kMaskHandle = kOther.m_kMaskHandle;
		return *this;
		}	

	void Load(ZFVFile* pkFile);
	void Save(ZFVFile* pkFile);
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

		int					m_iID;						// ID Assigned to each Hm in the game. Used for savefile names.

		int					m_iTilesSide;				// The number of edges/tiles on each side.
		int					m_iVertexSide;				// The number of vertex on each side (tiles + 1)
		int					m_iNumOfHMVertex;			// Total number of HMVertex that are in this map.
		float					m_fTileSize;				// The size in meters of each Tile.

		HM_vert*				verts;						// Ptr to array of HMVertex. 
		vector<HM_Layer>	m_kLayer;					// All layers in the HM.
		unsigned char*		m_pkTileFlags;

		Vector3				m_kPosition;				// Position of Center of HMAP

		bool AllocHMMemory(int iSize);				// Alloc memory for HMVertex.
		void Clear();

	public:
		Vector3				m_kCornerPos;				// Position of the Corner of the HM (for rendering).
		Vector3				m_kCornerOffset;			// Offset of Corner of the HM from the center.
		bool					m_bInverted;				

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

		// Set/Get Function
		void SetPosition(Vector3 kNewPos);
		Vector3 &GetPos(){return m_kPosition;};
		void SetID(int iId) { m_iID = iId; }
		void		Invert() { m_bInverted = !m_bInverted; }
		int GetTopLowTriangle(Vector3 kPos);
		float GetTileSize() { return m_fTileSize; }
		int	GetSize() {return int(m_iTilesSide * m_fTileSize); };				// Return the size of one side of the Hm.
		HM_vert* GetHMVertex()	{	return verts;	}
		HM_vert* GetVert(int x,int z);		
		void GetMapXZ(float& x,float& z);
		Point GetSqrFromPos(Vector3 pos);
		Vector3 GetPosFromSqr(Point square);
 		float GetBrushSizeInAlphaUVSpace(float fSize);
		Vector3 WorldToMap(Vector3 kVec);
		Vector3 MapToLocal(Vector3 kVec);

		// Sample
		float Height(float x,float z);
		Vector3 Tilt(float x,float z);		

		// Edit Commands / Selection
		vector<HMSelectVertex> GetSelection(Vector3 kCenter, float fInRadius, float fOutRadius);
		void Smooth(vector<HMSelectVertex> kSelected);
		void Flatten(vector<HMSelectVertex> kSelected, Vector3 kPos);
		void Raise(vector<HMSelectVertex> kSelected, float fSize);		// Raise or lower a selection.
		void DrawMask(Vector3 kPos,int iMode,float fSize,int r,int g,int b,int a);
		void DrawVisible(Vector3 kPos, bool bVisible);
	
		// Pick / Trace
		HM_vert* LinePick(Vector3 kPos,Vector3 kDir,Vector3 kCenterPos,int iWidth,Vector3& kHitPos);		
		bool LineVSPolygon(Vector3* pkVerts,Vector3 kPos1,Vector3 kPos2,Vector3& kColPos);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos);
		void GetCollData(vector<Mad_Face>* pkFace,vector<Vector3>* pkVertex , vector<Vector3>* pkNormal);

		//Vector3* m_pkVertex;			// Precalc vertex coo. Created at load time.
		bool	IsIndexOutOfMap(int iIndex);
		
		// Layer Mangment
		bool Layer_Create(string strName, string strTexture);
		bool Layer_Delete(string strName);
		bool Layer_Clone(string strNameFrom, string strName);
		vector<string>	Layers_GetNames();
		
		friend class Render;
};

#endif




