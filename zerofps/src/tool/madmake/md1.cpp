#include <iostream>
#include "mdl.h"
#include "qpack.h"

int GetQuakeModellVersion(PAKFileFp *mdlfp, char* filename);

void ModellMD1::PrintInfo()
{
	cout << "head.boundingradius: " << head.boundingradius << endl; 
	cout << "head.boundingradius: " << head.boundingradius << endl; 
	cout << "head.numskins: " << head.numskins << endl; 
	cout << "head.skinwidth: " << head.skinwidth << endl; 
	cout << "head.skinheight: " << head.skinheight << endl; 
	cout << "head.numverts: " << head.numverts << endl; 
	cout << "head.numtris: " << head.numtris << endl; 
	cout << "head.numframes: " << head.numframes << endl; 
	cout << "head.synctype: " << head.synctype << endl; 
	cout << "head.flags: " << head.flags << endl; 
	cout << "head.size: " << head.size << endl; 
}

void ModellMD1::TransformTexCoo(int t, int v, MadTextureCoo *st)
{
	st->s = (float) skinvert[v].s;		
	st->t = (float) skinvert[v].t;	

	if( (skinvert[v].onseam) && (!tris[t].facesfront)) {
		st->s += (head.skinwidth / 2);
		}

	st->s = (float) st->s / head.skinwidth;	
	st->t = (float) st->t / head.skinheight;
}


int ModellMD1::ReadSkins(void)
{
	int skintype;
	int skinsize;
	int num_of_muliskins;

	skinsize = head.skinwidth * head.skinheight;

	for(int i=0; i<head.numskins; i++) {
		md1fp.Read(&skintype, 4,1);

		if(skintype == 0) {
			//if(!img->pixels)
			//	mdl_loadskin(mdl->mdlfp->fp, mdl->head.skinwidth, mdl->head.skinheight,img);
			//else	
				md1fp.Seek(skinsize,SEEK_CUR);
			}
		else {
			md1fp.Read(&num_of_muliskins, 4,1);
			md1fp.Seek(sizeof(float)*num_of_muliskins,SEEK_CUR);	// Skip time values.
			
			// If no skin read first of the multiskins.
			/*if(!img->pixels) {
				mdl_loadskin(mdlfp->fp, head.skinwidth, head.skinheight,img);
				num_of_muliskins--;
				}*/
				
			// Skip the rest of the skins.
			md1fp.Seek(num_of_muliskins * skinsize,SEEK_CUR);	
			}
		}

	skinvert = new dstvert_s [head.numverts];
	md1fp.Read(skinvert, sizeof(dstvert_s),head.numverts);
	
	return true;
}

void ModellMD1::ReadTriangles(void)
{
	// Alloc triangles.
	tris = new dtriangle_s [head.numtris];
	md1fp.Read(tris, sizeof(dtriangle_s),head.numtris);
}

bool ModellMD1::ReadFrames(void)
{
	int type;					

	// Alloc mem for frames.
	frames = new mframe_s [head.numframes];
	memset(frames,0,sizeof(mframe_s) * head.numframes);

	for(int i=0; i<head.numframes; i++) {
		md1fp.Read(&type, 4, 1);

		if(type == 0)	{
			// Read a single frame.
			md1fp.Read(&frames[i].head, sizeof(daliasframe_s),1);
			frames[i].vertex = new dtrivertx_s [head.numverts];
			md1fp.Read(frames[i].vertex, sizeof(dtrivertx_s),head.numverts);
			}
		
		else		return false;	// Don't support multiframes.
		}

	return true;
}

void ModellMD1::Read( char* filename )
{
	g_PakFileSystem.RegisterPak("c:\\spel\\quake\\id1\\pak0.pak");
	g_PakFileSystem.RegisterPak("c:\\spel\\quake\\id1\\pak1.pak");

	md1fp.Open(filename);
	if(md1fp.pkFp == NULL)	return;

	// Check that its a quake series modell file.
	int q_ver = GetQuakeModellVersion(&md1fp, filename);

	if (q_ver == 0)	{
		md1fp.Close();	
		return;
		}

	md1fp.Read(&head, sizeof(mdlhead_s),1);
	PrintInfo();

	ReadSkins();
	ReadTriangles();
	ReadFrames();
	

}

