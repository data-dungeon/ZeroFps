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

void MadExporter::Save_SD(const char* filename)
{
	FILE* fp = fopen(filename, "wb");
	
	int iNumOfBones = m_akSkelleton.size();
	fwrite(&iNumOfBones, sizeof(int),1,fp);
	for(int i=0; i < m_akSkelleton.size(); i++) {
		fwrite(&m_akSkelleton[i],sizeof(Mad_CoreBone),1,fp);
	}

	fclose(fp);
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
	m_kMesh[0].CreateVertexNormals();

	FILE* MadFp = fopen(filename, "wb");
	if(!MadFp) {
		cout << "Failed to open '" << filename << "' for writing" << endl;
		return;
		}

	// Write MAD Header.
	m_kMadHeader.m_iNumOfMeshes = m_kMesh.size();
	fwrite(&m_kMadHeader,sizeof(Mad_Header), 1, MadFp);
	// Write SD

	// Write AD

	// Write MD
	for(int i=0; i<m_kMadHeader.m_iNumOfMeshes; i++)
		m_kMesh[i].Save(MadFp);

	// Write MA

	// Update Mad Header.

	fclose(MadFp);

}









