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
#include <bitset>


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
	string			file;					// Path/Filename
	GLuint			index;				// OpenGL Index.
	int				TexID;				// Texture ID.

	bool				m_bMipMapping;		// True if we would like to have mipmapping.
	bool				m_bCompression;
	bool				b_bClamp;			// Texture Clamping.

	bitset<20>		m_abLevels;			//	level x is true if mipmap level is loaded	
	SDL_Surface*	m_pkImage;			//	for realtime editing of surface
};

/// Handles all textures in the game. Texture can be refered to by name
///	or id (not same thing as opengl handle).
class RENDER_API TextureManager : public ZFObject {
	private:
		enum FuncId_e
		{
			FID_LISTTEXTURES,										// List all Textures to the console.
			FID_FORCERELOAD,										// Reload all textures.
			FID_TESTLOADER,
			FID_UNLOAD,
		};

//		int					m_iCurrentTexture;				// TexMgr ID of Active Texture.
//		vector<texture*>	m_iTextures;				
//		vector<int>			m_iFreeIndex;

		int					m_iCurrentTexture;
		vector<texture*>	m_iTextures;
		vector<int>			m_iFreeID;
		texture*	GetFreeTexture();

		
		FileIo* m_pkFile;			
		SDL_Surface *LoadImage(const char *filename);	
		Image* LoadImage2(const char *filename);	
		bool LoadTexture(texture *pkTex,const char* acFilename);	
		void RunCommand(int cmdid, const CmdArgument* kCommand);

		void ListTextures(void);
		void ReloadAll(void);
		void SetOptions(texture *pkTex, int iOptions);

		ZFVFileSystem*	m_pkZFFileSystem;

		void PutPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);

		int GetOptionsFromFileName(string strName);

	public:
		void FreeTexture(texture* pkTex);

		TextureManager(FileIo* pkFile);
		int Load(const char* acFileName,int iOption);		
		bool UnLoad(const char* acFileName);		
		bool UnLoad(int iTextureID);		
		bool AddMipMapLevel(int iLevel,const char* acNewFile);
		void BindTexture(int iTexture);
		void BindTexture(const char* acFileName,int iOption);
		void ClearAll();
		int CurentTexture() { return m_iCurrentTexture;};

		int GetIndex(const char* szFileName);
		const char* GetFileName(unsigned int uiIndex);

		SDL_Surface* GetTexture(int iLevel);
		bool PutTexture(SDL_Surface* pkImage,bool bMipMaping);
		bool SwapTexture();
		bool MakeTextureEditable();
		bool PsetRGB(int x,int y,int r,int g,int b);
		bool PsetRGBA(int x,int y,int r,int g,int b,int a);
		Uint32 GetPixel(int x,int y);
		bool Blit(SDL_Surface* pkImage,int x,int y);
		SDL_Surface* GetImage();
		bool SaveTexture(const char* acFile,int iLevel);
		

		bool StartUp()	{ return true;	}
		bool ShutDown()	{ return true;	}
		bool IsValid()	{ return true;	}

		void Debug_TestTexturesLoader(void);
};



#endif




