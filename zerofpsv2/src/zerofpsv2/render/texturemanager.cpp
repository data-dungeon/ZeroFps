#include "texturemanager.h"
#include "../ogl/zfpsgl.h"
#include "../basic/zfvfs.h"
#include "../basic/zfsystem.h"
#include "../basic/image.h"
#include "render.h"
#include "../basic/basicconsole.h"
#include "zshadersystem.h"


TextureManager::TextureManager()
 : ZFSubSystem("TextureManager") 
{
	m_iCurrentTexture		= NO_TEXTURE;
	m_iEditLastTextureID = NO_TEXTURE;

	m_bSupportS3TC 		= false;
	m_bSupportARBTC 		= false;
	//m_bUseTC					= true;
	
	Register_Cmd("t_list",		FID_LISTTEXTURES);
	Register_Cmd("t_reload",	FID_FORCERELOAD);
	Register_Cmd("t_testload",	FID_TESTLOADER);
	Register_Cmd("t_unload",	FID_UNLOAD);
	
	
	//RegisterVariable("r_usetc",	&m_bUseTC,	CSYS_BOOL);	
	m_kbUseTC.Register(this, "r_usetc","1");
}	

bool TextureManager::StartUp()	
{
	m_pkZFFileSystem	=	static_cast<ZSSVFileSystem*>(GetSystem().GetObjectPtr("ZSSVFileSystem"));		
	m_pkZShaderSystem =  static_cast<ZShaderSystem*>(GetSystem().GetObjectPtr("ZShaderSystem"));		
	
 	m_bSupportARBTC = m_pkZShaderSystem->HaveExtension("ARB_texture_compression");
 	m_bSupportS3TC = m_pkZShaderSystem->HaveExtension("EXT_texture_compression_s3tc");
	
	if(!m_bSupportARBTC)
		cout<<"TextureManager: No ARB_texture_compression support"<<endl;

	if(!m_bSupportS3TC)
		cout<<"TextureManager: No EXT_texture_compression_s3tc support"<<endl;
		 	
	return true;
}	

bool TextureManager::ShutDown()
{
	if(m_iTextures.size()) {
		g_Logf("zerofps", "TextureManager::ShutDown: There are loaded textures left\n");
		
		for(unsigned int i=0; i < m_iTextures.size(); i++) {
			if(m_iTextures[i])
				g_Logf("zerofps", " [%d] = %s\n", i, m_iTextures[i]->file.c_str());
			}
		}
	
	return true;
}

void TextureManager::SetOptions(texture *pkTex, int iOption)
{
	pkTex->m_bClampToBorder	= false;
	pkTex->m_bClamp			= false;
	pkTex->m_bNoCompression	= true;
	pkTex->m_bMipMapping		= true;
	pkTex->m_bNoFilter		= false;
	pkTex->m_bAlphaOnly		= false;

	if(iOption!=0) 
	{	
		if((iOption & T_NOMIPMAPPING)){			
			pkTex->m_bMipMapping		=	false;
		}
		if((iOption & T_NOCOMPRESSION)) {
			pkTex->m_bNoCompression	=	false;
		}
		if((iOption & T_CLAMP)) {
			pkTex->m_bClamp			=	true;
		}	
		if((iOption & T_ALPHA)) {
			pkTex->m_bAlphaOnly		=	true;
		}	
		if((iOption & T_NOFILTER)) {
			pkTex->m_bNoFilter		=	true;
		}			
		if((iOption & T_CLAMPTOBORDER)) {
			pkTex->m_bClampToBorder	=	true;
		}			
	}
}

string GetTextureFlags(string strName)
{
	string strFlags;
	
	for(int i=strName.size();i>=0;i--)
	{
		if(strName[i] == '#')
		{
			for(int j=i-1;j>=0;j--)
			{
				strFlags.push_back(strName[j]);			
			}
			
			break;
		}
	}
	
	return strFlags;
}

