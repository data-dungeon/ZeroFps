#include <iostream>
#include "mdl.h"

// Returns mdl version for quake mdl (1-3) or 0 if not a quake series mdl.
int GetQuakeModellVersion(PAKFileFp *mdlfp, char* filename)
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

void ModellMD2::ReadMD2( char* filename )
{
	md2fp.Open(filename);
	if(md2fp.pkFp == NULL)	return;

	// Check that its a quake series modell file.
	int q_ver = GetQuakeModellVersion(&md2fp, filename);

	if (q_ver == 0)	{
		md2fp.Close();	
		return;
		}

	// Read the header.
	md2fp.Read(&head, sizeof(q2mdlhead_s),1);

	md2fp.Read(g_skins, head.num_skins * MD2_MAX_SKINNAME, 1);

	// Read texture coo
	skinvert = new md2_dstvert_t [head.num_st];
	md2fp.Read(skinvert, head.num_st * sizeof(md2_dstvert_t), 1);

	// Read triangles.
	tris = new md2_dtriangle_t [head.num_tris];
	md2fp.Read(tris, head.num_tris * sizeof(md2_dtriangle_t), 1);

	// Read frames.
	frames = new q2frame_t [head.num_frames];
	memset(frames,0,sizeof(q2frame_t) * head.num_frames);

	for(int i=0;i<head.num_frames;i++)
	{
		md2fp.Read(&frames[i].aliasframe, sizeof(md2_daliasframe_t), 1);
		frames[i].vertex = new md2_dtrivertx_t [head.num_xyz];
		md2fp.Read(frames[i].vertex, head.framesize - sizeof(md2_daliasframe_t),1);
	}

	md2fp.Close();
}

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



bool ReadQuakeSeriesModell(pmd_c* pmd, char* filename)
{
	ModellMD2 q2mdl;
	q2mdl.ReadMD2(filename);
	q2mdl.Export(pmd);
	return true;
}

