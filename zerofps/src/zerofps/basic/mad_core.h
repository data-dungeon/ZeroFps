#ifndef FILE_BASIC_MADCORE_H
#define FILE_BASIC_MADCORE_H


#include <vector>
using namespace std;
#include "basicmath.pkg"
#include "basic_x.h"

#define MAD_MAX_ANIMATIONNAME	256
#define MAD_MAX_TEXTURENAME		64
#define MAD_MAX_TEXTURES		64
#define MAX_MAD_TEXTURES		256
#define MAX_BONES	256

#define MAX_MAX_VERTEX			8192	// Max Vertex per mesh.

#define MAD_VERSION				1

// MAD - MD (Mesh Data)
struct BASIC_API Mad_TextureCoo
{
	float	s;							// s Texture Coo.
	float	t;							// t Texture Coo.
};

struct BASIC_API Mad_Face
{
	int		iIndex[3];					// Vertex Index of Face.
};

struct BASIC_API Mad_CoreMeshHeader
{
	int		iVersionNum;				// Version num.
	int		iNumOfTextures;				// Num of textures used by mesh.
	int		iNumOfVertex;				// Num of vertices in mesh.
	int		iNumOfFaces;				// Num of triangles in mesh.
	int		iNumOfFrames;				// Num of vertex frames.
	int		iNumOfSubMeshes;			// Num of texture coo.
	int		iNumOfAnimation;			// Antal Animationer.
};

struct BASIC_API Mad_CoreTexture
{
	bool	bIsAlphaTest;				// True if needs alpha test.
	bool	bTwoSided;					// True if two sided.
	bool	bClampTexture;				// True if texture should clamp texture coo.
	char	ucTextureName[64];			// path/name of texture.
};

class BASIC_API Mad_CoreVertexFrame
{
private:
	vector<Vector3>	akVertex;			// Array of all vertices.
	vector<Vector3>	akNormal;			// Array of all normals.
	
public:
	Mad_CoreVertexFrame() ;
	~Mad_CoreVertexFrame();

	void operator=(const Mad_CoreVertexFrame& kOther);

	int Size();
	void Resize(int iNewSize);
	void Clear();
	void PushBack(Vector3 kVertex, Vector3 kNormal);

	friend class Mad_CoreMesh;
	friend class Mad_Core;
};

class BASIC_API Mad_CoreKeyFrame
{
public:
	int		iVertexFrame;				// 
	float	fFrameTime;					// Antal Sekunder denna frame ska vara.

	void Clear(void);
	void operator=(const Mad_CoreKeyFrame& kOther);

	friend class Mad_CoreMesh;
};

class BASIC_API Mad_CoreMeshAnimation
{
private:
	vector<Mad_CoreKeyFrame>	KeyFrame;	

public:
	char						Name[MAD_MAX_ANIMATIONNAME];

	void Clear(void);
	void operator=(const Mad_CoreMeshAnimation& kOther);

	friend class Mad_CoreMesh;

	int Size();
	void Resize(int iNewSize);
	void PushBack(Mad_CoreKeyFrame kKeyFrame);

	friend class Mad_Core;

};

struct BASIC_API Mad_CoreSubMesh
{
	int		iTextureIndex;				// Texture used.
	int		iFirstTriangle;				// First triangle to use texture.
	int		iNumOfTriangles;			// Num of triangles that use texture.
};

class BASIC_API Mad_CoreMesh
{ 
private:
	vector<Mad_CoreTexture>			akTextures;				
	vector<Mad_TextureCoo>			akTextureCoo;
	vector<Mad_Face>				akFaces;
	vector<Mad_CoreVertexFrame>		akFrames;
	vector<Mad_CoreSubMesh>			akSubMeshes;
	vector<Mad_CoreMeshAnimation>	akAnimation;
	vector<int>						akBoneConnections;

	int								iTextureID[256];

public:
	bool							bNotAnimated;
	int								iDisplayID;
	void							SetDisplayID(int iId) { iDisplayID = iId; }
	int								GetDisplayID() { return iDisplayID; }

	
	char							m_acName[64];
	Mad_CoreMeshHeader				kHead;

