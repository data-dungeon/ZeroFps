#include <iostream>
#include "obj.h"

extern char ErrorMsg[512];


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

		for(int i=0; i<3; i++)
		{
			// Read Vertex i
			ucpToken = pkScript->GetToken();
			sscanf(ucpToken, "%d/%d/%d", &iVert, &iTex, &iNorm);  		
			kVertex			= m_kVertex[iVert];
			kTextureCoo		= m_kTexture[iTex];
			kNormal			= m_kNormal[iNorm];
			kFace.iIndex[i] = kFrame.Size();
			kFrame.PushBack(kVertex, kNormal);
			pkRawMesh->PushBackBoneConnection(0);
			pkRawMesh->PushBackTextureCoo(kTextureCoo);
		}

		pkRawMesh->PushBackFaces(kFace);
		iNumOfTri++;

		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "usemtl"))
	{
		ucpToken = pkScript->GetToken();
		MayaMtl* pkMat = GetMaterial(ucpToken);
		strcpy(m_szTextureName, pkMat->m_strDiffuseColorTexture.c_str());
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


	cout << "Unknown Command: Line = " << pkScript->iLine << ". Cmd = '" << ucpTokenCmd << "'" << endl;
	pkScript->SkipLine();

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
	iNumOfTri = 0;

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

	// Create Submesh and Add texture
	Mad_CoreSubMesh newsub;
	newsub.iFirstTriangle = 0;
	newsub.iNumOfTriangles = iNumOfTri;
	newsub.iTextureIndex = 0;
	pkRawMesh->PushBackSubMeshes(newsub);
	pkRawMesh->AddTexture(m_szTextureName);

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

	return true;
}
