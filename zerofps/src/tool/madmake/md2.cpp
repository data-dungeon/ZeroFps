#include <iostream>
#include "mdl.h"
#include "qpack.h"

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

void ModellMD2::Read( char* filename )
{
	g_PakFileSystem.RegisterPak("c:\\spel\\quake2\\baseq2\\pak0.pak");
	g_PakFileSystem.RegisterPak("c:\\spel\\quake2\\baseq2\\pak1.pak");

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

bool ModellMD2::Export(MadExporter* mad)
{
	int i,f;

	mad->kHead.iVersionNum		= 1;
	mad->kHead.iNumOfTextures	= head.num_skins;
	mad->kHead.iNumOfVertex		= head.num_xyz;
	mad->kHead.iNumOfFaces		= head.num_tris;
	mad->kHead.iNumOfFrames		= head.num_frames;
//	mad->kHead.iNumOfVertex		= head.num_st;
	mad->kHead.iNumOfSubMeshes	= 1;
	mad->kHead.iNumOfAnimation	= 0;

	// Force one skin.
	if(mad->kHead.iNumOfTextures == 0)
		mad->kHead.iNumOfTextures = 1;

	for(i=0; i<mad->kHead.iNumOfTextures; i++) {
		sprintf(NewSkinName,"skin_%d", i);
		strcpy(mad->akTextures[i].ucTextureName, NewSkinName);
		if(i < head.num_skins)
			g_PakFileSystem.Unpack(g_skins[i], NewSkinName);
		}

	vector<pmd_triangle_s>	akPmdTriangles;
	vector<Mad_VertexFrame>	akVertexFrames;
	vector<MadTextureCoo>	akTextureCoo;

	akVertexFrames.resize(mad->kHead.iNumOfFrames);
	akPmdTriangles.resize(mad->kHead.iNumOfFaces);
	akTextureCoo.resize(head.num_st);

	// Read Triangles
	int vi = 0;
	for(i=0; i<mad->kHead.iNumOfFaces; i++) {
		akPmdTriangles[i].vertex_index[0] = tris[i].index_xyz[0];
		akPmdTriangles[i].texcoo_index[0] = tris[i].index_st[0];
		akPmdTriangles[i].vertex_index[1] = tris[i].index_xyz[1];
		akPmdTriangles[i].texcoo_index[1] = tris[i].index_st[1];
		akPmdTriangles[i].vertex_index[2] = tris[i].index_xyz[2];
		akPmdTriangles[i].texcoo_index[2] = tris[i].index_st[2];
		akPmdTriangles[i].texture_num = 0;
		}

	for(i=0; i<head.num_st; i++) {
		akTextureCoo[i].s = (float) skinvert[i].s / head.skinwidth;
		akTextureCoo[i].t = (float) skinvert[i].t / head.skinheight;
		}

	md2_dtrivertx_t		*vert;
	md2_daliasframe_t	*af;
	af = &frames[0].aliasframe;
	float x,y,z;

	Mad_Animation* pkAnim;
	Mad_KeyFrame kKeyFrame;
	int iAnimNum, iFrameNum;
	char AnimName[256];
	char AnimNumAsString[256];

	for(f=0; f<mad->kHead.iNumOfFrames; f++) {
		// Alloc frame mem
		af = &frames[f].aliasframe;

		int iFrameNumIndex = strcspn( af->name, "1234567890" );

		int iAnimFrameNum = atoi(&af->name[iFrameNumIndex]);
		SplitAnimNumAndFrameNum(iAnimFrameNum, iAnimNum, iFrameNum);
		strncpy( AnimName, af->name, iFrameNumIndex);
		AnimName[iFrameNumIndex] = 0;
		
		_itoa(iAnimNum,AnimNumAsString,10);
		strcat(AnimName,AnimNumAsString);
		pkAnim = mad->GetAnimation(AnimName);

		kKeyFrame.Clear();
		kKeyFrame.fFrameTime = 0.1;
		kKeyFrame.iVertexFrame = f;
		pkAnim->KeyFrame.push_back(kKeyFrame);

		akVertexFrames[f].akVertex.resize(mad->kHead.iNumOfVertex);
		//akPmdVertexFrames[f].vertex.resize(mad->kHead.iNumOfVertex);

		for(int v=0; v < mad->kHead.iNumOfVertex; v++) {
			vert = &frames[f].vertex[v];

			x = (float) (af->scale[0] * vert->v[0]) + af->translate[0];
			y = (float) (af->scale[1] * vert->v[1]) + af->translate[1];
			z = (float) (af->scale[2] * vert->v[2]) + af->translate[2];
			
			akVertexFrames[f].akVertex[v].x = y;
			akVertexFrames[f].akVertex[v].y = z;
			akVertexFrames[f].akVertex[v].z = x;
			}
	}




	mad->akFrames.resize(mad->kHead.iNumOfFrames);
	mad->akFaces.resize(mad->kHead.iNumOfFaces);
	mad->akSubMeshes.resize(mad->kHead.iNumOfSubMeshes);
	
	// Copy Submeshes.
	mad->akSubMeshes[0].iFirstTriangle	= 0;
	mad->akSubMeshes[0].iTextureIndex	= 0;
	mad->akSubMeshes[0].iNumOfTriangles	= mad->kHead.iNumOfFaces;

	// Copy Faces
	for(i=0; i<mad->kHead.iNumOfFaces; i++) 
	{
		mad->akFaces[i].iIndex[0] = akPmdTriangles[i].vertex_index[0];
		mad->akFaces[i].iIndex[1] = akPmdTriangles[i].vertex_index[1];
		mad->akFaces[i].iIndex[2] = akPmdTriangles[i].vertex_index[2];
	}

	MadTextureCoo kNyTextureCoo;

	// Skapa array med alla texture coo. Sätt alla till <0,0>
	vector<MadTextureCoo> kTextureCoo;
	kTextureCoo.resize(mad->kHead.iNumOfVertex);

	// Skapa vertex list med index. Sätt alla till -1.
	vector<int> kVertexBufferIndex;
 	kVertexBufferIndex.resize(mad->kHead.iNumOfVertex);
	for(i = 0; i < kVertexBufferIndex.size(); i++)
		kVertexBufferIndex[i] = -1;

	// loopa alla face index.
	for(i=0; i<mad->kHead.iNumOfFaces; i++) 
	{
		for(int v = 0; v<3; v++)
		{
			int iActiveIndex = mad->akFaces[i].iIndex[v];
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
					mad->akFaces[i].iIndex[v] = kVertexBufferIndex.size() - 1;
				}
			}
		}
	}

	int texcoosize = kTextureCoo.size();
	int iNewNumOfVertex = kVertexBufferIndex.size();
	mad->kHead.iNumOfVertex = iNewNumOfVertex;	

	// Patch alla vertex buffers med nyskapade vertex.
	// Cope Vertex Frame Data.
	for(f=0; f<mad->kHead.iNumOfFrames; f++) {
		// Create memory
//		pakFrames[f].pVertex = new MadVertex [iNewNumOfVertex];
		mad->akFrames[f].akVertex.resize(iNewNumOfVertex);

		for(int v=0; v < iNewNumOfVertex; v++) {

			mad->akFrames[f].akVertex[v].x = akVertexFrames[f].akVertex[kVertexBufferIndex[v]].x;
			mad->akFrames[f].akVertex[v].y = akVertexFrames[f].akVertex[kVertexBufferIndex[v]].y;
			mad->akFrames[f].akVertex[v].z = akVertexFrames[f].akVertex[kVertexBufferIndex[v]].z;
			}
	}


//	pakTextureCoo		= new MadTextureCoo [kHead.iNumOfVertex];
	mad->akTextureCoo.resize(mad->kHead.iNumOfVertex);
	for(i = 0; i<iNewNumOfVertex;i++)
	{
		mad->akTextureCoo[i].s = kTextureCoo[i].s;
		mad->akTextureCoo[i].t = kTextureCoo[i].t;
	}

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

