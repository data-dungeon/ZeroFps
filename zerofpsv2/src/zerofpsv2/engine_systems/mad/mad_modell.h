#ifndef _ENGINE_MADMODELL_H_
#define _ENGINE_MADMODELL_H_

#include "../../basic/zfresourcedb.h"
#include "../engine_systems_x.h"
#include "mad_core.h"
#include <vector>
#include "../../render/texturemanager.h"
#include "../../render/zshader.h"

#define MAD_DRAW_MESH		1
#define MAD_DRAW_NORMAL		2
#define MAD_DRAW_BONES		4
#define MAD_DRAW_SPHERE		8
#define MAD_DRAW_LINES		16
#define MAD_DRAW_NOLIGHT	32

/// A MAD that can play anims.
class ENGINE_SYSTEMS_API Mad_Modell 
{
private:
	vector<int>				m_kActiveMesh;
	
	TextureManager*		m_pkTex;
	ZShader*					m_pkShader;

	// Render Interface
	Mad_CoreMesh*		m_pkMesh;
	Mad_CoreSubMesh*	m_pkSubMesh;
	int					m_iSubMesh;

	ZMaterial			m_kDefaultMat;

	int						m_aiReplaceTextures[256];
	ZFResourceHandle		m_akReplaceTexturesHandles[256];


	void LoadTextures();
	void DrawNormal(Vector3* pkVertex, Vector3* pkNormals);
	void DrawSkelleton();
	
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

	//	Mad_Core*	pkCore;
	ZFResourceHandle	kMadHandle;
	
	Mad_Modell();
//	Mad_Modell(Mad_Core* pkModell);
	Mad_Modell(string strResName);

	void SetBasePtr(string strResName);

//	void SetBasePtr(Mad_Core* pkModell);			// Set base modell to use.
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
	void	SetReplaceTexture(char* szOrgName, char* szNew);

	void Draw_All(int iDrawFlags = MAD_DRAW_MESH);	// Draw modell.
	
	float GetRadius();

	bool AddMesh(int iSId);
	bool RemoveMesh(int iId)		{ return false; }
	bool AddMesh(char* szName)		{ return false; }
	bool RemoveMesh(char* szName)	{ return false; }
	bool IsMeshActive(int iId)		{ return false; }
	bool IsMeshActive(char* szName) { return false; }
};


#endif