int TextureManager::GetOptionsFromFileName(string strName)
{
	int iOptions = 0;
	string StrFlags = GetTextureFlags(strName);
	
	
	if(StrFlags.size() == 0)
		return 0;

	for(unsigned int i=0; i<StrFlags.size(); i++) 
	{
		switch(StrFlags[i]) 
		{
			case 'p':
				iOptions = iOptions | T_NOCOMPRESSION;
				break;			
			case 'c':
				iOptions = iOptions | T_CLAMP;
				break;
			case 'b':
				iOptions = iOptions | T_CLAMPTOBORDER;
				break;			
			case 'n':
				iOptions = iOptions | T_NOMIPMAPPING;			//undrar om detta funkar?
				break;				
			case 'a':
				iOptions = iOptions | T_ALPHA; 
				break;
			case 'f':
				iOptions = iOptions | T_NOFILTER; 
				break;
				
		}
	}

	return iOptions;
}

// Strip away any flags at the end of the filename
string TextureManager::StripFlags(string strName)
{
	string strNoFlags;

	for(int i=0;i<strName.size();i++)
	{
		strNoFlags.push_back(strName[i]);
	
		if(strName[i] == '#')
		{
			strNoFlags = "";
		}			
	}
	
	return strNoFlags;
}


texture*	TextureManager::GetFreeTexture()
{
	int iTexID;
	texture* pkTex;

	if(m_iFreeID.size() > 0) {
		iTexID = m_iFreeID[m_iFreeID.size() - 1];
		m_iFreeID.pop_back();
		pkTex = new texture;
		m_iTextures[ iTexID ] = pkTex;
		pkTex->TexID = iTexID;
		}
	else {
		pkTex = new texture;									// LEAK - MistServer, Nothing loaded.		
		m_iTextures.push_back(pkTex);
		pkTex->TexID = m_iTextures.size() - 1;
		}

	return pkTex;
}

void TextureManager::FreeTexture(texture* pkTex)
{
	glDeleteTextures(1, &pkTex->index);
	m_iTextures[pkTex->TexID] = NULL;
	m_iFreeID.push_back(pkTex->TexID);
	delete pkTex;
}

/**	\brief	Load a texture.

	Load texture and upload it to opengl.
*/
bool TextureManager::LoadTexture(texture *pkTex,const char *acFilename) 
{	
	GLint iInternalFormat	=	GL_RGB;
	
	//make sure the m_pkImage is null for swaping;
	pkTex->m_pkImage2 = NULL;	
	
	// Load Image
	Image* pkImage;
	pkImage = LoadImage(acFilename);
	if(!pkImage)
		return false;

	// Calc Size of texture.
	pkTex->m_iSizeInBytes = pkImage->m_iWidth * pkImage->m_iHeight * 4;

	
	//check for error size
	float fCW = pkImage->m_iWidth / 2.0;
	float fCH = pkImage->m_iHeight / 2.0;
	if( (fCW != int(fCW)) ||  (fCH != int(fCH)) )
	{
		cout<<"WARNING: texture "<<acFilename<<" is not power of 2"<<endl;
		delete pkImage;
		return false;
	}	
	
	
	glGenTextures(1,&pkTex->index);
	glBindTexture(GL_TEXTURE_2D,pkTex->index);

	// Set All Options.

	//is this a tga?
	if(strncmp(&acFilename[strlen(acFilename)-4],".tga",4)==0)
	{
		//cout << "Setting Tga format" << endl;
// 		iInternalFormat=GL_RGBA8;
 		iInternalFormat=GL_RGBA;	
	}

	//is this a alphaonly textre
	if(pkTex->m_bAlphaOnly) 
	{
		//cout << "Setting alpha on " <<acFilename << endl;
		iInternalFormat=GL_ALPHA;
	}
	
// 	//texture compression
//  	if(m_bSupportS3TC)
//  	{
//  		switch(iInternalFormat)
//  		{
//  			case GL_RGB: iInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break;
//  			case GL_RGBA: iInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
//  			case GL_ALPHA: iInternalFormat = GL_COMPRESSED_ALPHA_ARB; break;	
//  		}
//  	}	
//  	else 	
	if(m_bSupportARBTC && !pkTex->m_bNoCompression)
	{		
		switch(iInternalFormat)
		{
			case GL_RGB: iInternalFormat = GL_COMPRESSED_RGB_ARB; break;
			case GL_RGBA: iInternalFormat = GL_COMPRESSED_RGBA_ARB; break;
			case GL_ALPHA: iInternalFormat = GL_COMPRESSED_ALPHA_ARB; break;	
		}
	}

				
	//setup clamping
	if(pkTex->m_bClampToBorder)
	{					
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);	
	} 	
	else if(pkTex->m_bClamp)
	{				
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);	
	} 
	else 
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);		
	}

	//setup filters
	if(pkTex->m_bNoFilter)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	
		
		if(pkTex->m_bMipMapping)
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
		else
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);  
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
		
		if(pkTex->m_bMipMapping)
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
	}
	
	
	
	
	//create texture
	if(pkTex->m_bMipMapping)
		gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,pkImage->m_iWidth,pkImage->m_iHeight,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);  			
	else
		glTexImage2D(GL_TEXTURE_2D,0,iInternalFormat,pkImage->m_iWidth,pkImage->m_iHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);		
	
