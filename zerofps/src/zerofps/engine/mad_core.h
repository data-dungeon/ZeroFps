#ifndef _BASIC_MADCORE_H
#define _BASIC_MADCORE_H


#include <vector>
using namespace std;
#include "../basic/basic.pkg"
#include "engine_x.h"
#include "zfresource.h"
#include "fh.h"

#define MAD_MAX_ANIMATIONNAME	256		 
#define MAD_MAX_NAME			64		
#define MAD_MAX_TEXTURENAME		64		// Max Size p� texturename/path.
#define MAD_MAX_TEXTURES		64		// Max textures in one Mesh.
//#define MAX_MAD_TEXTURES		256		// Max textures in one Mesh.
#define MAX_BONES				256		// Max Bones in skelleton.
#define MAX_JOINTNAME			32		// Max Joint Name Size.
#define MAX_MAX_VERTEX			8192	// Max Vertex per mesh.

#define MAD_VERSION				1		// Current Versions.
#define MAD_SD_VERSION			1
#define MAD_AD_VERSION			1
#define MAD_MD_VERSION			1

// MAD - MD (Mesh Data)

//! Texture coo.
struct ENGINE_API Mad_TextureCoo
{
	float	s;							// s Texture Coo.
	float	t;							// t Texture Coo.
};

//! A Face in a MAD (with index to vertices).
struct ENGINE_API Mad_Face
{
	int		iIndex[3];					// Vertex Index of Face.
};

//! File Header for a MAD Mesh.
struct ENGINE_API Mad_CoreMeshHeader
{
	int		iVersionNum;				// Version num.
	int		iNumOfTextures;				// Num of textures used by mesh.
	int		iNumOfVertex;				// Num of vertices in mesh.
	int		iNumOfFaces;				// Num of triangles in mesh.
	int		iNumOfFrames;				// Num of vertex frames.
	int		iNumOfSubMeshes;			// Num of texture coo.
	int		iNumOfAnimation;			// Num of animations.
};

//! Information about a texture used by a MAD (no raw texture data).
class ENGINE_API Mad_CoreTexture
{
public:
	bool	bIsAlphaTest;						// True if needs alpha test.
	bool	bTwoSided;							// True if two sided.
	bool	bClampTexture;						// True if texture should clamp texture coo.
	char	ucTextureName[MAD_MAX_TEXTURES];	// path/name of texture.

	Mad_CoreTexture() 
	{
		bIsAlphaTest	= false;
		bTwoSided		= false;
		bClampTexture	= false;
	}
};

/// Mesh VertexFrame
class ENGINE_API Mad_CoreVertexFrame
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
	
	vector<Vector3>* GetVertexPointer() {return &akVertex;};
	vector<Vector3>* GetNormalPointer() {return &akNormal;};
	Vector3* GetVertexPointer2() {return &akVertex[0];};
	Vector3* GetNormalPointer2() {return &akNormal[0];};

	friend class Mad_CoreMesh;
	friend class Mad_Core;
	friend class Body;
};

class ENGINE_API Mad_CoreKeyFrame
{
public:
	int		iVertexFrame;				// 
	float	fFrameTime;					// Antal Sekunder denna frame ska vara.

	void Clear(void);
	void operator=(const Mad_CoreKeyFrame& kOther);

	friend class Mad_CoreMesh;
};

class ENGINE_API Mad_CoreMeshAnimation
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

/// SubMesh in a Mad. A Submesh is part of a mesh were all surfaces has
/// the same material properties.
struct ENGINE_API Mad_CoreSubMesh
{
	int		iTextureIndex;				///< Texture used.
	int		iFirstTriangle;				///< First triangle to use texture.
	int		iNumOfTriangles;			///< Num of triangles that use texture.
};

/// A Mesh in a MAD. 
class ENGINE_API Mad_CoreMesh
{ 
private:
	vector<Mad_CoreTexture>			akTextures;						///< Texturers used in mesh.			
	vector<Mad_TextureCoo>			akTextureCoo;					///< Texture Coo for mesh.
	vector<Mad_Face>					akFaces;							///< Faces in mesh.
	vector<Mad_CoreVertexFrame>	akFrames;						///< Vertex frames for mesh.
	vector<Mad_CoreSubMesh>			akSubMeshes;					///< Submeshes.
	vector<Mad_CoreMeshAnimation>	akAnimation;					///< Animations.
	vector<int>							akBoneConnections;			///< Vertex -> Bone index.

	int									iTextureID[256];				///< Texture ID's Assigned by rendering sys.
	ZFResourceHandle					akTexturesHandles[256];		// Texture resurs handles

public:
	bool							bNotAnimated;		///< True if this is a static mesh that we could put in a display list.
	int								iDisplayID;			///< Display List ID if any.
	void							SetDisplayID(int iId) { iDisplayID = iId; }
	int								GetDisplayID() { return iDisplayID; }

	char							m_acName[MAD_MAX_NAME];		///< Name of mesh.
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

	Mad_CoreSubMesh*		GetSubMeshPointer()	{	return &akSubMeshes[0];		};
	Mad_Face*				GetFacesPointer2()	{	return &akFaces[0];			};
	Mad_CoreVertexFrame*	GetFramesPointer2()	{	return &akFrames[0];			};
	Mad_TextureCoo*		GetTexCooPointer2()	{	return &akTextureCoo[0];			};
	Mad_CoreTexture*		GetTexture2()			{	return &akTextures[0];			};


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
	
	void					SetTextureHandle(int iTextureIndex, string strName);
	ZFResourceHandle*	GetTextureHandle(int iTextureIndex);

	vector<Mad_Face>* GetFacesPointer() {return &akFaces;};
	vector<Mad_CoreVertexFrame>* GetVertexFramePointer() {return &akFrames;};	

