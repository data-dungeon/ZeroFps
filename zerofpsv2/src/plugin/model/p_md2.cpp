#pragma warning(disable : 4275)		
#include "p_md2.h"

extern ZSSPakFile g_PakFileSystem;

#define MD2TOMAD_SCALE 0.04

float Md2Normals[] = {
	-0.525731, 0.000000, 0.850651,
	 -0.442863, 0.238856, 0.864188 , 
	 -0.295242, 0.000000, 0.955423 , 
	 -0.309017, 0.500000, 0.809017 , 
	 -0.162460, 0.262866, 0.951056 , 
	 0.000000, 0.000000, 1.000000 , 
	 0.000000, 0.850651, 0.525731 , 
	 -0.147621, 0.716567, 0.681718 , 
	 0.147621, 0.716567, 0.681718 , 
	 0.000000, 0.525731, 0.850651 , 
	 0.309017, 0.500000, 0.809017 , 
	 0.525731, 0.000000, 0.850651 , 
	 0.295242, 0.000000, 0.955423 , 
	 0.442863, 0.238856, 0.864188 , 
	 0.162460, 0.262866, 0.951056 , 
	 -0.681718, 0.147621, 0.716567 , 
	 -0.809017, 0.309017, 0.500000 , 
	 -0.587785, 0.425325, 0.688191 , 
	 -0.850651, 0.525731, 0.000000 , 
	 -0.864188, 0.442863, 0.238856 , 
	 -0.716567, 0.681718, 0.147621 , 
	 -0.688191, 0.587785, 0.425325 , 
	 -0.500000, 0.809017, 0.309017 , 
	 -0.238856, 0.864188, 0.442863 , 
	 -0.425325, 0.688191, 0.587785 , 
	 -0.716567, 0.681718, -0.147621 , 
	 -0.500000, 0.809017, -0.309017 , 
	 -0.525731, 0.850651, 0.000000 , 
	 0.000000, 0.850651, -0.525731 , 
	 -0.238856, 0.864188, -0.442863 , 
	 0.000000, 0.955423, -0.295242 , 
	 -0.262866, 0.951056, -0.162460 , 
	 0.000000, 1.000000, 0.000000 , 
	 0.000000, 0.955423, 0.295242 , 
	 -0.262866, 0.951056, 0.162460 , 
	 0.238856, 0.864188, 0.442863 , 
	 0.262866, 0.951056, 0.162460 , 
	 0.500000, 0.809017, 0.309017 , 
	 0.238856, 0.864188, -0.442863 , 
	 0.262866, 0.951056, -0.162460 , 
	 0.500000, 0.809017, -0.309017 , 
	 0.850651, 0.525731, 0.000000 , 
	 0.716567, 0.681718, 0.147621 , 
	 0.716567, 0.681718, -0.147621 , 
	 0.525731, 0.850651, 0.000000 , 
	 0.425325, 0.688191, 0.587785 , 
	 0.864188, 0.442863, 0.238856 , 
	 0.688191, 0.587785, 0.425325 , 
	 0.809017, 0.309017, 0.500000 , 
	 0.681718, 0.147621, 0.716567 , 
	 0.587785, 0.425325, 0.688191 , 
	 0.955423, 0.295242, 0.000000 , 
	 1.000000, 0.000000, 0.000000 , 
	 0.951056, 0.162460, 0.262866 , 
	 0.850651, -0.525731, 0.000000 , 
	 0.955423, -0.295242, 0.000000 , 
	 0.864188, -0.442863, 0.238856 , 
	 0.951056, -0.162460, 0.262866 , 
	 0.809017, -0.309017, 0.500000 , 
	 0.681718, -0.147621, 0.716567 , 
	 0.850651, 0.000000, 0.525731 , 
	 0.864188, 0.442863, -0.238856 , 
	 0.809017, 0.309017, -0.500000 , 
	 0.951056, 0.162460, -0.262866 , 
	 0.525731, 0.000000, -0.850651 , 
	 0.681718, 0.147621, -0.716567 , 
	 0.681718, -0.147621, -0.716567 , 
	 0.850651, 0.000000, -0.525731 , 
	 0.809017, -0.309017, -0.500000 , 
	 0.864188, -0.442863, -0.238856 , 
	 0.951056, -0.162460, -0.262866 , 
	 0.147621, 0.716567, -0.681718 , 
	 0.309017, 0.500000, -0.809017 , 
	 0.425325, 0.688191, -0.587785 , 
	 0.442863, 0.238856, -0.864188 , 
	 0.587785, 0.425325, -0.688191 , 
	 0.688191, 0.587785, -0.425325 , 
	 -0.147621, 0.716567, -0.681718 , 
	 -0.309017, 0.500000, -0.809017 , 
	 0.000000, 0.525731, -0.850651 , 
	 -0.525731, 0.000000, -0.850651 , 
	 -0.442863, 0.238856, -0.864188 , 
	 -0.295242, 0.000000, -0.955423 , 
	 -0.162460, 0.262866, -0.951056 , 
	 0.000000, 0.000000, -1.000000 , 
	 0.295242, 0.000000, -0.955423 , 
	 0.162460, 0.262866, -0.951056 , 
	 -0.442863, -0.238856, -0.864188 , 
	 -0.309017, -0.500000, -0.809017 , 
	 -0.162460, -0.262866, -0.951056 , 
	 0.000000, -0.850651, -0.525731 , 
	 -0.147621, -0.716567, -0.681718 , 
	 0.147621, -0.716567, -0.681718 , 
	 0.000000, -0.525731, -0.850651 , 
	 0.309017, -0.500000, -0.809017 , 
	 0.442863, -0.238856, -0.864188 , 
	 0.162460, -0.262866, -0.951056 , 
	 0.238856, -0.864188, -0.442863 , 
	 0.500000, -0.809017, -0.309017 , 
	 0.425325, -0.688191, -0.587785 , 
	 0.716567, -0.681718, -0.147621 , 
	 0.688191, -0.587785, -0.425325 , 
	 0.587785, -0.425325, -0.688191 , 
	 0.000000, -0.955423, -0.295242 , 
	 0.000000, -1.000000, 0.000000 , 
	 0.262866, -0.951056, -0.162460 , 
	 0.000000, -0.850651, 0.525731 , 
	 0.000000, -0.955423, 0.295242 , 
	 0.238856, -0.864188, 0.442863 , 
	 0.262866, -0.951056, 0.162460 , 
	 0.500000, -0.809017, 0.309017 , 
	 0.716567, -0.681718, 0.147621 , 
	 0.525731, -0.850651, 0.000000 , 
	 -0.238856, -0.864188, -0.442863 , 
	 -0.500000, -0.809017, -0.309017 , 
	 -0.262866, -0.951056, -0.162460 , 
	 -0.850651, -0.525731, 0.000000 , 
	 -0.716567, -0.681718, -0.147621 , 
	 -0.716567, -0.681718, 0.147621 , 
	 -0.525731, -0.850651, 0.000000 , 
	 -0.500000, -0.809017, 0.309017 , 
	 -0.238856, -0.864188, 0.442863 , 
	 -0.262866, -0.951056, 0.162460 , 
	 -0.864188, -0.442863, 0.238856 , 
	 -0.809017, -0.309017, 0.500000 , 
	 -0.688191, -0.587785, 0.425325 , 
	 -0.681718, -0.147621, 0.716567 , 
	 -0.442863, -0.238856, 0.864188 , 
	 -0.587785, -0.425325, 0.688191 , 
	 -0.309017, -0.500000, 0.809017 , 
	 -0.147621, -0.716567, 0.681718 , 
	 -0.425325, -0.688191, 0.587785 , 
	 -0.162460, -0.262866, 0.951056 , 
	 0.442863, -0.238856, 0.864188 , 
	 0.162460, -0.262866, 0.951056 , 
	 0.309017, -0.500000, 0.809017 , 
	 0.147621, -0.716567, 0.681718 , 
	 0.000000, -0.525731, 0.850651 , 
	 0.425325, -0.688191, 0.587785 , 
	 0.587785, -0.425325, 0.688191 , 
	 0.688191, -0.587785, 0.425325 , 
	 -0.955423, 0.295242, 0.000000 , 
	 -0.951056, 0.162460, 0.262866 , 
	 -1.000000, 0.000000, 0.000000 , 
	 -0.850651, 0.000000, 0.525731 , 
	 -0.955423, -0.295242, 0.000000 , 
	 -0.951056, -0.162460, 0.262866 , 
	 -0.864188, 0.442863, -0.238856 , 
	 -0.951056, 0.162460, -0.262866 , 
	 -0.809017, 0.309017, -0.500000 , 
	 -0.864188, -0.442863, -0.238856 , 
	 -0.951056, -0.162460, -0.262866 , 
	 -0.809017, -0.309017, -0.500000 , 
	 -0.681718, 0.147621, -0.716567 , 
	 -0.681718, -0.147621, -0.716567 , 
	 -0.850651, 0.000000, -0.525731 , 
	 -0.688191, 0.587785, -0.425325 , 
	 -0.587785, 0.425325, -0.688191 , 
	 -0.425325, 0.688191, -0.587785 , 
	 -0.425325, -0.688191, -0.587785 , 
	 -0.587785, -0.425325, -0.688191 , 
	 -0.688191, -0.587785, -0.425325 , 
};

