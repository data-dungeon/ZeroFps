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

class RENDER_API Core
{
public:
//	void operator=(const Core& kOther);
	char				Name[256];

	Core();
	~Core();

	Mad_Header			kHead;
	Mad_Texture			akTextures[MAX_MAD_TEXTURES];

	MadTextureCoo*		pakTextureCoo;
	Mad_VertexFrame*	pakFrames;
	MadFace*			pakFaces;

	TextureManager*		pkTextureManger;
	Mad_SubMesh*		pkSubMesh;
	
	vector<Mad_Animation>	akAnimation;

	int					iActiveFrame;
	int					iActiveKeyFrame;

	void SetTextureManger(TextureManager* ptex)
	{
		pkTextureManger = ptex;
	}

	int		aiTextureIndex[MAX_MAD_TEXTURES];
	int		aiReplaceTextureIndex[MAX_MAD_TEXTURES];
	void	ClearReplaceTexture(void);
	void	SetReplaceTexture(char* szFileName);
	
	void Load(const char* MadFileName);


	void SetFrameI(int iFrame);
	/* Sets current frame to use for rendering */
	void SetFrame_NormalizedTime(float fNormTime);
	/* Sets current frame to use for rendering as a 0.0 - 1.0 value for whole animation.
		Values above one or below zero will wrapp to a valid value */
	void SetFrame_InSeconds(float fSeconds);
	/* Sets animation time as a num of seconds. Values outside current anim length will wrap
		to valid values. */

	void LoopPlayAnim(int iAnim);

	char* GetName(void);

	void	draw(void);

	float GetAnimationLengthInS(int iAnim);
	int GetAnimationTimeInFrames(int iAnim);


};

/*
class MadManger
{
	public:	
	bool Load(char* MadFileName);
	
	Core* GetCore(char* MadFileName);
	bool UnLoad(char* MadFileName);
};*/

/*
class RENDER_API MadInstans
{
private:
	Core* pkCore;

	int		iActiveAnimation;
	float	fCurrentTime;
	float	fLastUpdate;

public:
	void SetBase(Core* pkModell);	// Väljer vilken bas modell denna instans är.

	void PlayAnimation(int iAnimNum, float fStartTime);
	void UpdateAnimation(void);
	void Draw(void);

	
	void SetNextAnimation(int iAnimNum);
	
	void EnableLoopedAnimation(void);
	void DisableLoopedAnimation(void);
	bool IsAnimationLooped(void);


	void PlayNextAnimations(void);

	void StopAnimation(void);
	void StartAnimation(void);

	int GetCurrentAnimation();
	int GetNextAnimation();

	float GetAnimationTimeInS(void);

	float GetAnimationTimeInFrames(void);

	float GetAnimationLengthInS(void);

	MadRender* BeginRendering(void);
	void EndRendering(void);

	void SetPlayBackSpeedScale(float fScale);

	Vector3	FH_Pos;
};*/



#endif