/*	if(pkTex->m_bMipMapping)
	{
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,pkImage->m_iWidth,pkImage->m_iHeight,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);  		
	}else{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
		glTexImage2D(GL_TEXTURE_2D,0,iInternalFormat,pkImage->m_iWidth,pkImage->m_iHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);
	}
*/
	glBindTexture(GL_TEXTURE_2D,0);
	m_iCurrentTexture = NO_TEXTURE;
	delete pkImage;

	return true;
}

/**	\brief	Loads a texture from file and returns it as a Image.
	
	This is the low level loading function that handles the loading.
*/
Image* TextureManager::LoadImage(const string& strFileName)
{
	string strFile;

	if(strFileName.find("data/textures/") == -1)
	{
		strFile ="data/textures/" + strFileName;
	}
	else
	{
		strFile = strFileName;
// 		cerr<<"Warning: old resource path:"<<strFileName<<endl;
	}


	Image* kImage = new Image;
	if(!kImage)
		return NULL;
	
	ZFVFile kFile;
	if(! kFile.Open(strFile,0, false)) 
	{
		delete kImage;
		fprintf(stderr, "Unable to Open %s: \n", strFile.c_str());
		return(NULL);
	}

	if(! kImage->load(kFile.m_pkFilePointer, strFile.c_str())) 
	{
		delete kImage;
		kFile.Close();
		fprintf(stderr, "Unable to Read %s: \n", strFile.c_str());
		return(NULL);
	}

	kFile.Close();
	return kImage;
}



bool TextureManager::UnLoad(const char* acFileName)
{
	int iTexture;
	iTexture = GetIndex(acFileName);
	UnLoad( iTexture );
	return true;
}

bool TextureManager::UnLoad(int iTextureID)
{
	if(iTextureID == NO_TEXTURE)
		return false;
	if(m_iTextures[iTextureID] == NULL)
		return true;

	FreeTexture(m_iTextures[iTextureID]);
	return true;
}

/**	\brief	Load a texture.

	Use to load textures. Gets a TextureManger texture object, sets parameters for it and then
	use LoadTexture to load it. If loads fails then error texture will be loaded.
*/
int TextureManager::Load(const char* szFileName, int iOption)
{
	int iTexture;
	
	// Check if texture is already loaded.
	iTexture = GetIndex(szFileName);
	if(iTexture != NO_TEXTURE)
		return iTexture;

//	cout<<"loading texture:"<<szFileName<<endl;
		
//	g_ZFObjSys.Logf("resdb", "Load Texture: %s \n", szFileName);

	texture *pkTex = GetFreeTexture();
	pkTex->file	=	szFileName;

	iOption |= GetOptionsFromFileName(szFileName);
	SetOptions(pkTex, iOption);

	// If texture can't be loaded, Load error texture.
	if(!LoadTexture(pkTex,StripFlags(szFileName).c_str()))
	{
		// If error texture fails to load cry a little and return NO_TEXTURE.
		cout << "Failed to find texture '" << pkTex->file << "'" << endl;

		if(!LoadTexture(pkTex, ERROR_TEXTURE)) 
		{
			cout<<"Error Loading texture: "<< pkTex->file <<endl;
			return NO_TEXTURE;
		}

		pkTex->file = NO_TEXTURE;
	}
	
	return pkTex->TexID;
}  

