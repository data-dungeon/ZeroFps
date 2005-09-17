#ifndef _RES_TEXTURE_H_
#define _RES_TEXTURE_H_

#include "../basic/zfresource.h"
#include "zshadersystem.h"
//#include "../basic/zfsystem.h"
#include "render_x.h"

#define TEXTURE_NOT_FOUND	"notexture.tga"


/// texture options
enum TEXTURE_OPTIONS 
{
	T_NOMIPMAPPING	=	1,		///< No mipmapping on this texture.
	T_NOCOMPRESSION=	8,		///< Use texture compression on this texture.
	T_CLAMP			=	16,	///< Use clamp on this texture (s and t).
	T_ALPHA			=	32,	///< This is a alpha only texture.
	T_NOFILTER		=	64,	///< use nearest mfilter .
	T_CLAMPTOBORDER=	128,	///< Clamp to border color
	T_DEPTH			=	256,	///< depth texture
	T_RGB				=	1024,	///< RGB texture
	T_RGBA			=	2048,	///< RGBA texture
};

/// A Texture in the Resource SubSystem.
class RENDER_API ResTexture : public ZFResource
{
	private:
		ZShaderSystem*	m_pkZShaderSystem;
		
		GLuint			m_iOpenGLID;					///< actual opengl texture ID
		string			m_strTextureName;				///< name of texture, usualy the filename	
		Image*			m_pkImage;						///< for realtime editing of surface	
		int				m_iSizeInBytes;				///< Size of texture in bytes.	
		int				m_iOptions;						///< texture options		
	
		Vector4			m_kBorderColor;				///< border color used with clamptoborder,
	
		
		bool		SetupZShader();													///< sets up pointer to zshadersystem
		Image*	LoadImage(const string& strFileName);						///< load image from file
		string	GetTextureFlags(const string& strName);					///< gets textureflags from path name
		string	StripFlags(const string& strName);							///< returns a flag stripped string
		int		GetOptionsFromFileName(const string& strName);			///< get options from file name
	
		
		/** downloads current bound texture from opengl
			\param iMipMapLevel mipmap level to download			*/
		Image*	GetTexture(int iMipMapLevel);
				
		/** upload image to current bound opengl texture
			\param iMipMapLevel is the mipmap level to upload to, -1 regenerates entire mipmap hirarcy */		
		void		PutTexture(Image* pkImage,int iMipMapLevel = -1);				///< upload 
		
	public:
	
		ResTexture();
		~ResTexture();
	
		bool Create(const string& strName);
		void Clear();		
		int  CalculateSize();
								
		///return opengl texturehandle	
		GLuint GetOpenGLTextureID()	{	return m_iOpenGLID;	};
								
		///create texture from file
		bool CreateTextureFromFile(string strName,int iOptions = 0);				
		
		///create texture from an Image
		bool CreateTextureFromImage(Image* pkImage,const string& strName = "Image",int iOptions = 0);						
		
		///create a blank texture of a specified size
		bool CreateBlankTexture(int iW,int iH,int iOptions = 0);
				
		///create and empty uninitialized texture of specified size and type
		bool CreateEmptyTexture(int iW,int iH,int iOptions = 0);				
				
		///save texture to file, iMipMapLevel : mipmaplevel to save
		bool SaveTexture(const string& strFile,int iMipMapLevel = 0);	
				
				
		// Edit Textures
		bool EditBegin();								///< downloads texture from opengl to make it available for editing
		bool EditEnd();								///< removes local copy of texture without commiting the changes
		bool EditCommit();							///< commits the changes made to the texture
		Image* GetEditImage();						///< returns pointer to edit image,or NULL if no edit image is available
				
		//texture settings
		void SetBorderColor(Vector4 kColor);
				
		friend class ZShaderSystem;
};

RENDER_API ZFResource* Create__ResTexture();

#endif



