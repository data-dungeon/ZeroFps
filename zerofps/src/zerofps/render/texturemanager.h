#ifndef _RENDER_TEXTUREMANAGER_H_
#define _RENDER_TEXTUREMANAGER_H_

#include "../basic/basic.pkg"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <list>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "render_x.h"
#include "../ogl/zfpsgl.h"

using namespace std;

enum OPTIONS {
	T_NOMIPMAPPING=1,
	T_COMPRESSION=8,
	T_CLAMP=16
};

#define	NO_TEXTURE	-1

/// Texture file name and all paramerers.
struct texture
{
	string file;			// Path/Filename
	GLuint index;			// OpenGL Index.

	bool m_bMipMapping;		// True if we would like to have mipmapping.
	bool m_bCompression;
	bool b_bClamp;			// Texture Clamping.
};

/// Handles all textures in the game. Texture can be refered to by name
///	or id (not same thing as opengl handle).
class RENDER_API TextureManager : public ZFObject {
	private:
		enum FuncId_e
		{
			FID_LISTTEXTURES,
			FID_FORCERELOAD,
		};

		int m_iCurrentTexture;
		vector<texture*> m_iTextures;
//		list<texture*> m_iTextures;
		
		FileIo* m_pkFile;			
		SDL_Surface *LoadImage(const char *filename);	
		bool LoadTexture(texture *pkTex,const char* acFilename);	
		void RunCommand(int cmdid, const CmdArgument* kCommand);

		void ListTextures(void);
		void ReloadAll(void);
		void SetOptions(texture *pkTex, int iOptions);

		ZFVFileSystem*	m_pkZFFileSystem;

	public:
		TextureManager(FileIo* pkFile);
		int Load(const char* acFileName,int iOption);		
		void BindTexture(int iTexture);
		void BindTexture(const char* acFileName,int iOption);
		void ClearAll();

		int GetIndex(const char* szFileName);
		const char* GetFileName(unsigned int uiIndex);

		bool StartUp()	{ return true;	}
		bool ShutDown()	{ return true;	}
		bool IsValid()	{ return true;	}
};



#endif