int TextureManager::CreateTextureFromRGB(const char* acName, color_rgb* pkPixels, 
										 int iWidth, int iHeight, PixelFormat eFormat,
										 bool bOverWrite, PixelFormat iInternalFormat)
{
	texture *pkTex;

	int iTexture;
	
	// Check if texture is already loaded.
	iTexture = GetIndex(acName);

	// If NOT Exist
	if(iTexture == NO_TEXTURE)
	{
		pkTex = GetFreeTexture();
		pkTex->file	= acName;
	}
	else
	{
		if(bOverWrite == false)
			return iTexture;
		else
		{
			// Else find the texture
			for(unsigned int i=0; i<m_iTextures.size(); i++)
				if(m_iTextures[i]->file == acName) 
				{
					pkTex = m_iTextures[i];
					break;
				}

			glDeleteTextures(1,&pkTex->index);
		}
	}

	//GLint iInternalFormat =	GL_RGBA;
	
	// make sure the m_pkImage is null for swaping;
	pkTex->m_pkImage2 = NULL;
	
	// Calc Size of texture.
	pkTex->m_iSizeInBytes = iWidth * iHeight * 3;

	glGenTextures(1,&pkTex->index);
	glBindTexture(GL_TEXTURE_2D,pkTex->index);

	//printf("Creating new texture ID: %i\n", pkTex->index);

	// Set All Options.
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);		

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
	glTexImage2D(GL_TEXTURE_2D,0,iInternalFormat,iWidth,iHeight,0,eFormat,GL_UNSIGNED_BYTE,pkPixels);

	glBindTexture(GL_TEXTURE_2D,0);
	m_iCurrentTexture = NO_TEXTURE;

	return pkTex->TexID;
}

// Bind Textures
void TextureManager::BindTexture(int iTexture) 
{
	if(m_iTextures[iTexture] == NULL)
		return;		


	//evil multitexturing bugfix, texturemanager does not understan multitexturing =(
	m_iCurrentTexture = NO_TEXTURE;

	if(iTexture != m_iCurrentTexture)
	{
		m_iCurrentTexture = iTexture;
		glBindTexture(GL_TEXTURE_2D,m_iTextures[iTexture]->index);
	//	cout<<"setting texture "<<iTexture<<"  "<<m_iTextures[iTexture]->index<<endl;
	}
}

bool TextureManager::ValidIndex(int iTextureID)
{
	if(iTextureID == NO_TEXTURE)
		return false;
	if(iTextureID < 0)
		return false;
	if((unsigned int)iTextureID > m_iTextures.size())  
		return false;
	if(m_iTextures[iTextureID] == NULL)
		return false;

	return true;
}

void TextureManager::BindTexture(const char* acFileName,int iOption) 
{
	BindTexture( Load(acFileName,iOption) );
}

void TextureManager::ClearAll()
{
	for(unsigned int i=0;i<m_iTextures.size();i++)
	{
		glDeleteTextures(1,&m_iTextures[i]->index);
		delete m_iTextures[i];
	}

	m_iTextures.clear();
	m_iFreeID.clear();
}

// Get Information about textures.
int TextureManager::GetIndex(const char* szFileName)
{
	for(unsigned int i=0; i<m_iTextures.size(); i++)
	{
		if(m_iTextures[i] == NULL)		continue;
		
		if(m_iTextures[i]->file == szFileName) 
			return i;		
	}

	return NO_TEXTURE;
}

