#include "p_obj.h"

P_Obj::P_Obj()
{
	strcpy(m_acName,"P_Obj");		
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_bNetwork = false;	
	m_iVersion = 1;
	m_iSortPlace=4;
	
	m_pkZShaderSystem	=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));				
	m_pkTextureManger	=	static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));			
	m_pkRender			=	static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));			
	m_iCurrentMaterial = -1;
	
	m_bNonTriangles = false;

	LoadModel("town.obj");
}

P_Obj::~P_Obj()
{
}

void P_Obj::Update()
{

	if(m_kVertex.size() == 0)
		return;

	Matrix3 kRot;
	kRot = m_pkEntity->GetWorldRotM();		
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetIWorldPosV());
	m_pkZShaderSystem->MatrixMult(Matrix4(kRot));

	char szTextureName[256];
	strcpy(szTextureName, m_szTextureName.c_str());

	for(int i=0; i<m_kMayaMat.size(); i++)
	{
		m_kMayaMat[i].m_iTextureIndex = m_pkTextureManger->Load(m_kMayaMat[i].m_strDiffuseColorTexture.c_str());
	}

	glEnable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
	Vector3 kTexCoo;

	int iDx = 0;
	Vector3 kTextureCoo;

	for(int i=0; i<m_kFace.size(); i++)
	{
		m_pkTextureManger->BindTexture( m_kMayaMat[ m_kFace[i].m_iMaterialIndex ].m_iTextureIndex );
		glBegin(GL_TRIANGLES);
	
		iDx = 0;
		
		kTexCoo = m_kTexture[m_kFace[i].m_iTextureIndex[ iDx] ];
		glTexCoord2f( kTexCoo.x, kTexCoo.y );
		glNormal3fv((float*)&m_kNormal[m_kFace[i].m_iNormalIndex[iDx]]);
		glVertex3fv((float*)&m_kVertex[m_kFace[i].m_iVertexIndex[iDx]]);

		iDx = 1;
		kTexCoo = m_kTexture[m_kFace[i].m_iTextureIndex[ iDx] ];
		glTexCoord2f( kTexCoo.x, kTexCoo.y );
		glNormal3fv((float*)&m_kNormal[m_kFace[i].m_iNormalIndex[iDx]]);
		glVertex3fv((float*)&m_kVertex[m_kFace[i].m_iVertexIndex[iDx]]);

		iDx = 2;
		kTexCoo = m_kTexture[m_kFace[i].m_iTextureIndex[ iDx] ];
		glTexCoord2f( kTexCoo.x, kTexCoo.y );
		glNormal3fv((float*)&m_kNormal[m_kFace[i].m_iNormalIndex[iDx]]);
		glVertex3fv((float*)&m_kVertex[m_kFace[i].m_iVertexIndex[iDx]]);

		glEnd();
	}

	m_pkZShaderSystem->MatrixPop();

}



vector<PropertyValues> P_Obj::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	kReturn[0].kValueName = "filename";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_strFileName;
		
	return kReturn;
}

bool P_Obj::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(strcmp(kValueName.c_str(), "filename") == 0) 
	{
		LoadModel(kValue.c_str());
		return true;
	}
	
	return false;
}

class ObjFaceVertex
{
public:
	int	m_iVertexIndex;
	int	m_iTextureIndex;
	int	m_iNormalIndex;
};