	Mad_CoreMesh();
	~Mad_CoreMesh();
	void Clear(void);
	void operator=(const Mad_CoreMesh& kOther);

	void ResizeTextures(int iNewSize);
	void ResizeTexturesCoo(int iNewSize);
	void ResizeFaces(int iNewSize);
	void ResizeFrames(int iNewSize);
	void ResizeSubMesh(int iNewSize);
	void ResizeAnimations(int iNewSize);
	void ResizeBoneConnections(int iNewSize);
	
	int SizeTextures();
	int SizeTexturesCoo();
	int SizeFaces();
	int SizeFrames();
	int SizeSubMesh();
	int SizeAnimations();
	int SizeBoneConnections();

	int NumOfVertexPerFrame();

	void PushBackTexture(Mad_CoreTexture kTexture);
	void PushBackTextureCoo(Mad_TextureCoo kTextureCoo);
	void PushBackFaces(Mad_Face kFace);
	void PushBackFrames(Mad_CoreVertexFrame kFrames);
	void PushBackSubMeshes(Mad_CoreSubMesh kSubMesh);
	void PushBackAnimation(Mad_CoreMeshAnimation kAnimation);
	void PushBackBoneConnection(int iBoneConnection);

	Mad_TextureCoo* GetTexCoo();
	Mad_CoreSubMesh* GetSubMesh(int iSubMesh);
	Mad_Face* GetFace(int iFace);
	char* GetTextureName(int iTextureIndex);
	Mad_CoreTexture* GetTextureInfo(int iTextureIndex);
	void SetTextureID(int iTextureIndex, int iID);
	int GetTextureID(int iTextureIndex);

	void CreateRigidBoneConnections(int iBoneId = 0);
	void Save(FILE* fp);
	void Load(FILE* fp);
	void ShowInfo(void);
	Mad_CoreMeshAnimation*	Mad_CoreMesh::GetAnimation(char* ucaName);
	void CreateVertexNormals();
	void OptimizeSubMeshes();
	int	AddTexture(char* ucpTextureName);

	friend class Mad_Core;

};

// MAD - SD (Skeletal Data)
class BASIC_API Mad_CoreBone
{
public:
	char				m_acName[32];
	int					m_iParent;
	Vector3				m_kPosition;
	Vector3				m_kRotation;

	Vector3				m_kPosScale;
	Vector3				m_kRotScale;

	Mad_CoreBone();
	~Mad_CoreBone();

	void Clear(void);
	void operator=(const Mad_CoreBone& kOther);
};

// MAD - AD (Bone Animation Data)
class BASIC_API Mad_CoreBoneKey
{
public:
	Vector3				m_kPosition;
	Vector3				m_kRotation;

	Mad_CoreBoneKey() { }
	~Mad_CoreBoneKey() { }

	void Clear(void) { }
	void operator=(const Mad_CoreBoneKey& kOther)
	{
		m_kPosition = kOther.m_kPosition;
		m_kRotation = kOther.m_kRotation;
	}

	void Save(FILE* fp);
	void Load(FILE* fp);
	friend class Mad_Core;

};

class BASIC_API Mad_CoreBoneKeyFrame
{
private:
	vector<Mad_CoreBoneKey>		m_kBonePose;	

public:

	Mad_CoreBoneKeyFrame() { }
	~Mad_CoreBoneKeyFrame() { }

	void Clear(void) 
	{
		m_kBonePose.clear();
	}

	void operator=(const Mad_CoreBoneKeyFrame& kOther)
	{
		m_kBonePose = kOther.m_kBonePose;
	}

	void Save(FILE* fp);
	void Load(FILE* fp);

	int Size();
	void Resize(int iNewSize);
	void PushBack(Mad_CoreBoneKey kBoneKey);
	friend class Mad_Core;

};

class BASIC_API Mad_CoreBoneAnimation
{
private:
	vector<Mad_CoreBoneKeyFrame>	m_kBoneKeyFrames;

public:
	char	m_szName[MAD_MAX_ANIMATIONNAME];

	Mad_CoreBoneAnimation();
	~Mad_CoreBoneAnimation();
	void Clear(void);
	void operator=(const Mad_CoreBoneAnimation& kOther);

