/*
	Version 1. 
	Quake 2 Modeller och 3DS filer export.
	Vertex animerade object.
*/
#include "pmd.h"
#include <string>
#include "mad.h"
#include "vector"
#include "iostream"
#include <cstdio>

using namespace std;



MadExporter::MadExporter()
{

}

MadExporter::~MadExporter()
{

}

/*
void MadExporter::ImportPMD(pmd_c* pmd)
{
	int i;

	// Copy head Data.
	kHead.iVersionNum		= 1;
	kHead.iNumOfTextures	= pmd->head.num_of_textures;
	kHead.iNumOfVertex		= pmd->head.num_of_vertex;
	kHead.iNumOfFaces		= pmd->head.num_of_triangles;
	kHead.iNumOfFrames		= pmd->head.num_of_frames;
	kHead.iNumOfVertex		= pmd->head.num_of_vertexcoo;

	kHead.iNumOfSubMeshes = 1;
	kHead.iNumOfAnimation = 0;

	// Copy Texture Names
	for(i=0; i<kHead.iNumOfTextures; i++)
		strcpy(akTextures[i].ucTextureName, pmd->textures[i].texname);

	akFrames.resize(kHead.iNumOfFrames);
	akFaces.resize(kHead.iNumOfFaces);
	akSubMeshes.resize(kHead.iNumOfSubMeshes);
//	pakSubMeshes		= new Mad_SubMesh [kHead.iNumOfSubMeshes];
//	pakFaces			= new MadFace [kHead.iNumOfFaces];
//	pakFrames			= new Mad_VertexFrame [kHead.iNumOfFrames];
//	pakVertexNormals	= new MadVertex [kHead.iNumOfVertex];
//	pakFaceNormals		= new MadVertex [kHead.iNumOfFaces];

	
	// Copy Submeshes.
	akSubMeshes[0].iFirstTriangle	= 0;
	akSubMeshes[0].iTextureIndex	= 0;
	akSubMeshes[0].iNumOfTriangles	= kHead.iNumOfFaces;

	// Copy Faces
	for(i=0; i<kHead.iNumOfFaces; i++) 
	{
		akFaces[i].iIndex[0] = pmd->triangle[i].vertex_index[0];
		akFaces[i].iIndex[1] = pmd->triangle[i].vertex_index[1];
		akFaces[i].iIndex[2] = pmd->triangle[i].vertex_index[2];
	}

	MadTextureCoo kNyTextureCoo;

	// Skapa array med alla texture coo. Sätt alla till <0,0>
	vector<MadTextureCoo> kTextureCoo;
	kTextureCoo.resize(kHead.iNumOfVertex);

	// Skapa vertex list med index. Sätt alla till -1.
	vector<int> kVertexBufferIndex;
 	kVertexBufferIndex.resize(kHead.iNumOfVertex);
	for(i = 0; i < kVertexBufferIndex.size(); i++)
		kVertexBufferIndex[i] = -1;

	// loopa alla face index.
	for(i=0; i<kHead.iNumOfFaces; i++) 
	{
		for(int v = 0; v<3; v++)
		{
			int iActiveIndex = akFaces[i].iIndex[v];
			kNyTextureCoo.s = pmd->texture_coo[pmd->triangle[i].texcoo_index[v]].s;
			kNyTextureCoo.t = pmd->texture_coo[pmd->triangle[i].texcoo_index[v]].t;

			// Om denna vertex inte är satt dvs != -1
			if(kVertexBufferIndex[ iActiveIndex ] == -1) 
			{
				// Sätt dess texture coo.
				kTextureCoo[iActiveIndex].s = pmd->texture_coo[pmd->triangle[i].texcoo_index[v]].s;
				kTextureCoo[iActiveIndex].t = pmd->texture_coo[pmd->triangle[i].texcoo_index[v]].t;
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
					akFaces[i].iIndex[v] = kVertexBufferIndex.size() - 1;
				}
			}
		}
	}

	int texcoosize = kTextureCoo.size();
	int iNewNumOfVertex = kVertexBufferIndex.size();
	kHead.iNumOfVertex = iNewNumOfVertex;	

	// Patch alla vertex buffers med nyskapade vertex.
	// Cope Vertex Frame Data.
	for(int f=0; f<kHead.iNumOfFrames; f++) {
		// Create memory
//		pakFrames[f].pVertex = new MadVertex [iNewNumOfVertex];
		akFrames[f].akVertex.resize(iNewNumOfVertex);

		for(int v=0; v < iNewNumOfVertex; v++) {

			akFrames[f].akVertex[v].x = pmd->frames[f].vertex[kVertexBufferIndex[v]].x;
			akFrames[f].akVertex[v].y = pmd->frames[f].vertex[kVertexBufferIndex[v]].y;
			akFrames[f].akVertex[v].z = pmd->frames[f].vertex[kVertexBufferIndex[v]].z;
			}
	}


//	pakTextureCoo		= new MadTextureCoo [kHead.iNumOfVertex];
	akTextureCoo.resize(kHead.iNumOfVertex);
	for(i = 0; i<iNewNumOfVertex;i++)
	{
		akTextureCoo[i].s = kTextureCoo[i].s;
		akTextureCoo[i].t = kTextureCoo[i].t;
	}

	// Import Animations.
	vector<Pmd_Animation>::iterator		itAnim;
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
	}
}
*/

