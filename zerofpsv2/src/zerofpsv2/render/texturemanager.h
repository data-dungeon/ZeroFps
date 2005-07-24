#ifndef _RENDER_TEXTUREMANAGER_H_
#define _RENDER_TEXTUREMANAGER_H_

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <SDL/SDL.h>
#include "render_x.h"
#include "../ogl/zfpsgl.h"
#include <bitset>
#include "../basic/zfvfs.h"
#include "../basic/image.h"

using namespace std;

class ZShaderSystem;

enum OPTIONS 
{
	T_NOMIPMAPPING	=	1,		// No mipmapping on this texture.
	T_COMPRESSION	=	8,		// Use texture compression on this texture.
	T_CLAMP			=	16,	// Use clamp on this texture (s and t).
	T_ALPHA			=	32,		// This is a alpha only texture.
	T_NOFILTER		=	64,		// use nearest mfilter .
};

#define ERROR_TEXTURE	"notex.bmp"
#define	NO_TEXTURE	-1		// Texuremanger index for a non existing texture.

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
	bool				m_bClamp;			// Texture Clamping.
	bool				m_bAlphaOnly;
	bool				m_bNoFilter;
	
	bitset<20>		m_abLevels;			//	level x is true if mipmap level is loaded	
	Image*			m_pkImage2;			//	for realtime editing of surface

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

		
		bool					m_bSupportARBTC;
		bool					m_bSupportS3TC;
		
		bool					m_bUseTC;
		
		int					m_iCurrentTexture;
		vector<texture*>	m_iTextures;
		vector<int>			m_iFreeID;
		
		ZFVFileSystem*		m_pkZFFileSystem;
		ZShaderSystem*		m_pkZShaderSystem;

		texture*	GetFreeTexture();												// Returns a free texture manger texture object.
      void FreeTexture(texture* pkTex);									// Free texture manger texture object.
		Image* LoadImage(const string& strFileName);	
		bool LoadTexture(texture *pkTex,const char* acFilename);	

		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void ListTextures(void);
		void ReloadAll(void);

		void SetOptions(texture *pkTex, int iOptions);
		int GetOptionsFromFileName(string strName);
		string StripFlags(string strName);

		bool	ValidIndex(int iTextureID);	// Returns true if iTextureID is a valid texture id.
	
		// Edit Textures
		int	m_iEditLastTextureID;
		Image* GetTexture(int iLevel);								
		bool PutTexture(Image* pkImage,bool bMipMaping);

	public:
		TextureManager();
		bool StartUp();
		bool ShutDown();
		bool IsValid()	{ return true;	}

		// Load/Unload textures.
		int Load(const char* szFileName,int iOption=1);		
		bool UnLoad(int iTextureID);		
		bool UnLoad(const char* acFileName);		
		void ClearAll();

		enum PixelFormat { RGB=GL_RGB, BGR=GL_BGR, RGBA=GL_RGBA };

		int CreateTextureFromRGB(const char* acName, color_rgb* pixels, 
			int width, int height, PixelFormat eFormat, bool bOverWrite, PixelFormat TargetFormat);

		// Bind Textures
		void BindTexture(const char* acFileName,int iOption);	// Bind texture by name. Load it if not loaded.
		void BindTexture(int iTexture);								// Bind texture by texturemanger index.

		// Get Information about textures.
		int GetIndex(const char* szFileName);
		const char* GetFileName(unsigned int uiIndex);
		int GetSizeOfTexture(int iTexture);							// Returns size of texture in bytes.
		bool TextureHaveAlpha(int iTexture);							// speaks for it self I think.
      int GetTextureID (int iTexture);								// Returns opengl texture id of texture.

		// Edit Textures
		bool EditStart(string strName);								//	Make it so you can edit a texture.
		bool EditStart(int iTextureID);								//	
		bool EditEnd(string strName);									//	Stop edit of texture
		bool EditEnd(int iTextureID);									//
		Image* EditGetImage(int iTextureID);						// Get ptr to Image of texture.
		Image* EditGetImage(string strName);	
		bool EditCommit(string strName);								// Commit changes of texture.
		
		//	bool SwapTexture();
		//bool PsetRGB(int x,int y,int r,int g,int b);
		//bool PsetRGBA(int x,int y,int r,int g,int b,int a);
		//color_rgba GetPixel(int x,int y);

		// Save Textures
		bool SaveTexture(const char* acFile,int iLevel);		

		void Debug_TestTexturesLoader(void);
		bool AddMipMapLevel(int iLevel,const char* acNewFile);
		int CurentTexture() { return m_iCurrentTexture;};
		string GetTextureNameFromOpenGlIndex(int iGlObject);	// Returns name of texture from opengl texture object ID:

	
		friend class ZFResourceDB;

		//bool Blit(SDL_Surface* pkImage,int x,int y);
		//void PutPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
//		SDL_Surface *LoadImage(const char *filename);	

		bool TextureIsTGA(int iID);
		map<int,bool> m_kIsTGAMap;

};



#endif




