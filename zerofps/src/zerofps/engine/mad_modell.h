#ifndef _ENGINE_MADMODELL_H_
#define _ENGINE_MADMODELL_H_

#include "../../zerofps/basic/basic.pkg"
#include "../render/render.pkg"
#include "engine_x.h"

#define MAD_DRAW_MESH		1
#define MAD_DRAW_NORMAL		2
#define MAD_DRAW_BONES		4


class ENGINE_API Mad_Modell 
{
private:
	TextureManager*	m_pkTex;

	void LoadTextures();

	void DrawNormal(Vector3* pkVertex, Vector3* pkNormals);
	void DrawSkelleton();

	// Render Interface
	Mad_CoreMesh*		m_pkMesh;
	Mad_CoreSubMesh*	m_pkSubMesh;
	int					m_iSubMesh;

	void Begin();
	void End();
	int GetNumOfMesh();
	int GetNumOfSubMesh(int iMeshID);
	void SelectMesh(int iMeshID);
	void SelectSubMesh(int iSubMeshID);
	int GetNumVertices();
	int GetNumFaces();
	Vector3*  GetVerticesPtr();
	int* GetFacesPtr();
	Mad_TextureCoo* GetTextureCooPtr();
	Vector3* GetNormalsPtr();
	int GetTextureID();
	char* GetTextureName();
	Mad_CoreTexture* GetTextureInfo();

	void	Create_GLList(Mad_CoreMesh* pkMesh);

public:
	string	m_kMadFile;

	int		iActiveAnimation;
	float	fCurrentTime;
	float	fLastUpdate;

	float	m_fScale;

	int		m_iNextAnimation;
	bool	m_bLoop;

	bool	m_bActive;			// True if animation system is active.

	Mad_Core*	pkCore;
	Mad_Modell();
	Mad_Modell(Mad_Core* pkModell);

	void SetBasePtr(Mad_Core* pkModell);			// Set base modell to use.
	void UpdateAnimation(float dDelta);		// Move all animations forward.
	
	void PlayAnimation(int iAnimNum, float fStartTime);
	int	 GetCurrentAnimation();
	void SetLoopedStatus(bool bLoop);
	bool IsLooped();

	void SetNextAnimation(int iAnimNum);
	void PlayNextAnimations(void);
	int	 GetNextAnimation();

	void SetAnimationActive(bool bPause);
	bool GetAnimationActive();

	void NextCoreAnimation(void);
		
	void SetScale(float fScale);

	void SetReplaceTexture(char* szName);

	void Draw_All(int iDrawFlags = MAD_DRAW_MESH);	// Draw modell.
	
};


#endif










