#ifndef FILE__XXX_H
#define FILE__XXX_H

#include "madexport.h"
#include <string>
#include <vector>

using namespace std;

/** \brief	Import for files from MayaExporter
	 \ingroup MadMake
*/
class ModellXXX : public IMadImport
{
public:
	ModellXXX() {};
	~ModellXXX() {};

	void Read( const char* filename );	// Read data in own format to this.
	bool Export(MadExporter* mad, const char* filename);	// Export this to mad.

	vector<Mad_CoreBone>			m_akSkelleton;
	vector<Mad_CoreBoneAnimation>	m_kBoneAnim;
	vector<Mad_CoreMesh>			m_kMesh;

private:
	Mad_CoreMesh* GetMesh(const char* ucaName);

	void ReadVertexFrame(FILE* fp,int iNumTriangles);

	void ReadCoreMesh(const char* filename,	const char* szName, float fLodRange);
	void ReadBaseFrame(const char* filename);						// Triangles / Textures
	void ReadAnimation(const char* filename);	// Position Data.
	void ReadAnimationFrame(FILE* fp, int iNumTriangles);

	void ReadExportSD(const char* filename);
	void ReadExportAD(const char* filename,	const char* szName);
};

#endif