const char* TextureManager::GetFileName(unsigned int uiIndex)
{
    for(unsigned int i=0; i<m_iTextures.size(); i++)
	 {
		if(m_iTextures[i] == NULL)		continue;

		//if(m_iTextures[i]->index == uiIndex+1)		// m�ste l�gga till 1!
		if(m_iTextures[i]->index == uiIndex)		// m�ste l�gga till 1!
			return m_iTextures[i]->file.c_str();        
	}

	return NULL;
}

int TextureManager::GetSizeOfTexture(int iTexture)
{
	if(!ValidIndex(iTexture))
		return 0;
	return m_iTextures[iTexture]->m_iSizeInBytes;
}

bool TextureManager::TextureHaveAlpha(int iTexture)
{
	if(!ValidIndex(iTexture))
		return false;

	int iASize;

	BindTexture(iTexture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_ALPHA_SIZE,&iASize);
	
	return (iASize > 0);
}

int TextureManager::GetTextureID (int iTexture)
{
	if(!ValidIndex(iTexture))
		return 0;
	//m_iCurrentTexture = NO_TEXTURE;
	return m_iTextures[iTexture]->index;
}

void TextureManager::ListTextures(void)
{
	BasicConsole* pkConsole = static_cast<BasicConsole*>(GetSystem().GetObjectPtr("ZSSConsole"));

	pkConsole->Printf("Texture Dump");
	for(unsigned int i=0; i<m_iTextures.size(); i++){
		if(m_iTextures[i] == NULL)
			pkConsole->Printf("[%d]: %s", i, "NULL");
		else
			pkConsole->Printf("[%d]: %s", i, m_iTextures[i]->file.c_str());
	}
}

void TextureManager::ReloadAll(void)
{
	BasicConsole* pkConsole = static_cast<BasicConsole*>(GetSystem().GetObjectPtr("ZSSConsole"));
	texture *pkTex;

	pkConsole->Printf("Texture Force Reload");
	for(unsigned int i=0; i<m_iTextures.size(); i++){
		if(m_iTextures[i] == NULL)
			continue;

		pkTex = m_iTextures[i];
		
		// Free Old Texture Object.
		glDeleteTextures(1,&pkTex->index);

		// If texture can't be loaded, Load error texture.
		//SetOptions(temp, iOption);
		if(!LoadTexture(pkTex,pkTex->file.c_str())) {
			// If error texture fails to load cry a little and return NO_TEXTURE.
			cout << "Failed to find texture '" << pkTex->file.c_str() << "'" << endl;

			if(!LoadTexture(pkTex,ERROR_TEXTURE)) {
				cout<<"Error Loading texture: "<< ERROR_TEXTURE <<endl;
			}
		}

	}
	
}

bool TextureManager::AddMipMapLevel(int iLevel,const char* acNewFile)
{
	//check if level already is loaded then return false
	if(m_iTextures[m_iCurrentTexture]->m_abLevels.test(iLevel))
		return false;
		
	if(!m_iTextures[m_iCurrentTexture]->m_bMipMapping)
		if(iLevel != 0)
			return false;
		
	GLint iInternalFormat=GL_RGB;
	GLint iFormat=GL_BGR;

	Image* image;
	image = LoadImage(acNewFile);
	if(!image) {    
   	return false;
	};
	
	
	//enable mipmaping on curent texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);  
	
	//load texture to opengl from sdl surface *image
	glTexImage2D(GL_TEXTURE_2D,iLevel,iInternalFormat,image->m_iWidth,image->m_iHeight,0,iFormat,GL_UNSIGNED_BYTE,image->m_pkPixels);

	delete image;
	cout<<"Added "<<acNewFile<<" as mipmap level "<<iLevel<<" for curent texture"<<endl;

	//set the level bit
	m_iTextures[m_iCurrentTexture]->m_abLevels.flip(iLevel);
	
	return true;
}

// Edit Textures
Image* TextureManager::GetTexture(int iLevel)
{
	glGetError();
	
	Image*	pkImage;
	int		iHeight;
	int		iWidth;
	int		iInternalFormat;
	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_WIDTH,				&iWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_HEIGHT,				&iHeight);	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_INTERNAL_FORMAT,	&iInternalFormat);		

	pkImage = new Image;
	pkImage->CreateEmpty(iWidth,iHeight);
	glGetTexImage(GL_TEXTURE_2D,iLevel,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);
	return pkImage;
}