bool ModellMD1::Export(MadExporter* mad)
{
	// Transfer to pmd struct.
	mad->kHead.iVersionNum		= 1;
	mad->kHead.iNumOfTextures	= 1;
	mad->kHead.iNumOfVertex		= head.numverts;
	mad->kHead.iNumOfFaces		= head.numtris;
	mad->kHead.iNumOfFrames		= head.numframes;
	mad->kHead.iNumOfSubMeshes	= 1;
	mad->kHead.iNumOfAnimation	= 1;

	strcpy(mad->akTextures[0].ucTextureName, "mdlskin");

	vector<pmd_triangle_s>	akPmdTriangles;
	vector<Mad_VertexFrame>	akVertexFrames;
	vector<MadTextureCoo>	akTextureCoo;

	akVertexFrames.resize(mad->kHead.iNumOfFrames);
	akPmdTriangles.resize(mad->kHead.iNumOfFaces);

	// Read Triangles
	int vi = 0;
	int i;
	int iTextureCooIndex = 0;
	MadTextureCoo kNyTextureCoo;

	for(i=0; i<mad->kHead.iNumOfFaces; i++) {
		akPmdTriangles[i].vertex_index[0] = tris[i].vertices[0];
		TransformTexCoo(i, tris[i].vertices[0], &kNyTextureCoo);
		akTextureCoo.push_back(kNyTextureCoo);
		akPmdTriangles[i].texcoo_index[0] = iTextureCooIndex++;

		akPmdTriangles[i].vertex_index[1] = tris[i].vertices[1];
		TransformTexCoo(i, tris[i].vertices[1], &kNyTextureCoo);
		akTextureCoo.push_back(kNyTextureCoo);
		akPmdTriangles[i].texcoo_index[1] = iTextureCooIndex++;

		akPmdTriangles[i].vertex_index[2] = tris[i].vertices[2];
		TransformTexCoo(i, tris[i].vertices[2], &kNyTextureCoo);
		akTextureCoo.push_back(kNyTextureCoo);
		akPmdTriangles[i].texcoo_index[2] = iTextureCooIndex++;
		}

	dtrivertx_s			*vert;
	daliasframe_s		*af;
	af = &frames[0].head;
	float x,y,z;
	int f;

	for(f=0; f<mad->kHead.iNumOfFrames; f++) {
		// Alloc frame mem
		af = &frames[f].head;

		akVertexFrames[f].akVertex.resize(mad->kHead.iNumOfVertex);

		for(int v=0; v < mad->kHead.iNumOfVertex; v++) {
			vert = &frames[f].vertex[v];

			x = (float) (head.scale.x * vert->packedposition[0]) + head.scale_origin.x;
			y = (float) (head.scale.y * vert->packedposition[1]) + head.scale_origin.y;
			z = (float) (head.scale.z * vert->packedposition[2]) + head.scale_origin.z;
			
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

/*	for(f=0; f<mad->kHead.iNumOfFrames; f++) {
		mad->akFrames[f].akVertex = akVertexFrames[f].akVertex;
	}*/

	int texcoosize = kTextureCoo.size();
	int iNewNumOfVertex = kVertexBufferIndex.size();
	mad->kHead.iNumOfVertex = iNewNumOfVertex;	

	// Patch alla vertex buffers med nyskapade vertex.
	// Cope Vertex Frame Data.
	for(f=0; f<mad->kHead.iNumOfFrames; f++) {
		// Create memory
		mad->akFrames[f].akVertex.resize(iNewNumOfVertex);

		for(int v=0; v < iNewNumOfVertex; v++) {

			mad->akFrames[f].akVertex[v].x = akVertexFrames[f].akVertex[kVertexBufferIndex[v]].x;
			mad->akFrames[f].akVertex[v].y = akVertexFrames[f].akVertex[kVertexBufferIndex[v]].y;
			mad->akFrames[f].akVertex[v].z = akVertexFrames[f].akVertex[kVertexBufferIndex[v]].z;
			}
	}


//	mad->akTextureCoo = akTextureCoo;
	mad->akTextureCoo.resize(mad->kHead.iNumOfVertex);
	for(i = 0; i<iNewNumOfVertex;i++)
	{
		mad->akTextureCoo[i].s = kTextureCoo[i].s;
		mad->akTextureCoo[i].t = kTextureCoo[i].t;
	}

	Mad_Animation* pkAnim;
	Mad_KeyFrame kKeyFrame;
	pkAnim = mad->GetAnimation("std");

	for( i=0; i<head.numframes; i++) {
		kKeyFrame.Clear();
		kKeyFrame.fFrameTime = 0.1;
		kKeyFrame.iVertexFrame = i;
		pkAnim->KeyFrame.push_back(kKeyFrame);
	}


	return true;
}
