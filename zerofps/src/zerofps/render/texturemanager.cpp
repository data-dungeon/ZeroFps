#include "texturemanager.h"
#include "../ogl/zfpsgl.h"
#include "../basic/zfvfs.h"

#include "../basic/image.h"
#include "render.h"

#define ERROR_TEXTURE	"../data/textures/notex.bmp"

TextureManager::TextureManager(FileIo* pkFile)
 : ZFObject("TextureManager") {

	m_pkFile=pkFile;
	m_pkZFFileSystem	=	static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));		

	m_iCurrentTexture = NO_TEXTURE;

	g_ZFObjSys.Register_Cmd("t_list",FID_LISTTEXTURES,this);
	g_ZFObjSys.Register_Cmd("t_reload",FID_FORCERELOAD,this);
	g_ZFObjSys.Register_Cmd("t_testload",FID_TESTLOADER,this);
	g_ZFObjSys.Register_Cmd("t_unload",FID_UNLOAD,this);
}	

void TextureManager::SetOptions(texture *pkTex, int iOption)
{
	pkTex->b_bClamp			= false;
	pkTex->m_bCompression	= false;
	pkTex->m_bMipMapping	= true;
	
	if(iOption!=0) {	
		if((iOption & T_NOMIPMAPPING)){
			pkTex->m_bMipMapping	=	false;
		}
		if((iOption & T_COMPRESSION)) {
			pkTex->m_bCompression	=	true;
		}
		if((iOption & T_CLAMP)) {
			pkTex->b_bClamp			=	true;
		}	
	}
}

string GetTextureFlags(string strName)
{
	string	strFlags;
	char		szFlags[256];
	strFlags = "";

	char szName[256];
	strcpy(szName, strName.c_str());
	char* szSOFlags = strrchr(szName, '/');
	if(!szSOFlags)
		return strFlags;
	
	char* szEOFlags = strchr(szSOFlags, '-');
	if(!szEOFlags)
		return strFlags;

	int iNumOfFlags = szEOFlags - szSOFlags;
	strncpy(szFlags,szSOFlags,iNumOfFlags);
	szFlags[iNumOfFlags] = 0;
   strFlags = szFlags;
	return strFlags;
}

int TextureManager::GetOptionsFromFileName(string strName)
{
	int iOptions = 0;
	string StrFlags = GetTextureFlags(strName);
	char		szFlags[256];
	strcpy(szFlags,StrFlags.c_str());
	if(strlen(szFlags) == 0)
		return 0;

	for(int i=0; i<strlen(szFlags); i++) {
		switch(szFlags[i]) {
			case 'c':
				iOptions = iOptions | T_CLAMP;
				break;
			}
		}

	return iOptions;
}


texture*	TextureManager::GetFreeTexture()
{
	int iTexID;
	texture* pkTex;

	if(m_iFreeID.size() > 0) {
		iTexID = m_iFreeID[m_iFreeID.size() - 1];
		m_iFreeID.pop_back();
		pkTex = new texture;
		m_iTextures[ iTexID ] = pkTex;
		pkTex->TexID = iTexID;
		}
	else {
		pkTex = new texture;
		m_iTextures.push_back(pkTex);
		pkTex->TexID = m_iTextures.size() - 1;
		}

	return pkTex;
}

void TextureManager::FreeTexture(texture* pkTex)
{
	glDeleteTextures(1, &pkTex->index);
	m_iTextures[pkTex->TexID] = NULL;
	m_iFreeID.push_back(pkTex->TexID);
	delete pkTex;
}


bool TextureManager::LoadTexture(texture *pkTex,const char *acFilename) 
{	
	GLint iInternalFormat	=	GL_RGB;
	GLint iFormat				=	GL_RGB;
	GLint iType					=	GL_UNSIGNED_BYTE;
	
	//make sure the m_pkImage is null for swaping;
	pkTex->m_pkImage = NULL;	
	
	//is this a tga?
	bool isTga=false;
	if(strncmp(&acFilename[strlen(acFilename)-4],".tga",4)==0) {
		iInternalFormat=GL_RGBA4;
		iFormat=GL_RGBA;
	}
	
	Image* pkImage;
	pkImage = LoadImage2(acFilename);
	if(!pkImage)
		return false;



	glGenTextures(1,&pkTex->index);
	glBindTexture(GL_TEXTURE_2D,pkTex->index);

	if(pkTex->b_bClamp){
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);	
	} else {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);		
	}

	if(pkTex->m_bMipMapping){
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,pkImage->width,pkImage->height,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->pixels);  		
	}else{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
		glTexImage2D(GL_TEXTURE_2D,0,iInternalFormat,pkImage->width,pkImage->height,0,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->pixels);
	}
  glBindTexture(GL_TEXTURE_2D,0);
  m_iCurrentTexture = NO_TEXTURE;
	delete pkImage;

