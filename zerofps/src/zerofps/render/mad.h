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
	/* V�lj submesh att rita ut.*/

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

class Mad_Animation
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
	
	vector<Mad_Animation>	akAnimation;

	int					iActiveFrame;
	int					iActiveKeyFrame;

	void SetTextureManger(TextureManager* ptex)
	{
		pkTextureManger = ptex;
	}

	
	void Load(char* MadFileName);


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

class MadManger
{
	public:	
	bool Load(char* MadFileName);
	
	Core* GetCore(char* MadFileName);
	bool UnLoad(char* MadFileName);
};

class RENDER_API MadInstans
{
private:
	Core* pkCore;

	int		iActiveAnimation;
	float	fCurrentTime;
	float	fLastUpdate;

public:
//	void operator=(const Core& MadInstans);

	void SetBase(Core* pkModell);	// V�ljer vilken bas modell denna instans �r.

	void PlayAnimation(int iAnimNum, float fStartTime);
	/*	B�rjar spela vald animation. fStartTime kan v�ljas f�r att b�rja en bit in i
		vald animation.*/

	void UpdateAnimation(void);
	void Draw(void);

	
	void SetNextAnimation(int iAnimNum);
	/* V�ljer n�sta animation som ska spelas upp efter att den nuvarande �r slut. */
	
	void EnableLoopedAnimation(void);
	void DisableLoopedAnimation(void);
	bool IsAnimationLooped(void);


	void PlayNextAnimations(void);
	/* Force start of next animation. */

	void StopAnimation(void);
	void StartAnimation(void);

	int GetCurrentAnimation();
	int GetNextAnimation();

	float GetAnimationTimeInS(void);
	/* Returns time in s from start of animation */

	float GetAnimationTimeInFrames(void);
	/* Returns time i Frame since start of animation.*/

	float GetAnimationLengthInS(void);

	MadRender* BeginRendering(void);
	/* Gets a rendering object so its possible to render Modell. */
	void EndRendering(void);
	/* Tell system that we are done rendering this object. */

	void SetPlayBackSpeedScale(float fScale);

	Vector3	FH_Pos;
};



#endif
