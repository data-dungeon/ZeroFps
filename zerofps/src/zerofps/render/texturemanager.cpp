#include "texturemanager.h"
#include "../ogl/zfpsgl.h"
#include "../basic/zfvfs.h"

#include "render.h"

#define ERROR_TEXTURE	"../data/textures/notex.bmp"


TextureManager::TextureManager(FileIo* pkFile)
 : ZFObject("TextureManager") {

	m_pkFile=pkFile;
	m_pkZFFileSystem	=	static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));		

	m_iCurrentTexture = NO_TEXTURE;

	g_ZFObjSys.Register_Cmd("t_list",FID_LISTTEXTURES,this);
	g_ZFObjSys.Register_Cmd("t_reload",FID_FORCERELOAD,this);

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

bool TextureManager::LoadTexture(texture *pkTex,const char *acFilename) {	
	GLint iInternalFormat=GL_RGB;
	GLint iFormat=GL_BGR;
	GLint iType=GL_UNSIGNED_BYTE;
	
	//make sure the m_pkImage is null for swaping;
	pkTex->m_pkImage = NULL;	
	
	//is this a tga?
	bool isTga=false;
	if(strncmp(&acFilename[strlen(acFilename)-4],".tga",4)==0) {
		iInternalFormat=GL_RGBA4;
		iFormat=GL_BGRA;
	}
	
	
	SDL_Surface *image;

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

  return true;
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

int TextureManager::Load(const char* acFileName,int iOption)
{
	int iTexture;
	iTexture = GetIndex(acFileName);
	if(iTexture != NO_TEXTURE)
		return iTexture;
	cout << "Load Texture: "<<  acFileName << endl;
	
	//else load it
	texture *temp = new texture;
	temp->file=acFileName;
	
	// If texture can't be loaded, Load error texture.
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
	m_iTextures.push_back(temp);
	
//	cout<<"Loaded texture: "<<m_iTextures.back()->file<<" index:"<<m_iTextures.back()->index<<endl;
	
	//return our new texture index
//	return m_iTextures.back()->index;
	return m_iTextures.size() - 1;
}

void TextureManager::BindTexture(int iTexture) {
	m_iCurrentTexture = NO_TEXTURE;

	if(iTexture != m_iCurrentTexture){
		m_iCurrentTexture=iTexture;
		glBindTexture(GL_TEXTURE_2D,m_iTextures[iTexture]->index);
	}
}

void TextureManager::BindTexture(const char* acFileName,int iOption) {
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
}

int TextureManager::GetIndex(const char* szFileName)
{
/*	
	for(list<texture*>::iterator it=m_iTextures.begin();it!=m_iTextures.end();it++)
	{
		if((*it)->file == szFileName) {
			return i;		
	}
*/	

	for(unsigned int i=0; i<m_iTextures.size(); i++){
		if(m_iTextures[i]->file == szFileName) {
			return i;		
		}		
	}


	return NO_TEXTURE;
}

const char* TextureManager::GetFileName(unsigned int uiIndex)
{
    for(unsigned int i=0; i<m_iTextures.size(); i++){
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
		pkConsole->Printf("[%d]: %s", i, m_iTextures[i]->file.c_str());
	}
}

void TextureManager::ReloadAll(void)
{
	BasicConsole* pkConsole = static_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("Console"));
	texture *pkTex;

	pkConsole->Printf("Texture Force Reload");
	for(unsigned int i=0; i<m_iTextures.size(); i++){
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
	
	glGetError();

	//download pixels from opengl
	glGetTexImage(GL_TEXTURE_2D,iLevel,iFormat,iType,image->pixels);
	

	return image;
}


bool TextureManager::PutTexture(SDL_Surface* pkImage)
{
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
	
	
	//load texture to opengl from sdl surface *image
	if(iFormat == GL_RGB)
		gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,pkImage->w,pkImage->h,iFormat,GL_UNSIGNED_SHORT_5_6_5,pkImage->pixels);  		
			
	if(iFormat == GL_RGBA)
		gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,pkImage->w,pkImage->h,iFormat,GL_UNSIGNED_SHORT_4_4_4_4,pkImage->pixels);  		

	return true;
}

bool TextureManager::SwapTexture()
{
	if(m_iTextures[m_iCurrentTexture]->m_pkImage == NULL)
		return false;
		
	bool works = PutTexture(m_iTextures[m_iCurrentTexture]->m_pkImage);	
	
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

	Uint32 color = SDL_MapRGB(m_iTextures[m_iCurrentTexture]->m_pkImage->format, r, g, b);
	
	PutPixel(m_iTextures[m_iCurrentTexture]->m_pkImage,x,y,color);

	return true;
}

bool TextureManager::PsetRGBA(int x,int y,int r,int g,int b,int a)
{
	if(!MakeTextureEditable()){
		return false;
	}
	
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

SDL_Surface* TextureManager::GetImage()
{
	return m_iTextures[m_iCurrentTexture]->m_pkImage;
}

void TextureManager::RunCommand(int cmdid, const CmdArgument* kCommand)
{ 
	switch(cmdid) {
		case FID_LISTTEXTURES:	ListTextures();	break;
		case FID_FORCERELOAD:	ReloadAll();	break;
		

	};
}


