#include <iostream>
#include "mdl.h"
#include "qpack.h"

#define	MD1TOMAD_SCALE 0.02
float		g_fMd1ToMadScale;

float MdlNormals[] = {
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
 
color_rgb quake_palette[768];

int GetQuakeModellVersion(PAKFileFp *mdlfp, const char* filename);

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

void ModellMD1::TransformTexCoo(int t, int v, Mad_TextureCoo *st)
{
	st->s = (float) skinvert[v].s;		
	st->t = (float) skinvert[v].t;	

	if( (skinvert[v].onseam) && (!tris[t].facesfront)) {
		st->s += (head.skinwidth / 2);
		}

	st->s = (float) st->s / head.skinwidth;	
	st->t = (float) st->t / head.skinheight;
}


void mdl_loadskin(FILE* fp, int w, int h,Image* img)
{
	img->create_empty(w, h);
/*	img->height = h;
	img->width  = w;
	int img_size = img->width * img->height;
	img->pixels = new color_rgba [img_size];*/
	int img_size = img->width * img->height;
	
	unsigned char* data = new unsigned char [img_size];
	fread(data,1,img_size,fp);

	// Translate image to rgb-24.
	for(int i = 0; i<img_size; i++) {
		img->pixels[i].r = quake_palette[ data[i] ].r;
		img->pixels[i].g = quake_palette[ data[i] ].g;
		img->pixels[i].b = quake_palette[ data[i] ].b;
		img->pixels[i].a = 0;
		}

	delete [] data;
}

int ModellMD1::ReadSkins(void)
{
	int skintype;
	int skinsize;
	int num_of_muliskins;

	skinsize = head.skinwidth * head.skinheight;

	Image kSkinImage;
	Image kSkinImage2;

	for(int i=0; i<head.numskins; i++) {
		kSkinImage.free();
		md1fp.Read(&skintype, 4,1);

		if(skintype == 0) {
			//if(!img->pixels)
			//	mdl_loadskin(mdl->mdlfp->fp, mdl->head.skinwidth, mdl->head.skinheight,img);
			//else	
				mdl_loadskin(md1fp.pkFp, head.skinwidth, head.skinheight,&kSkinImage);
				kSkinImage.save("ost.tga",false);
				kSkinImage2 = kSkinImage;
				kSkinImage2.save("ost2.tga",false);
				Skins.push_back(kSkinImage);
				kSkinImage.free();
				//md1fp.Seek(skinsize,SEEK_CUR);
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

void ModellMD1::Read( const char* filename )
{
	// Register Path to Quake Pak's.
	g_PakFileSystem.RegisterPak("c:\\spel\\quake\\id1\\pak0.pak");
	g_PakFileSystem.RegisterPak("c:\\spel\\quake\\id1\\pak1.pak");

	// Load Quake Palette.
	PAKFileFp	kQuakePal;
	kQuakePal.Open("gfx/palette.lmp");
	if(kQuakePal.pkFp == NULL)	{
		cout << "Failed to load palette" << endl;
		return;
		}
	kQuakePal.Read(quake_palette,1,768);

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

extern void SplitAnimNumAndFrameNum(int AnimAndFrame, int& Anim, int& Frame);

bool ModellMD1::Export(MadExporter* mad, const char* filename)
{
	g_fMd1ToMadScale = MD1TOMAD_SCALE;

	unsigned int i;
	Mad_CoreMesh* pkMesh = mad->GetMesh("mesh");

	// Transfer to pmd struct.
	pkMesh->kHead.iVersionNum		= 1;
	pkMesh->kHead.iNumOfTextures	= 1;
	pkMesh->kHead.iNumOfVertex		= head.numverts;
	pkMesh->kHead.iNumOfFaces		= head.numtris;
	pkMesh->kHead.iNumOfFrames		= head.numframes;
	pkMesh->kHead.iNumOfSubMeshes	= 1;
	pkMesh->kHead.iNumOfAnimation	= 1;

	pkMesh->ResizeTextures(pkMesh->kHead.iNumOfTextures);

	strcpy(pkMesh->GetTexture2()[0].ucTextureName, "mdlskin");
	if(strcmp(filename, "none") != 0)
		strcpy(pkMesh->GetTexture2()[0].ucTextureName, filename);

	// Save all textures
	char FullTextureName[256];

	for(i=0; i<Skins.size(); i++) {
		sprintf(FullTextureName, "%s.tga",pkMesh->GetTexture2()[i].ucTextureName);
		Skins[i].flip(false, true);	
		Skins[i].save(FullTextureName,false);
		}

	vector<pmd_triangle_s>	akPmdTriangles;
	vector<Mad_CoreVertexFrame>	akVertexFrames;
	vector<Mad_TextureCoo>	akTextureCoo;

	akVertexFrames.resize(pkMesh->kHead.iNumOfFrames);
	akPmdTriangles.resize(pkMesh->kHead.iNumOfFaces);

	// Read Triangles
	int vi = 0;
	int iTextureCooIndex = 0;
	Mad_TextureCoo kNyTextureCoo;

	for(i=0; i<pkMesh->kHead.iNumOfFaces; i++) {
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

	Mad_CoreMeshAnimation* pkAnim;
	Mad_CoreKeyFrame kKeyFrame;
	int iAnimNum, iFrameNum;
	char AnimName[256];
	char AnimNumAsString[256];

	for(f=0; f<pkMesh->kHead.iNumOfFrames; f++) {
		// Alloc frame mem
		af = &frames[f].head;

		// Put this in correct anim.	
		int iFrameNumIndex = strcspn( af->name, "1234567890" );

		int iAnimFrameNum = atoi(&af->name[iFrameNumIndex]);
		SplitAnimNumAndFrameNum(iAnimFrameNum, iAnimNum, iFrameNum);
		strncpy( AnimName, af->name, iFrameNumIndex);
		AnimName[iFrameNumIndex] = 0;
		
		cout << f << ": "<< af->name  << " = " << AnimName << "/" << iAnimFrameNum << endl; 

 		_itoa(iAnimNum,AnimNumAsString,10);
		strcat(AnimName,AnimNumAsString);
		pkAnim = pkMesh->GetAnimation(AnimName);

		kKeyFrame.Clear();
		kKeyFrame.fFrameTime = 0.1;
		kKeyFrame.iVertexFrame = f;
		pkAnim->PushBack(kKeyFrame);
//		pkAnim->KeyFrame.push_back(kKeyFrame);


//		akVertexFrames[f].akVertex.resize(pkMesh->kHead.iNumOfVertex);
//		akVertexFrames[f].akNormal.resize(pkMesh->kHead.iNumOfVertex);
		akVertexFrames[f].Resize(pkMesh->kHead.iNumOfVertex);

		for(int v=0; v < pkMesh->kHead.iNumOfVertex; v++) {
			vert = &frames[f].vertex[v];

			x = (float) (head.scale.x * vert->packedposition[0]) + head.scale_origin.x;
			y = (float) (head.scale.y * vert->packedposition[1]) + head.scale_origin.y;
			z = (float) (head.scale.z * vert->packedposition[2]) + head.scale_origin.z;

			akVertexFrames[f].GetVertexPointer2()[v].x = y;
			akVertexFrames[f].GetVertexPointer2()[v].y = z;
			akVertexFrames[f].GetVertexPointer2()[v].z = x;
	
			Vector3* pkNormals = (Vector3*) MdlNormals;
			akVertexFrames[f].GetNormalPointer2()[v] = pkNormals[vert->lightnormalindex];
			//akVertexFrames[f].akNormal[v] = akVertexFrames[f].akVertex[v];
			//akVertexFrames[f].akNormal[v].Normalize();
			}
	}

/*	pkMesh->akFrames.resize(pkMesh->kHead.iNumOfFrames);
	pkMesh->akFaces.resize(pkMesh->kHead.iNumOfFaces);
	pkMesh->akSubMeshes.resize(pkMesh->kHead.iNumOfSubMeshes);*/
	pkMesh->ResizeFrames(pkMesh->kHead.iNumOfFrames);
	pkMesh->ResizeFaces(pkMesh->kHead.iNumOfFaces);
	pkMesh->ResizeSubMesh(pkMesh->kHead.iNumOfSubMeshes);
	
	// Copy Submeshes.
	pkMesh->GetSubMeshPointer()[0].iFirstTriangle	= 0;
	pkMesh->GetSubMeshPointer()[0].iTextureIndex	= 0;
	pkMesh->GetSubMeshPointer()[0].iNumOfTriangles	= pkMesh->kHead.iNumOfFaces;

	// Copy Faces
	for(i=0; i<pkMesh->kHead.iNumOfFaces; i++) 
	{
		pkMesh->GetFacesPointer2()[i].iIndex[0] = akPmdTriangles[i].vertex_index[0];
		pkMesh->GetFacesPointer2()[i].iIndex[1] = akPmdTriangles[i].vertex_index[1];
		pkMesh->GetFacesPointer2()[i].iIndex[2] = akPmdTriangles[i].vertex_index[2];
	}


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
				}
			}
		}
	}

/*	for(f=0; f<mad->kHead.iNumOfFrames; f++) {
		mad->akFrames[f].akVertex = akVertexFrames[f].akVertex;
	}*/

	int texcoosize = kTextureCoo.size();
	int iNewNumOfVertex = kVertexBufferIndex.size();
	pkMesh->kHead.iNumOfVertex = iNewNumOfVertex;	

	// Patch alla vertex buffers med nyskapade vertex.
	// Cope Vertex Frame Data.
	for(f=0; f<pkMesh->kHead.iNumOfFrames; f++) {
		// Create memory
		pkMesh->GetFramesPointer2()[f].Resize(iNewNumOfVertex);
		//pkMesh->GetFramesPointer2()[f].akNormal.resize(iNewNumOfVertex);

		for(int v=0; v < iNewNumOfVertex; v++) {

			pkMesh->GetFramesPointer2()[f].GetVertexPointer2()[v] = akVertexFrames[f].GetVertexPointer2()[kVertexBufferIndex[v]] * g_fMd1ToMadScale;
			/*pkMesh->akFrames[f].akVertex[v].x = akVertexFrames[f].akVertex[kVertexBufferIndex[v]].x;
			pkMesh->akFrames[f].akVertex[v].y = akVertexFrames[f].akVertex[kVertexBufferIndex[v]].y;
			pkMesh->akFrames[f].akVertex[v].z = akVertexFrames[f].akVertex[kVertexBufferIndex[v]].z;*/
			pkMesh->GetFramesPointer2()[f].GetNormalPointer2()[v] = akVertexFrames[f].GetNormalPointer2()[kVertexBufferIndex[v]];
			}
	}


//	mad->akTextureCoo = akTextureCoo;
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

/*	Mad_Animation* pkAnim;
	Mad_KeyFrame kKeyFrame;
	pkAnim = mad->GetAnimation("std");

	for( i=0; i<head.numframes; i++) {
		kKeyFrame.Clear();
		kKeyFrame.fFrameTime = 0.1;
		kKeyFrame.iVertexFrame = i;
		pkAnim->KeyFrame.push_back(kKeyFrame);
	}*/


	return true;
}