void P_Obj::HandleObjCommand(SimpleScriptFile* pkScript, char* ucpTokenCmd)
{
	char* ucpToken;
	float x,y,z;

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
		Vector3 kVert;
		kVert.x = x;
		kVert.y = y;
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
		
		int iVert, iNorm, iTex;

		vector<ObjFaceVertex>	kFaceVertexList;
		ObjFaceVertex			kFaceVertex;

		int i=0;
		while(ucpToken = pkScript->GetTokenUntilEOL())
		{
			sscanf(ucpToken, "%d/%d/%d", &iVert, &iTex, &iNorm);  		
			kFaceVertex.m_iVertexIndex = iVert - 1;
			kFaceVertex.m_iTextureIndex = iTex - 1;
			kFaceVertex.m_iNormalIndex = iNorm - 1;
			kFaceVertexList.push_back(kFaceVertex);
		}

		if(kFaceVertexList.size() == 3)
		{
			ObjFace kFace;
			kFace.m_iVertexIndex[0] = kFaceVertexList[0].m_iVertexIndex;
			kFace.m_iTextureIndex[0] = kFaceVertexList[0].m_iTextureIndex;
			kFace.m_iNormalIndex[0] = kFaceVertexList[0].m_iNormalIndex;

			kFace.m_iVertexIndex[1] = kFaceVertexList[1].m_iVertexIndex;
			kFace.m_iTextureIndex[1] = kFaceVertexList[1].m_iTextureIndex;
			kFace.m_iNormalIndex[1] = kFaceVertexList[1].m_iNormalIndex;

			kFace.m_iVertexIndex[2] = kFaceVertexList[2].m_iVertexIndex;
			kFace.m_iTextureIndex[2] = kFaceVertexList[2].m_iTextureIndex;
			kFace.m_iNormalIndex[2] = kFaceVertexList[2].m_iNormalIndex;
			kFace.m_iMaterialIndex = m_iCurrentMaterial;

			m_kFace.push_back(kFace);
		}
		else 
		{
			m_bNonTriangles = true;
		}

		pkScript->SkipLine();
		return;	
	}

	if (!strcmp (ucpTokenCmd, "usemtl"))
	{
/*		if(m_kSubMesh.iNumOfTriangles != -1)
		{
			m_kSubMesh.iNumOfTriangles = iNumOfTri - m_kSubMesh.iFirstTriangle;
			pkRawMesh->PushBackSubMeshes(m_kSubMesh);
			m_kSubMesh.iFirstTriangle = iNumOfTri;
			m_kSubMesh.iNumOfTriangles = 0;
			m_kSubMesh.iTextureIndex = 0;
		}*/

		ucpToken = pkScript->GetToken();
		MayaMtl* pkMat = GetMaterial(ucpToken);
		m_iCurrentMaterial = GetMaterialIndex(ucpToken);
		m_szTextureName = pkMat->m_strDiffuseColorTexture;
		//strcpy(m_szTextureName, pkMat->m_strDiffuseColorTexture.c_str());

		m_szTextureName = pkMat->m_strDiffuseColorTexture;	// + string(".bmp");
		/*
		char szArghh[128];
		strcpy(szArghh, strMatName.c_str());
		m_kSubMesh.iTextureIndex = pkRawMesh->AddTexture(szArghh);
		m_kSubMesh.iNumOfTriangles = 0;*/
		

		pkScript->SkipLine();
		return;	
	}

	cout << "Unknown Command: Line = " << pkScript->iLine << ". Cmd = '" << ucpTokenCmd << "'" << endl;
	pkScript->SkipLine();

}

int P_Obj::GetMaterialIndex(string strName)
{
	for(int i=0; i<m_kMayaMat.size(); i++)
	{
		if(m_kMayaMat[i].m_strName == strName)
			return i;
	}

	return -1;
}

MayaMtl*	P_Obj::GetMaterial(string strName)
{
	for(int i=0; i<m_kMayaMat.size(); i++)
	{
		if(m_kMayaMat[i].m_strName == strName)
			return &m_kMayaMat[i];
	}

	return NULL;
}
void P_Obj::HandleMtlCommand(SimpleScriptFile* pkScript, char* ucpTokenCmd)
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

void P_Obj::ReadMtlFile(char* szFileName)
{
	SimpleScriptFile kMMScipt;
	if(kMMScipt.LoadScript(szFileName) == false) 
	{
		cout << "Failed to load maya material" << endl;
		return;
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

void P_Obj::SaveMaterials()
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


void P_Obj::LoadModel(string strFileName)
{
	SimpleScriptFile kMMScipt;
	if(kMMScipt.LoadScript(strFileName.c_str()) == false) {
		cout << "Failed to load script file " << strFileName << endl;
		return;
		}

	char* ucpToken;
	ucpToken = kMMScipt.GetToken();
	
	while(ucpToken)
	{
		HandleObjCommand(&kMMScipt, ucpToken);
		ucpToken = kMMScipt.GetToken();
	}
	SaveMaterials();

	if(m_bNonTriangles)
		cout << "The mesh '" << strFileName << "' hade one or more non triangles in the mesh." << endl;
}


Property* Create_P_Obj()
{
	return new P_Obj;
}

void Register_P_Obj(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Obj", Create_P_Obj);					
}
