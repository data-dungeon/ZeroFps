#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include "../basic/basic.pkg"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "render_x.h"
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

/*
	Handles all textures in the game. Texture can be refered to by name
	or id (not same thing as opengl handle).
*/
#define	NO_TEXTURE	-1

class RENDER_API TextureManager : public ZFObject {
	private:

		
		int m_iCurrentTexture;
		vector<texture*> m_iTextures;
		FileIo* m_pkFile;			
		SDL_Surface *LoadImage(char *filename);	
		bool LoadTexture(GLuint &iNr,char* acFilename,int iOption);	
		void RunCommand(int cmdid, const CmdArgument* kCommand) { }

	public:
		TextureManager(FileIo* pkFile);
		int Load(char* acFileName,int iOption);		
		void BindTexture(int iTexture);
		void BindTexture(char* acFileName,int iOption);
		void ClearAll();

		int GetIndex(char* szFileName);
};



#endif




