#ifndef _ENGINE_MADMODELL_H_
#define _ENGINE_MADMODELL_H_

#include "../../basic/zfresourcedb.h"
#include "../engine_systems_x.h"
#include "mad_core.h"
#include <vector>
#include "../../render/texturemanager.h"
#include "../../render/zshadersystem.h"
#include "../../engine/zssrenderengine.h"

#define MAD_DRAW_MESH		1
#define MAD_DRAW_NORMAL		2
#define MAD_DRAW_BONES		4
#define MAD_DRAW_SPHERE		8
#define MAD_DRAW_LINES		16
#define MAD_DRAW_NOLIGHT	32
#define MAD_DRAW_LODCODE	64

struct AnimInfo
{
	int	m_iAnimationIndex;
	float	m_fAnimationTime;
	float m_fAnimationLength;
};

/// A MAD that can play anims.
class ENGINE_SYSTEMS_API Mad_Modell
{
private:
	
	TextureManager*		m_pkTex;

	// Render Interface
	Mad_CoreMesh*		m_pkMesh;
	Mad_RawMesh*		m_pkRawMesh;
	Mad_CoreSubMesh*	m_pkSubMesh;
	int					m_iSubMesh;

	ZVertexBuffer*		m_pkVBO;
	vector<Vector3>	m_kTangents;
	vector<Vector3>	m_kBiTangents;
	
	ZFResourceHandle*	m_akReplaceTexturesHandles[256];				//ytterst in effektivt, o skapa 256 st res handles varje g�ng en modell skapas, blir m�nga konstruktor anrop i on�dan

	void LoadTextures();
	void DrawNormal(Vector3* pkVertex, Vector3* pkNormals);
	void DrawSkelleton();
	void DrawBone(const Vector3& From,const Vector3& To, bool bRoot);
	
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


protected:
	ZShaderSystem*					m_pkShader;
	vector<int>				m_kActiveMesh;

	vector<RenderPackage>	m_kRenderPackage;			// VimRp - Our list of all renderpackages.
	BoneTransform	kFinalBoneTransform[MAX_BONES];		// VimRp - We now store bonetransforms in each mad_modell as they are to be used later by renderpackages.
	Matrix4			kFinalMatrixTransform[MAX_BONES];	// VimRp2 - Bone transformation matrix.


public:
	string	m_kMadFile;

	int		iActiveAnimation;
	float		fCurrentTime;
	float		m_fScale;
	int		m_iNextAnimation;
	bool		m_bLoop;
	bool		m_bActive;			// True if animation system is active.
	int		m_iFirstMaterialID;
	bool		m_bBlended;
	
	AnimInfo	m_kLastAnim;
	float		m_fAnimTrans;		// 0.0 - 1.0

	//	Mad_Core*	pkCore;
	ZFResourceHandle	kMadHandle;
	
	Mad_Modell();
	~Mad_Modell();	

//	Mad_Modell(Mad_Core* pkModell);
//	Mad_Modell(string strResName);

	void SetBasePtr(string strResName);

//	void SetBasePtr(Mad_Core* pkModell);			// Set base modell to use.
	void UpdateAnimation(float dDelta);		// Move all animations forward.
	
	void PlayAnimation(int iAnimNum, float fStartTime);
	void PlayAnimation(const char* szName, float fStartTime);
	int  GetCurrentAnimation();
	string GetCurrentAnimationName();
	void SetLoopedStatus(bool bLoop);
	bool IsLooped();

	void SetNextAnimation(int iAnimNum);
	virtual void SetNextAnimation(const char* szName);
	void PlayNextAnimations(void);
	int  GetNextAnimation();

	void SetAnimationActive(bool bPause);
	bool GetAnimationActive();

	void NextCoreAnimation(void);
		
	void SetScale(float fScale);
	float GetScale();

	void SetReplaceTexture(char* szName);
	void SetReplaceTexture(char* szOrgName, char* szNew);
	ZMaterial* GetMaterial(int iMesh,int iSubMesh);

	void UpdateBones();
	void Draw_All(int iDrawFlags = MAD_DRAW_MESH);	// Draw modell.
	void Draw_All_RenderP();	
	
	float GetRadius();
	float GetSize();

	bool RemoveMesh(int iId);	
	bool AddMesh(char* szName);	
	bool RemoveMesh(char* szName);	
	bool IsMeshActive(int iId);	
	bool IsMeshActive(char* szName);

	void ClearAllMesh();
	void SetRawMesh(Mad_CoreMesh* pkMesh);

	virtual bool AddMesh(int iSId);

	float fRenderDistance;


	friend class P_Mad;
};


#endif










