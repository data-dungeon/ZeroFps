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
		mad->m_akSkelleton.push_back(NewBone);
		}

	// Export Animation
	Mad_CoreBoneAnimation	NewBoneAnim;

	for(i=0; i < m_pkHeader->m_iNumOfSequences; i++) {
		Print_SegmentInfo(&m_pkSeqDesc[i]);
		NewBoneAnim.Clear();
	
		for(int f=0; f< pkSeg->m_iNumOfFrames ;f++) {
			
			}

		mad->m_kBoneAnim.push_back(NewBoneAnim);

		}

	return false;
}

void ModellHalfLife::Print_SegmentInfo(HLSequenceDesc* pkSeg)
{
	cout << "m_kHeader.m_szLabel: " << pkSeg->m_szLabel << endl; 
	cout << "m_kHeader.m_iNumOfFrames: " << pkSeg->m_iNumOfFrames << endl; 
}

