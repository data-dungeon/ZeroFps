#ifndef _RENDER_TEXTUREMANAGER_H_
#define _RENDER_TEXTUREMANAGER_H_

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <list>
#include <string>
#include <SDL/SDL.h>
//#include <SDL/SDL_image.h>
#include "render_x.h"
#include "../ogl/zfpsgl.h"
#include <bitset>
#include "../basic/zfvfs.h"
#include "../basic/image.h"

using namespace std;

enum OPTIONS 
{
	T_NOMIPMAPPING	=	1,
	T_COMPRESSION	=	8,
	T_CLAMP			=	16,
	T_ALPHA			=	32
};

#define	NO_TEXTURE	-1

/**	\brief	Texture file name and all paramerers.
		\ingroup Render
*/
struct texture
{
	string			file;					// Path/Filename
	GLuint			index;				// OpenGL Index.
	int				TexID;				// Texture ID.

	bool				m_bMipMapping;		// True if we would like to have mipmapping.
	bool				m_bCompression;
	bool				b_bClamp;			// Texture Clamping.
	bool				m_bAlphaOnly;

	bitset<20>		m_abLevels;			//	level x is true if mipmap level is loaded	
	//SDL_Surface*	m_pkImage;			//	for realtime editing of surface
	Image*			m_pkImage2;

	int				m_iSizeInBytes;	// Size of texture in bytes.
};

/**	\brief	The SubSystem that handles all textures in ZeroFPS.
		\ingroup Render

Texture can be refered to by name or id (not same thing as opengl handle).
The texturemanger loads textures and put them in OpenGl so they can be used by the
Shader SubSystem.
*/
class RENDER_API TextureManager : public ZFSubSystem {
	private:

		enum FuncId_e
		{
			FID_LISTTEXTURES,										// List all Textures to the console.
			FID_FORCERELOAD,										// Reload all textures.
			FID_TESTLOADER,
			FID_UNLOAD,
		};

		int					m_iCurrentTexture;
		vector<texture*>	m_iTextures;
		vector<int>			m_iFreeID;
		texture*	GetFreeTexture();

		
		SDL_Surface *LoadImage(const char *filename);	

		bool LoadTexture(texture *pkTex,const char* acFilename);	
		void RunCommand(int cmdid, const CmdArgument* kCommand);

		void ListTextures(void);
		void ReloadAll(void);
		void SetOptions(texture *pkTex, int iOptions);
		string GetFileName(string strFileExtFlags);

		ZFVFileSystem*	m_pkZFFileSystem;

		//void PutPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);

		int GetOptionsFromFileName(string strName);

		bool UnLoad(const char* acFileName);		


	public:
		string GetTextureNameFromOpenGlIndex(int iGlObject);	// Returns name of texture from opengl texture object ID:

      void FreeTexture(texture* pkTex);

		TextureManager();
		bool AddMipMapLevel(int iLevel,const char* acNewFile);
		void BindTexture(const char* acFileName,int iOption);
		
		int GetSizeOfTexture(int iTexture);
		void BindTexture(int iTexture);
      int GetTextureID (int iTexture);

		bool UnLoad(int iTextureID);		
		
		int Load(const char* acFileName,int iOption=1);		
		
		void ClearAll();
		int CurentTexture() { return m_iCurrentTexture;};

		int GetIndex(const char* szFileName);
		const char* GetFileName(unsigned int uiIndex);

		
		Image* GetTexture(int iLevel);							//dont use, use GetImage() insted
		bool PutTexture(Image* pkImage,bool bMipMaping);
		bool SwapTexture();
		bool MakeTextureEditable();
		bool PsetRGB(int x,int y,int r,int g,int b);
		bool PsetRGBA(int x,int y,int r,int g,int b,int a);
		color_rgba GetPixel(int x,int y);
		//bool Blit(SDL_Surface* pkImage,int x,int y);
		

		Image* GetImage();		
		bool SaveTexture(const char* acFile,int iLevel);		
		Image* LoadImage2(const char *filename);	

		void Debug_TestTexturesLoader(void);

		bool StartUp();
		bool ShutDown();

		bool IsValid()	{ return true;	}


		
		friend class ZFResourceDB;
};



#endif




