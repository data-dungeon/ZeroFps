#ifndef FILE__MAD_H
#define FILE__MAD_H

#define MAX_MAD_TEXTURES	256

#include <vector>
using namespace std;
#include "../../zerofps/basic/basicmath.pkg"

class MadExporter;

#define MAD_MAX_ANIMATIONNAME	256
#define MAD_MAX_TEXTURENAME		64
#define MAD_MAX_TEXTURES		64

#define MAX_MAX_VERTEX			8192	// Max Vertex per mesh.


/*
Bas klass för alla klasser som kan importeras till MAD.
*/
class IMadImport
{
public:
	virtual ~IMadImport() {};
	// Read data in own format to this.
	virtual	void Read( const char* filename )	= 0;	
	// Export this to mad.
	virtual	bool Export(MadExporter* mad, const char* filename) = 0;
};

// MAD FILE STRUCTURES.


// MAD - MD
struct MadTextureCoo
{
	float s,t;
};

struct MadFace
{
	int iIndex[3];
};

struct Mad_CoreMeshHeader
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


class Mad_VertexFrame
{
public:
	Mad_VertexFrame() ;
	~Mad_VertexFrame();

	void operator=(const Mad_VertexFrame& kOther);
	vector<Vector3>	akVertex;
	vector<Vector3>	akNormal;
	
};

class Mad_KeyFrame
{
public:
	void Clear(void);
	void operator=(const Mad_KeyFrame& kOther);

	int		iVertexFrame;				// 
	float	fFrameTime;					// Antal Sekunder denna frame ska vara.
};

class Mad_CoreMeshAnimation
{
public:
	void Clear(void);
	void operator=(const Mad_CoreMeshAnimation& kOther);

	char	Name[MAD_MAX_ANIMATIONNAME];
	vector<Mad_KeyFrame>	KeyFrame;	
};

struct Mad_CoreSubMesh
{
	int		iTextureIndex;				// Texture used.
	int		iFirstTriangle;				// First triangle to use texture.
	int		iNumOfTriangles;			// Num of triangles that use texture.
};

class Mad_CoreMesh
{ 
public:
	Mad_CoreMesh();
	~Mad_CoreMesh();
	void Clear(void);
	void operator=(const Mad_CoreMesh& kOther);


	char							m_acName[64];
	Mad_CoreMeshHeader				kHead;

	Mad_Texture						akTextures[MAD_MAX_TEXTURES];
	vector<MadTextureCoo>			akTextureCoo;
	vector<MadFace>					akFaces;
	vector<Mad_VertexFrame>			akFrames;
	vector<Mad_CoreSubMesh>			akSubMeshes;
	vector<Mad_CoreMeshAnimation>	akAnimation;
	vector<int>						akBoneConnections;

	void CreateRigidBoneConnections(int iBoneId = 0);
	void Save(FILE* fp);
	void Load(FILE* fp);
	void ShowInfo(void);
	Mad_CoreMeshAnimation*	Mad_CoreMesh::GetAnimation(char* ucaName);
	void CreateVertexNormals();
};

// MAD - SD
class Mad_CoreBone
{
public:
	Mad_CoreBone();
	~Mad_CoreBone();

	void Clear(void);
	void operator=(const Mad_CoreBone& kOther);

	char				m_acName[32];
	int					m_iParent;
	Vector3				m_kPosition;
	Vector3				m_kRotation;

	Vector3				m_kPosScale;
	Vector3				m_kRotScale;

};

// MAD - AD
class Mad_BoneKey
{
public:
	Vector3				m_kPosition;
	Vector3				m_kRotation;

	Mad_BoneKey() { }
	~Mad_BoneKey() { }
	void Clear(void) { }
	void operator=(const Mad_BoneKey& kOther)
	{
		m_kPosition = kOther.m_kPosition;
		m_kRotation = kOther.m_kRotation;
	}

	void Save(FILE* fp);
	void Load(FILE* fp);

};

class Mad_BoneKeyFrame
{
public:
	vector<Mad_BoneKey>	m_kBonePose;	

	Mad_BoneKeyFrame() { }
	~Mad_BoneKeyFrame() { }
	void Clear(void) 
	{
		m_kBonePose.clear();
	}

	void operator=(const Mad_BoneKeyFrame& kOther)
	{
		m_kBonePose = kOther.m_kBonePose;
	}

	void Save(FILE* fp);
	void Load(FILE* fp);
};

class Mad_CoreBoneAnimation
{
public:
	Mad_CoreBoneAnimation();
	~Mad_CoreBoneAnimation();
	void Clear(void);
	void operator=(const Mad_CoreBoneAnimation& kOther);


	char	m_szName[MAD_MAX_ANIMATIONNAME];
	vector<Mad_BoneKeyFrame>	m_kBoneKeyFrames;

	void Save(FILE* fp);
	void Load(FILE* fp);
};





struct Mad_Header
{
	int			m_iNumOfMeshes;
};


class MadExporter
{
private:
	Mad_Header	m_kMadHeader;

public:
	MadExporter();
	~MadExporter();

	vector<Mad_CoreMesh>			m_kMesh;
	vector<Mad_CoreBone>			m_akSkelleton;
	vector<Mad_CoreBoneAnimation>	m_kBoneAnim;
	
	Mad_CoreMesh* GetMesh(char* ucaName);

	void Save_SD(const char* filename);
	void Save_MD(const char* filename);
	void Save_AD(const char* filename);
	void Save_MAD(const char* filename);
};




#endif