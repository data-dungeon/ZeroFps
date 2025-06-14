#ifndef _BASIC_MADCORE_H
#define _BASIC_MADCORE_H



#include <vector>
#include "../engine_systems_x.h"
#include "../../basic/zfresource.h"
#include "../../engine/fh.h"
#include "../../basic/matrix4.h"
#include "../../basic/zfvfs.h"
#include "../../basic/quaternion.h"

using namespace std;

#define MAD_MAX_ANIMATIONNAME		256		 
#define MAD_MAX_NAME					64		
#define MAD_MAX_TEXTURENAME		64		// Max Size p� texturename/path.
#define MAD_MAX_TEXTURES			64		// Max textures in one Mesh.
#define MAX_BONES						256	// Max Bones in skelleton.
#define MAX_JOINTNAME				32		// Max Joint Name Size.
#define MAX_MAX_VERTEX				8192	// Max Vertex per mesh.

#define MAD_VERSION					3		// Current Versions.
#define MAD_SD_VERSION				1
#define MAD_AD_VERSION				1
#define MAD_MD_VERSION				1

#define MAD_NOANIMINDEX				-1
#define MAD_NOLOOP					-2

struct BoneTransform
{
	Quaternion	m_kRot;
	Vector3		m_kPos;
};

// MAD - MD (Mesh Data)

//! Texture coo.
struct ENGINE_SYSTEMS_API Mad_TextureCoo
{
	float	s;							// s Texture Coo.
	float	t;							// t Texture Coo.
};

//! A Face in a MAD (with index to vertices).
struct ENGINE_SYSTEMS_API Mad_Face
{
	int		iIndex[3];					// Vertex Index of Face.
};

//! File Header for a MAD Mesh.
struct ENGINE_SYSTEMS_API Mad_CoreMeshHeader
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
class ENGINE_SYSTEMS_API Mad_CoreTexture
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
class ENGINE_SYSTEMS_API Mad_CoreVertexFrame
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
	
	vector<Vector3>* GetVertexPointer() {return &akVertex;		};
	vector<Vector3>* GetNormalPointer() {return &akNormal;		};
	Vector3* GetVertexPointer2()			{return &akVertex[0];	};
	Vector3* GetNormalPointer2()			{return &akNormal[0];	};

	int  GetSizeInBytes();					

	friend class Mad_CoreMesh;
	friend class Mad_RawMesh;
	friend class Mad_Core;
	friend class Body;
	friend class Modell3DS;
};

/// Key frame in a Vertex Animation.
class ENGINE_SYSTEMS_API Mad_CoreKeyFrame
{
public:
	int		iVertexFrame;				// 
	float	fFrameTime;					// Antal Sekunder denna frame ska vara.

	void Clear(void);
	void operator=(const Mad_CoreKeyFrame& kOther);

	friend class Mad_CoreMesh;
	friend class Mad_RawMesh;
};

/// A Vertex animation with all keys.
class ENGINE_SYSTEMS_API Mad_CoreMeshAnimation
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

	int  GetSizeInBytes();					

	friend class Mad_Core;
	friend class Mad_RawMesh;
	friend class Modell3DS;

};

/// SubMesh in a Mad.
/// A Submesh is part of a mesh were all surfaces has the same material properties.
struct ENGINE_SYSTEMS_API Mad_CoreSubMesh
{
	int		iTextureIndex;				///< Texture used.
	int		iFirstTriangle;				///< First triangle to use texture.
	int		iNumOfTriangles;			///< Num of triangles that use texture.
};

