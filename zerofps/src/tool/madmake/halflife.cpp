#include <iostream>
#include "halflife.h"

ModellHalfLife::ModellHalfLife()
{

}

ModellHalfLife::~ModellHalfLife()
{

}

void ModellHalfLife::PrintBones()
{
	cout << "m_pkBones->m_acName: " << m_pkBones->m_acName << endl; 
	cout << "m_pkBones->m_afScale: " << m_pkBones->m_afScale << endl; 
	cout << "m_pkBones->m_iParent: " << m_pkBones->m_iParent << endl; 
}


void ModellHalfLife::PrintHeader()
{
	cout << "m_kHeader.m_cName: " << m_pkHeader->m_cName << endl; 
	cout << "m_kHeader.m_iNumOfBodyParts: " << m_pkHeader->m_iNumOfBodyParts << endl; 
	cout << "m_kHeader.m_iNumOfAttachments: " << m_pkHeader->m_iNumOfAttachments << endl; 
	cout << "m_kHeader.m_iNumOfBoneControllers: " << m_pkHeader->m_iNumOfBoneControllers << endl; 
	cout << "m_kHeader.m_iNumOfBones: " << m_pkHeader->m_iNumOfBones << endl; 
	cout << "m_kHeader.m_iNumOfHitBoxes: " << m_pkHeader->m_iNumOfHitBoxes << endl; 
	cout << "m_kHeader.m_iNumOfSegGroups: " << m_pkHeader->m_iNumOfSegGroups << endl; 
	cout << "m_kHeader.m_iNumOfTextures: " << m_pkHeader->m_iNumOfTextures << endl; 
}

void ModellHalfLife::Read( const char* filename )
{
	// Read whole model inte memory
	FILE* fp = fopen(filename,"rt");
	int iSize;

	fseek(fp, 0, SEEK_END);
	iSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	m_pcFileBuffer = new char [iSize];
	fread(m_pcFileBuffer, iSize, 1, fp);
	fclose(fp);

	m_pkHeader	= (HLHeader*) m_pcFileBuffer;
	m_pkBones	= (HLBone*) (m_pcFileBuffer + m_pkHeader->m_iBoneIndex);
	m_pkSeqDesc = (HLSequenceDesc*) (m_pcFileBuffer + m_pkHeader->m_iSequencesIndex);

	PrintHeader();
	PrintBones();
}

HLAnimation* ModellHalfLife::GetAnim( HLSequenceDesc* pkSeq )
{
//	if (pseqdesc->seqgroup == 0)
//	{
//		return (mstudioanim_t *)((byte *)m_pstudiohdr + pseqgroup->data + pseqdesc->animindex);
//	}

//	return (HLAnimation *)((char *)m_panimhdr[pseqdesc->seqgroup] + pseqdesc->animindex);
	HLSegGroup*	pkSegGroup;
	pkSegGroup = (HLSegGroup *)((char *) m_pkHeader + m_pkHeader->m_iSegGroupsIndex) + pkSeq->m_iSegGroup;
	return (HLAnimation *)((char *) m_pkHeader + pkSegGroup->m_iData + pkSeq->m_iAnimIndex);
}