md2_dtrivertx_t max_xyz[4096];
char g_skins[MD2_MAX_SKINS][MD2_MAX_SKINNAME];
char NewSkinName[256];

P_Md2::P_Md2()
{
	strcpy(m_acName,"P_Md2");		
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_bNetwork = true;	
	m_iVersion = 2;
	m_iSortPlace=4;
	
	m_pkTextureManger =	static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));				
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));		
	m_pkVertex = NULL;
	m_kHead.num_xyz = 0;
	m_iRenderFrame = 33;
	m_fLastFrame = m_fNextFrame = 0;
}

P_Md2::~P_Md2()
{
}

void P_Md2::Update()
{
	float fDelta = 0;

	float fCurrentTime = m_pkEntityManager->GetSimTime();
	if(fCurrentTime >= m_fNextFrame)
	{
		m_iRenderFrame++;
		m_fNextFrame = fCurrentTime +  0.1;
		if(m_iRenderFrame >= m_kHead.num_frames)
			m_iRenderFrame = 0;
		m_fLastFrame = fCurrentTime;

	}
	else
	{
		fDelta = (fCurrentTime - m_fLastFrame) * 10;
	}
	
	if(m_kHead.num_xyz == 0)
		return;

	Matrix3 kRot;
	kRot = m_pkEntity->GetWorldRotM();		

	//m_pkLight->Update(&m_kLightProfile,GetEntity()->GetWorldPosV());						

	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetIWorldPosV());
	m_pkZShaderSystem->MatrixMult(Matrix4(kRot));
		
	char szTextureName[256];
	strcpy(szTextureName, m_strFileName.c_str());
	char* ext = strstr(szTextureName, ".md2");
	if(ext)
		strcpy(ext, ".bmp");

	m_iTexture = m_pkTextureManger->Load(szTextureName);
	m_pkTextureManger->BindTexture( m_iTexture );
	glEnable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
	//glDisable(GL_LIGHTING);

	if(!m_pkVertex)
	{
		m_pkVertex = new Vector3 [m_kHead.num_xyz];
		m_pkNormal = new Vector3 [m_kHead.num_xyz];
		//m_pkTextureCo = new int [m_kHead.num_xyz];
		//m_FaceIndex = new int [m_kHead.num_tris * 3];
	}

	Vector3* pkNormals = (Vector3*) Md2Normals;

	int						m_iFromFrame, m_iToFrame;
	m_iFromFrame = m_iRenderFrame;
	m_iToFrame = m_iRenderFrame + 1;
	if(m_iToFrame >= m_kHead.num_frames)
		m_iToFrame = 0;

	md2_daliasframe_t		*pkFromFrame, *pkToFrame;
	pkFromFrame = &m_pakFrames[m_iFromFrame].aliasframe;
	pkToFrame = &m_pakFrames[m_iToFrame].aliasframe;
	
	md2_dtrivertx_t *pkFromVertex, *pkToVertex;
	Vector3 kFrom, kTo;
	Vector3 kFinal;

	for(int v=0; v < m_kHead.num_xyz; v++)
	{
		pkFromVertex = &m_pakFrames[m_iFromFrame].vertex[v];
		pkToVertex   = &m_pakFrames[m_iToFrame].vertex[v];

		kFrom.x = (float) (pkFromFrame->scale[0] * pkFromVertex->v[0]) + pkFromFrame->translate[0];
		kFrom.y = (float) (pkFromFrame->scale[1] * pkFromVertex->v[1]) + pkFromFrame->translate[1];
		kFrom.z = (float) (pkFromFrame->scale[2] * pkFromVertex->v[2]) + pkFromFrame->translate[2];

		kTo.x = (float) (pkToFrame->scale[0] * pkToVertex->v[0]) + pkToFrame->translate[0];
		kTo.y = (float) (pkToFrame->scale[1] * pkToVertex->v[1]) + pkToFrame->translate[1];
		kTo.z = (float) (pkToFrame->scale[2] * pkToVertex->v[2]) + pkToFrame->translate[2];

		kFinal = kFrom + fDelta * (kTo - kFrom);

		m_pkVertex[v].x = kFinal.x;	 
		m_pkVertex[v].y = kFinal.z;	
		m_pkVertex[v].z = kFinal.y;	 
		m_pkVertex[v] = m_pkVertex[v] * MD2TOMAD_SCALE;

		// Interpol vertex normals.
		kFrom.x = pkNormals[pkFromVertex->lightnormalindex].x;
		kFrom.y = pkNormals[pkFromVertex->lightnormalindex].z;
		kFrom.z = pkNormals[pkFromVertex->lightnormalindex].y;

		kTo.x = pkNormals[pkToVertex->lightnormalindex].x;
		kTo.y = pkNormals[pkToVertex->lightnormalindex].z;
		kTo.z = pkNormals[pkToVertex->lightnormalindex].y;

		m_pkNormal[v] = kFrom + fDelta * (kTo - kFrom);
	}

	float s,t;
	int iDx = 0;
	glBegin(GL_TRIANGLES);
	for(int i=0; i<m_kHead.num_tris; i++)
	{
		iDx = 0;
		glNormal3fv((float*)&m_pkNormal[m_pakTris[i].index_xyz[iDx]]);
		s = m_pakSkinVert[ m_pakTris[i].index_st[iDx] ].s / (float)m_kHead.skinwidth;
      t = m_pakSkinVert[ m_pakTris[i].index_st[iDx] ].t / (float)m_kHead.skinheight;
      glTexCoord2f( s, t );
		glVertex3f(m_pkVertex[m_pakTris[i].index_xyz[iDx]].x,
			m_pkVertex[m_pakTris[i].index_xyz[iDx]].y,
			m_pkVertex[m_pakTris[i].index_xyz[iDx]].z);

		iDx = 1;
		glNormal3fv((float*)&m_pkNormal[m_pakTris[i].index_xyz[iDx]]);
		s = m_pakSkinVert[ m_pakTris[i].index_st[iDx] ].s / (float)m_kHead.skinwidth;
      t = m_pakSkinVert[ m_pakTris[i].index_st[iDx] ].t / (float)m_kHead.skinheight;
      glTexCoord2f( s, t );
		glVertex3f(m_pkVertex[m_pakTris[i].index_xyz[iDx]].x,
			m_pkVertex[m_pakTris[i].index_xyz[iDx]].y,
			m_pkVertex[m_pakTris[i].index_xyz[iDx]].z);

		iDx = 2;
		glNormal3fv((float*)&m_pkNormal[m_pakTris[i].index_xyz[iDx]]);
		s = m_pakSkinVert[ m_pakTris[i].index_st[iDx] ].s / (float)m_kHead.skinwidth;
      t = m_pakSkinVert[ m_pakTris[i].index_st[iDx] ].t / (float)m_kHead.skinheight;
      glTexCoord2f( s, t );
		glVertex3f(m_pkVertex[m_pakTris[i].index_xyz[iDx]].x,
			m_pkVertex[m_pakTris[i].index_xyz[iDx]].y,
			m_pkVertex[m_pakTris[i].index_xyz[iDx]].z);
	}

	glEnd();
	m_pkZShaderSystem->MatrixPop();
}

