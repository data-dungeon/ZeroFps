#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include "render_x.h"
#include "../oglload/zfpsgl.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include "../basic/basic.pkg"

using namespace std;
enum OPTIONS {
	T_NOMIPMAPPING=1,
	T_COMPRESSION=8


};


class RENDER_API TextureManager {
	private:
		struct texture{
			string file;
			GLuint index;
		};
		
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