/*	SDL_Surface *image;
	image= LoadImage(acFilename);
	if(!image) {    
   	return false;
	};
  

	glGenTextures(1,&pkTex->index);
	glBindTexture(GL_TEXTURE_2D,pkTex->index);

	if(pkTex->b_bClamp){
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);	
	} else {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);		
	}

	if(pkTex->m_bMipMapping){
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,image->w,image->h,iFormat,iType,image->pixels);  		
	}else{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
		glTexImage2D(GL_TEXTURE_2D,0,iInternalFormat,image->w,image->h,0,iFormat,iType,image->pixels);
	}
  
  glBindTexture(GL_TEXTURE_2D,0);
  m_iCurrentTexture = NO_TEXTURE;
  	SDL_FreeSurface(image); 
*/  

  return true;
}

Image* TextureManager::LoadImage2(const char *filename)
{
	Image* kImage = new Image;
	if(!kImage)
		return NULL;
	
	ZFVFile kFile;
	if(! kFile.Open(m_pkFile->File(filename),0, false)) {
		delete kImage;
		fprintf(stderr, "Unable to load %s: \n", filename);
		return(NULL);
		}

	if(! kImage->load(kFile.m_pkFilePointer, filename)) {
		delete kImage;
		kFile.Close();
		fprintf(stderr, "Unable to load %s: \n", filename);
		return(NULL);
		}

	kFile.Close();
	return kImage;

/*	Image* kImage = new Image;
	if(!kImage)
		return NULL;

	if(! kImage->load(m_pkFile->File(filename))) {
		delete kImage;
		fprintf(stderr, "Unable to load %s: \n", filename);
		return(NULL);
		}

	return kImage;*/
	
	// VIM
/*	SDL_Surface *image;
	image = IMG_Load(m_pkFile->File(acFilename));
	if ( image == NULL ) {
		return(NULL);
	}

   return(image);*/

}

SDL_Surface *TextureManager::LoadImage(const char *acFilename) 
{
/*	VIM
	fprintf(stderr, "TextureManager::LoadImage(%s)\n", m_pkFile->File(acFilename));

	  bool bIsTga = false;

	if(strstr(acFilename,".tga"))
		bIsTga = true;
	
	SDL_Surface *image;

	fprintf(stderr, "SDL_GetError(%s)\n",SDL_GetError());

	ZFVFile ZFFile;
	if(! ZFFile.Open(m_pkFile->File(acFilename), 0, false)) {
		cout << "Failed to open file " << endl;
		return NULL;
		}
	else
		cout << "ZFFile open OK " << endl;

  fprintf(stderr, "SDL_GetError(%s)\n",SDL_GetError());
	
	SDL_RWops* pkRWOps = SDL_RWFromFP(ZFFile.m_pkFilePointer, 0);
	if(pkRWOps == NULL) {
      fprintf(stderr, "Unable to create RWop\n");
		ZFFile.Close();
		return NULL;
		}
	else
		cout << "RWop created OK " << endl;

  fprintf(stderr, "SDL_GetError(%s)\n",SDL_GetError());

  if(bIsTga)
		image = IMG_LoadTGA_RW(pkRWOps);
	else
		image = IMG_Load_RW(pkRWOps, 0);
   SDL_FreeRW(pkRWOps);

  fprintf(stderr, "SDL_GetError(%s)\n",SDL_GetError());

  if ( image == NULL ) {
		fprintf(stderr, "Unable to load %s\n", m_pkFile->File(acFilename));
		ZFFile.Close();
      return(NULL);
    }
	else
		fprintf(stderr, "Image loaded %s\n", m_pkFile->File(acFilename));
			

	ZFFile.Close();
*/
	// VIM
	SDL_Surface *image;
	image = IMG_Load(m_pkFile->File(acFilename));
	if ( image == NULL ) {
		fprintf(stderr, "Unable to load %s: %s\n", acFilename, SDL_GetError());
		return(NULL);
	}

   return(image);
};

