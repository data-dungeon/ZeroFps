/// \cond Model

#ifndef _P_MDLHL_H
#define _P_MDLHL_H

#include "../../zerofpsv2/engine/property.h"
#include "../../zerofpsv2/engine/zerofps.h"
#include "../../zerofpsv2/basic/simplescript.h"
#include "../../zerofpsv2/basic/quaternion.h"

using namespace std;

/// HalfLife Header.
struct HLHeader
{
	int					m_Id;
	int					m_Version;
	char					m_cName[64];
	int					m_iLength;
	
	Vector3				m_kEyePosition;
	Vector3				m_kMin;
	Vector3				m_kMax;
	Vector3				m_kBBMin;
	Vector3				m_kBBMax;

	int					m_iFlags;
	
	int					m_iNumOfBones;
	int					m_iBoneIndex;

	int					m_iNumOfBoneControllers;
	int					m_iBoneControllerIndex;

	int					m_iNumOfHitBoxes;
	int					m_iHitBoxesIndex;

	int					m_iNumOfSequences;
	int					m_iSequencesIndex;

	int					m_iNumOfSegGroups;
	int					m_iSegGroupsIndex;

	int					m_iNumOfTextures;
	int					m_iTextureIndex;
	int					m_iTextureDataIndex;

	int					m_iNumOfSkins;
	int					m_iNumSkinFamiliesIndex;
	int					m_iSkinIndex;

	int					m_iNumOfBodyParts;
	int					m_iBodyPartsIndex;

	int					m_iNumOfAttachments;
	int					m_iAttachmentIndex;

	int					m_iSoundTable;
	int					m_iSoundIndex;

	int					m_iSoundGroups;
	int					m_iSoundGroupIndex;
	int					m_iNumOfTransitions;
	int					m_iTransitionIndex;
};

/// HalfLife Bone.
struct HLBone
{
	char				m_acName[32];
	int					m_iParent;
	int					m_iFlags;
	int					m_aiBoneController[6];
	float				m_afValue[6];
	float				m_afScale[6];

};

/// HalfLife Sequence Descriptor.
struct HLSequenceDesc 
{
	char				m_szLabel[32];

	float				m_fFps;
	int					m_iFlags;

	int					m_iActivity;
	int					m_iActWeight;

	int					m_iNumOfEvents;
	int					m_iEvenetIndex;

	int					m_iNumOfFrames;

	int					m_iNumOfPiviots;
	int					m_iPivotIndex;

	int					m_iMotionType;
	int					m_iMotionBone;
	Vector3				m_kLinerarMovment;
	int					m_iAutoMovePosIndex;
	int					m_iAoutMoveAngleIndex;

	Vector3				m_kBBMin;
	Vector3				m_kBBMax;

	int					m_iNumOfBlends;
	int					m_iAnimIndex;

	int					m_iBlendType[2];
	float				m_fBlendStart[2];
	float				m_fBlendEnd[2];
	int					m_iBlendParent;

	int					m_iSegGroup;
	
	int					m_iEntryNode;
	int					m_iExitNode;
	int					m_iNodeFlags;

	int					m_iNextSeq;
};

struct HLAnimation
{
	unsigned short		fAngleX;
	unsigned short		fAngleY;
	unsigned short		fAngleZ;
	unsigned short		fTranslateX;
	unsigned short		fTranslateY;
	unsigned short		fTranslateZ;
};

struct HLSeqHeader 
{
	int					m_iId;
	int					m_iVersion;

	char				m_szName[64];
	int					m_iLength;
};

struct HLCacheUser
{
	void*				m_pvData;
};

struct HLSegGroup
{
	char				m_szLabel[32];	
	char				m_szName[64];	
	HLCacheUser			m_kCache;		
	int					m_iData;		
};

struct HLBodyPart
{
	char				m_szName[64];
	int					m_iNumOfModels;
	int					m_iBase;
	int					m_iModelIndex;
};

/// HalfLife Model.
struct HLModel
{
	char				m_szName[64];
	int					m_iType;
	float				m_fBoundRadius;
	int					m_iNumOfMesh;
	int					m_iMeshIndex;

	int					m_iNumOfVertex;
	int					m_iVertexInfoIndex;
	int					m_iVertexIndex;

	int					m_iNumOfNormals;
	int					m_iNormalInfoIndex;
	int					m_iNormalIndex;
	
	int					m_iNumOfDeformationGroups;
	int					m_iDeformationGroupIndex;
};

/// HalfLife 3D Mesh.
struct HLMesh
{
	int					m_iNumOfTriangles;
	int					m_iTriangleIndex;
	int					m_iSkinRef;
	int					m_iNumOfNormals;
	int					m_iNormalIndex;
};

/// HalfLife Mesh Vertex.
struct HLMeshVertex
{
	short				m_VertexIndex;
	short				m_NormalIndex;
	short				m_TexCooS;
	short				m_TexCooT;				
};

class P_MdlHL : public Property 
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		TextureManager*	m_pkTextureManger;
		ZSSRender*			m_pkRender;

		bool HandleSetValue( const string& kValueName ,const string& kValue );

		HLSeqHeader*		m_pkAnimationHeader[32];

		HLHeader*			m_pkHeader;
		HLBone*				m_pkBones;
		HLSequenceDesc*	m_pkSeqDesc;

		char*		m_pcFileBuffer;

		void PrintHeader();
		void PrintBones();


	public:
		P_MdlHL();
		~P_MdlHL();
		
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );		

		void LoadMdl(string strFileName);

};

Property* Create_P_MdlHL();
void Register_P_MdlHL(ZSSZeroFps* pkZeroFps);

#endif

/// \endcond