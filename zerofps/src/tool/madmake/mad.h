#ifndef FILE__MAD_H
#define FILE__MAD_H

#define MAX_MAD_TEXTURES	256

#include <vector>
using namespace std;

class MadExporter;

/*
Bas klass för alla klasser som kan importeras till MAD.
*/
class IMadImport
{
public:
	virtual ~IMadImport() {};
	// Read data in own format to this.
	virtual	void Read( const char* filename )	= 0;	
	// Export this to mad.
	virtual	bool Export(MadExporter* mad, const char* filename) = 0;
};

// MAD FILE STRUCTURES.
struct MadVertex
{
	float x,y,z;
};

struct MadTextureCoo
{
	float s,t;
};

struct MadFace
{
	int iIndex[3];
};

struct Mad_Header
{
	int		iVersionNum;				// Version num.
	int		iNumOfTextures;				// Num of textures used by mesh.
	int		iNumOfVertex;				// Num of vertices in mesh.
	int		iNumOfFaces;				// Num of triangles in mesh.
	int		iNumOfFrames;				// Num of vertex frames.
	int		iNumOfSubMeshes;			// Num of texture coo.
	int		iNumOfAnimation;			// Antal Animationer.
};

struct Mad_Texture
{
	char	ucTextureName[64];			// path/name of texture.
};

struct Mad_SubMesh
{
	int		iTextureIndex;				// Texture used.
	int		iFirstTriangle;				// First triangle to use texture.
	int		iNumOfTriangles;			// Num of triangles that use texture.
};

class Mad_VertexFrame
{
public:
	Mad_VertexFrame() ;
	~Mad_VertexFrame();

	void operator=(const Mad_VertexFrame& kOther);
	vector<MadVertex>	akVertex;
	vector<MadVertex>	akNormal;
	
};

class Mad_KeyFrame
{
public:
	void Clear(void);
	void operator=(const Mad_KeyFrame& kOther);

	int		iVertexFrame;				// 
	float	fFrameTime;					// Antal Sekunder denna frame ska vara.
};

class Mad_Animation
{
public:
	void Clear(void);
	void operator=(const Mad_Animation& kOther);

	char	Name[256];
	vector<Mad_KeyFrame>	KeyFrame;	
};

class MadExporter
{
public:
	Mad_Header				kHead;
	Mad_Texture				akTextures[MAX_MAD_TEXTURES];

	vector<MadTextureCoo>	akTextureCoo;
	vector<MadFace>			akFaces;
	vector<Mad_VertexFrame>	akFrames;
	vector<Mad_SubMesh>		akSubMeshes;
	vector<Mad_Animation>	akAnimation;
	
	MadExporter();
	~MadExporter();

	void Save(const char* filename);
	void Load(const char* filename);

//	void ImportPMD(pmd_c* pmd);

	Mad_Animation*	GetAnimation(char* ucaName);
	void ShowInfo(void);
};

#endif