bool TextureManager::UnLoad(const char* acFileName)
{
	int iTexture;
	iTexture = GetIndex(acFileName);
	if(iTexture == NO_TEXTURE)
		return false;

	FreeTexture(m_iTextures[iTexture]);
	return true;
}

bool TextureManager::UnLoad(int iTextureID)
{
	if(m_iTextures[iTextureID] == NULL)
		return true;

	FreeTexture(m_iTextures[iTextureID]);
	return true;
}

int TextureManager::Load(const char* acFileName,int iOption)
{
	int iTexture;
	iTexture = GetIndex(acFileName);
	if(iTexture != NO_TEXTURE)
		return iTexture;

	cout << "Load Texture: "<<  acFileName << endl;
	
	//else load it
	texture *temp = GetFreeTexture();
	temp->file=acFileName;
	
	// If texture can't be loaded, Load error texture.
	iOption |= GetOptionsFromFileName(acFileName);
	SetOptions(temp, iOption);
	if(!LoadTexture(temp,acFileName)) {
		// If error texture fails to load cry a little and return NO_TEXTURE.
		cout << "Failed to find texture '" << acFileName << "'" << endl;

		if(!LoadTexture(temp,ERROR_TEXTURE)) {
			cout<<"Error Loading texture: "<<temp->file<<endl;
			return NO_TEXTURE;
		}
	}
	//add the texture to the loaded textures vector
//	m_iTextures.push_back(temp);
	
//	cout<<"Loaded texture: "<<m_iTextures.back()->file<<" index:"<<m_iTextures.back()->index<<endl;
	
	//return our new texture index
//	return m_iTextures.back()->index;
//	return m_iTextures.size() - 1;
	return temp->TexID;
}

void TextureManager::BindTexture(int iTexture) 
{
	if(m_iTextures[iTexture] == NULL)
		return;

	m_iCurrentTexture = NO_TEXTURE;

	if(iTexture != m_iCurrentTexture){
		m_iCurrentTexture = iTexture;
		glBindTexture(GL_TEXTURE_2D,m_iTextures[iTexture]->index);
	}
}

void TextureManager::BindTexture(const char* acFileName,int iOption) 
{
	int iTexture=Load(acFileName,iOption);
	m_iCurrentTexture = NO_TEXTURE;
	BindTexture(iTexture);
}


void TextureManager::ClearAll()
{
	for(unsigned int i=0;i<m_iTextures.size();i++){
		glDeleteTextures(1,&m_iTextures[i]->index);
		delete m_iTextures[i];
	}

	m_iTextures.clear();
	m_iFreeID.clear();
}

int TextureManager::GetIndex(const char* szFileName)
{
	for(unsigned int i=0; i<m_iTextures.size(); i++){
		if(m_iTextures[i] == NULL)
			continue;
		
		if(m_iTextures[i]->file == szFileName) {
			return i;		
		}		
	}

	return NO_TEXTURE;
}

const char* TextureManager::GetFileName(unsigned int uiIndex)
{
    for(unsigned int i=0; i<m_iTextures.size(); i++){
		if(m_iTextures[i] == NULL)
			continue;

		if(m_iTextures[i]->index == uiIndex+1) { // måste lägga till 1!
			return m_iTextures[i]->file.c_str();        
        }        
	}

    return NULL;
}

void TextureManager::ListTextures(void)
{
	BasicConsole* pkConsole = static_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("Console"));

	pkConsole->Printf("Texture Dump");
	for(unsigned int i=0; i<m_iTextures.size(); i++){
		if(m_iTextures[i] == NULL)
			pkConsole->Printf("[%d]: %s", i, "NULL");
		else
			pkConsole->Printf("[%d]: %s", i, m_iTextures[i]->file.c_str());
	}
}

void TextureManager::ReloadAll(void)
{
	BasicConsole* pkConsole = static_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("Console"));
	texture *pkTex;

	pkConsole->Printf("Texture Force Reload");
	for(unsigned int i=0; i<m_iTextures.size(); i++){
		if(m_iTextures[i] == NULL)
			continue;

		pkTex = m_iTextures[i];
		
		// Free Old Texture Object.
		glDeleteTextures(1,&pkTex->index);

		// If texture can't be loaded, Load error texture.
		//SetOptions(temp, iOption);
		if(!LoadTexture(pkTex,pkTex->file.c_str())) {
			// If error texture fails to load cry a little and return NO_TEXTURE.
			cout << "Failed to find texture '" << pkTex->file.c_str() << "'" << endl;

			if(!LoadTexture(pkTex,ERROR_TEXTURE)) {
				cout<<"Error Loading texture: "<< ERROR_TEXTURE <<endl;
			}
		}

	}
	
}