	void Save(FILE* fp);
	void Load(FILE* fp);

	int Size();
	void Resize(int iNewSize);
	void PushBack(Mad_CoreBoneKeyFrame kBoneKeyFrame);
	friend class Mad_Core;


};

// MAD File 
struct BASIC_API Mad_Header
{
	int	m_iVersionNum;									// MAD_VERSION
	
	unsigned int	m_iNumOfMeshes;						// Num of meshes in this file.
	unsigned int	m_iNumOfAnimations;					// Num of bone animations in this file.
};


class BASIC_API Mad_Core
{
private:
	vector<Mad_CoreBone>			m_kSkelleton;		// Skelleton for modell.
	vector<Mad_CoreBoneAnimation>	m_kBoneAnim;		// List of all bone animations for modell.
	vector<Mad_CoreMesh>			m_kMesh;			// List of all mesh object for modell.

	Matrix4		g_MadkbonetransformI[MAX_BONES];		// Inverse bone transformation matrix for bind pose

	// Radius from local origo that contain all of modell.
	float							m_fBoundRadius;

public:
	Mad_Header	kMadHeader;

	char		Name[256];
	float		fFrameOffs;
	int			iBoneFrame;

	int			iActiveAnimation;
	int			iActiveFrame;
	int			iActiveKeyFrame;
	float		fActiveAnimationTime;
	
	Mad_Core();
	~Mad_Core();
	
	Mad_CoreMesh*	GetMeshByID(int iMesh);
	Mad_CoreMesh* CreateCoreMesh(const char* szName);
	int	NumOfMeshes();

	void Save_SD(FILE* pkFp);
	void Save_MD(int iMeshId, FILE* pkFp);
	void Save_AD(int iAnimId, FILE* pkFp);

	void Save_SD(const char* filename);
	void Save_MD(int iMeshId, const char* filename);
	void Save_AD(int iMeshId, const char* filename);
	void Save_MAD(const char* filename);

	void LoadSkelleton(FILE* pkFp);
	void LoadAnimation(FILE* pkFp); 
	void LoadMesh(FILE* pkFp);

	void LoadSkelleton(const char* MadFileName);
	void LoadAnimation(const char* MadFileName); 
	void LoadMesh(const char* MDFileName);
	bool LoadMad(const char* MadFileName);

	void SetAnimationTime(int iAnim, float fTime );
	void SetBoneAnimationTime(int iAnim, float fTime );

	bool m_bInterpolVertexFrames;
	bool m_bDrawNormals;

	int	 aiTextureIndex[MAD_MAX_TEXTURES];
	int	 aiReplaceTextureIndex[MAD_MAX_TEXTURES];
	void ClearReplaceTexture(void);
	void SetReplaceTexture(char* szFileName);

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

	float GetAnimationLengthInS(int iAnim);
	int	  GetAnimationTimeInFrames(int iAnim);
	void  SetupBonePose();
	void  SetUpBindPose();
	int	  GetNumOfAnimations();

	void SetInterpol(bool bNewInterpol) { m_bInterpolVertexFrames = bNewInterpol; }
	bool GetInterpol() { return m_bInterpolVertexFrames; }

	int GetNumOfBones();
	Vector3 GetBonePosition(int iBone);
	int GetBoneParent(int iBone);

	void SetSkelleton(vector<Mad_CoreBone>	kSkelleton);
	void SetMesh(vector<Mad_CoreMesh>	kMesh);
	void SetAnimation(vector<Mad_CoreBoneAnimation>	kBoneAnim);

	void PrintCoreInfo();

	// Render Interface
	Vector3*  GetVerticesPtr();
	Vector3* GetNormalsPtr();
	Mad_TextureCoo* GetTextureCooPtr(Mad_CoreMesh* pkMesh);
	int* GetFacesPtr(Mad_CoreMesh* pkMesh,Mad_CoreSubMesh* pkSubMesh);
	int GetTextureID();
	void PrepareMesh(Mad_CoreMesh* pkMesh);

	float GetRadius();
	void CalculateRadius();
};

#endif








