#ifndef FILE__MADEXPORT_H
#define FILE__MADEXPORT_H

#include "../../zerofpsv2/engine_systems/mad/mad_core.h"

void Error(char* SzError);
void LogIt(const char *fmt, ...);

class MadExporter;

/*
Bas klass för alla klasser som kan importeras till MAD.
*/
class IMadImport
{
public:
	virtual ~IMadImport() {};
	// Read data in own format to this.
	virtual	void Read( const char* filename)	= 0;	
	// Export this to mad.
	virtual	bool Export(MadExporter* mad, const char* filename) = 0;
};

class MadExporter : public Mad_Core
{
private:
	Mad_Header	m_kMadHeader;

public:
	MadExporter();
	~MadExporter();

//	vector<Mad_CoreMesh>			m_kMesh;
//	vector<Mad_CoreBoneAnimation>	m_kBoneAnim;
//	vector<Mad_CoreBone>			m_akSkelleton;
	
	Mad_CoreMesh* GetMesh(char* ucaName);

/*	void Save_SD(FILE* pkFp);
	void Save_MD(int iMeshId, FILE* pkFp);
	void Save_AD(int iAnimId, FILE* pkFp);

	void Save_SD(const char* filename);
	void Save_MD(int iMeshId, const char* filename);
	void Save_AD(int iMeshId, const char* filename);
	void Save_MAD(const char* filename);*/
	void MadExporter::Print_MAD();

};



#endif
