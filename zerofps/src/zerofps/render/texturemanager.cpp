#include "texturemanager.h"
#include "../ogl/zfpsgl.h"

#define ERROR_TEXTURE	"../data/textures/notex.bmp"


TextureManager::TextureManager(FileIo* pkFile)
 : ZFObject("TextureManager") {
	m_pkFile=pkFile;

	m_iCurrentTexture = NO_TEXTURE;

}	


bool TextureManager::LoadTexture(GLuint &iNr,char *acFilename,int iOption) {	
	bool isTga=false;
	bool MipMapping=true;
	bool Compression=false;
	GLint iInternalFormat=GL_RGB;
	GLint iFormat=GL_BGR;
	
	if(iOption!=0) {	
		if((iOption & T_NOMIPMAPPING)){
//			cout<<"NOMIPMAPING"<<endl;
			MipMapping=false;
		}
		if((iOption & T_COMPRESSION)) {
//			cout<<"COMPRESSION"<<endl;
			Compression=true;
		}
	}
	
	//is this a tga?
  if(strncmp(&acFilename[strlen(acFilename)-4],".tga",4)==0) {
		iInternalFormat=GL_RGBA;
		iFormat=GL_BGRA;
	}
	
	
	SDL_Surface *image;

  image= LoadImage(acFilename);
  if(!image) {    
    return false;
  };
  

  glGenTextures(1,&iNr);
  glBindTexture(GL_TEXTURE_2D,iNr);

	if(MipMapping){
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

  return true;
}

SDL_Surface *TextureManager::LoadImage(char *acFilename) {
  //Uint8 *rowhi, *rowlo;
  //Uint8 *tmpbuf, tmpch;
  SDL_Surface *image;
  //int i, j;


	image = IMG_Load(m_pkFile->File(acFilename));
    if ( image == NULL ) {
    //  fprintf(stderr, "Unable to load %s: %s\n", acFilename, SDL_GetError());
      return(NULL);
    }

/*
  // GL surfaces are upsidedown and RGB, not BGR :-) 
  tmpbuf = (Uint8 *)malloc(image->pitch);
  if ( tmpbuf == NULL ) {
    fprintf(stderr, "Out of memory\n");
    return(NULL);
    }
    rowhi = (Uint8 *)image->pixels;
    rowlo = rowhi + (image->h * image->pitch) - image->pitch;
    for ( i=0; i<image->h/2; ++i ) {
      for ( j=0; j<image->w; ++j ) {
        tmpch = rowhi[j*3];
        rowhi[j*3] = rowhi[j*3+2];
        rowhi[j*3+2] = tmpch;
        tmpch = rowlo[j*3];
        rowlo[j*3] = rowlo[j*3+2];
        rowlo[j*3+2] = tmpch;
        }
      memcpy(tmpbuf, rowhi, image->pitch);
      memcpy(rowhi, rowlo, image->pitch);
      memcpy(rowlo, tmpbuf, image->pitch);
      rowhi += image->pitch;
      rowlo -= image->pitch;
    }
  free(tmpbuf);
  */
  return(image);
};

int TextureManager::Load(char* acFileName,int iOption)
{
	int iTexture;
	iTexture = GetIndex(acFileName);
	if(iTexture != NO_TEXTURE)
		return iTexture;
	
	//else load it
	texture *temp = new texture;
	temp->file=acFileName;
	
	// If texture can't be loaded, Load error texture.
	if(!LoadTexture(temp->index,acFileName,iOption)) {
		// If error texture fails to load cry a little and return NO_TEXTURE.
		cout << "Failed to find texture '" << acFileName << "'" << endl;

		if(!LoadTexture(temp->index,ERROR_TEXTURE,iOption)) {
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

void TextureManager::BindTexture(char* acFileName,int iOption) {
	int iTexture=Load(acFileName,iOption);
	m_iCurrentTexture = NO_TEXTURE;
	BindTexture(iTexture);
}


void TextureManager::ClearAll()
{
	for(int i=0;i<m_iTextures.size();i++){
		glDeleteTextures(1,&m_iTextures[i]->index);
		delete m_iTextures[i];
	}

	m_iTextures.clear();
}

int TextureManager::GetIndex(char* szFileName)
{
	for(int i=0; i<m_iTextures.size(); i++){
		if(m_iTextures[i]->file == szFileName) {
			return i;		
		}		
	}

	return NO_TEXTURE;
}