bool TextureManager::AddMipMapLevel(int iLevel,const char* acNewFile)
{
	
	//check if level already is loaded then return false
	if(m_iTextures[m_iCurrentTexture]->m_abLevels.test(iLevel))
		return false;
		
	if(!m_iTextures[m_iCurrentTexture]->m_bMipMapping)
		if(iLevel != 0)
			return false;
		
	GLint iInternalFormat=GL_RGB;
	GLint iFormat=GL_BGR;
	
	//use sdlimage to load texture
	SDL_Surface *image;
	image= LoadImage(acNewFile);	
	if(!image) {    
   	return false;
	};
	
	//enable mipmaping on curent texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);  
	
	//load texture to opengl from sdl surface *image
	glTexImage2D(GL_TEXTURE_2D,iLevel,iInternalFormat,image->w,image->h,0,iFormat,GL_UNSIGNED_BYTE,image->pixels);

	//frea sdlsurface
	SDL_FreeSurface(image); 

	cout<<"Added "<<acNewFile<<" as mipmap level "<<iLevel<<" for curent texture"<<endl;

	//set the level bit
	m_iTextures[m_iCurrentTexture]->m_abLevels.flip(iLevel);
	
	
	return true;
}

SDL_Surface* TextureManager::GetTexture(int iLevel)
{
	glGetError();
	
	SDL_Surface* image;
	int iHeight;
	int iWidth;
	int iDepth;
	int iInternalFormat;
	int iRSize;
	int iGSize;	
	int iBSize;	
	int iASize;	
	Uint32 rmask, gmask, bmask, amask;
	
	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_WIDTH,&iWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_HEIGHT,&iHeight);	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_INTERNAL_FORMAT,&iInternalFormat);		
	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_RED_SIZE,&iRSize);		
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_GREEN_SIZE,&iGSize);		
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_BLUE_SIZE,&iBSize);		
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_ALPHA_SIZE,&iASize);		
	
	iDepth = iRSize+iGSize+iBSize+iASize;
	
	/*
	cout << " iInternalFormat "<< iInternalFormat << "\n";
	cout << " iDepth "<< iDepth << "\n";	
	
	cout<<"width: "<<iWidth<<endl;
	cout<<"Height:"<<iHeight<<endl;
	cout<<"Depth: "<<iDepth<<endl;		
	
	cout<<"red size:  "<<iRSize<<endl;
	cout<<"green size:"<<iGSize<<endl;	
	cout<<"blue size: "<<iBSize<<endl;	
	cout<<"alpha size:"<<iASize<<endl;	
	*/

	int iFormat=-1;
	int iType=-1;
	
	
	if(iDepth == 16)
	{
		if(iInternalFormat == GL_RGB || iInternalFormat == GL_RGB5)
		{		
			//cout<<"GL_RGB"<<endl;
			iFormat = GL_RGB;
			iType = GL_UNSIGNED_SHORT_5_6_5;

			rmask = 0xf800;
			gmask = 0x07e0;		
			bmask = 0x001f;		
			amask = 0x0000;		
		}
		
		
		if(iInternalFormat == GL_RGBA || iInternalFormat == GL_RGBA4)
		{
			//cout<<"GL_RGBA"<<endl;
			iFormat = GL_RGBA;
			iType = GL_UNSIGNED_SHORT_4_4_4_4;//GL_UNSIGNED_SHORT_4_4_4_4;
		
			rmask = 0xf000;
			gmask = 0x0f00;
			bmask = 0x00f0;
			amask = 0x000f;

		}
	}
	
	
	if(iFormat == -1)
	{
		cout<<"Cant download texture Unsupored format"<<endl;
		return NULL;
	}
	
	//create sdl surface
	image = SDL_CreateRGBSurface(SDL_SWSURFACE, iWidth, iHeight, iDepth,rmask,gmask,bmask,amask);
	if(image == NULL)
	{
		cout<<"error creating sdlsurface"<<endl;
		return NULL;
	}
	
//	cout << "TextureManager::GetTexture: " glGetError(); << endl;
//	cout << "GetTexture:" << GetOpenGLErrorName(glGetError()) << "\n";

	//download pixels from opengl
	glGetTexImage(GL_TEXTURE_2D,iLevel,iFormat,iType,image->pixels);