class ENGINE_SYSTEMS_API Mad_RawMesh
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
	Mad_RawMesh();
	Mad_RawMesh(const Mad_RawMesh& kOther);

	Mad_CoreMeshHeader				kHead;
	float									m_fMaxDistance;	// Max distance this LOD level is used.

	void Clear(void);
	void operator=(const Mad_RawMesh& kOther);

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

	void PushBackTexture(Mad_CoreTexture kTexture);
	void PushBackTextureCoo(Mad_TextureCoo kTextureCoo);
	void PushBackFaces(Mad_Face kFace);
	void PushBackFrames(Mad_CoreVertexFrame kFrames);
	void PushBackSubMeshes(Mad_CoreSubMesh kSubMesh);
	void PushBackAnimation(Mad_CoreMeshAnimation kAnimation);
	void PushBackBoneConnection(int iBoneConnection);

	void FlipFaces();

	Mad_TextureCoo* GetTexCoo();
	Mad_CoreSubMesh* GetSubMesh(int iSubMesh);
	Mad_Face* GetFace(int iFace);
	char* GetTextureName(int iTextureIndex);
	Mad_CoreTexture* GetTextureInfo(int iTextureIndex);
	void SetTextureID(int iTextureIndex, int iID);
	int GetTextureID(int iTextureIndex);
	bool					SetTextureHandle(int iTextureIndex, string strName);
	ZFResourceHandle*	GetTextureHandle(int iTextureIndex);

	vector<Mad_Face>* GetFacesPointer()							{return &akFaces;		};
	vector<Mad_CoreVertexFrame>* GetVertexFramePointer()	{return &akFrames;	};	

	void CreateRigidBoneConnections(int iBoneId = 0);
	Mad_CoreMeshAnimation*	GetAnimation(char* ucaName);
	int	AddTexture(char* ucpTextureName);
	void CreateVertexNormals();
	void  OptimizeSubMeshes();

	void Save(int iVersion, ZFVFile* pkZFile);
	void Load(int iVersion, ZFVFile* pkZFile);

	void SetTextureFlags(void);
	int NumOfVertexPerFrame();
	int  GetSizeInBytes();

	string ShowInfo(void);

	friend class Body;
	friend class Mad_Core;
	friend class Mad_Modell;
};

/// A Mesh in a MAD. 
class ENGINE_SYSTEMS_API Mad_CoreMesh
{ 
public:
	// Data
	int									m_iMadVersion;					///< Version num loaded from file.
	vector<Mad_RawMesh>				m_kLodMesh;						///< All lod levels of this mesh.
	bool									bNotAnimated;					///< True if this is a static mesh that we could put in a display list.
	char									m_acName[MAD_MAX_NAME];		///< Name of mesh.

// Constructors
	Mad_CoreMesh();
	~Mad_CoreMesh();
	Mad_CoreMesh(const Mad_CoreMesh& kOther);

// Assignment 
	void operator=(const Mad_CoreMesh& kOther);
	void Clear(void);

// Access 
	Mad_RawMesh* GetLODMesh(int iId);
	Mad_RawMesh* GetLODRender(float fDistance);
	int GetLODRenderIndex(float fDistance);

// Load/Save
	void Save(ZFVFile* pkZFile);
	void Load(ZFVFile* pkZFile);

// Info
	void ShowInfo(void);
	int  GetSizeInBytes();

	friend class Mad_Core;
	friend class Body;
	friend class Modell3DS;
};

// MAD - SD (Skeletal Data)

/// A Bone in a Bones animated Mesh.
class ENGINE_SYSTEMS_API Mad_CoreBone
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
/// Key for a bone. 
class ENGINE_SYSTEMS_API Mad_CoreBoneKey
{
public:
	Vector3				m_kPosition;		// Position of bone.
	Vector3				m_kRotation;		// Rotation of bone.
	Quaternion			m_kQuatRotation;	// Rotation as a quat.

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

/// All keys for a single bone in one animation.
class ENGINE_SYSTEMS_API Mad_CoreBoneKeyFrame
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
	int  GetSizeInBytes();

	friend class Mad_Core;
	friend class ModellHalfLife;
};

/// Contains all bone keyframes in a bone animation.
class ENGINE_SYSTEMS_API Mad_CoreBoneAnimation
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
	int  GetSizeInBytes();
	
	friend class Mad_Core;
	friend class ModellHalfLife;	
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
struct ENGINE_SYSTEMS_API Mad_Header
{
	int	m_iVersionNum;									// MAD_VERSION
	
	unsigned int	m_iNumOfMeshes;						// Num of meshes in this file.
	unsigned int	m_iNumOfAnimations;					// Num of bone animations in this file.
};

