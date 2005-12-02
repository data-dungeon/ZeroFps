#include "res_texture.h"

GLuint ResTexture::m_iCopyFBO = 0;
GLuint ResTexture::m_iCopyFBOcolor = 0;

ResTexture::ResTexture()
{
	m_pkZShaderSystem		=	NULL;

	m_iOpenGLID 			=	0;
// 	m_pkImage 				=	NULL;		
	m_strTextureName 		=	"";
	m_iSizeInBytes 		=	0;			
	m_iOptions				=	0;
	
	m_iWidth					=	0;
	m_iHeight				=	0;
}

ResTexture::ResTexture(const string& strName)
{
	ResTexture();
	
	CreateTextureFromFile(strName);
}

ResTexture::~ResTexture()
{
	Clear();
}


bool ResTexture::Create(const string& strName)
{
	return CreateTextureFromFile(strName);
}

bool ResTexture::SetupZShader()
{
	if(m_pkZShaderSystem)
		return true;

	if(!m_pkZShaderSystem)
		m_pkZShaderSystem = (ZShaderSystem*)(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
		
	if(!m_pkZShaderSystem)
		cerr<<"WARNING: ResTexture coudl not get zshadersystem pointer"<<endl;
		
	return m_pkZShaderSystem;
}

string ResTexture::StripFlags(const string& strName)
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


string ResTexture::GetTextureFlags(const string& strName)
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

int ResTexture::GetOptionsFromFileName(const string& strName)
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
				iOptions = iOptions | T_NOMIPMAPPING;
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

bool ResTexture::CreateBlankTexture(int iW,int iH,int iOptions)
{
	Clear();

	Image* pkImage = new Image;
	pkImage->CreateEmpty(iW,iH);

	//create texture from the empty texture
	bool bSuccess = CreateTextureFromImage(pkImage,"blank",iOptions);
	
	//delete the empty texture after creation
	delete pkImage;
	
	return bSuccess;
}

bool ResTexture::CreateEmptyTexture(int iW,int iH,int iOptions)
{
	if(!SetupZShader())
		return false;

	Clear();

	m_strTextureName = "empty";
	m_iSizeInBytes = iW * iH * 4;
	m_iOptions = iOptions;
	m_iWidth = iW;
	m_iHeight = iH;
	
	//generate new texture
	glGenTextures(1, &m_iOpenGLID);	
	m_pkZShaderSystem->BindTexture(this);	
	
	
	int iFormat = GL_RGBA;	
	if(iOptions & T_DEPTH) 	iFormat = GL_DEPTH_COMPONENT;
	if(iOptions & T_RGB) 	iFormat = GL_RGB;
	if(iOptions & T_RGBA) 	iFormat = GL_RGBA;
	if(iOptions & T_ALPHA) 	iFormat = GL_ALPHA;

	
	//use texture compression
	if(m_pkZShaderSystem->SupportARBTC() && !(m_iOptions & T_NOCOMPRESSION) )
	{		
		switch(iFormat)
		{
			case GL_RGB: iFormat 	= GL_COMPRESSED_RGB_ARB; break;
			case GL_RGBA: iFormat 	= GL_COMPRESSED_RGBA_ARB; break;
			case GL_ALPHA: iFormat 	= GL_COMPRESSED_ALPHA_ARB; break;	
		}
	}	
					
					
	//setup clamping
	if(m_iOptions & T_CLAMPTOBORDER)
	{					
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);	
	} 	
	else if(m_iOptions & T_CLAMP)
	{				
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);	
	} 
	else 
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);		
	}


	//setup filters
	if(m_iOptions & T_NOFILTER)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);  
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
	}		
	
	//create texture
	glTexImage2D( GL_TEXTURE_2D, 0, iFormat, iW, iH, 0, iFormat, GL_UNSIGNED_BYTE, NULL);
	
	return true;
}