//	cout << "GetTexture:" << GetOpenGLErrorName(glGetError()) << "\n";

	return image;
}


bool TextureManager::PutTexture(SDL_Surface* pkImage,bool bMipMaping)
{
	glGetError();

	int iInternalFormat;
	int iFormat=-1;
	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,GL_TEXTURE_INTERNAL_FORMAT,&iInternalFormat);		
		
		
	if(iInternalFormat == GL_RGB || iInternalFormat == GL_RGB5)
	{		
		//cout<<"GL_RGB"<<endl;
		iFormat = GL_RGB;
	}
	
	if(iInternalFormat == GL_RGBA || iInternalFormat == GL_RGBA4)
	{
		//cout<<"GL_RGBA"<<endl;	
		iFormat = GL_RGBA;		
	}

	
	if(iFormat == -1)
	{
		cout<<"cant upload texture unsupored format"<<endl;
		return false;
	}
	
	
	if(bMipMaping)
	{
		//cout<<"rebuilding mipmaps"<<endl;	
		if(iFormat == GL_RGB)
			gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,pkImage->w,pkImage->h,iFormat,GL_UNSIGNED_SHORT_5_6_5,pkImage->pixels);  					
		if(iFormat == GL_RGBA)
			gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,pkImage->w,pkImage->h,iFormat,GL_UNSIGNED_SHORT_4_4_4_4,pkImage->pixels);  		
	}
	else
	{
		//cout<<"no mipmaps"<<endl;		
		if(iFormat == GL_RGB)
			glTexImage2D(GL_TEXTURE_2D,0,iInternalFormat,pkImage->w,pkImage->h,0,iFormat,GL_UNSIGNED_SHORT_5_6_5,pkImage->pixels);  					
		if(iFormat == GL_RGBA)
			glTexImage2D(GL_TEXTURE_2D,0,iInternalFormat,pkImage->w,pkImage->h,0,iFormat,GL_UNSIGNED_SHORT_4_4_4_4,pkImage->pixels);  								
	}
	
	return true;
}

bool TextureManager::SwapTexture()
{
	if(m_iTextures[m_iCurrentTexture]->m_pkImage == NULL)
		return false;
		
	bool works = PutTexture(m_iTextures[m_iCurrentTexture]->m_pkImage,m_iTextures[m_iCurrentTexture]->m_bMipMapping);
	
	if(works)
	{
		SDL_FreeSurface(m_iTextures[m_iCurrentTexture]->m_pkImage);
		m_iTextures[m_iCurrentTexture]->m_pkImage = NULL;
		return true;
	}
	else
	{
		cout<<"Error while swaping texture, not changed?"<<endl;
		return false;
	}
	
}

bool TextureManager::MakeTextureEditable()
{
	if(m_iTextures[m_iCurrentTexture]->m_pkImage != NULL)
		return true;

	m_iTextures[m_iCurrentTexture]->m_pkImage = GetTexture(0);
	
	if(m_iTextures[m_iCurrentTexture]->m_pkImage == NULL)
		return false;

	return true;
}

bool TextureManager::PsetRGB(int x,int y,int r,int g,int b)
{
	if(!MakeTextureEditable())
		return false;

	if(x < 0 || x >m_iTextures[m_iCurrentTexture]->m_pkImage->w ||
		y < 0 || y >m_iTextures[m_iCurrentTexture]->m_pkImage->h)
		return false;
		

	Uint32 color = SDL_MapRGB(m_iTextures[m_iCurrentTexture]->m_pkImage->format, r, g, b);
	
	PutPixel(m_iTextures[m_iCurrentTexture]->m_pkImage,x,y,color);

	return true;
}

bool TextureManager::PsetRGBA(int x,int y,int r,int g,int b,int a)
{
	if(!MakeTextureEditable()){
		return false;
	}
	
	if(x < 0 || x >m_iTextures[m_iCurrentTexture]->m_pkImage->w ||
		y < 0 || y >m_iTextures[m_iCurrentTexture]->m_pkImage->h)
		return false;
	
	Uint32 color = SDL_MapRGBA(m_iTextures[m_iCurrentTexture]->m_pkImage->format, r, g, b,a);	

	PutPixel(m_iTextures[m_iCurrentTexture]->m_pkImage,x,y,color);

	return true;
}


void TextureManager::PutPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	 int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;


    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

