#include <iostream>
#include "mdl.h"
#include "qpack.h"

int g_fMd2ToMadScale;


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
 

color_rgb quake2_palette[768];

// Returns mdl version for quake mdl (1-3) or 0 if not a quake series mdl.
int GetQuakeModellVersion(PAKFileFp *mdlfp, const char* filename)
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


// Quake2 Loading Code
md2_dtrivertx_t max_xyz[4096];
char g_skins[MD2_MAX_SKINS][MD2_MAX_SKINNAME];
char NewSkinName[256];

void SplitAnimNumAndFrameNum(int AnimAndFrame, int& Anim, int& Frame)
{
	Anim = Frame = 0;

	while(AnimAndFrame >= 100)
	{
		AnimAndFrame -= 100;
		Anim += 1;
	}

	Frame = AnimAndFrame;
}

void ModellMD2::Read( const char* filename )
{
	// Register Path to Quake2 Pak's.
	g_PakFileSystem.RegisterPak("c:\\spel\\quake2\\baseq2\\pak0.pak");
	g_PakFileSystem.RegisterPak("c:\\spel\\quake2\\baseq2\\pak1.pak");

	// Load Quake2 Palette.
/*	PAKFileFp	kQuakePal;
	kQuakePal.Open("quake2.pal");
	if(kQuakePal.pkFp == NULL)	{
		cout << "Failed to load palette" << endl;
		return;
		}
	kQuakePal.Read(quake2_palette,1,768);*/

	m_kFile.Open(filename);
	if(m_kFile.pkFp == NULL)	return;

	// Check that its a quake series modell file.
	int q_ver = GetQuakeModellVersion(&m_kFile, filename);

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

/*
bool ModellMD2::Export(pmd_c* pmd)
{
	int i;

	// Transfer to pmd struct.
	pmd->head.num_of_vertex		= head.num_xyz;
	pmd->head.num_of_triangles	= head.num_tris;
	pmd->head.num_of_vertexcoo  = head.num_st;
	pmd->head.num_of_frames		= head.num_frames;
	pmd->head.num_of_drawstates = 0;
	pmd->head.version_num		= VERSION_NUM;
	pmd->head.num_of_textures	= head.num_skins;

	// Force one skin.
	if(pmd->head.num_of_textures == 0)
		pmd->head.num_of_textures = 1;

	for(i=0; i<pmd->head.num_of_textures; i++) {
		sprintf(NewSkinName,"skin_%d", i);
		strcpy(pmd->textures[i].texname, NewSkinName);
		if(i < head.num_skins)
			g_PakFileSystem.Unpack(g_skins[i], NewSkinName);
		}


	pmd->frames = new pmd_vertexframe [pmd->head.num_of_frames];
	pmd->triangle	= new pmd_triangle_s [pmd->head.num_of_triangles];
	pmd->texture_coo = new pmd_texcoo_s [pmd->head.num_of_vertexcoo];

	pmd->vertex_normals		= new Vector3 [pmd->head.num_of_vertex];
	pmd->surface_normals	= new Vector3 [pmd->head.num_of_triangles];

	// DONE
	int vi = 0;

	// Read Triangles
	for(i=0; i<pmd->head.num_of_triangles; i++) {
		pmd->triangle[i].vertex_index[0] = tris[i].index_xyz[0];
		pmd->triangle[i].texcoo_index[0] = tris[i].index_st[0];
		pmd->triangle[i].vertex_index[1] = tris[i].index_xyz[1];
		pmd->triangle[i].texcoo_index[1] = tris[i].index_st[1];
		pmd->triangle[i].vertex_index[2] = tris[i].index_xyz[2];
		pmd->triangle[i].texcoo_index[2] = tris[i].index_st[2];
		pmd->triangle[i].texture_num = 0;
		}

	for(i=0; i<pmd->head.num_of_vertexcoo; i++) {
		pmd->texture_coo[i].s = (float) skinvert[i].s / head.skinwidth;
		pmd->texture_coo[i].t = (float) skinvert[i].t / head.skinheight;
		}
	
	
	md2_dtrivertx_t		*vert;
	md2_daliasframe_t	*af;
	af = &frames[0].aliasframe;
	float x,y,z;

	Pmd_Animation* pkAnim;
	Pmd_KeyFrame kKeyFrame;
	int iAnimNum, iFrameNum;
	char AnimName[256];
	char AnimNumAsString[256];

	for(int f=0; f<pmd->head.num_of_frames; f++) {
		// Alloc frame mem
		af = &frames[f].aliasframe;

		int iFrameNumIndex = strcspn( af->name, "1234567890" );

		int iAnimFrameNum = atoi(&af->name[iFrameNumIndex]);
		SplitAnimNumAndFrameNum(iAnimFrameNum, iAnimNum, iFrameNum);
		strncpy( AnimName, af->name, iFrameNumIndex);
		AnimName[iFrameNumIndex] = 0;
		
		_itoa(iAnimNum,AnimNumAsString,10);
		strcat(AnimName,AnimNumAsString);
		pkAnim = pmd->GetAnimation(AnimName);

		kKeyFrame.Clear();
		kKeyFrame.fFrameTime = 0.1;
		kKeyFrame.iVertexFrame = f;
		pkAnim->KeyFrame.push_back(kKeyFrame);

		pmd->frames[f].vertex = new Vector3 [pmd->head.num_of_vertex];

		for(int v=0; v < pmd->head.num_of_vertex; v++) {
			vert = &frames[f].vertex[v];

			x = (float) (af->scale[0] * vert->v[0]) + af->translate[0];
			y = (float) (af->scale[1] * vert->v[1]) + af->translate[1];
			z = (float) (af->scale[2] * vert->v[2]) + af->translate[2];
			
			pmd->frames[f].vertex[v].x = y;
			pmd->frames[f].vertex[v].y = z;
			pmd->frames[f].vertex[v].z = x;
			}
	}
	
	return true;
}
*/

bool ModellMD2::Export(MadExporter* mad, const char* filename)
{
	g_fMd2ToMadScale = 0.1;
	
	int i,f;

	Mad_CoreMesh* pkMesh = mad->GetMesh("mesh");
	pkMesh->kHead.iVersionNum		= 1;
	pkMesh->kHead.iNumOfTextures	= m_kHead.num_skins;
	pkMesh->kHead.iNumOfVertex		= m_kHead.num_xyz;
	pkMesh->kHead.iNumOfFaces		= m_kHead.num_tris;
	pkMesh->kHead.iNumOfFrames		= m_kHead.num_frames;
	pkMesh->kHead.iNumOfSubMeshes	= 1;
	pkMesh->kHead.iNumOfAnimation	= 0;

	// Force one skin.
	if(pkMesh->kHead.iNumOfTextures == 0)
		pkMesh->kHead.iNumOfTextures = 1;

	pkMesh->ResizeTextures(pkMesh->kHead.iNumOfTextures);

	char ucSkinName[64];
	strcpy(ucSkinName, "skin_");
	if(strcmp(filename, "none") != 0)
		strcpy(ucSkinName, filename);

	for(i=0; i<pkMesh->kHead.iNumOfTextures; i++) {
		sprintf(NewSkinName,"%s%d", ucSkinName, i);
		strcpy(pkMesh->GetTexture2()[i].ucTextureName, NewSkinName);
		if(i < m_kHead.num_skins)
			g_PakFileSystem.Unpack(g_skins[i], NewSkinName);
		}

	vector<pmd_triangle_s>	akPmdTriangles;
	vector<Mad_CoreVertexFrame>	akVertexFrames;
	vector<Mad_TextureCoo>	akTextureCoo;

	akVertexFrames.resize(pkMesh->kHead.iNumOfFrames);
	akPmdTriangles.resize(pkMesh->kHead.iNumOfFaces);
	akTextureCoo.resize(m_kHead.num_st);

	// Read Triangles
	int vi = 0;
	for(i=0; i<pkMesh->kHead.iNumOfFaces; i++) {
		akPmdTriangles[i].vertex_index[0] = m_pakTris[i].index_xyz[0];
		akPmdTriangles[i].texcoo_index[0] = m_pakTris[i].index_st[0];
		akPmdTriangles[i].vertex_index[1] = m_pakTris[i].index_xyz[1];
		akPmdTriangles[i].texcoo_index[1] = m_pakTris[i].index_st[1];
		akPmdTriangles[i].vertex_index[2] = m_pakTris[i].index_xyz[2];
		akPmdTriangles[i].texcoo_index[2] = m_pakTris[i].index_st[2];
		akPmdTriangles[i].texture_num = 0;
		}

	for(i=0; i<m_kHead.num_st; i++) {
		akTextureCoo[i].s = (float) m_pakSkinVert[i].s / m_kHead.skinwidth;
		akTextureCoo[i].t = (float) m_pakSkinVert[i].t / m_kHead.skinheight;
		}

	md2_dtrivertx_t		*vert;
	md2_daliasframe_t	*af;
	af = &m_pakFrames[0].aliasframe;
	float x,y,z;

	Mad_CoreMeshAnimation* pkAnim;
	Mad_CoreKeyFrame kKeyFrame;
	int iAnimNum, iFrameNum;
	char AnimName[256];
	char AnimNumAsString[256];

	for(f=0; f<pkMesh->kHead.iNumOfFrames; f++) {
		// Alloc frame mem
		af = &m_pakFrames[f].aliasframe;

		int iFrameNumIndex = strcspn( af->name, "1234567890" );

		int iAnimFrameNum = atoi(&af->name[iFrameNumIndex]);
		SplitAnimNumAndFrameNum(iAnimFrameNum, iAnimNum, iFrameNum);
		strncpy( AnimName, af->name, iFrameNumIndex);
		AnimName[iFrameNumIndex] = 0;
		
		_itoa(iAnimNum,AnimNumAsString,10);
		strcat(AnimName,AnimNumAsString);
		pkAnim = pkMesh->GetAnimation(AnimName);

		kKeyFrame.Clear();
		kKeyFrame.fFrameTime = 0.1;
		kKeyFrame.iVertexFrame = f;
		pkAnim->PushBack(kKeyFrame);

		akVertexFrames[f].Resize(pkMesh->kHead.iNumOfVertex);
		akVertexFrames[f].Resize(pkMesh->kHead.iNumOfVertex);

		for(int v=0; v < pkMesh->kHead.iNumOfVertex; v++) {
			vert = &m_pakFrames[f].vertex[v];

			x = (float) (af->scale[0] * vert->v[0]) + af->translate[0];
			y = (float) (af->scale[1] * vert->v[1]) + af->translate[1];
			z = (float) (af->scale[2] * vert->v[2]) + af->translate[2];
			
			akVertexFrames[f].GetVertexPointer2()[v].x = y;
			akVertexFrames[f].GetVertexPointer2()[v].y = z;
			akVertexFrames[f].GetVertexPointer2()[v].z = x;
			
			Vector3* pkNormals = (Vector3*) Md2Normals;
			akVertexFrames[f].GetNormalPointer2()[v] = pkNormals[vert->lightnormalindex];
			}
	}



/*
	pkMesh->akFrames.resize(pkMesh->kHead.iNumOfFrames);
	pkMesh->akFaces.resize(pkMesh->kHead.iNumOfFaces);
	pkMesh->akSubMeshes.resize(pkMesh->kHead.iNumOfSubMeshes);
*/

	pkMesh->ResizeFrames(pkMesh->kHead.iNumOfFrames);
	pkMesh->ResizeFaces(pkMesh->kHead.iNumOfFaces);
	pkMesh->ResizeSubMesh(pkMesh->kHead.iNumOfSubMeshes);
	
	// Copy Submeshes.
	pkMesh->GetSubMeshPointer()[0].iFirstTriangle	= 0;
	pkMesh->GetSubMeshPointer()[0].iTextureIndex	= 0;
	pkMesh->GetSubMeshPointer()[0].iNumOfTriangles	= pkMesh->kHead.iNumOfFaces;
/*
	pkMesh->akSubMeshes[0].iFirstTriangle	= 0;
	pkMesh->akSubMeshes[0].iTextureIndex	= 0;
	pkMesh->akSubMeshes[0].iNumOfTriangles	= pkMesh->kHead.iNumOfFaces;
*/

	// Copy Faces
	for(i=0; i<pkMesh->kHead.iNumOfFaces; i++) 
	{
		pkMesh->GetFacesPointer2()[i].iIndex[0] = akPmdTriangles[i].vertex_index[0];
		pkMesh->GetFacesPointer2()[i].iIndex[1] = akPmdTriangles[i].vertex_index[1];
		pkMesh->GetFacesPointer2()[i].iIndex[2] = akPmdTriangles[i].vertex_index[2];
/*
		pkMesh->akFaces[i].iIndex[0] = akPmdTriangles[i].vertex_index[0];
		pkMesh->akFaces[i].iIndex[1] = akPmdTriangles[i].vertex_index[1];
		pkMesh->akFaces[i].iIndex[2] = akPmdTriangles[i].vertex_index[2];
*/
	}

	Mad_TextureCoo kNyTextureCoo;

	// Skapa array med alla texture coo. Sätt alla till <0,0>
	vector<Mad_TextureCoo> kTextureCoo;
	kTextureCoo.resize(pkMesh->kHead.iNumOfVertex);

	// Skapa vertex list med index. Sätt alla till -1.
	vector<int> kVertexBufferIndex;
 	kVertexBufferIndex.resize(pkMesh->kHead.iNumOfVertex);
	for(i = 0; i < kVertexBufferIndex.size(); i++)
		kVertexBufferIndex[i] = -1;

	// loopa alla face index.
	for(i=0; i<pkMesh->kHead.iNumOfFaces; i++) 
	{
		for(int v = 0; v<3; v++)
		{
			
			int iActiveIndex = pkMesh->GetFacesPointer2()[i].iIndex[v];
			//int iActiveIndex = pkMesh->akFaces[i].iIndex[v];
			kNyTextureCoo.s = akTextureCoo[akPmdTriangles[i].texcoo_index[v]].s;
			kNyTextureCoo.t = akTextureCoo[akPmdTriangles[i].texcoo_index[v]].t;

			// Om denna vertex inte är satt dvs != -1
			if(kVertexBufferIndex[ iActiveIndex ] == -1) 
			{
				// Sätt dess texture coo.
				kTextureCoo[iActiveIndex].s = akTextureCoo[akPmdTriangles[i].texcoo_index[v]].s;
				kTextureCoo[iActiveIndex].t = akTextureCoo[akPmdTriangles[i].texcoo_index[v]].t;
				// Sätt index till sig själv.
				kVertexBufferIndex[ iActiveIndex ] = iActiveIndex;

			}

			// Om vertex e satt 
			else
			{
				//och texture coo != aktiv tex coo
				if( !(kNyTextureCoo.s == kTextureCoo[iActiveIndex].s && kNyTextureCoo.t == kTextureCoo[iActiveIndex].t))
				{
					// insert ny tex coo vid slutet av listan
					kTextureCoo.push_back(kNyTextureCoo);
					// Insert ny vertex vid slutet av listan. Sätt index till orginal vertex index.
					kVertexBufferIndex.push_back(iActiveIndex);
					pkMesh->GetFacesPointer2()[i].iIndex[v] = kVertexBufferIndex.size() - 1;
					//pkMesh->GetFacesPointer2()akFaces[i].iIndex[v] = kVertexBufferIndex.size() - 1;
				}
			}
		}
	}

	int texcoosize = kTextureCoo.size();
	int iNewNumOfVertex = kVertexBufferIndex.size();
	pkMesh->kHead.iNumOfVertex = iNewNumOfVertex;	

	// Patch alla vertex buffers med nyskapade vertex.
	// Cope Vertex Frame Data.
	for(f=0; f<pkMesh->kHead.iNumOfFrames; f++) {
		// Create memory
//		pakFrames[f].pVertex = new MadVertex [iNewNumOfVertex];
		pkMesh->GetFramesPointer2()[f].Resize(iNewNumOfVertex);
		pkMesh->GetFramesPointer2()[f].Resize(iNewNumOfVertex);
//		pkMesh->akFrames[f].akVertex.resize(iNewNumOfVertex);
//		pkMesh->akFrames[f].akNormal.resize(iNewNumOfVertex);

		for(int v=0; v < iNewNumOfVertex; v++) {
			pkMesh->GetFramesPointer2()[f].GetVertexPointer2()[v].x = akVertexFrames[f].GetVertexPointer2()[kVertexBufferIndex[v]].x * 0.1;
			pkMesh->GetFramesPointer2()[f].GetVertexPointer2()[v].y = akVertexFrames[f].GetVertexPointer2()[kVertexBufferIndex[v]].y * 0.1;
			pkMesh->GetFramesPointer2()[f].GetVertexPointer2()[v].z = akVertexFrames[f].GetVertexPointer2()[kVertexBufferIndex[v]].z * 0.1;
			pkMesh->GetFramesPointer2()[f].GetNormalPointer2()[v] = akVertexFrames[f].GetNormalPointer2()[kVertexBufferIndex[v]];
			}
	}


//	pakTextureCoo		= new MadTextureCoo [kHead.iNumOfVertex];
//	pkMesh->akTextureCoo.resize(pkMesh->kHead.iNumOfVertex);
	pkMesh->ResizeTexturesCoo(pkMesh->kHead.iNumOfVertex);
	for(i = 0; i<iNewNumOfVertex;i++)
	{
		pkMesh->GetTexCooPointer2()[i].s = kTextureCoo[i].s;
		pkMesh->GetTexCooPointer2()[i].t = kTextureCoo[i].t;
	}

	pkMesh->CreateRigidBoneConnections();
	pkMesh->FlipFaces();
	pkMesh->CreateVertexNormals();

	// Add Bone
	vector<Mad_CoreBone>	kSkelleton;
	Mad_CoreBone NewBone;
	strcpy(NewBone.m_acName,"Base");
	NewBone.m_iParent =	-1;
	NewBone.m_kPosition.x = 0;
	NewBone.m_kPosition.y = 0;
	NewBone.m_kPosition.z = 0;
	NewBone.m_kRotation.x = 0;
	NewBone.m_kRotation.y = 0;
	NewBone.m_kRotation.z = 0;
	kSkelleton.push_back(NewBone);
	mad->SetSkelleton(kSkelleton);

	// Add Static Anim
	vector<Mad_CoreBoneAnimation>		kAnim;

	Mad_CoreBoneKey						kNewBoneKey;
	Mad_CoreBoneKeyFrame					kNewBoneKeyFrame;
	Mad_CoreBoneAnimation				kNewBoneAnim;

	kNewBoneKey.Clear();
	kNewBoneKeyFrame.Clear();
	kNewBoneAnim.Clear();

	kNewBoneKey.m_kPosition.Set(0,0,0);
	kNewBoneKey.m_kRotation.Set(0,0,0);
	kNewBoneKeyFrame.PushBack(kNewBoneKey);
	kNewBoneAnim.PushBack(kNewBoneKeyFrame);
	kAnim.push_back(kNewBoneAnim);
	mad->SetAnimation(kAnim);



	// Import Animations.
/*	vector<Pmd_Animation>::iterator		itAnim;
	vector<Pmd_KeyFrame>::iterator		itKeyF;

	Mad_Animation kNyAnimation;
	Mad_KeyFrame kNyKeyF;

	for(itAnim = pmd->kAnimation.begin(); itAnim != pmd->kAnimation.end(); itAnim++)
	{
		kNyAnimation.Clear();
		strcpy(kNyAnimation.Name, itAnim->Name); 

		for(itKeyF = itAnim->KeyFrame.begin(); itKeyF != itAnim->KeyFrame.end(); itKeyF++)
		{
			kNyKeyF.Clear();
			kNyKeyF.iVertexFrame = itKeyF->iVertexFrame;
			kNyKeyF.fFrameTime = itKeyF->fFrameTime;
			kNyAnimation.KeyFrame.push_back(kNyKeyF);
		}

		akAnimation.push_back(kNyAnimation);
	}*/




	return true;

}

