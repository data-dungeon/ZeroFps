#include <iostream>
#include <vector>
#include "obj.h"

using namespace std;

extern char ErrorMsg[512];

class ObjFaceVertex
{
public:
	int	m_iVertexIndex;
	int	m_iTextureIndex;
	int	m_iNormalIndex;
};

void ModellObj::AddFace(int iVertex1, int iTexture1, int iNormal1,
		int iVertex2, int iTexture2, int iNormal2,
		int iVertex3, int iTexture3, int iNormal3)
{
	Vector3					kVertex;
	Vector3					kNormal;
	Mad_Face					kFace;
	Mad_TextureCoo			kTextureCoo;

	kVertex			= m_kVertex[ iVertex1  ];
	kTextureCoo		= m_kTexture[ iTexture1 ];
	kNormal			= m_kNormal[ iNormal1 ];
	kFace.iIndex[0] = kFrame.Size();
	kFrame.PushBack(kVertex, kNormal);
	pkRawMesh->PushBackBoneConnection(0);
	pkRawMesh->PushBackTextureCoo(kTextureCoo);

	kVertex			= m_kVertex[ iVertex2  ];
	kTextureCoo		= m_kTexture[ iTexture2 ];
	kNormal			= m_kNormal[ iNormal2 ];
	kFace.iIndex[1] = kFrame.Size();
	kFrame.PushBack(kVertex, kNormal);
	pkRawMesh->PushBackBoneConnection(0);
	pkRawMesh->PushBackTextureCoo(kTextureCoo);

	kVertex			= m_kVertex[ iVertex3  ];
	kTextureCoo		= m_kTexture[ iTexture3 ];
	kNormal			= m_kNormal[ iNormal3 ];
	kFace.iIndex[2] = kFrame.Size();
	kFrame.PushBack(kVertex, kNormal);
	pkRawMesh->PushBackBoneConnection(0);
	pkRawMesh->PushBackTextureCoo(kTextureCoo);

	pkRawMesh->PushBackFaces(kFace);
	iNumOfTri++;
}

