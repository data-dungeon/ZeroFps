/*
	Version 1. 
	Quake 2 Modeller och 3DS filer export.
	Vertex animerade object.
*/
#include <string>
#include "mad.h"
#include "vector"
#include "iostream"
#include <cstdio>

#include "../../zerofps/basic/basicmath.pkg"

using namespace std;


Vector3 Spya;

MadExporter::MadExporter()
{

}

MadExporter::~MadExporter()
{

}

void MadExporter::Save_SD(FILE* pkFp)
{
	int iNumOfBones = m_akSkelleton.size();
	fwrite(&iNumOfBones, sizeof(int),1,pkFp);
	for(int i=0; i < m_akSkelleton.size(); i++) {
		fwrite(&m_akSkelleton[i],sizeof(Mad_CoreBone),1,pkFp);
	}
}

void MadExporter::Save_MD(int iMeshId, FILE* pkFp)
{
	m_kMesh[iMeshId].Save(pkFp);
}

void MadExporter::Save_AD(int iMeshId, FILE* pkFp)
{
	m_kBoneAnim[iMeshId].Save(pkFp);
}


void MadExporter::Save_SD(const char* filename)
{
	FILE* fp = fopen(filename, "wb");
	Save_SD(fp);
	fclose(fp);
}

void MadExporter::Save_AD(int iMeshId, const char* filename)
{
	FILE* fp = fopen(filename, "wb");
	Save_AD(iMeshId, fp);
	fclose(fp);
}


void MadExporter::Save_MD(int iMeshId, const char* filename)
{
	FILE* MadFp = fopen(filename, "wb");
	Save_MD(iMeshId, MadFp);
	fclose(MadFp);
	
}


Mad_CoreMesh* MadExporter::GetMesh(char* ucaName)
{
	vector<Mad_CoreMesh>::iterator it;

	for(it = m_kMesh.begin(); it != m_kMesh.end(); it++)
	{
		if(strcmp(it->m_acName, ucaName) == 0)
			return it;
	}

	// Finns ingen mesh med det namnet så skapa den och returnera den.
	Mad_CoreMesh kNewMesh;
	kNewMesh.Clear();
	strcpy(kNewMesh.m_acName, ucaName);
	m_kMesh.push_back(kNewMesh);
	return &m_kMesh.back();
}

void MadExporter::Save_MAD(const char* filename)
{
	int i;

//	m_kMesh[0].CreateVertexNormals();

	FILE* MadFp = fopen(filename, "wb");
	if(!MadFp) {
		cout << "Failed to open '" << filename << "' for writing" << endl;
		return;
		}

	// Setup MAD Header.
	m_kMadHeader.m_iVersionNum		= MAD_VERSION;
	m_kMadHeader.m_iNumOfMeshes		= m_kMesh.size();
	m_kMadHeader.m_iNumOfAnimations = m_kBoneAnim.size();
		
	// Write MAD Header.
	fwrite(&m_kMadHeader,sizeof(Mad_Header), 1, MadFp);
	
	// Write SD
	Save_SD(MadFp);

	// Write AD
	for(i=0; i<m_kMadHeader.m_iNumOfAnimations; i++)
		Save_AD(i, MadFp);

	// Write MD
	for(i=0; i<m_kMadHeader.m_iNumOfMeshes; i++)
		Save_MD(i, MadFp);

	// Write MA

	fclose(MadFp);

}









