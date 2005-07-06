#include "p_mdl.h"

extern ZSSPakFile g_PakFileSystem;

#define MD2_ALIAS_VERSION	8
#define MD2_IDENT_TAG		0x32504449

#define	MD1TOMAD_SCALE 0.02

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


P_Mdl::P_Mdl()
{
	strcpy(m_acName,"P_Mdl");		
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_bNetwork = false;	
	m_iVersion = 1;
	m_iSortPlace=4;
	
	m_pkZShaderSystem	=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));				
	m_pkTextureManger =	static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));			
	m_pkRender			=	static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	
	head.numverts		=	0;
	m_iRenderFrame		=	0;
	m_fNextFrame		=	0;
	m_pkVertex			=	NULL;
}

P_Mdl::~P_Mdl()
{
}

void P_Mdl::Update()
{
	float fDelta = 0;

	float fCurrentTime = m_pkEntityManager->GetSimTime();
	if(fCurrentTime >= m_fNextFrame)
	{
		m_iRenderFrame++;
		m_fNextFrame = fCurrentTime +  0.1;
		if(m_iRenderFrame >= head.numframes)
			m_iRenderFrame = 0;
		m_fLastFrame = fCurrentTime;

	}
	else
	{
		fDelta = (fCurrentTime - m_fLastFrame) * 10;
	}

	if(head.numverts == 0)
		return;

	Matrix3 kRot;
	kRot = m_pkEntity->GetWorldRotM();		
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetIWorldPosV());
	m_pkZShaderSystem->MatrixMult(Matrix4(kRot));

	char szTextureName[256];
	strcpy(szTextureName, "ost2.tga");

	int m_iTexture = m_pkTextureManger->Load(szTextureName);
	m_pkTextureManger->BindTexture( m_iTexture );
	glEnable(GL_TEXTURE_2D);
	glColor3f(1,1,1);

	if(!m_pkVertex)
	{
		m_pkVertex = new Vector3 [head.numverts];
		m_pkNormal = new Vector3 [head.numverts];
	}

	daliasframe_s		*af;
	af = &frames[m_iRenderFrame].head;
	dtrivertx_s			*vert;

	for(int v=0; v < head.numverts; v++)
	{
		vert = &frames[m_iRenderFrame].vertex[v];

		float x = (float) (head.scale.x * vert->packedposition[0]) + head.scale_origin.x;
		float y = (float) (head.scale.y * vert->packedposition[1]) + head.scale_origin.y;
		float z = (float) (head.scale.z * vert->packedposition[2]) + head.scale_origin.z;

		m_pkVertex[v].x = y * MD1TOMAD_SCALE;
		m_pkVertex[v].y = z * MD1TOMAD_SCALE;
		m_pkVertex[v].z = x * MD1TOMAD_SCALE;

		Vector3* pkNormals = (Vector3*) MdlNormals;
		m_pkNormal[v] = pkNormals[vert->lightnormalindex];
	}

	int iDx = 0;
	glBegin(GL_TRIANGLES);
	Vector3 kTextureCoo;
	for(int i=0; i<head.numtris; i++)
	{
		iDx = 2;

		kTextureCoo = GetTextureCoo(i, tris[i].vertices[iDx]);
      glTexCoord2f( kTextureCoo.x, kTextureCoo.y );
		glNormal3fv((float*)&m_pkNormal[tris[i].vertices[iDx]]);
		glVertex3fv((float*)&m_pkVertex[tris[i].vertices[iDx]]);

		iDx = 1;
		kTextureCoo = GetTextureCoo(i, tris[i].vertices[iDx]);
      glTexCoord2f( kTextureCoo.x, kTextureCoo.y );
		glNormal3fv((float*)&m_pkNormal[tris[i].vertices[iDx]]);
		glVertex3fv((float*)&m_pkVertex[tris[i].vertices[iDx]]);

		iDx = 0;
		kTextureCoo = GetTextureCoo(i, tris[i].vertices[iDx]);
      glTexCoord2f( kTextureCoo.x, kTextureCoo.y );
		glNormal3fv((float*)&m_pkNormal[tris[i].vertices[iDx]]);
		glVertex3fv((float*)&m_pkVertex[tris[i].vertices[iDx]]);
	}

	glEnd();
	m_pkZShaderSystem->MatrixPop();

}

