#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include <SDL/SDL_opengl.h>
#include <SDL/SDL.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include "../basic/basic.pkg"

using namespace std;

class TextureManager {
	private:
		struct texture{
			string file;
			GLuint index;
		};
		
		int m_iCurrentTexture;
		vector<texture*> m_iTextures;
		FileIo* m_pkFile;			
		SDL_Surface *LoadImage(char *filename);	
		bool LoadTexture(GLuint &iNr,char* acFilename);	
	
	public:
		TextureManager(FileIo* pkFile);
		int Load(char* acFileName);		
		void BindTexture(int iTexture);
		void BindTexture(char* acFileName);
};



#endif




