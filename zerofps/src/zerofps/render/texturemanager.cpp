#include "texturemanager.h"
#include "../ogl/zfpsgl.h"
#include "../basic/zfvfs.h"

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
/*	bool MipMapping=true;
	bool Compression=false;
	bool Clamp=false;*/
	GLint iInternalFormat=GL_RGB;
	GLint iFormat=GL_BGR;
	
/*	if(iOption!=0) {	
		if((iOption & T_NOMIPMAPPING)){
//			cout<<"NOMIPMAPING"<<endl;
			MipMapping=false;
		}
		if((iOption & T_COMPRESSION)) {
//			cout<<"COMPRESSION"<<endl;
			Compression=true;
		}
		if((iOption & T_CLAMP)) {
			Clamp=true;
		}	
	}
*/
	
	//is this a tga?
	bool isTga=false;
  if(strncmp(&acFilename[strlen(acFilename)-4],".tga",4)==0) {
		iInternalFormat=GL_RGBA;
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
		gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,image->w,image->h,iFormat,GL_UNSIGNED_BYTE,image->pixels);  		
	}else{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
		glTexImage2D(GL_TEXTURE_2D,0,iInternalFormat,image->w,image->h,0,iFormat,GL_UNSIGNED_BYTE,image->pixels);
	}
  
  glBindTexture(GL_TEXTURE_2D,0);
  m_iCurrentTexture = NO_TEXTURE;
  
  	SDL_FreeSurface(image); ;

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

void TextureManager::RunCommand(int cmdid, const CmdArgument* kCommand)
{ 
	switch(cmdid) {
		case FID_LISTTEXTURES:	ListTextures();	break;
		case FID_FORCERELOAD:	ReloadAll();	break;
		

	};
}