bool TextureManager::PutTexture(Image* pkImage,bool bMipMaping)
{
	glGetError();

	int iInternalFormat;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,GL_TEXTURE_INTERNAL_FORMAT,&iInternalFormat);		

	// Upload into same format.
	if(bMipMaping)
		gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,pkImage->m_iWidth,pkImage->m_iHeight,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);  					
	else
		glTexImage2D(GL_TEXTURE_2D,0,iInternalFormat,pkImage->m_iWidth,pkImage->m_iHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);  					

	return true;
}

bool TextureManager::EditStart(int iTextureID)
{
	if(!ValidIndex(iTextureID))
		return false;

	// If texture already in edit mode return.
	if(m_iTextures[iTextureID]->m_pkImage2 != NULL)
		return true;

	BindTexture(iTextureID);
	m_iTextures[iTextureID]->m_pkImage2 = GetTexture(0);

	if(m_iTextures[iTextureID]->m_pkImage2 == NULL)
		return false;
	
	m_iEditLastTextureID = iTextureID;
	return true;
}

bool TextureManager::EditStart(string strName)
{
	return EditStart(GetIndex(strName.c_str()));
}

bool TextureManager::EditEnd(int iTextureID)
{
	if(!ValidIndex(iTextureID))
		return false;

	if(m_iTextures[iTextureID]->m_pkImage2 == NULL)
		return false;

	delete m_iTextures[iTextureID]->m_pkImage2;
	m_iTextures[iTextureID]->m_pkImage2 = NULL;
	return true;
}

bool TextureManager::EditEnd(string strName)
{
	return EditEnd(GetIndex(strName.c_str()));
}

bool TextureManager::EditCommit(string strName)
{
	int iTextureID = GetIndex(strName.c_str());
	if(!ValidIndex(iTextureID))
		return false;
	if(m_iTextures[iTextureID]->m_pkImage2 == NULL)
		return false;

	BindTexture(iTextureID);
	return PutTexture(m_iTextures[iTextureID]->m_pkImage2, m_iTextures[iTextureID]->m_bMipMapping);
}

Image* TextureManager::EditGetImage(int iTextureID)
{
	if(!ValidIndex(iTextureID))
		return NULL;
	return m_iTextures[iTextureID]->m_pkImage2;
}

Image* TextureManager::EditGetImage(string strName)
{
	return EditGetImage(GetIndex(strName.c_str()));
}





bool TextureManager::SaveTexture(const char* acFile,int iLevel)
{
	Image temp;
	
	int iHeight;
	int iWidth;
	int iDepth;
	int iInternalFormat;
	int iRSize;
	int iGSize;	
	int iBSize;	
	int iASize;	
	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_WIDTH,&iWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_HEIGHT,&iHeight);	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_INTERNAL_FORMAT,&iInternalFormat);			
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_RED_SIZE,&iRSize);		
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_GREEN_SIZE,&iGSize);		
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_BLUE_SIZE,&iBSize);		
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel,GL_TEXTURE_ALPHA_SIZE,&iASize);		
	
	iDepth = iRSize+iGSize+iBSize+iASize;
	
	
	cout << " iInternalFormat "<< iInternalFormat << "\n";
	
	cout<<"width: "<<iWidth<<endl;
	cout<<"Height:"<<iHeight<<endl;
	cout<<"Depth: "<<iDepth<<endl;		
	
	cout<<"red size:  "<<iRSize<<endl;
	cout<<"green size:"<<iGSize<<endl;	
	cout<<"blue size: "<<iBSize<<endl;	
	cout<<"alpha size:"<<iASize<<endl;	
	

	int iFormat=-1;
	int iType=-1;
	
		