/// 3D Modells with support for Skellet animation. 
class ENGINE_SYSTEMS_API Mad_Core : public ZFResource
{
private:
	vector<Mad_CoreBone>					m_kSkelleton;								// Skelleton for modell.
	vector<Mad_CoreBoneAnimation>		m_kBoneAnim;								// List of all bone animations for modell.
	vector<Mad_CoreMesh>					m_kMesh;										// List of all mesh object for modell.

	// Inverse bone transformation matrix for bind pose
	Matrix4									m_MadkbonetransformI[MAX_BONES];

	// Radius from local origo that contain all of modell.
	float										m_fBoundRadius;

	// longest distance from two vertises / 2
	float										m_fSize;

	int										iStartFrame;
	int										iEndFrame;
	
	bool										m_bLoaded;

public:
	Mad_Header								kMadHeader;

	char										Name[256];
	float										fFrameOffs;
	int										iBoneFrame;

	int										iActiveAnimation;
	int										iActiveFrame;
	int										iActiveKeyFrame;
	float										fActiveAnimationTime;
	
	bool										m_bInterpolVertexFrames;
	bool										m_bDrawNormals;

	int										aiTextureIndex[MAD_MAX_TEXTURES];
	int										aiReplaceTextureIndex[MAD_MAX_TEXTURES];

	vector<Controller>					m_kControllers;

	Mad_Core();
	~Mad_Core();
	
	Mad_CoreMesh*	GetMeshByID(int iMesh);
	int	GetMeshIDByName(char* szName);
	Mad_CoreMesh* CreateCoreMesh(const char* szName);
	int	NumOfMeshes()														{	return m_kMesh.size();	};

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
	
	bool IsValid()	const								{	return m_bLoaded;	};			
	bool Create(const string& strName);
	int CalculateSize();

//	void SetAnimationTime(int iAnim, float fTime );
	void SetBoneAnimationTime(int iAnim, float fTime, bool bLoop );

	void ClearReplaceTexture(void);
	void SetReplaceTexture(char* szFileName);

//	void SetFrameI(int iFrame);
	/* Sets current frame to use for rendering */
	void SetFrame_NormalizedTime(float fNormTime);
	/* Sets current frame to use for rendering as a 0.0 - 1.0 value for whole animation.
		Values above one or below zero will wrapp to a valid value */
	void SetFrame_InSeconds(float fSeconds);
	/* Sets animation time as a num of seconds. Values outside current anim length will wrap
		to valid values. */

//	void LoopPlayAnim(int iAnim);

	char* GetName(void);

	float GetAnimationLengthInS(int iAnim);
//	int	  GetAnimationTimeInFrames(int iAnim);
	int	GetAnimIndex(const char* szName);
	string GetAnimationName(int iIndex);
	void  SetupBonePose(BoneTransform* pkBones);
	void  GenerateBoneMatris(BoneTransform* pkBones, Matrix4* pkMatrix);	// VimRp2
	void InterPolTransforms(BoneTransform* pkResultat, BoneTransform* pkFrom, BoneTransform* pkTo, float fDelta);

	void  SetUpBindPose();
	int	  GetNumOfAnimations();

	void SetInterpol(bool bNewInterpol) { m_bInterpolVertexFrames = bNewInterpol; }
	bool GetInterpol() { return m_bInterpolVertexFrames; }

	unsigned int GetNumOfBones();
	Vector3 GetBonePosition(int iBone);
	Matrix4 GetBoneTransform(int iBone);
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
	void PrepareMesh(Mad_CoreMesh* pkMesh, Mad_RawMesh* pkRawMesh);

	float GetRadius();
	float GetSize()																						{	return m_fSize;	};
	void CalculateRadius();

	int GetJointID(const char* szJointName);
	vector<int> GetJointSelection(const char* szJointNames);
	vector<int> GetJointAndChildID(const char* szJointName);
	vector<int> GetAllJointID();
	Vector3 GetJointPosition(char* szJointName);	// Use NULL to get pos for first root joint.

	void CreateController(char* szName, char* szJoint, ControllAxis eAxis, float fMin, float fMax);
	void SetControll(char* szName, float fValue);

	float		fRenderDistance;
	Mad_RawMesh*  g_pkSelectedRawMesh;

	friend class Body;
	friend class ModellXXX;
};

ENGINE_SYSTEMS_API ZFResource* Create__Mad_Core();


#endif