vector<PropertyValues> P_Md2::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
	kReturn[0].kValueName = "m_kMadFile";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_strFileName;

	kReturn[1].kValueName = "scale";
	kReturn[1].iValueType = VALUETYPE_FLOAT;
	kReturn[1].pkValue    = (void*)&m_fScale;
		
	return kReturn;
}

bool P_Md2::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(strcmp(kValueName.c_str(), "m_kMadFile") == 0) 
	{
		LoadModel(kValue);
		return true;
	}
	
	return false;
}


void P_Md2::Save(ZFIoInterface* pkPackage)
{
	char temp[128];
	strcpy(temp,m_strFileName.c_str());
	pkPackage->Write((void*)temp,128,1);

//	pkPackage->Write(m_fSize);
}

void P_Md2::Load(ZFIoInterface* pkPackage,int iVersion)
{
	char temp[128];
	pkPackage->Read((void*)temp,128,1);	
	LoadModel(temp);
}

// Returns mdl version for quake mdl (1-3) or 0 if not a quake series mdl.
int P_Md2::GetQuakeModellVersion(PAKFileFp *mdlfp, const char* filename)
{
	int	name_version = 0;	// Version from file ext.
	int head_version = 0;	// Version from file header.

	// First get version from file name. 
	char* strpre = strchr( filename, '.');

	if(_stricmp(strpre,".mdl") == 0)	name_version = 1;	
	if(_stricmp(strpre,".md2") == 0)	name_version = 2;	

	// Now chek file head.
	int		ident;
	int		version;			

	mdlfp->Read(&ident,	sizeof(ident),	1);
	mdlfp->Read(&version,	sizeof(version),1);
	
	if(name_version == 1) {
		if(ident != MDL_IDENT_TAG)			return 0;
		if(version == MDL_ALIAS_VERSION)	return 1;
		}

	if(name_version == 2) {
		if(ident != MD2_IDENT_TAG)			return 0;
		if(version == MD2_ALIAS_VERSION)	return 2;
		}

	return 0;
}

