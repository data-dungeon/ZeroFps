#ifndef FILE__HALFLIFE_H
#define FILE__HALFLIFE_H

#include "mad.h"

struct HLHeader
{
	int					m_Id;
	int					m_Version;
	char				m_cName[64];
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

struct HLBone
{
	char				m_acName[32];
	int					m_iParent;
	int					m_iFlags;
	int					m_aiBoneController[6];
	float				m_afValue[6];
	float				m_afScale[6];

};

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
	
};

typedef struct
{
	unsigned short	offset[6];
} mstudioanim_t;


class ModellHalfLife : public IMadImport
{
private:
	HLHeader*		m_pkHeader;
	HLBone*			m_pkBones;
	HLSequenceDesc*	m_pkSeqDesc;

	char*		m_pcFileBuffer;

	void PrintHeader();
	void PrintBones();

public:
	ModellHalfLife();
	~ModellHalfLife();

	void			Print_SegmentInfo(HLSequenceDesc* pkSeg);

	void Read( const char* filename );	// Read data in own format to this.
	bool Export(MadExporter* mad, const char* filename);	// Export this to mad.
};

#endif