bool ResTexture::CreateTextureFromImage(Image* pkImage,const string& strName,int iOptions)
{
	if(!SetupZShader())
		return false;

	//clear texture
	Clear();
	
	//set options
	m_iOptions = iOptions;
	
	//set name
	m_strTextureName = strName;
	
	// Calc Size of texture.
	m_iSizeInBytes = pkImage->m_iWidth * pkImage->m_iHeight * 4;	
	
	//save resulution	
	m_iWidth = pkImage->m_iWidth;
	m_iHeight = pkImage->m_iHeight;	
	
	//check for error size
	float fCW = pkImage->m_iWidth / 2.0;
	float fCH = pkImage->m_iHeight / 2.0;
	if( (fCW != int(fCW)) ||  (fCH != int(fCH)) )
	{
		cerr<<"WARNING: image is not power of 2"<<endl;
		return false;
	}	
	
	//generate a texture
	glGenTextures(1,&m_iOpenGLID);
	m_pkZShaderSystem->BindTexture(this);	
	
	
	//internal format
	int iInternalFormat = GL_RGB;
	
	if(pkImage->m_bHasAlpha)
		iInternalFormat=GL_RGBA;	
		
	//alpha only texture
	if(m_iOptions & T_ALPHA) 
		iInternalFormat=GL_ALPHA;
			
	//use texture compression
	if(m_pkZShaderSystem->SupportARBTC() && !(m_iOptions & T_NOCOMPRESSION) )
	{		
		switch(iInternalFormat)
		{
			case GL_RGB: iInternalFormat = GL_COMPRESSED_RGB_ARB; break;
			case GL_RGBA: iInternalFormat = GL_COMPRESSED_RGBA_ARB; break;
			case GL_ALPHA: iInternalFormat = GL_COMPRESSED_ALPHA_ARB; break;	
		}
	}	
					
	//setup clamping
	if(m_iOptions & T_CLAMPTOBORDER)
	{					
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);	
	} 	
	else if(m_iOptions & T_CLAMP)
	{				
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);	
	} 
	else 
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);		
	}


	//setup filters
	if(m_iOptions & T_NOFILTER)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	
		
		if(m_iOptions & T_NOMIPMAPPING)
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);  
		else			
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
		
		if(m_iOptions & T_NOMIPMAPPING)
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  
		else			
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	}	

	//create texture
	if(m_iOptions & T_NOMIPMAPPING)
		glTexImage2D(GL_TEXTURE_2D,0,iInternalFormat,pkImage->m_iWidth,pkImage->m_iHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);		
	else		
		gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,pkImage->m_iWidth,pkImage->m_iHeight,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);  			
	
	
	m_pkZShaderSystem->BindTexture(NULL);

	return true;
}

bool ResTexture::CreateTextureFromFile(string strName, int iOptions)
{
	if(!SetupZShader())
		return false;
		
	//extract options flags from filename	
	iOptions |= GetOptionsFromFileName(strName);
			
	//load texture from disk
	Image* pkImage = LoadImage(StripFlags(strName));	
	
	//cold not load texture, try to load bakuptexture
	if(!pkImage)
	{
		cerr<<"WARNING: texture "<<strName<<" not found"<<endl;
		pkImage = LoadImage(TEXTURE_NOT_FOUND);
		if(!pkImage) return false;
	}
		
	//create a texture from the loaded image
	bool bSuccess = CreateTextureFromImage(pkImage,m_strTextureName,iOptions);	
	
	//finaly delete the image
	delete pkImage;
	
	return bSuccess;
		
}

void ResTexture::Clear()
{
	if(m_iOpenGLID != 0) glDeleteTextures(1, &m_iOpenGLID);
	m_iOpenGLID				=	0;
				
// 	if(m_pkImage) delete m_pkImage;
// 	m_pkImage 				=	NULL;
		
	m_strTextureName 		=	"";
	m_iSizeInBytes 		=	0;		
	m_iOptions				=	0;
	m_iWidth					=	0;			
	m_iHeight				=	0;
}


