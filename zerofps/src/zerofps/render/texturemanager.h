#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include <SDL/SDL_opengl.h>
#include <SDL/SDL.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

using namespace std;

class TextureManager {
	private:
		struct texture{
//			texture(string nfile,GLuint nindex) {file=nfile;index=nindex;};
			string file;
			GLuint index;
		};
		
		int m_iCurrentTexture;
		vector<texture*> m_iTextures;
		
//		GLuint m_iTextures[20];
	
		SDL_Surface *LoadImage(char *filename);	
		bool LoadTexture(GLuint &iNr,char* acFilename);
	
	
	public:
		int Load(char* acFileName);
		
		void BindTexture(int iTexture);
		void BindTexture(char* acFileName);
};



#endif




