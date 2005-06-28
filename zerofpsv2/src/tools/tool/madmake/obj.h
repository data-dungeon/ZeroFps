#ifndef FILE__OBJ_H
#define FILE__OBJ_H

#include "madexport.h"
#include <string>
#include <vector>
#include "../../../zerofpsv2/basic/simplescript.h"

using namespace std;

class MayaMtl
{
public:
	string	m_strName;
	string	m_strDiffuseColorTexture;
	Vector3	m_kAmbientColor;
};

/** \brief	Import for Obj files from maya.
	 \ingroup MadMake
*/
class ModellObj : public IMadImport
{
public:
	ModellObj() {};
	~ModellObj() {};

	void Read( const char* filename );	// Read data in own format to this.
	bool Export(MadExporter* mad, const char* filename);	// Export this to mad.

private:
	vector<Vector3>			m_kVertex;
	vector<Vector3>			m_kNormal;
	vector<Mad_TextureCoo>	m_kTexture;
	Mad_CoreVertexFrame		kFrame;
	Mad_RawMesh*				pkRawMesh;

	vector<MayaMtl>			m_kMayaMat;
	MayaMtl						m_kMaterial;
	char	m_szTextureName[256];
	int	iNumOfTri;

	vector<Mad_CoreBone>				m_akSkelleton;
	vector<Mad_CoreBoneAnimation>	m_kBoneAnim;
	vector<Mad_CoreMesh>				m_kMesh;

	MayaMtl*	GetMaterial(string strName);

	void ReadMtlFile(char* szFileName);
	void HandleMtlCommand(SimpleScriptFile* pkScript, char* ucpToken);
	void HandleObjCommand(SimpleScriptFile* pkScript, char* ucpToken);
	void SaveMaterials();

	void AddFace(int iVertex1, int iTexture1, int iNormal1,
		int iVertex2, int iTexture2, int iNormal2,
		int iVertex3, int iTexture3, int iNormal3);

	Mad_CoreSubMesh			m_kSubMesh;

	bool	m_bWarningTriangulate;
};

#endif