int ResTexture::CalculateSize()
{
	return m_iSizeInBytes;
}


/**	\brief	Loads a texture from file and returns it as a Image.
	
	This is the low level loading function that handles the loading.
*/
Image* ResTexture::LoadImage(const string& strFileName)
{
	string strFile;

	if(strFileName.find("data/textures/") == -1)
	{
		strFile ="data/textures/" + strFileName;
	}
	else
	{
		strFile = strFileName;
		cerr<<"Warning: old resource path:"<<strFileName<<endl;
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

bool ResTexture::RegenerateMipmaps()
{
	if(!SetupZShader())
		return false;	
	
	if(m_iOpenGLID == 0)
		return false;
	
	m_pkZShaderSystem->PushTexture();
	m_pkZShaderSystem->BindTexture(this);

	glGenerateMipmapEXT(GL_TEXTURE_2D);

	m_pkZShaderSystem->PopTexture();
	
	return true;
}

void ResTexture::PutTexture(Image* pkImage,int iMipMapLevel)
{			
	int iInternalFormat;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,GL_TEXTURE_INTERNAL_FORMAT,&iInternalFormat);		

	// Upload into same format.
	if(iMipMapLevel == -1)
		gluBuild2DMipmaps(GL_TEXTURE_2D,iInternalFormat,pkImage->m_iWidth,pkImage->m_iHeight,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);  					
	else
		glTexImage2D(GL_TEXTURE_2D,iMipMapLevel,iInternalFormat,pkImage->m_iWidth,pkImage->m_iHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);  							


	return;
}

bool ResTexture::PutTextureImage(Image* pkImage,int iMipMapLevel )
{
	if(!SetupZShader())
		return false;	
		
	if(m_iOpenGLID == 0)
		return false;

	m_pkZShaderSystem->PushTexture();		
	m_pkZShaderSystem->BindTexture(this);	

	PutTexture(pkImage,iMipMapLevel);
	
	m_pkZShaderSystem->PopTexture();	
	
	return true;
}

Image* ResTexture::GetTextureImage(int iMipMapLevel)
{
	if(!SetupZShader())
		return NULL;	
		
	if(m_iOpenGLID == 0)
		return NULL;

	m_pkZShaderSystem->PushTexture();		
	m_pkZShaderSystem->BindTexture(this);	

	Image* pkImage = GetTexture(iMipMapLevel);
	
	m_pkZShaderSystem->PopTexture();		
	
	return pkImage;
}

Image* ResTexture::GetTexture(int iMipMapLevel)
{
	Image*	pkImage;
	int		iHeight;
	int		iWidth;
	int		iInternalFormat;
	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iMipMapLevel,GL_TEXTURE_WIDTH,				&iWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iMipMapLevel,GL_TEXTURE_HEIGHT,				&iHeight);	
	glGetTexLevelParameteriv(GL_TEXTURE_2D, iMipMapLevel,GL_TEXTURE_INTERNAL_FORMAT,	&iInternalFormat);		

	pkImage = new Image;
	pkImage->CreateEmpty(iWidth,iHeight);
	glGetTexImage(GL_TEXTURE_2D,iMipMapLevel,GL_RGBA,GL_UNSIGNED_BYTE,pkImage->m_pkPixels);	
	
	return pkImage;
}


void ResTexture::SetBorderColor(Vector4 kColor)
{
	if(!SetupZShader())
		return;	
		
	if(m_iOpenGLID == 0)
		return;
		
		
	m_pkZShaderSystem->PushTexture();		
	m_pkZShaderSystem->BindTexture(this);
	
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &kColor[0]);

	m_pkZShaderSystem->PopTexture();		
}

