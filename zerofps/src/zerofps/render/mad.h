#ifndef FILE__MAD_H
#define FILE__MAD_H

#include <stdio.h>
#include <vector>
#include "render_x.h"
#include "texturemanager.h"

using namespace std;


#define MAX_MAD_TEXTURES	256

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
	MadVertex*	pNormal;				// Vertices.
};

class MadRender
{
	public:
	int GetSubMeshCount(void);
	/* Ta fram antal submeshes i modellen.*/
	void SetSubMesh(int iMeshNum);
	/* Välj submesh att rita ut.*/

	int GetMaterial(void);

	int GetVertexCount();
	int GetVertex(MadVertex* pVertexBuffer);
	int GetTextureCoo(MadTextureCoo* pTextureCooBuffer);
	int GetNormals(MadVertex* pNormalsBuffer);
	int GetFaceCount(void);
	int GetFaces(MadFace* pFaceBuffer);
};


class Mad_KeyFrame
{
public:
	void Clear(void);
	void operator=(const Mad_KeyFrame& kOther);

	int		iVertexFrame;				// 
	float	fFrameTime;					// Antal Sekunder denna frame ska vara.
};

class RENDER_API Mad_Animation
{
public:
	void Clear(void);
	void operator=(const Mad_Animation& kOther);

	char	Name[256];
	vector<Mad_KeyFrame>	KeyFrame;	
};

class Mad_BonePos
{
public:
	Vector3	kPosition;
	Vector3	kRotation;
};

class Mad_Bone
{
public:
	int			m_iParent;
	char		m_szName[64];
	Mad_BonePos	kBindPose;
};

/*class Mad_CoreKeyFrame
{
public:

	Mad_BonePos	m_
};*/

/*
class Mad_CoreAnimationTrack
{
public:
	int					m_iBone;
	vector<KeyFrame>	m_kKeyFrames;
};*/
/*
class Mad_CoreAnimation
{
public:
	vector<Mad_CoreAnimationTrack>	m_kTracks;
};*/

/*
	Class CoreSkelleton

	Class CoreMesh
		Frames
		Texture Coo
		Faces
		SubMesh
		Textures
		MeshAnim

	Class CoreAnimation

	
	MadTextureCoo*			pakTextureCoo;
	Mad_VertexFrame*		pakFrames;
	MadFace*				pakFaces;

	TextureManager*			pkTextureManger;
	Mad_SubMesh*			pkSubMesh;
	
	vector<Mad_Animation>	akAnimation;

*/

class RENDER_API Core
{
public:
	char					Name[256];

	Core();
	~Core();

	Mad_Header				kHead;
	Mad_Texture				akTextures[MAX_MAD_TEXTURES];

	MadTextureCoo*			pakTextureCoo;
	Mad_VertexFrame*		pakFrames;
	MadFace*				pakFaces;

	TextureManager*			pkTextureManger;
	Mad_SubMesh*			pkSubMesh;
	
	vector<Mad_Animation>	akAnimation;

	int						iActiveFrame;
	int						iActiveKeyFrame;

	void SetTextureManger(TextureManager* ptex)
	{
		pkTextureManger = ptex;
	}

	int		aiTextureIndex[MAX_MAD_TEXTURES];
	int		aiReplaceTextureIndex[MAX_MAD_TEXTURES];
	void	ClearReplaceTexture(void);
	void	SetReplaceTexture(char* szFileName);
	
	void Load(const char* MadFileName);

	void LoadSkelleton(const char* MadFileName);
	void LoadAnimation(const char* MadFileName) {} 
	void LoadMesh(const char* MadFileName) {} 

	void DrawSkelleton(float x, float y, float z);
	void SetPosRot(int iChildBone);

	void SetFrameI(int iFrame);
	/* Sets current frame to use for rendering */
	void SetFrame_NormalizedTime(float fNormTime);
	/* Sets current frame to use for rendering as a 0.0 - 1.0 value for whole animation.
		Values above one or below zero will wrapp to a valid value */
	void SetFrame_InSeconds(float fSeconds);
	/* Sets animation time as a num of seconds. Values outside current anim length will wrap
		to valid values. */

	void	LoopPlayAnim(int iAnim);

	char*	GetName(void);

	void	draw(void);

	float	GetAnimationLengthInS(int iAnim);
	int		GetAnimationTimeInFrames(int iAnim);

	vector<Mad_Bone>	kSkelleton;	
};




#endif
