#ifndef FILE__XXX_H
#define FILE__XXX_H

#include "mad.h"
#include <string>
#include <vector>

using namespace std;



class ModellXXX : public IMadImport
{
public:
	ModellXXX() {};
	~ModellXXX() {};

	void Read( char* filename );	// Read data in own format to this.
	bool Export(MadExporter* mad);	// Export this to mad.

//	vector<MadVertex>	m_akVertex;
	vector<MadFace>		m_akFace;

	vector<Mad_VertexFrame>		m_akFrames;
	vector<MadTextureCoo>		m_akTextureCoo;
	vector<Mad_Texture>			m_akTextureNames;

	vector<Mad_SubMesh>			m_akSubMesh;
	vector<Mad_Animation>		akAnimation;

private:
	int	AddTexture(char* ucpTextureName);

	void ReadVertexFrame(FILE* fp,int iNumTriangles);

	void ReadBaseFrame(char* filename);		// Triangles / Textures
	void ReadAnimation(char* filename);		// Position Data.
	void ReadAnimationFrame(FILE* fp, int iNumTriangles);

};

#endif