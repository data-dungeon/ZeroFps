#include "TextureManager.h"




bool TextureManager::LoadTexture(GLuint &iNr,char *acFilename) {
        SDL_Surface *image;

  image= LoadImage(acFilename);
  if(!image) {    
    return false;
  };

  glGenTextures(1,&iNr);
  glBindTexture(GL_TEXTURE_2D,iNr);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

//  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,image->w,image->h,0,GL_RGB,GL_UNSIGNED_BYTE,image->pixels);
//  gluBuild2DMipmaps
  gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,image->w,image->h,GL_RGB,GL_UNSIGNED_BYTE,image->pixels);
  glBindTexture(GL_TEXTURE_2D,0);
  return true;
}

SDL_Surface *TextureManager::LoadImage(char *acFilename) {
  Uint8 *rowhi, *rowlo;
  Uint8 *tmpbuf, tmpch;
  SDL_Surface *image;
  int i, j;

  image = SDL_LoadBMP(acFilename);
    if ( image == NULL ) {
    //  fprintf(stderr, "Unable to load %s: %s\n", acFilename, SDL_GetError());
      return(NULL);
    }

  /* GL surfaces are upsidedown and RGB, not BGR :-) */
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
  return(image);
};

int TextureManager::Load(char* acFileName) {
	
	//first check if the texture has already been loaded,and return its index
	for(int i=0;i<m_iTextures.size();i++){
		if(m_iTextures[i]->file==acFileName){
			//return the texure index
			return (int)m_iTextures[i]->index;		
		}		
	}
	
	//else load it
	texture *temp=new texture;
	temp->file=acFileName;
	
	//if the texture cant be loaded then complain and return 0 
	if(!LoadTexture(temp->index,acFileName)){
		cout<<"Error Loading texture: "<<temp->file<<endl;
		return 0;
	}
	//add the texture to the loaded textures vector
	m_iTextures.push_back(temp);
	
	cout<<"Loaded texture: "<<m_iTextures.back()->file<<" index:"<<m_iTextures.back()->index<<endl;
	
	//return our new texture index
	return m_iTextures.back()->index;
}

void TextureManager::BindTexture(int iTexture) {
	if(iTexture!=m_iCurrentTexture){
		m_iCurrentTexture=iTexture;
		glBindTexture(GL_TEXTURE_2D,iTexture);
	}
}

void TextureManager::BindTexture(char* acFileName) {
	int iTexture=Load(acFileName);
	if(iTexture!=m_iCurrentTexture){
		m_iCurrentTexture=iTexture;
		glBindTexture(GL_TEXTURE_2D,iTexture);
	}
}