bool TextureManager::Blit(SDL_Surface* pkImage,int x,int y)
{
	if(!MakeTextureEditable()){
		return false;
	}
	
	SDL_Rect temp;
	temp.x = x;
	temp.y = y;
	temp.w = pkImage->w;
	temp.h = pkImage->h;
	
//	SDL_SetAlpha(m_iTextures[m_iCurrentTexture]->m_pkImage,0, 128);
	SDL_SetAlpha(pkImage,SDL_RLEACCEL|SDL_SRCALPHA, 255);	
	
	if(SDL_BlitSurface(pkImage,NULL, m_iTextures[m_iCurrentTexture]->m_pkImage, &temp) != 0)
	{		
		cout<<"error while blitting to texture"<<endl;
		return false;
	}


	return true;
}


SDL_Surface* TextureManager::GetImage()
{
	if(!m_iTextures[m_iCurrentTexture])
		return NULL;
	
	if(!MakeTextureEditable())
		return NULL;

	return m_iTextures[m_iCurrentTexture]->m_pkImage;
}

Uint32 TextureManager::GetPixel(int x,int y)
{
	if(!MakeTextureEditable()){
		return 0;
	}

	SDL_Surface* surface = m_iTextures[m_iCurrentTexture]->m_pkImage;

	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
	case 1:
		return *p;

	case 2:
		return *(Uint16 *)p;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;

	case 4:
		return *(Uint32 *)p;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

bool TextureManager::SaveTexture(const char* acFile,int iLevel)
{
	Image temp;
	
	int iHeight;
	int iWidth;
	int iDepth;
	int iInternalFormat;
	int iRSize;
	int iGSize;	
	int iBSize;	
	int iASize;	
	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_WIDTH,&iWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_HEIGHT,&iHeight);	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_INTERNAL_FORMAT,&iInternalFormat);			
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_RED_SIZE,&iRSize);		
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_GREEN_SIZE,&iGSize);		
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_BLUE_SIZE,&iBSize);		
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_ALPHA_SIZE,&iASize);		
	
	iDepth = iRSize+iGSize+iBSize+iASize;
	
/*	
	cout << " iInternalFormat "<< iInternalFormat << "\n";
	
	cout<<"width: "<<iWidth<<endl;
	cout<<"Height:"<<iHeight<<endl;
	cout<<"Depth: "<<iDepth<<endl;		
	
	cout<<"red size:  "<<iRSize<<endl;
	cout<<"green size:"<<iGSize<<endl;	
	cout<<"blue size: "<<iBSize<<endl;	
	cout<<"alpha size:"<<iASize<<endl;	
*/	

	int iFormat=-1;
	int iType=-1;
	
		
//	if(iInternalFormat == GL_RGBA || iInternalFormat == GL_RGBA4 || iInternalFormat == GL_RGBA8)
	if(iInternalFormat == GL_RGBA || iInternalFormat == GL_RGBA4 || iInternalFormat == GL_RGBA8 || iInternalFormat == GL_RGB || iInternalFormat == GL_RGB5)	
	{
		iFormat = GL_RGBA;
		iType = GL_UNSIGNED_INT_8_8_8_8_REV;
	}
	
	if(iFormat == -1)
	{
		cout<<"Cant download texture Unsupored format"<<endl;
		return false;
	}
	

	temp.create_empty(iWidth,iHeight);

	if(iFormat == GL_RGBA)	
	{
		glGetTexImage(GL_TEXTURE_2D,iLevel,iFormat,iType,temp.pixels);
		//cout << "Arghhhhhhhhhhhhh:" << GetOpenGLErrorName(glGetError()) << "\n";		
		temp.save(acFile,true);
	}



	return true;
}

void TextureManager::RunCommand(int cmdid, const CmdArgument* kCommand)
{ 
	switch(cmdid) {
		case FID_LISTTEXTURES:	ListTextures();					break;
		case FID_FORCERELOAD:	ReloadAll();						break;
		case FID_TESTLOADER:		Debug_TestTexturesLoader();	break;
		
		case FID_UNLOAD:			
			UnLoad(atoi(kCommand->m_kSplitCommand[1].c_str()));	
			break;
		

	};
}

/*
	Test Image class by loading diffrent types of images and then saving them again.
*/
void TextureManager::Debug_TestTexturesLoader(void)
{
	Image kTest1;
	kTest1.load("../data/textures/test/test.tga");
	kTest1.save("test.tga", 0);
}