void P_Md2::LoadModel(string strFileName)
{
	if(strFileName.empty())
		return;

	m_strFileName = strFileName;

	m_kFile.Open(strFileName.c_str());
	if(m_kFile.pkFp == NULL)
	{
		return;
	}

	// Check that its a quake series modell file.
	int q_ver = GetQuakeModellVersion(&m_kFile, strFileName.c_str());

	if (q_ver == 0)	{
		m_kFile.Close();	
		return;
		}

	// Read the header.
	m_kFile.Read(&m_kHead, sizeof(q2mdlhead_s),1);

	m_kFile.Read(g_skins, m_kHead.num_skins * MD2_MAX_SKINNAME, 1);

	// Read texture coo
	m_pakSkinVert = new md2_dstvert_t [m_kHead.num_st];
	m_kFile.Read(m_pakSkinVert, m_kHead.num_st * sizeof(md2_dstvert_t), 1);

	// Read triangles.
	m_pakTris = new md2_dtriangle_t [m_kHead.num_tris];
	m_kFile.Read(m_pakTris, m_kHead.num_tris * sizeof(md2_dtriangle_t), 1);

	// Read frames.
	m_pakFrames = new q2frame_t [m_kHead.num_frames];
	memset(m_pakFrames,0,sizeof(q2frame_t) * m_kHead.num_frames);

	for(int i=0;i<m_kHead.num_frames;i++)
	{
		m_kFile.Read(&m_pakFrames[i].aliasframe, sizeof(md2_daliasframe_t), 1);
		m_pakFrames[i].vertex = new md2_dtrivertx_t [m_kHead.num_xyz];
		m_kFile.Read(m_pakFrames[i].vertex, m_kHead.framesize - sizeof(md2_daliasframe_t),1);
	}

	m_kFile.Close();
}



Property* Create_P_Md2()
{
	return new P_Md2;
}

void Register_P_Md2(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Md2", Create_P_Md2);					

	// Register Property Script Interface
}