bool ResTexture::SaveTexture(const string& strFile,int iMipMapLevel)
{
	if(!SetupZShader())
		return false;	
		
	if(m_iOpenGLID == 0)
		return false;
		
	m_pkZShaderSystem->PushTexture();
	m_pkZShaderSystem->BindTexture(this);

	//get image from opengl
	Image* pkImage = GetTexture(iMipMapLevel);

	m_pkZShaderSystem->PopTexture();		

	//save image to file
	bool bSuccess = pkImage->Save(strFile.c_str());
	
	//delete image after it's saved
	delete pkImage;
	
	return bSuccess;
}

bool ResTexture::CopyFrameBuffer(int iX,int iY,int iW,int iH)
{
	if(!SetupZShader())
		return false;		

	if(m_iOpenGLID == 0)
		return false;


	m_pkZShaderSystem->PushTexture();		
	m_pkZShaderSystem->BindTexture(this);
		
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iX, iY,iW, iH);		
				
		
	m_pkZShaderSystem->PopTexture();		

	return true;
}

bool ResTexture::CopyTexture(ResTexture* pkSource)
{
	if(!SetupZShader())
		return false;		

	if(!m_pkZShaderSystem->SupportFBO())
	{
		cerr<<"WARNING: ResTexture::CopyTexture not possible without FBO support"<<endl;
		return false;
	}
	
	if(m_iOpenGLID == 0 || pkSource->m_iOpenGLID == 0)
		return false;
	
	
/*	if(m_iCopyFBO == 0)
	{
		cout<<"setting up copy fbo"<<endl;
	
		glGetError();
		glGenFramebuffersEXT(1, &m_iCopyFBO);
		if(glGetError() != GL_NO_ERROR) cout<<"ERROR generating FBO"<<endl;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iCopyFBO);			
		if(glGetError() != GL_NO_ERROR) cout<<"ERROR binding FBO"<<endl;
	
		glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT,
											GL_COLOR_ATTACHMENT0_EXT,
											GL_TEXTURE_2D,m_iOpenGLID, 0);			
		
		if(glGetError() != GL_NO_ERROR) cout<<"ERROR glFramebufferTexture2DEXT"<<endl;
	
	}
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iCopyFBO);		*/	


	static Matrix4	kOrtho(	1,0,0,0,
									0,1,0,0,
									0,0,-1,0,
									0,0,-1,1);

	static float data[]= {	-1,-1,-1,
									 1,-1,-1,
							 		 1, 1,-1,
									-1, 1,-1};
	
	static float uvdata[]= {0,0,
									1,0,
							 		1,1,
									0,1};	

// 	//calculate how to stretch the texture									
// 	float xs = 	float(m_pkRender->GetWidth()) / float(m_iFSSTextureWidth);								
// 	float ys = 	float(m_pkRender->GetHeight()) / float(m_iFSSTextureHeight);
// 
//  	uvdata[2] = xs;
//  	uvdata[4] = xs;
//  	uvdata[5] = ys;
//  	uvdata[7] = ys;

	//orhto projection
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixLoad(&kOrtho);

	//identity modelmatrix
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixIdentity();
// 
// 	m_pkZShaderSystem->PushTexture();	

	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,data);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,uvdata);
	m_pkZShaderSystem->SetNrOfVertexs(4);
			
  	//we dont wan to write any depth data
  	m_pkZShaderSystem->SetDepthMask(false);
  	
	m_pkZShaderSystem->Push("copytexture");	
	m_pkZShaderSystem->BindTexture(pkSource);
	
	m_pkZShaderSystem->DrawArray(QUADS_MODE);  

	m_pkZShaderSystem->BindTexture(this);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,512, 512);		

	m_pkZShaderSystem->Pop();

  	//pop matrices
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);	
	m_pkZShaderSystem->MatrixPop();	
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);	
	m_pkZShaderSystem->MatrixPop();	

	
// 	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);			
	
	cout<<"COPY COMPLETE"<<endl;
	
}

ZFResource* Create__ResTexture()
{
	return new ResTexture;
}