//	if(iInternalFormat == GL_RGBA || iInternalFormat == GL_RGBA4 || iInternalFormat == GL_RGBA8)
	if(iInternalFormat == GL_RGBA || iInternalFormat == GL_RGBA4 || iInternalFormat == GL_RGBA8 || iInternalFormat == GL_RGB || iInternalFormat == GL_RGB5)	
	{
		iFormat = GL_RGBA;
		iType = GL_UNSIGNED_INT_8_8_8_8_REV;
	}
	
	if(iFormat == -1)
	{
		cout<<"Cant download texture Unsupored format"<<endl;
		return false;
	}
	

	temp.CreateEmpty(iWidth,iHeight);

	if(iFormat == GL_RGBA)	
	{
		glGetTexImage(GL_TEXTURE_2D,iLevel,iFormat,iType,temp.m_pkPixels);
		cout << "Arghhhhhhhhhhhhh:" << GetOpenGLErrorName(glGetError()) << "\n";		
		temp.Save(acFile,true);
	}



	return true;
}

void TextureManager::RunCommand(int cmdid, const ConCommandLine* kCommand)
{ 
	switch(cmdid) {
		case FID_LISTTEXTURES:	ListTextures();					break;
		case FID_FORCERELOAD:	ReloadAll();						break;
		case FID_TESTLOADER:		Debug_TestTexturesLoader();	break;
		
		case FID_UNLOAD:			
			UnLoad(atoi(kCommand->m_kSplitCommand[1].c_str()));	
			break;
		

	};
}

/*
	Test Image class by loading diffrent types of images and then saving them again.
*/
void TextureManager::Debug_TestTexturesLoader(void)
{
	Image kTest1;
	kTest1.load("../data/textures/test/test.tga");
	kTest1.Save("test.tga", 0);
}

string TextureManager::GetTextureNameFromOpenGlIndex(int iGlObject)
{
	string strName("Unknown");

	for(unsigned int i=0; i<m_iTextures.size(); i++){
		if(m_iTextures[i] == NULL)
			continue;
		
		if((int)m_iTextures[i]->index == iGlObject) { // 040701 : if(m_iTextures[i]->index == iGlObject) {
			return m_iTextures[i]->file;		
		}		
	}	

	return strName;
}


// ***************** OLD REMOVE *****************************************************************



/*SDL_Surface *TextureManager::LoadImage(const char *acFilename) 
{
	VIM
	fprintf(stderr, "TextureManager::LoadImage(%s)\n", m_pkFile->File(acFilename));

	  bool bIsTga = false;

	if(strstr(acFilename,".tga"))
		bIsTga = true;
	
	SDL_Surface *image;

	fprintf(stderr, "SDL_GetError(%s)\n",SDL_GetError());

	ZFVFile ZFFile;
	if(! ZFFile.Open(m_pkFile->File(acFilename), 0, false)) {
		cout << "Failed to open file " << endl;
		return NULL;
		}
	else
		cout << "ZFFile open OK " << endl;

  fprintf(stderr, "SDL_GetError(%s)\n",SDL_GetError());
	
	SDL_RWops* pkRWOps = SDL_RWFromFP(ZFFile.m_pkFilePointer, 0);
	if(pkRWOps == NULL) {
      fprintf(stderr, "Unable to create RWop\n");
		ZFFile.Close();
		return NULL;
		}
	else
		cout << "RWop created OK " << endl;

  fprintf(stderr, "SDL_GetError(%s)\n",SDL_GetError());

  if(bIsTga)
		image = IMG_LoadTGA_RW(pkRWOps);
	else
		image = IMG_Load_RW(pkRWOps, 0);
   SDL_FreeRW(pkRWOps);

  fprintf(stderr, "SDL_GetError(%s)\n",SDL_GetError());

  if ( image == NULL ) {
		fprintf(stderr, "Unable to load %s\n", m_pkFile->File(acFilename));
		ZFFile.Close();
      return(NULL);
    }
	else
		fprintf(stderr, "Image loaded %s\n", m_pkFile->File(acFilename));
			

	ZFFile.Close();
*/
	// VIM
/*	SDL_Surface *image;
	image = IMG_Load(acFilename);
	if ( image == NULL ) {
		fprintf(stderr, "Unable to load %s: %s\n", acFilename, SDL_GetError());
		return(NULL);
	}

   return(image);

   return NULL;
};*/