void ModellObj::HandleObjCommand(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	float x,y,z;
	char* ucpToken;

	if (!strcmp (ucpTokenCmd, "#"))
	{
		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "mtllib"))
	{
		ucpToken = pkScript->GetToken();
		ReadMtlFile(ucpToken);
		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "g"))
	{
		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "v"))
	{
		ucpToken = pkScript->GetToken();
		x = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		y = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		z = atof(ucpToken);
		Vector3 kVert(x,y,z);
		m_kVertex.push_back(kVert);
		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "vt"))
	{
		ucpToken = pkScript->GetToken();
		x = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		y = atof(ucpToken);
		Mad_TextureCoo kVert;
		kVert.s = x;
		kVert.t = y;
		m_kTexture.push_back(kVert);
		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "vn"))
	{
		ucpToken = pkScript->GetToken();
		x = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		y = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		z = atof(ucpToken);
		Vector3 kVert(x,y,z);
		m_kNormal.push_back(kVert);
		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "s"))
	{
		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "f"))
	{
		Vector3					kVertex;
		Vector3					kNormal;
		Mad_Face					kFace;
		Mad_TextureCoo			kTextureCoo;
		
		int iVert, iNorm, iTex;

		vector<ObjFaceVertex>	kFaceVertexList;
		ObjFaceVertex				kFaceVertex;

		int i=0;
		while(ucpToken = pkScript->GetTokenUntilEOL())
		{
			sscanf(ucpToken, "%d/%d/%d", &iVert, &iTex, &iNorm);  		
			kFaceVertex.m_iVertexIndex = iVert;
			kFaceVertex.m_iTextureIndex = iTex;
			kFaceVertex.m_iNormalIndex = iNorm;
			kFaceVertexList.push_back(kFaceVertex);
		}

		if(kFaceVertexList.size() == 3)
		{
			AddFace(kFaceVertexList[0].m_iVertexIndex, kFaceVertexList[0].m_iTextureIndex, kFaceVertexList[0].m_iNormalIndex,
				kFaceVertexList[1].m_iVertexIndex, kFaceVertexList[1].m_iTextureIndex, kFaceVertexList[1].m_iNormalIndex,
				kFaceVertexList[2].m_iVertexIndex, kFaceVertexList[2].m_iTextureIndex, kFaceVertexList[2].m_iNormalIndex);
		}
		else if(kFaceVertexList.size() == 4)
		{
			AddFace(kFaceVertexList[0].m_iVertexIndex, kFaceVertexList[0].m_iTextureIndex, kFaceVertexList[0].m_iNormalIndex,
				kFaceVertexList[1].m_iVertexIndex, kFaceVertexList[1].m_iTextureIndex, kFaceVertexList[1].m_iNormalIndex,
				kFaceVertexList[2].m_iVertexIndex, kFaceVertexList[2].m_iTextureIndex, kFaceVertexList[2].m_iNormalIndex);

			AddFace(kFaceVertexList[0].m_iVertexIndex, kFaceVertexList[0].m_iTextureIndex, kFaceVertexList[0].m_iNormalIndex,
				kFaceVertexList[2].m_iVertexIndex, kFaceVertexList[2].m_iTextureIndex, kFaceVertexList[2].m_iNormalIndex,
				kFaceVertexList[3].m_iVertexIndex, kFaceVertexList[3].m_iTextureIndex, kFaceVertexList[3].m_iNormalIndex);
		}
		else
		{
			Vector3 kCVertex, kCNormal;
			kCVertex = kCNormal = Vector3::ZERO;
			// Calculate center vertex
			for(i=0; i<kFaceVertexList.size(); i++)
			{
				kCVertex			+= m_kVertex[ kFaceVertexList[i].m_iVertexIndex  ];
				kCNormal			+= m_kNormal[kFaceVertexList[i].m_iNormalIndex];
			}

			kCNormal.Normalize();
			kCVertex = kCVertex / kFaceVertexList.size();

			int iV1, iV2;
			iV1 = kFaceVertexList.size() - 1;
			for(iV2=0; iV2<kFaceVertexList.size(); iV2++)
			{
				if(iV1 == kFaceVertexList.size())
					iV1 = 0;

				kVertex			= kCVertex;
				kTextureCoo.s = 0;
				kTextureCoo.t = 0;
				kNormal			= kCNormal;
				kFace.iIndex[2] = kFrame.Size();
				kFrame.PushBack(kVertex, kNormal);
				pkRawMesh->PushBackBoneConnection(0);
				pkRawMesh->PushBackTextureCoo(kTextureCoo);

				kVertex			= m_kVertex[ kFaceVertexList[iV2].m_iVertexIndex  ];
				kTextureCoo		= m_kTexture[kFaceVertexList[iV2].m_iTextureIndex];
				kNormal			= m_kNormal[kFaceVertexList[iV2].m_iNormalIndex];
				kFace.iIndex[0] = kFrame.Size();
				kFrame.PushBack(kVertex, kNormal);
				pkRawMesh->PushBackBoneConnection(0);
				pkRawMesh->PushBackTextureCoo(kTextureCoo);

				kVertex			= m_kVertex[ kFaceVertexList[iV1].m_iVertexIndex  ];
				kTextureCoo		= m_kTexture[kFaceVertexList[iV1].m_iTextureIndex];
				kNormal			= m_kNormal[kFaceVertexList[iV1].m_iNormalIndex];
				kFace.iIndex[1] = kFrame.Size();
				kFrame.PushBack(kVertex, kNormal);
				pkRawMesh->PushBackBoneConnection(0);
				pkRawMesh->PushBackTextureCoo(kTextureCoo);

				pkRawMesh->PushBackFaces(kFace);
				iNumOfTri++;
				iV1++;
			}
		}

		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "usemtl"))
	{
		if(m_kSubMesh.iNumOfTriangles != -1)
		{
			m_kSubMesh.iNumOfTriangles = iNumOfTri - m_kSubMesh.iFirstTriangle;
			pkRawMesh->PushBackSubMeshes(m_kSubMesh);
			m_kSubMesh.iFirstTriangle = iNumOfTri;
			m_kSubMesh.iNumOfTriangles = 0;
			m_kSubMesh.iTextureIndex = 0;
		}

		ucpToken = pkScript->GetToken();
		MayaMtl* pkMat = GetMaterial(ucpToken);
		strcpy(m_szTextureName, pkMat->m_strDiffuseColorTexture.c_str());

		char szArghh[128];
		string strMatName = pkMat->m_strName + string(".bmp");
		strcpy(szArghh, strMatName.c_str());
		m_kSubMesh.iTextureIndex = pkRawMesh->AddTexture(szArghh);
		m_kSubMesh.iNumOfTriangles = 0;

		pkScript->SkipLine();
		return;	
	}

	cout << "Unknown Command: Line = " << pkScript->iLine << ". Cmd = '" << ucpTokenCmd << "'" << endl;
	pkScript->SkipLine();
}

MayaMtl*	ModellObj::GetMaterial(string strName)
{
	for(int i=0; i<m_kMayaMat.size(); i++)
	{
		if(m_kMayaMat[i].m_strName == strName)
			return &m_kMayaMat[i];
	}

	return NULL;
}


