#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "render_x.h"
#include "../basic/basic.pkg"
#include "../ogl/zfpsgl.h"

using namespace std;
enum OPTIONS {
	T_NOMIPMAPPING=1,
	T_COMPRESSION=8


};

struct texture{
	string file;
	GLuint index;
};

class RENDER_API TextureManager {
	private:

		
		int m_iCurrentTexture;
		vector<texture*> m_iTextures;
		FileIo* m_pkFile;			
		SDL_Surface *LoadImage(char *filename);	
		bool LoadTexture(GLuint &iNr,char* acFilename,int iOption);	
	
	public:
		TextureManager(FileIo* pkFile);
		int Load(char* acFileName,int iOption);		
		void BindTexture(int iTexture);
		void BindTexture(char* acFileName,int iOption);
};



#endif