/*
void TextureManager::PutPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	 int bpp = surface->format->BytesPerPixel;
    // Here p is the address to the pixel we want to set
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;


    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

bool TextureManager::Blit(SDL_Surface* pkImage,int x,int y)
{
	if(!MakeTextureEditable()){
		return false;
	}
	
	SDL_Rect temp;
	temp.x = x;
	temp.y = y;
	temp.w = pkImage->w;
	temp.h = pkImage->h;
	
//	SDL_SetAlpha(m_iTextures[m_iCurrentTexture]->m_pkImage,0, 128);
	SDL_SetAlpha(pkImage,SDL_RLEACCEL|SDL_SRCALPHA, 255);	
	
	if(SDL_BlitSurface(pkImage,NULL, m_iTextures[m_iCurrentTexture]->m_pkImage, &temp) != 0)
	{		
		cout<<"error while blitting to texture"<<endl;
		return false;
	}


	return true;
	

	return false;
}
*/
/*
bool TextureManager::SwapTexture()
{
	if(m_iTextures[m_iCurrentTexture]->m_pkImage2 == NULL)
		return false;
		
	bool works = PutTexture(m_iTextures[m_iCurrentTexture]->m_pkImage2,
		m_iTextures[m_iCurrentTexture]->m_bMipMapping);
	
	if(works)
	{
		//SDL_FreeSurface(m_iTextures[m_iCurrentTexture]->m_pkImage);
		delete m_iTextures[m_iCurrentTexture]->m_pkImage2;
		m_iTextures[m_iCurrentTexture]->m_pkImage2 = NULL;
		return true;
	}
	else
	{
		cout<<"Error while swaping texture, not changed?"<<endl;
		return false;
	}
}*/

/*
bool TextureManager::MakeTextureEditable()
{
	if(m_iTextures[m_iCurrentTexture]->m_pkImage2 != NULL)
		return true;

	m_iTextures[m_iCurrentTexture]->m_pkImage2 = GetTexture(0);
	
	if(m_iTextures[m_iCurrentTexture]->m_pkImage2 == NULL)
		return false;

	return true;
}*/

/*
bool TextureManager::PsetRGB(int x,int y,int r,int g,int b)
{
	if(x < 0 || x >m_iTextures[m_iEditLastTextureID]->m_pkImage2->m_iWidth ||
		y < 0 || y >m_iTextures[m_iEditLastTextureID]->m_pkImage2->m_iHeight)
		return false;

	m_iTextures[m_iEditLastTextureID]->m_pkImage2->set_pixel(x,y,r,g,b);
	return true;
}

bool TextureManager::PsetRGBA(int x,int y,int r,int g,int b,int a)
{
	if(x < 0 || x >m_iTextures[m_iEditLastTextureID]->m_pkImage2->m_iWidth ||
		y < 0 || y >m_iTextures[m_iEditLastTextureID]->m_pkImage2->m_iHeight)
		return false;
	
	m_iTextures[m_iEditLastTextureID]->m_pkImage2->set_pixel(x,y,r,g,b,a);
	return true;
}

color_rgba TextureManager::GetPixel(int x,int y)
{
	color_rgba kColor;
	kColor.r = kColor.g = kColor.b = kColor.a = 0;

/*	if(!MakeTextureEditable()){
		return kColor;
	}

	m_iTextures[m_iEditLastTextureID]->m_pkImage2->get_pixel(x,y, kColor);
	return kColor;
}*/





bool TextureManager::TextureIsTGA(int iID)
{
	if(iID == -1)
		return false;

	map<int,bool>::iterator find = m_kIsTGAMap.find(iID);
	if(find == m_kIsTGAMap.end())
	{
		char* find;
		const char* filename = GetFileName(iID);

		if(filename == NULL)
			return true;

		find = strstr(filename, ".tga");
		m_kIsTGAMap[iID] = (find == NULL) ? false : true;

		if(find == NULL)
			return false;
		else
			return true;
	}

	return find->second;
}