void ModellObj::HandleMtlCommand(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	char* ucpToken;
	float x,y,z;

	if (!strcmp (ucpTokenCmd, "newmtl"))
	{
		if(m_kMaterial.m_strName.length() != 0)
			m_kMayaMat.push_back(m_kMaterial);

		ucpToken = pkScript->GetToken();
		m_kMaterial.m_strName = ucpToken;

		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "map_Kd"))
	{
		ucpToken = pkScript->GetToken();
		m_kMaterial.m_strDiffuseColorTexture = ucpToken;

		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "##"))
	{
		pkScript->SkipLine();
		return;	
	}

	// Skip unused variables.
	if (!strcmp (ucpTokenCmd, "Kd"))
	{
		ucpToken = pkScript->GetToken();
		x = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		y = atof(ucpToken);
		ucpToken = pkScript->GetToken();
		z = atof(ucpToken);
		m_kMaterial.m_kAmbientColor.Set(x,y,z);

		pkScript->SkipLine();
		return;	
	}

		
	if (!strcmp (ucpTokenCmd, "Ka")
		|| !strcmp (ucpTokenCmd, "Ks") || !strcmp (ucpTokenCmd, "Ns")
		|| !strcmp (ucpTokenCmd, "d") || !strcmp (ucpTokenCmd, "Tr")
		|| !strcmp (ucpTokenCmd, "illum"))
	{
		pkScript->SkipLine();
		return;	
	}


	cout << "Unknown Command: Line = " << pkScript->iLine << ". Cmd = '" << ucpTokenCmd << "'" << endl;
	pkScript->SkipLine();

}

void ModellObj::SaveMaterials()
{
	FILE* pkFp;

	char szTextName[256];

	for(int i=0; i<m_kMayaMat.size(); i++)
	{
		string strMatName = m_kMayaMat[i].m_strName + string(".zmt");
		pkFp = fopen(strMatName.c_str(),"wt");	
		strcpy(szTextName, m_kMayaMat[i].m_strDiffuseColorTexture.c_str());

		fprintf(pkFp, "[global]\n\n");
		fprintf(pkFp, "[pass-0]\n");
		fprintf(pkFp, "polygonmodefront=0\n");

		if(strlen(szTextName))
		{
			fprintf(pkFp, "tu0=%s\n", m_kMayaMat[i].m_strDiffuseColorTexture.c_str());
		}
		else
		{
			fprintf(pkFp, "colormaterial=true\n");
			fprintf(pkFp, "vertexcolor-r=%f\n", m_kMayaMat[i].m_kAmbientColor.x);
			fprintf(pkFp, "vertexcolor-g=%f\n", m_kMayaMat[i].m_kAmbientColor.y);
			fprintf(pkFp, "vertexcolor-b=%f\n", m_kMayaMat[i].m_kAmbientColor.z);
		}

		fclose(pkFp);
	}
}


void ModellObj::ReadMtlFile(char* szFileName)
{
	SimpleScriptFile kMMScipt;
	if(kMMScipt.LoadScript(szFileName) == false) {
		sprintf(ErrorMsg, "Failed to load maya material file %s.\n",szFileName);
		Error(ErrorMsg);
		}
	
	char* ucpToken;
	ucpToken = kMMScipt.GetToken();

	while(ucpToken)
	{
		HandleMtlCommand(&kMMScipt, ucpToken);
		ucpToken = kMMScipt.GetToken();
	}

	if(m_kMaterial.m_strName.length() != 0)
		m_kMayaMat.push_back(m_kMaterial);

}