void MadExporter::Save(char* filename)
{
	FILE* fp = fopen(filename, "wb");
	// Write Head.
	fwrite(&kHead,sizeof(Mad_Header),1,fp);

	// Write Textures
	fwrite((void *)akTextures,sizeof(Mad_Texture),kHead.iNumOfTextures,fp);

	// Write Texture Coo
	fwrite((void *)&akTextureCoo[0],sizeof(MadTextureCoo),kHead.iNumOfVertex,fp);

	// Write Alla vertex Frames.
	for(int i=0; i<kHead.iNumOfFrames; i++)
		fwrite(&akFrames[i].akVertex[0],sizeof(MadVertex),kHead.iNumOfVertex,fp);

	// Write triangles.
	fwrite(&akFaces[0],sizeof(MadFace),kHead.iNumOfFaces,fp);

	// Write Animations.
	int iNumOfAnimations = this->akAnimation.size();
	fwrite(&iNumOfAnimations,sizeof(int), 1 ,fp);

	vector<Mad_Animation>::iterator		itAnim;
	vector<Mad_KeyFrame>::iterator		itKeyF;

	for(itAnim = akAnimation.begin(); itAnim != akAnimation.end(); itAnim++)
	{
		fwrite(itAnim->Name,sizeof(char), 64 ,fp);
		int iNumOfKeyFrames = itAnim->KeyFrame.size();
		fwrite(&iNumOfKeyFrames,sizeof(int), 1 ,fp);
		
		for(itKeyF = itAnim->KeyFrame.begin(); itKeyF != itAnim->KeyFrame.end(); itKeyF++)
		{
			fwrite(&itKeyF->iVertexFrame,sizeof(int), 1 ,fp);
		}
	}

	fclose(fp);
}

Mad_Animation*	MadExporter::GetAnimation(char* ucaName)
{
	vector<Mad_Animation>::iterator it;

	for(it = akAnimation.begin(); it != akAnimation.end(); it++)
	{
		if(strcmp(it->Name, ucaName) == 0)
			return it;
	}

	// Finns ingen animation med det namnet så skapa den och returnera den.
	Mad_Animation kNewAnim;
	kNewAnim.Clear();
	strcpy(kNewAnim.Name, ucaName);
	akAnimation.push_back(kNewAnim);
	return &akAnimation.back();
}


void Mad_KeyFrame::Clear(void)
{
	iVertexFrame = 0;
	fFrameTime = 0;
}

void Mad_KeyFrame::operator=(const Mad_KeyFrame& kOther)
{
	iVertexFrame = kOther.iVertexFrame;
	fFrameTime = kOther.fFrameTime;
}

void Mad_Animation::Clear(void)
{
	strcpy(Name, "");
	KeyFrame.clear();
}

void Mad_Animation::operator=(const Mad_Animation& kOther)
{
	strcpy(Name, kOther.Name);
	KeyFrame = kOther.KeyFrame;
}