bool ModellHalfLife::Export(MadExporter* mad, const char* filename)
{
	int i;
	// Export Skelleton Data
	Mad_CoreBone	NewBone;

	for(i=0; i < m_pkHeader->m_iNumOfBones; i++) {
		NewBone.Clear();
		strcpy(NewBone.m_acName, m_pkBones[i].m_acName);
		NewBone.m_iParent = m_pkBones[i].m_iParent;
		NewBone.m_kPosition.Set(m_pkBones[i].m_afValue[0], 
			m_pkBones[i].m_afValue[1],
			m_pkBones[i].m_afValue[2]);
		NewBone.m_kRotation.Set(m_pkBones[i].m_afValue[3],
			m_pkBones[i].m_afValue[4],
			m_pkBones[i].m_afValue[5]);

		NewBone.m_kPosScale.Set(m_pkBones[i].m_afScale[0], 
			m_pkBones[i].m_afScale[1],
			m_pkBones[i].m_afScale[2]);
		NewBone.m_kRotScale.Set(m_pkBones[i].m_afScale[3],
			m_pkBones[i].m_afScale[4],
			m_pkBones[i].m_afScale[5]);

		mad->m_akSkelleton.push_back(NewBone);
		}

	// Export Mesh Objects
/*	HLBodyPart*	pkBodyParts = (HLBodyPart*) (m_pcFileBuffer + m_pkHeader->m_iBodyPartsIndex);
	HLModel* pkModel;

	char* pcVertexBone;
	char* pcNormalBone;
	HLMesh*	 pkMesh;
	Vector3*	pkVertex;
	Vector3*	pkNormal;


	for(i=0; i<m_pkHeader->m_iNumOfBodyParts; i++) {
		cout << i << ": " << pkBodyParts[i].m_szName << ", "<< pkBodyParts[i].m_iNumOfModels << endl;	
		
		for(int iM=0; iM<pkBodyParts[i].m_iNumOfModels; iM++) {
			pkModel = (HLModel *)((char *)m_pcFileBuffer + pkBodyParts[i].m_iModelIndex) + iM;
			cout << " - " << pkModel->m_szName << endl;
		
			pcVertexBone = (char*) (m_pcFileBuffer + pkModel->m_iVertexInfoIndex);
			pcNormalBone = (char*) (m_pcFileBuffer + pkModel->m_iNormalInfoIndex);
			pkMesh = (HLMesh *)((char *)m_pcFileBuffer + pkModel->m_iMeshIndex);

			pkVertex = (Vector3 *)((char *)m_pcFileBuffer + pkModel->m_iVertexIndex);
			pkNormal = (Vector3 *)((char *)m_pcFileBuffer + pkModel->m_iNormalIndex);

			for(int iFace = 0; iFace < pkMesh->m_iNumOfTriangles; iFace++) {
					
				}
			}
		}

	return false;
*/

	// Export Animation
	Mad_CoreBoneAnimation	NewBoneAnim;
	Mad_BoneKeyFrame		NewBoneKeyFrame;
	Mad_BoneKey				NewBoneKey;
	HLSequenceDesc*			pkSequence;
	HLAnimation*			pkHLAnimation;

	for(i=0; i < m_pkHeader->m_iNumOfSequences; i++) {
		pkSequence = &m_pkSeqDesc[i];
		Print_SegmentInfo(pkSequence);
		NewBoneAnim.Clear();
		strcpy(NewBoneAnim.m_szName, pkSequence->m_szLabel);

		for(int f=0; f< pkSequence->m_iNumOfFrames ;f++) {
			NewBoneKeyFrame.Clear();
			pkHLAnimation = GetAnim(pkSequence);

			for (int b = 0; b < m_pkHeader->m_iNumOfBones; b++) 
			{
				NewBoneKey.Clear();
				NewBoneKey.m_kPosition.Set(pkHLAnimation->fTranslateX,
					pkHLAnimation->fTranslateY,
					pkHLAnimation->fTranslateZ);
				NewBoneKey.m_kRotation.Set(pkHLAnimation->fAngleX,
					pkHLAnimation->fAngleY,
					pkHLAnimation->fAngleZ);

				pkHLAnimation++;
				NewBoneKeyFrame.m_kBonePose.push_back(NewBoneKey);
			}

			cout << ".";
			NewBoneAnim.m_kBoneKeyFrames.push_back(NewBoneKeyFrame);
			}

		mad->m_kBoneAnim.push_back(NewBoneAnim);
		}

	return false;
}



void ModellHalfLife::Print_SegmentInfo(HLSequenceDesc* pkSeg)
{
	cout << "m_kHeader.m_szLabel: " << pkSeg->m_szLabel << endl; 
	cout << "m_kHeader.m_iNumOfFrames: " << pkSeg->m_iNumOfFrames << endl; 
	cout << "m_kHeader.m_iSegGroup " << pkSeg->m_iSegGroup << endl; 
}