/*	Read a file in .obj format. */
void ModellObj::Read( const char* filename )
{
	m_bWarningTriangulate = false;

	iNumOfTri = 0;
	m_kSubMesh.iNumOfTriangles = -1;
	m_kSubMesh.iFirstTriangle  = 0;

	SimpleScriptFile kMMScipt;
	if(kMMScipt.LoadScript(filename) == false) {
		sprintf(ErrorMsg, "Failed to load script file %s.\n",filename);
		Error(ErrorMsg);
		}

	char* ucpToken;
	ucpToken = kMMScipt.GetToken();
	
	Mad_CoreMesh	kMesh;
	Mad_RawMesh		kRaw;
	kMesh.m_kLodMesh.push_back(kRaw);
	pkRawMesh = kMesh.GetLODMesh(0);

	m_kVertex.push_back(Vector3(0,0,0));
	m_kNormal.push_back(Vector3(0,0,0));
	Mad_TextureCoo kTex;
	m_kTexture.push_back(kTex);

	while(ucpToken)
	{
		HandleObjCommand(&kMMScipt, ucpToken);
		ucpToken = kMMScipt.GetToken();
	}

	if(m_kSubMesh.iNumOfTriangles != -1)
	{
		m_kSubMesh.iNumOfTriangles = iNumOfTri - m_kSubMesh.iFirstTriangle;
		pkRawMesh->PushBackSubMeshes(m_kSubMesh);
	}

	for(int i=0; i<m_kMayaMat.size(); i++)
	{
		char szArghh[128];
		string strMatName = m_kMayaMat[i].m_strName + string(".bmp");
		strcpy(szArghh, strMatName.c_str());
		pkRawMesh->AddTexture(szArghh);
	}

	// Create Mesh
	pkRawMesh->PushBackFrames(kFrame);
	m_kMesh.push_back( kMesh );

	// Create a single Joint.
	Mad_CoreBone NewBone;
	strcpy(NewBone.m_acName, "root");
	NewBone.m_iParent = -1;
	NewBone.m_kPosition.x = 0;
	NewBone.m_kPosition.y = 0;
	NewBone.m_kPosition.z = 0;
	NewBone.m_kRotation.x = 0;
	NewBone.m_kRotation.y = 0;
	NewBone.m_kRotation.z = 0;
	m_akSkelleton.push_back(NewBone);

	// Create a Single Empty Animation.
	Mad_CoreBoneKey			kNewBoneKey;
	Mad_CoreBoneKeyFrame		kNewBoneKeyFrame;
	Mad_CoreBoneAnimation	kNewBoneAnim;
	kNewBoneKey.Clear();
	kNewBoneKeyFrame.Clear();
	kNewBoneAnim.Clear();

	kNewBoneKey.m_kPosition = Vector3::ZERO;
	kNewBoneKey.m_kRotation = Vector3::ZERO;
	kNewBoneKeyFrame.PushBack(kNewBoneKey);
	kNewBoneKeyFrame.PushBack(kNewBoneKey);
	kNewBoneAnim.PushBack(kNewBoneKeyFrame);
	strcpy(kNewBoneAnim.m_szName, "static");
	m_kBoneAnim.push_back(kNewBoneAnim);

	if(m_bWarningTriangulate)
		cout << "WARNING: One (or more) faces in the .obj was made of more then 3 vertex. Some faces will be missing in this export.";
		cout << " Triangulate before exporting.";
}

/* Export the data to Mad. */
bool ModellObj::Export(MadExporter* mad, const char* filename)
{
	cout << m_kBoneAnim.size() << endl;
	mad->SetSkelleton(m_akSkelleton);
	mad->SetAnimation(m_kBoneAnim);

	vector<Mad_CoreMesh>::iterator it;
//	for(it = m_kMesh.begin(); it != m_kMesh.end(); it++)
//		it->GetLODMesh(0)->OptimizeSubMeshes();

	for(it = m_kMesh.begin(); it != m_kMesh.end(); it++)
	{
//		int iTotalNumOfVertex = it->akFrames[0].akVertex.size();
//		int iTotalTriangles = it->akFaces.size();
//		int iAntalFrames = it->akFrames.size();
		int iTotalNumOfVertex = it->GetLODMesh(0)->NumOfVertexPerFrame();
		int iTotalTriangles = it->GetLODMesh(0)->SizeFaces();
		int iAntalFrames = it->GetLODMesh(0)->SizeFrames();
		
		it->GetLODMesh(0)->kHead.iVersionNum		= 1;
//		it->kHead.iNumOfTextures	= it->akTextures.size();
		it->GetLODMesh(0)->kHead.iNumOfTextures	= it->GetLODMesh(0)->SizeTextures();
		it->GetLODMesh(0)->kHead.iNumOfVertex		= iTotalNumOfVertex;
		it->GetLODMesh(0)->kHead.iNumOfFaces		= iTotalTriangles;
		it->GetLODMesh(0)->kHead.iNumOfFrames		= iAntalFrames;
//		it->kHead.iNumOfSubMeshes	= it->akSubMeshes.size();
//		it->kHead.iNumOfAnimation	= it->akAnimation.size();
		it->GetLODMesh(0)->kHead.iNumOfSubMeshes	= it->GetLODMesh(0)->SizeSubMesh();
		it->GetLODMesh(0)->kHead.iNumOfAnimation	= it->GetLODMesh(0)->SizeAnimations();
	}

//	mad->m_kMesh = m_kMesh;		
	mad->SetMesh(m_kMesh);
	SaveMaterials();
	return true;
}