Vector3 P_Mdl::GetTextureCoo(int iTriangle, int iVertex)
{
	Vector3 kRes;
	kRes.x = skinvert[ iVertex ].s;
   kRes.y = skinvert[ iVertex ].t;
	if( (skinvert[iVertex].onseam) && (!tris[ iTriangle ].facesfront))
		kRes.x += (head.skinwidth / 2);
	kRes.x = kRes.x / (float)head.skinwidth;
   kRes.y = kRes.y / (float)head.skinheight;
	return kRes;
}


// Returns mdl version for quake mdl (1-3) or 0 if not a quake series mdl.
int P_Mdl::GetQuakeModellVersion(PAKFileFp *mdlfp, const char* filename)
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

void P_Mdl::PrintInfo()
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

void P_Mdl::TransformTexCoo(int t, int v, Mad_TextureCoo *st)
{
	st->s = (float) skinvert[v].s;		
	st->t = (float) skinvert[v].t;	

	if( (skinvert[v].onseam) && (!tris[t].facesfront)) {
		st->s += (head.skinwidth / 2);
		}

	st->s = (float) st->s / head.skinwidth;	
	st->t = (float) st->t / head.skinheight;
}


void P_Mdl::mdl_loadskin(FILE* fp, int w, int h,Image* img)
{
	img->CreateEmpty(w, h);
/*	img->height = h;
	img->width  = w;
	int img_size = img->width * img->height;
	img->pixels = new color_rgba [img_size];*/
	int img_size = img->m_iWidth * img->m_iHeight;
	
	unsigned char* data = new unsigned char [img_size];
	fread(data,1,img_size,fp);

	// Translate image to rgb-24.
	for(int i = 0; i<img_size; i++) {
		img->m_pkPixels[i].r = quake_palette[ data[i] ].r;
		img->m_pkPixels[i].g = quake_palette[ data[i] ].g;
		img->m_pkPixels[i].b = quake_palette[ data[i] ].b;
		img->m_pkPixels[i].a = 0;
		}

	delete [] data;
}

int P_Mdl::ReadSkins(void)
{
	int skintype;
	int skinsize;
	int num_of_muliskins;

	skinsize = head.skinwidth * head.skinheight;

	Image kSkinImage;
	Image kSkinImage2;

	for(int i=0; i<head.numskins; i++) {
		kSkinImage.Free();
		md1fp.Read(&skintype, 4,1);

		if(skintype == 0) {
			//if(!img->pixels)
			//	mdl_loadskin(mdl->mdlfp->fp, mdl->head.skinwidth, mdl->head.skinheight,img);
			//else	
				mdl_loadskin(md1fp.pkFp, head.skinwidth, head.skinheight,&kSkinImage);
				kSkinImage2 = kSkinImage;

				kSkinImage.Save("ost.tga",false);
				kSkinImage2.DownSample();
				kSkinImage2.Save("ost2.tga",false);
				new_skinwidth = kSkinImage2.m_iWidth;
				new_skinheight = kSkinImage2.m_iHeight;
				Skins.push_back(kSkinImage);
				kSkinImage.Free();
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

void P_Mdl::ReadTriangles(void)
{
	// Alloc triangles.
	tris = new dtriangle_s [head.numtris];
	md1fp.Read(tris, sizeof(dtriangle_s),head.numtris);
}

bool P_Mdl::ReadFrames(void)
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


void P_Mdl::LoadModel(string strFileName)
{
	m_strFileName = strFileName;

	// Register Path to Quake Pak's.

	// Load Quake Palette.
	PAKFileFp	kQuakePal;
	kQuakePal.Open("gfx/palette.lmp");
	if(kQuakePal.pkFp == NULL)	{
		cout << "Failed to load palette" << endl;
		return;
		}
	kQuakePal.Read(quake_palette,1,768);

	md1fp.Open(strFileName.c_str());
	if(md1fp.pkFp == NULL)	return;

	// Check that its a quake series modell file.
	int q_ver = GetQuakeModellVersion(&md1fp, strFileName.c_str());

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


vector<PropertyValues> P_Mdl::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	kReturn[0].kValueName = "filename";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_strFileName;
		
	return kReturn;
}

bool P_Mdl::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(strcmp(kValueName.c_str(), "filename") == 0) 
	{
		LoadModel(kValue.c_str());
		return true;
	}
	
	return false;
}

Property* Create_P_Mdl()
{
	return new P_Mdl;
}

void Register_P_Mdl(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Mdl", Create_P_Mdl);					
}
