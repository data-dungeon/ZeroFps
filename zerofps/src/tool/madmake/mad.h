#ifndef FILE__MAD_H
#define FILE__MAD_H

#define MAX_MAD_TEXTURES	256

#include <vector>
using namespace std;

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

struct Mad_VertexFrame
{
	MadVertex*	pVertex;				// Vertices.
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
	Mad_Header			kHead;
	Mad_Texture			akTextures[MAX_MAD_TEXTURES];

	MadTextureCoo*		pakTextureCoo;
	Mad_VertexFrame*	pakFrames;
	Mad_SubMesh*		pakSubMeshes;
	MadFace*			pakFaces;

	MadVertex*			pakVertexNormals;
	MadVertex*			pakFaceNormals;

	vector<Mad_Animation>	Animation;

	MadExporter();
	~MadExporter();

	void Save(char* filename);
	void Load(char* filename);

	void ImportPMD(pmd_c* pmd);
};

#endif