	void CreateRigidBoneConnections(int iBoneId = 0);
	void Save(ZFVFile* pkZFile);
	void Load(ZFVFile* pkZFile);
	void ShowInfo(void);
	Mad_CoreMeshAnimation*	Mad_CoreMesh::GetAnimation(char* ucaName);
	void CreateVertexNormals();
	void OptimizeSubMeshes();
	int	AddTexture(char* ucpTextureName);

	void SetTextureFlags(void);

	void FlipFaces();

	friend class Mad_Core;
	friend class Body;
};

// MAD - SD (Skeletal Data)

/// A Bone in a Bones animated Mesh.
class ENGINE_API Mad_CoreBone
{
public:
	char				m_acName[32];		// Name of Joint / Bone.
	int					m_iParent;			// Parent Joint to this one.
	Vector3				m_kPosition;		// Bind Pose position of Bone.
	Vector3				m_kRotation;		// Bind Pose potation of Bone.

	Vector3				m_kPosScale;		// ??
	Vector3				m_kRotScale;		// ??

	Mad_CoreBone();
	~Mad_CoreBone();

	void Clear(void);
	void operator=(const Mad_CoreBone& kOther);
};

// MAD - AD (Bone Animation Data)
class ENGINE_API Mad_CoreBoneKey
{
public:
	Vector3				m_kPosition;		// Position of bone.
	Vector3				m_kRotation;		// Rotation of bone.

	Mad_CoreBoneKey() { }
	~Mad_CoreBoneKey() { }

	void Clear(void) { }
	void operator=(const Mad_CoreBoneKey& kOther)
	{
		m_kPosition = kOther.m_kPosition;
		m_kRotation = kOther.m_kRotation;
	}

	void Save(ZFVFile* pkZFile);
	void Load(ZFVFile* pkZFile);
	friend class Mad_Core;

};

class ENGINE_API Mad_CoreBoneKeyFrame
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

	void Save(ZFVFile* pkZFile);
	void Load(ZFVFile* pkZFile);

	int Size();
	void Resize(int iNewSize);
	void PushBack(Mad_CoreBoneKey kBoneKey);
	friend class Mad_Core;

};

/// Contains all bone keyframes in a bone animation.
class ENGINE_API Mad_CoreBoneAnimation
{
private:
	vector<Mad_CoreBoneKeyFrame>	m_kBoneKeyFrames;		///< vector of all bone key frames.

public:
	char	m_szName[MAD_MAX_ANIMATIONNAME];				///< Name of this animation.

	Mad_CoreBoneAnimation();				
	~Mad_CoreBoneAnimation();
	void Clear(void);
	void operator=(const Mad_CoreBoneAnimation& kOther);

	void Save(ZFVFile* pkZFile);									///< Save animation	
	void Load(ZFVFile* pkZFile);									///< Load animation

	int Size();												
	void Resize(int iNewSize);
	void PushBack(Mad_CoreBoneKeyFrame kBoneKeyFrame);
	friend class Mad_Core;
};

enum ControllAxis
{
	CONTROLL_ANGLE_X,
	CONTROLL_ANGLE_Y,
	CONTROLL_ANGLE_Z,
	CONTROLL_MOVE_X,
	CONTROLL_MOVE_Y,
	CONTROLL_MOVE_Z
};

struct Controller
{
	char			m_szName[MAD_MAX_NAME];
	char			m_szJointName[MAX_JOINTNAME];
	int				m_iJointID;
	ControllAxis	m_eAxis;
	float			m_fMin;
	float			m_fMax;
	float			m_fValue;
};

/// Header for a MAD File 
struct ENGINE_API Mad_Header
{
	int	m_iVersionNum;									// MAD_VERSION
	
	unsigned int	m_iNumOfMeshes;						// Num of meshes in this file.
	unsigned int	m_iNumOfAnimations;					// Num of bone animations in this file.
};

/// 3D Modells with support for Skellet animation. 
class ENGINE_API Mad_Core : public ZFResource
{
private:
	vector<Mad_CoreBone>			m_kSkelleton;		// Skelleton for modell.
	vector<Mad_CoreBoneAnimation>	m_kBoneAnim;		// List of all bone animations for modell.
	vector<Mad_CoreMesh>			m_kMesh;			// List of all mesh object for modell.

	// Inverse bone transformation matrix for bind pose
	Matrix4							m_MadkbonetransformI[MAX_BONES];

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
	int	GetMeshIDByName(char* szName);
	Mad_CoreMesh* CreateCoreMesh(const char* szName);
	int	NumOfMeshes();

	void Save_SD(ZFVFile* pkZFile);
	void Save_MD(int iMeshId, ZFVFile* pkZFile);
	void Save_AD(int iAnimId, ZFVFile* pkZFile);

	void Save_SD(const char* filename);
	void Save_MD(int iMeshId, const char* filename);
	void Save_AD(int iMeshId, const char* filename);
	void Save_MAD(const char* filename);

	void LoadSkelleton(ZFVFile* pkZFile);
	void LoadAnimation(ZFVFile* pkZFile); 
	void LoadMesh(ZFVFile* pkZFile);

	void LoadSkelleton(const char* MadFileName);
	void LoadAnimation(const char* MadFileName); 
	void LoadMesh(const char* MDFileName);
	//bool LoadMad(const char* MadFileName);
	bool Create(string strName);

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

	unsigned int GetNumOfBones();
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

	int GetJointID(char* szJointName);
	Vector3 GetJointPosition(char* szJointName);	// Use NULL to get pos for first root joint.

	vector<Controller>	m_kControllers;
	void CreateController(char* szName, char* szJoint, ControllAxis eAxis, float fMin, float fMax);
	void SetControll(char* szName, float fValue);

	friend class Body;
};

#endif





