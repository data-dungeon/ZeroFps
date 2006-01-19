#include "camera.h"
#include "../render/render.h"
#include "../ogl/zfpsgl.h"
#include "inputhandle.h"
#include "../basic/math.h"
#include "render_plugins.h"


bool Camera::m_bDrawOrthoGrid(false);
float	Camera::m_fGridSpace(1.0);
bool Camera::m_bGridSnap(false);

Camera::Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar)
 : I_Camera(kPos,kRot,fFov,fAspect,fNear,fFar) 
{
	m_pkRender = 			static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));
	m_pkZShaderSystem =	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkEntityMan = 		static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));
	m_pkZeroFps =			static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkZShadow = 			static_cast<ZShadow*>(g_ZFObjSys.GetObjectPtr("ZShadow"));
	m_pkLight	=			static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight"));
	m_pkTexMan	=			static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkZSSRenderEngine=	static_cast<ZSSRenderEngine*>(g_ZFObjSys.GetObjectPtr("ZSSRenderEngine"));
	
	SetView(fFov,fAspect,fNear,fFar);
	SetViewPort( 0, 0, float(m_pkRender->GetWidth()), float(m_pkRender->GetHeight()));
	SetPos(kPos);
	m_kRotM.Identity();
	m_kRenderPos.Set(0,0,0);	
	m_kLastShadowPos.Set(99999,99999,99999);
	m_kLastShadowLightPos.Set(99999,99999,99999);
	
	m_kOrthoSize.Set(15,15,0);				// Defualt Size is 15 x 15 meters
	
	m_strName = 			"A Camera";
	m_bRender		= 		true;
	m_bSelected		= 		false;
	m_iEntity		= 		-1;
	m_iRootEntity	= 		-1;
	m_bRootOnly		= 		false;
	m_pkCameraProp =		NULL;
	m_eMode			= 		CAMMODE_PERSP;		//	just initiating it
	m_fGridSpace	= 		1.0;					// Defualt grid space is one meter.
	m_iRenderTarget= 		RENDERTARGET_SCREEN;
	m_bClearViewPort=		true;	
	m_bForceFullScreen = false;
	m_fExposureFactor	=	0.5;
	
	m_kClearColor	=		Vector4(0.5,0.5,0.5,0);
	m_kFogColor		=		Vector4(0.5,0.5,0.5,0);
	m_fFogNear		=		50;
	m_fFogFar		=		100;
	m_bFogEnabled	=		false;
	
	m_fShakeAmount =		0;
	m_kCurrentShake=		Vector3(0,0,0);
	
	m_iForceLighing=		LIGHT_MATERIAL;
	
	m_bDrawInterface	=	true;
	m_bShadowMap	=		true;
	m_iCurrentRenderMode=RENDER_NONE;
	
	m_bFSSEnabled		=	false;
	m_bBloomEnabled	=	false;
	
	m_iShadowFBO = 		0;
	m_iShadowRBOcolor =	0;	
	
	//default renderstate settings
	m_kRenderState.AddPlugin("Shadowmap");
	m_kRenderState.AddPlugin("PreRender");
	m_kRenderState.AddPlugin("Render");
//  	m_kRenderState.AddPlugin("DepthMapRender");

// 	m_kRenderState.AddPlugin("Bloom");
	m_kRenderState.AddPlugin("InterfaceRender");
	m_kRenderState.AddPlugin("HdrExposure");
	m_kRenderState.AddPlugin("SkyRender");
 	m_kRenderState.AddPlugin("AttachToJoint");

// 	m_kRenderState.AddPlugin("DebugRender");
	
	/*
	//setup default shaders
	m_pkDefaultShadowmapShader = new ZFResourceHandle;
 	m_pkDefaultShadowmapShader->SetRes("shadowmap.vert#shadowmap.frag.glsl"); 				
	m_pkDefaultShadowmapShaderPPL= new ZFResourceHandle;
 	m_pkDefaultShadowmapShaderPPL->SetRes("pplshadowmap.vert#pplshadowmap.frag.glsl"); 				
	m_pkDefaultFastShadowmapShader = new ZFResourceHandle;
	m_pkDefaultFastShadowmapShader->SetRes("#fastshadowmap.frag.glsl");
 					
	//create fsstexture
  	m_iFSSTextureWidth = GetMinSize(m_pkRender->GetWidth());
 	m_iFSSTextureHeight = GetMinSize(m_pkRender->GetHeight());
	m_kFSSTexture.CreateEmptyTexture(m_iFSSTextureWidth,m_iFSSTextureHeight,T_RGBA|T_CLAMP|T_NOCOMPRESSION|T_NOMIPMAPPING);

	//create fssprojection matrix
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);		
	m_pkZShaderSystem->MatrixGenerateOrtho(-1,1,-1,1,0,2);
	m_pkZShaderSystem->MatrixSave(&m_kFSSProjMatrix);			

	//default fss
	m_pkFSSMaterial = new ZMaterial;
	//m_pkFSSMaterial->GetPass(0)->m_pkSLP->SetRes("#fssblackwhite.frag.glsl");
	m_pkFSSMaterial->GetPass(0)->m_pkTUs[0]->SetRes("notex.bmp");
	m_pkFSSMaterial->GetPass(0)->m_bLighting = false;
	m_pkFSSMaterial->GetPass(0)->m_bDepthTest = false;

	
	//BLOOM 
	m_pkBloomMaterial1 = new ZMaterial;
	m_pkBloomMaterial1->GetPass(0)->m_pkSLP->SetRes("#bloom1.frag.glsl");
	m_pkBloomMaterial1->GetPass(0)->m_pkTUs[0]->SetRes("notex.bmp");
	m_pkBloomMaterial1->GetPass(0)->m_bLighting = false;
	m_pkBloomMaterial1->GetPass(0)->m_bDepthTest = false;
	
	m_pkBloomMaterial2 = new ZMaterial;
	m_pkBloomMaterial2->GetPass(0)->m_pkSLP->SetRes("#bloom2.frag.glsl");
	m_pkBloomMaterial2->GetPass(0)->m_pkTUs[0]->SetRes("notex.bmp");
	m_pkBloomMaterial2->GetPass(0)->m_pkTUs[1]->SetRes("notex.bmp");
	m_pkBloomMaterial2->GetPass(0)->m_bLighting = false;
	m_pkBloomMaterial2->GetPass(0)->m_bDepthTest = false;
	
		
	m_kBloomTexture.CreateEmptyTexture(m_iFSSTextureWidth,m_iFSSTextureHeight,T_RGBA|T_CLAMP|T_NOCOMPRESSION|T_NOMIPMAPPING);
	
	//create shadowmap texture	
	if(	(!m_pkZShaderSystem->HaveExtension("GL_ARB_shadow")) || 
			(!m_pkZShaderSystem->HaveExtension("GL_ARB_depth_texture")) )
	{
		cout<<"WARNING: GL_ARB_shadow not supported shadows disabled"<<endl;
		m_bShadowMap = false;
	}
	else	
	{
		//SHADOW HACK
		
		//find shadowtexture size
		int iQuality = m_pkZeroFps->GetShadowMapQuality();
 		m_iShadowTexWidth = iQuality;
 		m_iShadowTexHeight = iQuality;
 		
 		
		
		if(m_pkZShaderSystem->SupportFBO())
		{
			int iMaxFBO;
			glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT,&iMaxFBO);
			
			g_Logf("CAMERA: Using FBO's for shadows , max resulution %dx%d", iMaxFBO, iMaxFBO);
			//cout<<"CAMERA: Using FBO's for shadows , max resulution "<<iMaxFBO<<"x"<<iMaxFBO<<endl;
	 		m_iShadowTexWidth = (int)Min(iQuality,iMaxFBO);
 			m_iShadowTexHeight = (int)Min(iQuality,iMaxFBO);;							
		}
		else
		{
			int iMaxWidth = GetMaxSize(m_pkRender->GetWidth());
			int iMaxHeight = GetMaxSize(m_pkRender->GetHeight());
		
			g_Logf("CAMERA: No FBO support, falling back to glCopyTexSubImage2D, max resulution %dx%d.\n",iMaxWidth,iMaxHeight);
			//cout<<"CAMERA: No FBO support, falling back to glCopyTexSubImage2D, max resulution "<<iMaxWidth<<"x"<<iMaxHeight<<endl;
	 		m_iShadowTexWidth = (int)Min(iQuality,iMaxWidth);
 			m_iShadowTexHeight = (int)Min(iQuality,iMaxHeight);;					
		}
			
		m_pkZShaderSystem->SetShadowMapSize(m_iShadowTexWidth,m_iShadowTexHeight);
		g_Logf("CAMERA: Using shadow texture size: %dx%d.\n", m_iShadowTexWidth, m_iShadowTexHeight);
		//cout<<"CAMERA: Using shadow texture size:"<<	m_iShadowTexWidth<<" "<<m_iShadowTexHeight<<endl;
		
		//are the shadowmap shuld cover
		m_fShadowArea = 50;
		
			m_kShadowTexture.CreateEmptyTexture(m_iShadowTexWidth,m_iShadowTexHeight,T_DEPTH|T_CLAMPTOBORDER|T_NOCOMPRESSION|T_NOMIPMAPPING);
		m_kShadowTexture.SetBorderColor(Vector4(1,1,1,1));
 		
		
		//USE FBO ?
		if(m_pkZShaderSystem->SupportFBO())
		{
			//create FBO and color buffer
			glGetError();
			glGenFramebuffersEXT(1, &m_iShadowFBO);
			glGenRenderbuffersEXT(1, &m_iShadowRBOcolor);
			if(glGetError() != GL_NO_ERROR) cout<<"ERROR generating FBO"<<endl;
			
			//bind frame buffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iShadowRBOcolor);			
			if(glGetError() != GL_NO_ERROR) cout<<"ERROR binding FBO"<<endl;
			
			
			//setup color buffer
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_iShadowRBOcolor);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
												GL_RGBA, m_iShadowTexWidth, m_iShadowTexHeight);			
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
													GL_COLOR_ATTACHMENT0_EXT,
													GL_RENDERBUFFER_EXT, m_iShadowRBOcolor);			
			if(glGetError() != GL_NO_ERROR) cout<<"ERROR color buffer"<<endl;
			
			
			// attach depth buffer texture                                 		
			glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT,
												GL_DEPTH_ATTACHMENT_EXT,
												GL_TEXTURE_2D, m_kShadowTexture.GetOpenGLTextureID(), 0);			
												
			if(glGetError() != GL_NO_ERROR) cout<<"ERROR binding depth "<<endl;
				
			//check framebuffer status
			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			switch(status)
			{
				case GL_FRAMEBUFFER_COMPLETE_EXT:					
					break;
					
				case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
					cout<<"ERROR unsupported FBO format"<<endl;
					break;
					
				default:
					cout<<"ERROR fbo totaly failed"<<endl;					
					break;
			}
					
			//return to normal window buffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);	
		}
	
		//Light projection matrix
		m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);		
		m_pkZShaderSystem->MatrixGenerateOrtho(-m_fShadowArea,m_fShadowArea,-m_fShadowArea,m_fShadowArea,m_fNear,m_fFar);
		m_pkZShaderSystem->MatrixSave(&m_kLightProjMatrix);			
	}*/
}

Camera::~Camera()
{
// 	if(m_pkDefaultShadowmapShaderPPL)
// 		delete m_pkDefaultShadowmapShaderPPL;			
// 	if(m_pkDefaultShadowmapShader)
// 		delete m_pkDefaultShadowmapShader;		
// 	if(m_pkDefaultFastShadowmapShader)
// 		delete m_pkDefaultFastShadowmapShader;
// 
// 				
// 	if(m_pkFSSMaterial)
// 		delete m_pkFSSMaterial;
// 	if(m_pkBloomMaterial1)
// 		delete m_pkBloomMaterial1;
// 	if(m_pkBloomMaterial2)
// 		delete m_pkBloomMaterial2;
// 		
// 	if(m_iShadowFBO != 0)
// 		glDeleteFramebuffersEXT(1,&m_iShadowFBO);
// 	
// 	if(m_iShadowRBOcolor != 0)
// 		glDeleteRenderbuffersEXT(1,&m_iShadowRBOcolor);

}

int Camera::GetMaxSize(int iRes)
{
	if(iRes >= 2048 )
		return(2048);
	else if(iRes >= 1024 )
		return(1024);
	else if(iRes >= 512 )
		return(512);
	else if(iRes >= 256 )
		return(256);
	
	return 128;
}

int Camera::GetMinSize(int iRes)
{
	if(iRes <= 256 )
		return 256;
	else if(iRes <= 512)
		return 512;
	else if(iRes <= 1024)
		return 1024;
	else if(iRes <= 2048)
		return 2048;
		
	return 256;
}

void Camera::SetFSSGLSLShader(const string& strShader)
{
	m_pkFSSMaterial->GetPass(0)->m_pkSLP->SetRes(strShader+string(".glsl"));
}

void Camera::FullScreenShader()
{
	static float data[]= {	-1,-1,-1,
									 1,-1,-1,
							 		 1, 1,-1,
									-1, 1,-1};

	static float uvdata[]= {0,0,
									1,0,
							 		1,1,
									0,1};
									
	//calculate how to stretch the texture									
 	float xs = 	float(m_kViewPortSize.x) / float(m_iFSSTextureWidth);								
 	float ys = 	float(m_kViewPortSize.y) / float(m_iFSSTextureHeight);


 	uvdata[2] = xs;
 	uvdata[4] = xs;
 	uvdata[5] = ys;
 	uvdata[7] = ys;
	
	//save screen surface to fss texture									
	m_kFSSTexture.CopyFrameBuffer((int)m_kViewPortCorner.x,(int)m_kViewPortCorner.y,(int)m_kViewPortSize.x, (int)m_kViewPortSize.y);
	
	//setup fss orthogonal projection matrix
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixLoad(&m_kFSSProjMatrix);
	
	//identity
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixIdentity();
	
	//bind fss material	
	m_pkZShaderSystem->BindMaterial(m_pkFSSMaterial);
	
	//setup vertex data
	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,data);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,uvdata);
	m_pkZShaderSystem->SetNrOfVertexs(4);
	
		
  	//we dont wan to write any depth data
  	m_pkZShaderSystem->SetDepthMask(false);
  	
  	//ugly haxk to force another texture then the one specified in the material
	m_pkZShaderSystem->BindTexture(&m_kFSSTexture);
  	
  	
	//draw scree surface
	m_pkZShaderSystem->DrawArray(QUADS_MODE);  	
  	

  	//reenable depth mask
	m_pkZShaderSystem->SetDepthMask(true);
  	
  	
  	//pop matrices
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);	
	m_pkZShaderSystem->MatrixPop();	
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);	
	m_pkZShaderSystem->MatrixPop();	
}

float Camera::GetExposureFactor()
{
	static ResTexture* pkTex = NULL;
	
	if(!pkTex)
	{
		pkTex = new ResTexture;
 		pkTex->CreateEmptyTexture(m_iFSSTextureWidth,m_iFSSTextureHeight,T_RGBA|T_CLAMP|T_NOCOMPRESSION);		
	}
	
	//get framebuffer
	pkTex->CopyFrameBuffer((int)m_kViewPortCorner.x,(int)m_kViewPortCorner.y,(int)m_kViewPortSize.x, (int)m_kViewPortSize.y);
	pkTex->RegenerateMipmaps();
	
	float fx= float(m_kViewPortSize.x)/float(m_iFSSTextureWidth);
	float fy= float(m_kViewPortSize.y)/float(m_iFSSTextureHeight);
	
	
	Image* pkImage = pkTex->GetTextureImage(4);


	color_rgba	kColor;	
	float fTotal = 0;
	int iSamples = 0;
	
	for(int y = 0;y<pkImage->m_iHeight*fy;y++)
	{			
		for(int x = 0;x<pkImage->m_iWidth*fx;x++)
		{
			pkImage->get_pixel(x,y,kColor);	
			fTotal += (kColor.r*0.3+kColor.g*0.59+kColor.b*0.11);
			iSamples++;
		}			
	}
	
	delete pkImage;
	
	//
	//luminance = 0.3*R + 0.59 * G + 0.11*B
	//
 	static const float fTargetBrightness = 0.45;
	
	//avrage	
	float fExp = fTotal/ (iSamples * (255*0.3 + 255*0.59 + 255*0.11));
	
	//get current factor 	
 	float fEF = m_fExposureFactor;
 	
 	//adjust exposure factor
  	fEF += (fTargetBrightness - fExp)*m_pkZeroFps->GetFrameTime()*0.5;
	
	//min and max exposure
	if(fEF < 0.1) fEF = 0.1; 	
 	if(fEF > 1.0) fEF = 1.0;	
 	
	
	return fEF;
}

void Camera::MakeBloom()
{
	static float data[]= {	-1,-1,-1,
									 1,-1,-1,
							 		 1, 1,-1,
									-1, 1,-1};

	static float uvdata[]= {0,0,
									1,0,
							 		1,1,
									0,1};
									
	//calculate how to stretch the texture									
 	float xs = 	float(m_kViewPortSize.x) / float(m_iFSSTextureWidth);								
 	float ys = 	float(m_kViewPortSize.y) / float(m_iFSSTextureHeight);

 	uvdata[2] = xs;
 	uvdata[4] = xs;
 	uvdata[5] = ys;
 	uvdata[7] = ys;
	
	//save screen surface to fss texture									
	m_kFSSTexture.CopyFrameBuffer((int)m_kViewPortCorner.x,(int)m_kViewPortCorner.y,(int)m_kViewPortSize.x, (int)m_kViewPortSize.y);
			
	//setup fss orthogonal projection matrix
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixLoad(&m_kFSSProjMatrix);
	
	//identity
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixIdentity();
	
	//bind bloom material	1
	m_pkZShaderSystem->BindMaterial(m_pkBloomMaterial1);
	
	//setup vertex data
	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,data);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,uvdata);
	m_pkZShaderSystem->SetNrOfVertexs(4);
	
		
  	//we dont wan to write any depth data
  	m_pkZShaderSystem->SetDepthMask(false);
  	
  	//ugly haxk to force another texture then the one specified in the material
	m_pkZShaderSystem->BindTexture(&m_kFSSTexture);
  	
  	
	//draw scree surface
	m_pkZShaderSystem->DrawArray(QUADS_MODE);  	
  	
  	
	//save image, and do another draw	
	m_kBloomTexture.CopyFrameBuffer((int)m_kViewPortCorner.x,(int)m_kViewPortCorner.y,(int)m_kViewPortSize.x, (int)m_kViewPortSize.y);
	
	m_pkZShaderSystem->BindMaterial(m_pkBloomMaterial2);	
	m_pkZShaderSystem->BindTexture(&m_kBloomTexture);		
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	m_pkZShaderSystem->BindTexture(&m_kFSSTexture);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	m_pkZShaderSystem->DrawArray(QUADS_MODE); 
	
		
  	//reenable depth mask
	m_pkZShaderSystem->SetDepthMask(true);
  	
  	
  	//pop matrices
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);	
	m_pkZShaderSystem->MatrixPop();	
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);	
	m_pkZShaderSystem->MatrixPop();	

}


void Camera::MakeShadowTexture(const Vector3& kLightPos,const Vector3& kCenter, ResTexture* pkTexture)
{
	//modelview matrix
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);	
	m_pkZShaderSystem->MatrixIdentity();
	gluLookAt( kLightPos.x, kLightPos.y, kLightPos.z,
					kCenter.x,kCenter.y,kCenter.z,
					0.0f, 1.0f, 0.0f);
					
	m_pkZShaderSystem->MatrixSave(&m_kLightViewMatrix);								
					

	//setup FBO if supported
	if(m_pkZShaderSystem->SupportFBO())
	{
		//enable FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iShadowFBO);
	}

	
	glViewport(0, 0, m_iShadowTexWidth, m_iShadowTexHeight);
	glScissor(0, 0, m_iShadowTexWidth, m_iShadowTexHeight);
	
	m_pkZShaderSystem->ClearBuffer(COLOR_BUFFER|DEPTH_BUFFER);



 	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
 	m_pkZShaderSystem->MatrixLoad(&m_kLightProjMatrix);
 
 	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
 	m_pkZShaderSystem->MatrixLoad(&m_kLightViewMatrix);
	

	
	m_kRenderPos = kLightPos;
	m_kFrustum.GetFrustum(m_kLightProjMatrix,m_kLightViewMatrix);	
			
	
	//Disable color writes, and use flat shading for speed
  	m_pkZShaderSystem->ForceColorMask(FORCE_DISABLE);

//  	m_pkZShaderSystem->ForceCullFace(CULL_FACE_FRONT);

	//setup depth offseting
//  	glDepthRange (0.003, 1.0);	//offset depthrange   default
//   	glDepthRange (0.001, 1.0);	//offset depthrange 
 	glEnable(GL_POLYGON_OFFSET_FILL);
  	glPolygonOffset(2,0);

	//reload last material
	m_pkZShaderSystem->ReloadMaterial();		
	
	
  	m_pkLight->SetLighting(false);
	m_pkZShaderSystem->ForceLighting(LIGHT_ALWAYS_OFF);
 	glShadeModel(GL_FLAT);
	
	//disable all shaders
	m_pkZShaderSystem->SetForceDisableGLSL(true);
	
	//update all render propertys that shuld cast a shadow
	Entity* pkRootEntity = m_pkEntityMan->GetEntityByID(m_iRootEntity);	
	m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly);
	
	
	//save texture	
	if(m_pkZShaderSystem->SupportFBO())
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	else
	{
// 	 	glBindTexture(GL_TEXTURE_2D, m_iShadowTexture);	
	 	m_pkZShaderSystem->BindTexture(pkTexture);
 		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_iShadowTexWidth, m_iShadowTexHeight);
	}
	
	
	//enable shaders again
	m_pkZShaderSystem->SetForceDisableGLSL(false);
	
  	glDepthRange (0.0, 1.0);
 	glDisable(GL_POLYGON_OFFSET_FILL);
  	glPolygonOffset(0,0);
	
  	m_pkLight->SetLighting(true);
	m_pkZShaderSystem->ForceLighting(LIGHT_MATERIAL);
 	glShadeModel(GL_SMOOTH);
	m_pkZShaderSystem->ForceColorMask(FORCE_DEFAULT);
//	m_pkZShaderSystem->ForceCullFace(-1);


	//reload last material
	m_pkZShaderSystem->ReloadMaterial();			
}

void Camera::SetupRenderState()
{
	//force full screen
	if(m_bForceFullScreen)
	{
		m_kViewPortCorner.x = 0;
		m_kViewPortCorner.y = 0;
		m_kViewPortSize.x = float(m_pkRender->GetWidth());
		m_kViewPortSize.y = float(m_pkRender->GetHeight());
	}

	//setup render state
	m_kRenderState.m_kCameraPosition 			=	m_kPos;
	m_kRenderState.m_kCameraProjectionMatrix 	= 	m_kCamProjectionMatrix;
	m_kRenderState.m_kCameraRotation 			=	m_kRotM;	
	m_kRenderState.m_kViewPortPos 				=	m_kViewPortCorner;
	m_kRenderState.m_kViewPortSize 				=	m_kViewPortSize;

	m_kRenderState.m_fFogFar 		= m_fFogFar;
	m_kRenderState.m_fFogNear 		= m_fFogNear;
	m_kRenderState.m_bFogEnabled 	= m_bFogEnabled;
	m_kRenderState.m_kFogColor 	= m_kFogColor;
	m_kRenderState.m_kClearColor 	= m_kClearColor;;

	m_kRenderState.m_pkRoot = m_pkEntityMan->GetEntityByID(m_iRootEntity);

	//save render pos in camera
	m_kRenderPos = m_kPos;
	
	
	//shake stuff	
	if(m_fShakeAmount > 0)
	{
		Vector3 kNew(	Math::Randomf(m_fShakeAmount)-m_fShakeAmount/2,
							Math::Randomf(m_fShakeAmount)-m_fShakeAmount/2,
							Math::Randomf(m_fShakeAmount)-m_fShakeAmount/2);
		m_kCurrentShake = (m_kCurrentShake + kNew) * 0.5;
		
		m_kRenderState.m_kCameraRotation.Rotate(m_kCurrentShake);
		
		m_fShakeAmount -= m_pkZeroFps->GetFrameTime()*10;		
	}
	
}

void Camera::InitView()//int iWidth,int iHeight) 
{
	m_fAppWidth  = m_pkRender->GetWidth();//;float(iWidth);
	m_fAppHeight = m_pkRender->GetHeight();//float(iHeight);

	// Load projection.
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixLoad(&m_kCamProjectionMatrix);

	//force full screen
	if(m_bForceFullScreen)
	{
		m_kViewPortCorner.x = 0;
		m_kViewPortCorner.y = 0;
		m_kViewPortSize.x = float(m_pkRender->GetWidth());
		m_kViewPortSize.y = float(m_pkRender->GetHeight());
	}

	//setup viewport
	glScissor  ( GLint(m_kViewPortCorner.x), GLint(m_kViewPortCorner.y),	GLsizei(m_kViewPortSize.x), GLsizei(m_kViewPortSize.y) );
	glViewport ( GLint(m_kViewPortCorner.x), GLint(m_kViewPortCorner.y),	GLsizei(m_kViewPortSize.x), GLsizei(m_kViewPortSize.y)  );		
		
	//reset modelview matrix and setup the newone
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
 	m_pkZShaderSystem->MatrixLoad(&m_kRotM);	
	
	
	//Set render position to cameras current position
	m_kRenderPos = m_kPos;
	
	//random camera movement
	if(m_fShakeAmount > 0)
	{
		m_kRenderPos += Vector3(Math::Randomf(m_fShakeAmount)-m_fShakeAmount/2,
										Math::Randomf(m_fShakeAmount)-m_fShakeAmount/2,
										Math::Randomf(m_fShakeAmount)-m_fShakeAmount/2);
		
		m_fShakeAmount -= m_pkZeroFps->GetFrameTime();		
	}
	
	//do camera translation
	m_pkZShaderSystem->MatrixTranslate(-m_kRenderPos);	 	

	//save render position (camera position may change during renderpass)
// 	m_kRenderPos = m_kPos;
	
	//set eye position in shadersystem
	m_pkZShaderSystem->SetEyePosition(m_kRenderPos);
	
	//save modelview matrix for future use =)
	m_pkZShaderSystem->MatrixSave(&m_kCamModelViewMatrix);
	
	//update the frustum
	m_kFrustum.GetFrustum(m_kCamProjectionMatrix,m_kCamModelViewMatrix);
	
	
	//last but not least setup fog and clear color
	m_pkZShaderSystem->SetClearColor(m_kClearColor);
	m_pkZShaderSystem->SetFog(m_kFogColor,m_fFogNear,m_fFogFar,m_bFogEnabled);

	
	//lighing
	m_pkZShaderSystem->ForceLighting(m_iForceLighing);
	
	//clear viewport
	if(m_bClearViewPort)
		ClearViewPort(true);	
	else
		ClearViewPort(false);	
}




Vector3 Camera::GetViewPortSize()
{
	Vector3 kRes;
	kRes.x = m_kViewPortSize.x;	
	kRes.y = m_kViewPortSize.y;	
	return kRes;
}


Vector3 Camera::GetViewPortCorner()
{
	Vector3 kRes;
	kRes.x = m_kViewPortCorner.x;	
	kRes.y = m_kViewPortCorner.y;	
	return kRes;
}


void Camera::SetView(float fFov,float fAspect,float fNear,float fFar)
{
	//m_bViewChange	= true;

	m_fFov		= fFov;
	m_fAspect	= fAspect;
	m_fNear		= fNear;
	m_fFar		= fFar;
	
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixGeneratePerspective(fFov, fAspect,fNear,fFar);
	m_pkZShaderSystem->MatrixSave(&m_kCamProjectionMatrix);
	m_pkZShaderSystem->MatrixPop();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);

}


void Camera::SetOrthoView()
{
	//m_bViewChange	= true;

	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixGenerateOrtho(-m_kOrthoSize.x, m_kOrthoSize.x, -m_kOrthoSize.y, m_kOrthoSize.y, -500, 500);
	m_pkZShaderSystem->MatrixSave(&m_kCamProjectionMatrix);
	m_pkZShaderSystem->MatrixPop();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	
}


void Camera::SetViewMode(CamMode eMode)
{
	m_eMode = eMode;

	if(m_eMode == CAMMODE_PERSP) {
		SetView(90,1.333,0.25,250);
		}
	if(m_eMode != CAMMODE_PERSP) {
		SetOrthoView();

		m_kRotM.Identity();

		switch(eMode) {
			case CAMMODE_ORTHO_TOP:		
				m_kRotM.Rotate(Vector3(-90,0,0));	
				m_kOrthoAxisX.Set(1,0,0);	
				m_kOrthoAxisY.Set(0,0,-1);	
				m_kOrthoAxisZ.Set(0,-1,0);	break;
			case CAMMODE_ORTHO_FRONT:	
				m_kRotM.Identity();						
				m_kOrthoAxisX.Set(1,0,0);	
				m_kOrthoAxisY.Set(0,1,0);	
				m_kOrthoAxisZ.Set(0,0,-1);	break;
			case CAMMODE_ORTHO_LEFT:	
				m_kRotM.Rotate(Vector3(0,-90,0));		
				m_kOrthoAxisX.Set(0,0,1);	
				m_kOrthoAxisY.Set(0,1,0);	
				m_kOrthoAxisZ.Set(-1,0,0);	break;
			case CAMMODE_ORTHO_BOT:		
				m_kRotM.Rotate(Vector3(90,0,0));		
				m_kOrthoAxisX.Set(1,0,0);	
				m_kOrthoAxisY.Set(0,0,1);	
				m_kOrthoAxisZ.Set(0,1,0);	break;
			case CAMMODE_ORTHO_BACK:	
				m_kRotM.Rotate(Vector3(0,90,0));		
				m_kOrthoAxisX.Set(1,0,0);	
				m_kOrthoAxisY.Set(0,1,0);	
				m_kOrthoAxisZ.Set(0,0,0);	break;
			case CAMMODE_ORTHO_RIGHT:	
				m_kRotM.Rotate(Vector3(0,90,0));	
				m_kOrthoAxisX.Set(0,0,-1);	
				m_kOrthoAxisY.Set(0,1,0);	
				m_kOrthoAxisZ.Set(-1,0,0);	break;
		
			}
		}
}


void Camera::SetViewMode(string strName)
{
	CamMode eMode = CAMMODE_PERSP;

	if(strcmp(strName.c_str(), "persp") == 0)		eMode = CAMMODE_PERSP;
	if(strcmp(strName.c_str(), "top") == 0)		eMode = CAMMODE_ORTHO_TOP;
	if(strcmp(strName.c_str(), "front") == 0)		eMode = CAMMODE_ORTHO_FRONT;
	if(strcmp(strName.c_str(), "left") == 0)		eMode = CAMMODE_ORTHO_LEFT;
	if(strcmp(strName.c_str(), "bot") == 0)		eMode = CAMMODE_ORTHO_BOT;
	if(strcmp(strName.c_str(), "back") == 0)		eMode = CAMMODE_ORTHO_BACK;
	if(strcmp(strName.c_str(), "right") == 0)		eMode = CAMMODE_ORTHO_RIGHT;
	SetViewMode(eMode);
}


void Camera::SetFov(float fFov)
{
	m_fFov = fFov;
	SetView(fFov, m_fAspect, m_fNear, m_fFar);
}


void Camera::SetViewPort(float fX,float fY,float fW,float fH) 
{
	//m_bViewPortChange = true;

	m_kViewPortCorner.Set(fX,fY);
	m_kViewPortSize.Set(fW,fH);
}


void Camera::DrawGrid()
{
	if(m_bDrawOrthoGrid == false)	return;
	if(m_eMode == CAMMODE_PERSP)	return;

	glPushAttrib(GL_LIGHTING_BIT|GL_ENABLE_BIT);
	
	Vector3 kStart, kEnd;
	float		fStart;
	int		iStart;
	Vector3	kColor;
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	//Vector3 kFrom, kTo;
	float fFromX	= GetPos().Dot(m_kOrthoAxisX) - m_kOrthoSize.x;
	float fToX		= GetPos().Dot(m_kOrthoAxisX) + m_kOrthoSize.x;
	float fFromY	= GetPos().Dot(m_kOrthoAxisY) - m_kOrthoSize.y;
	float fToY		= GetPos().Dot(m_kOrthoAxisY) + m_kOrthoSize.y;

	// Draw Grid Axis
	kColor = m_pkRender->GetEditColor("grid/axis");
	glColor3f(kColor.x,kColor.y,kColor.z);
	glLineWidth(2.0);
	glBegin(GL_LINES);	
		kStart	= m_kOrthoAxisX * fFromX	+ m_kOrthoAxisY * 0;	
		kEnd		= m_kOrthoAxisX * fToX		+ m_kOrthoAxisY * 0;	
		glVertex3f(kStart.x,kStart.y,kStart.z);
		glVertex3f(kEnd.x,kEnd.y,kEnd.z);
		kStart	= m_kOrthoAxisX * 0	+ m_kOrthoAxisY * fFromY;	
		kEnd		= m_kOrthoAxisX * 0		+ m_kOrthoAxisY * fToY;	
		glVertex3f(kStart.x,kStart.y,kStart.z);
		glVertex3f(kEnd.x,kEnd.y,kEnd.z);
	glEnd();
	glLineWidth(1.0);

	kColor = m_pkRender->GetEditColor("grid/line");
	glColor3f(kColor.x,kColor.y,kColor.z);

	glBegin(GL_LINES);	

	iStart = int( fFromX / m_fGridSpace );
	fStart = m_fGridSpace * (float)iStart;

	// Draw X-Grid Lines
	for(float x = fStart; x < fToX; x += m_fGridSpace) {
		kStart	= m_kOrthoAxisX * x + m_kOrthoAxisY * fFromY;	
		kEnd		= m_kOrthoAxisX * x + m_kOrthoAxisY * fToY;	
		glVertex3f(kStart.x,kStart.y,kStart.z);
		glVertex3f(kEnd.x,kEnd.y,kEnd.z);
		}

	// Draw Y-Grid Lines
	iStart = int( fFromY / m_fGridSpace );
	fStart = m_fGridSpace * (float)iStart;
	for(float y = fStart; y < fToY; y += m_fGridSpace) {
		kStart	= m_kOrthoAxisX * fFromX	+ m_kOrthoAxisY * y;	
		kEnd		= m_kOrthoAxisX * fToX		+ m_kOrthoAxisY * y;	
		glVertex3f(kStart.x,kStart.y,kStart.z);
		glVertex3f(kEnd.x,kEnd.y,kEnd.z);
		}

	glEnd();

	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	glPopAttrib();
}


float FSign(float fIn)
{
	if(fIn >= 0.0)	return 1;
		else return -1;
}


Vector3 Camera::SnapToGrid(Vector3 kPos)
{
	// Ignore if not a Ortho View.
	if(m_eMode == CAMMODE_PERSP)	return kPos;
	if(m_bGridSnap == false)		return kPos;

	int		iStart;
	Vector3	kNewPos;
	Vector3  kSign;
	kSign.x  = FSign(kPos.x);
	kSign.y  = FSign(kPos.y);
	kSign.z  = FSign(kPos.z);
   kPos.Abs();

	kPos += Vector3(m_fGridSpace / float(2.0), m_fGridSpace / float(2.0), m_fGridSpace / float(2.0));

	iStart = int(kPos.x / m_fGridSpace);
	kNewPos.x = m_fGridSpace * (float)iStart;
	iStart = int(kPos.y / m_fGridSpace);
	kNewPos.y = m_fGridSpace * (float)iStart;
	iStart = int(kPos.z / m_fGridSpace);
	kNewPos.z = m_fGridSpace * (float)iStart;

	kNewPos.x *= kSign.x;
	kNewPos.y *= kSign.y;
	kNewPos.z *= kSign.z;

	return kNewPos;
}


void Camera::ClearViewPort(bool bColor)
{
	if(bColor)
		m_pkZShaderSystem->ClearBuffer(COLOR_BUFFER);
		
	m_pkZShaderSystem->ClearBuffer(DEPTH_BUFFER);
	
	DrawGrid();
}


/* Returns a string that describes the camera. */
string Camera::GetCameraDesc()
{
	char desc[512];
	string strDesc;

	//sprintf(desc, "Cam '%s': <%.3f, %.3f, %.3f>, <%.3f, %.3f, %.3f>", m_strName.c_str(), m_kPos.x,m_kPos.y,m_kPos.z,
	//	m_kRot.x,m_kRot.y,m_kRot.z);
	sprintf(desc, "No Cam Desc");
	strDesc = desc;

	return strDesc;
}


void Camera::SetRotM(const Matrix4& kRotM)	
{	
	if(m_eMode != CAMMODE_PERSP)
		return;
	m_kRotM = kRotM;
}


void Camera::RotateV(const Vector3& kRot)			
{	
	if(m_eMode != CAMMODE_PERSP)
		return;
	m_kRotM.Rotate(kRot);
}


void Camera::MultRotM(const Matrix4& kRotM)
{	
	if(m_eMode != CAMMODE_PERSP)
		return;
	m_kRotM = kRotM * m_kRotM;
}


void Camera::OrthoZoom(float fZoom)
{
	m_kOrthoSize *= fZoom;
	SetOrthoView();
}


void Camera::OrthoMove(Vector3 kMove)
{
	Vector3 kPos = GetPos();
	kPos += m_kOrthoAxisX * kMove.x;
	kPos += m_kOrthoAxisY * kMove.y;
	SetPos(kPos);
}


Vector3 Camera::GetOrthoMove(Vector3 kMove)
{
	Vector3 kNewMove(0,0,0);
	kNewMove += m_kOrthoAxisX * kMove.x;
	kNewMove += m_kOrthoAxisY * kMove.y;
	return kNewMove;
}




void Camera::RenderView()
{
	//if this camera is attached with any camera property run an update on that 
	//property to make sure the camera position is up to date
	if(m_pkCameraProp)
	{
		if(!m_pkEntityMan->GetEntityByID(m_iEntity))
			cout<<"entity does not exist"<<endl;
		
		m_pkCameraProp->Update();
	}
	
	//set current camera in engine ( render propertys wants to know this)
	m_pkZeroFps->m_pkCamera=this;				
	
	//return if no rendering shuld be done
	if( (!m_bRender) || (!m_pkZeroFps->GetRenderOn()) || m_pkZeroFps->GetMinimized() )
	{		
		return;
	}

	SetupRenderState();
 	m_pkZSSRenderEngine->Render(m_kRenderState);	
return;

	//first make this camera matrises the current ones
	InitView();


	
	//set exposure
	if(m_pkZShaderSystem->UseHDR())
		m_pkZShaderSystem->SetExposure(m_fExposureFactor);
	
	//draw world
	DrawWorld();
	
	//get current frames exposure factor
	if(m_pkZShaderSystem->UseHDR())
		m_fExposureFactor = GetExposureFactor();
	
	//apply fss
	if(m_bFSSEnabled)
		FullScreenShader();
		
	//add bloom effect
	if(m_bBloomEnabled && m_pkZShaderSystem->SupportGLSLProgram())
		MakeBloom();
		
	if(m_bDrawInterface) 
	{
		//m_pkRender->Sphere(kCenter,1,1,Vector3(1,1,1),true);
		
// 		m_pkEntityMan->DrawSceneGraph();
		m_pkEntityMan->DrawSceneAABBTree();	
		m_pkEntityMan->DrawZones();				
		m_pkZeroFps->m_pkApp->RenderInterface();	
			
		//draw axes icon
		if(m_pkZeroFps->GetDrawAxesIcon())
			m_pkRender->Draw_AxisIcon(5);	
	}
		
	
	//reset camera
	m_pkZeroFps->m_pkCamera=NULL;

}


void Camera::DrawWorld()
{
	//get root entity		
	Entity* pkRootEntity = m_pkEntityMan->GetEntityByID(m_iRootEntity);
	
	//shuld we render shadowmaps?		
	if((m_bShadowMap && m_pkZeroFps->GetShadowMap() && (m_iForceLighing != LIGHT_ALWAYS_OFF) ))
	{	
		//scene center (use entity pos if any)
		static Matrix4 kTrans;
		static Vector3 kCenter = Vector3(0,0,0);
		kTrans = m_kRotM;
		kTrans.Transponse();	

// 		kCenter = m_kRenderPos + kTrans.VectorTransform(Vector3(0,0,-1))*m_fShadowArea;						
 		kCenter = m_kRenderPos;						
						
		//setup light
		LightSource* pkLight = m_pkLight->GetSunPointer();		
		Vector3 kLightPos = (kCenter + (pkLight->kRot.Unit() * 100));
		
		//create shadow map	realtime or not
		if(m_pkZeroFps->GetShadowMapRealtime())
		{
			m_iCurrentRenderMode = RENDER_CASTSHADOW;
 			MakeShadowTexture(kLightPos,kCenter,&m_kShadowTexture);		
		}
		else
		{
			if(m_kLastShadowPos.DistanceTo(kCenter) > m_fShadowArea/8.0 ||
				m_kLastShadowLightPos != pkLight->kRot)
			{
				m_kLastShadowLightPos = pkLight->kRot;
				m_kLastShadowPos = kCenter;
				m_iCurrentRenderMode = RENDER_CASTSHADOW;
				MakeShadowTexture(kLightPos,kCenter,&m_kShadowTexture);		
			}
		}
				
		//init tha damn view
		InitView();	


		//render no shadowed objects 
		m_iCurrentRenderMode = RENDER_NOSHADOWFIRST;
		m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly);				

		//render shadows depending on shadowmode
		int iShadowMode = m_pkZeroFps->GetShadowMapMode();
		if(m_pkZShaderSystem->SupportGLSLProgram() && iShadowMode)
		{		
 			m_pkZShaderSystem->UseDefaultGLSLProgram(true);
			
			//real no spec shadowmaps
			if(iShadowMode == 1)			
				m_pkZShaderSystem->SetDefaultGLSLProgram(m_pkDefaultShadowmapShader);
 				
 			//only darken
			if(iShadowMode == 2)			
				m_pkZShaderSystem->SetDefaultGLSLProgram(m_pkDefaultFastShadowmapShader);
			
			if(iShadowMode == 3)			
				m_pkZShaderSystem->SetDefaultGLSLProgram(m_pkDefaultShadowmapShaderPPL);						
			
			
			//draw shadowed scene
			m_iCurrentRenderMode = RENDER_SHADOWED;			
			DrawShadowedScene();
			
			m_pkZShaderSystem->UseDefaultGLSLProgram(false);
		} 
		else
		{
		
			//draw LIT light				
			m_iCurrentRenderMode = RENDER_SHADOWED;
  			m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly,false);				

		
			//draw shadowed scene
			if(pkLight)
			{
				//disable diffuse and specular lighting for the shadowcasting lightsource
				Vector4 kDiffuseBak  = pkLight->kDiffuse;
				Vector4 kSpecularBak = pkLight->kSpecular;;			
				pkLight->kDiffuse = kDiffuseBak*0.2;
				pkLight->kSpecular = kSpecularBak*0.2;
			
				m_iCurrentRenderMode = RENDER_SHADOWED;
				DrawShadowedScene();			
				
				pkLight->kDiffuse = kDiffuseBak;
				pkLight->kSpecular = kSpecularBak;		
			}
			else
			{
				m_pkLight->SetAmbientOnly(true);		
				m_iCurrentRenderMode = RENDER_SHADOWED;
				DrawShadowedScene();
				m_pkLight->SetAmbientOnly(false);				
			}
		}		
			
		//m_pkZShadow->Update();

		//Draw unshadows scene
 		m_iCurrentRenderMode = RENDER_NOSHADOWLAST;
 		m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly,false);
 	
		m_iCurrentRenderMode = RENDER_NONE;		
		
	}
	else
	{		
		//set default glsl shader
		switch(m_pkZeroFps->GetShadowMapMode())
		{
			case 0:
				m_pkZShaderSystem->UseDefaultGLSLProgram(false);
				break;
		
			case 1:
				m_pkZShaderSystem->UseDefaultGLSLProgram(true);
				m_pkZShaderSystem->SetDefaultGLSLProgram(m_pkDefaultShadowmapShader);
				break;
			case 2:
				m_pkZShaderSystem->UseDefaultGLSLProgram(true);
				m_pkZShaderSystem->SetDefaultGLSLProgram(m_pkDefaultFastShadowmapShader);
				break;
			case 3:
				m_pkZShaderSystem->UseDefaultGLSLProgram(true);
				m_pkZShaderSystem->SetDefaultGLSLProgram(m_pkDefaultShadowmapShaderPPL);
				break;
		}		
	
		//update all render propertys that shuld be shadowed
		m_iCurrentRenderMode = RENDER_NOSHADOWFIRST;
		m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly);		
		m_iCurrentRenderMode = RENDER_SHADOWED;
		m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly,false);

	
		//update shadow map
		m_pkZShadow->Update();
	
		//update all render propertys that shuld NOT be shadowed
		m_iCurrentRenderMode = RENDER_NOSHADOWLAST;
		m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly,false);
		
		//disable default glslprogram
		m_pkZShaderSystem->UseDefaultGLSLProgram(false);
		
		m_iCurrentRenderMode = RENDER_NONE;	
	}
}


void Camera::DrawShadowedScene()
{
	m_pkZShaderSystem->ForceTU3Disabled(true);

	//setup some textures
	glActiveTextureARB(GL_TEXTURE3_ARB);
	
	
	static Matrix4 biasMatrix(	0.5f, 0.0f, 0.0f, 0.0f,
										0.0f, 0.5f, 0.0f, 0.0f,
										0.0f, 0.0f, 0.5f, 0.0f,
										0.5f, 0.5f, 0.5f, 1.0f); //bias from [-1, 1] to [0, 1]

	static Matrix4 Proj;
	static Matrix4 View;
	static Matrix4 Bias;
	static Matrix4 textureMatrix;
	
	Proj = m_kLightProjMatrix;
	View = m_kLightViewMatrix;
	Bias = biasMatrix;	

	View.Transponse();
	Proj.Transponse();
	Bias.Transponse();

	textureMatrix=Bias*(Proj*View);									
	
	//Bind & enable shadow map texture
	m_pkZShaderSystem->BindTexture(&m_kShadowTexture);
	
	glEnable(GL_TEXTURE_2D);
	
	
	//Set up texture coordinate generation.
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, &(textureMatrix.RowCol[0][0]));
	glEnable(GL_TEXTURE_GEN_S);
	
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, &(textureMatrix.RowCol[1][0]));
	glEnable(GL_TEXTURE_GEN_T);
	
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, &(textureMatrix.RowCol[2][0]));
	glEnable(GL_TEXTURE_GEN_R);
	
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, &(textureMatrix.RowCol[3][0]));
	glEnable(GL_TEXTURE_GEN_Q);

	
	//Enable shadow comparison
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
	
	//Shadow comparison should be true (ie not in shadow) if r<=texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_GEQUAL);
	
	//Shadow comparison should generate an INTENSITY result
//  	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
// 	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);
  	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_ALPHA);

	
	//Set alpha test to discard false comparisons
	if((!m_pkZeroFps->GetShadowMapMode()) || (!m_pkZShaderSystem->SupportGLSLProgram()))
 		m_pkZShaderSystem->ForceAlphaTest(FORCE_OTHER);
		
	
	//reload last material
	m_pkZShaderSystem->ReloadMaterial();		
	
	Entity* pkRootEntity = m_pkEntityMan->GetEntityByID(m_iRootEntity);	
 	m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly,false);	

	
	
	glActiveTextureARB(GL_TEXTURE3_ARB);
	
	m_pkZShaderSystem->ForceAlphaTest(FORCE_DEFAULT);
	
	//Disable textures and texgen
	glDisable(GL_TEXTURE_2D);
	
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	
	//Restore other states
	//glDisable(GL_LIGHTING);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	
	m_pkZShaderSystem->ForceTU3Disabled(false);
}



void Camera::SetFog(const Vector4& kColor,float fStart,float fStop,bool bEnabled)
{
	m_kFogColor = 		kColor;
	m_fFogNear = 		fStart;
	m_fFogFar = 		fStop;	
	m_bFogEnabled =	bEnabled;
}


Vector3 Camera::Get3DCursorDir(float fMouseX,float fMouseY,bool bMouse)
{
	static Vector3 dir;
	static Vector3 kViewSize, kViewCorner;
	static Matrix4 rm;
	static Matrix4 pr;
	float x,y;		
	
	
	kViewSize = GetViewPortSize();
	kViewCorner = GetViewPortCorner();

	if(bMouse)
	{
  		x = -0.5f + (float) (fMouseX - kViewCorner.x) / (float) kViewSize.x;
  		y = -0.5f + (float) ((m_pkRender->GetHeight() - fMouseY) - kViewCorner.y) / (float) kViewSize.y;

		if(GetViewMode() == Camera::CAMMODE_PERSP) 
		{
			x*=2;
			y*=2;
			dir.Set(x,y,0);
		}
		else 
		{			
			dir.Set(x,y,0);
		}
	}
	else
	{
		dir.Set(0,0,0);
	}
	
	pr = GetProjectionMatrix();
	pr = pr.Inverse();	
	dir = pr.VectorTransform(dir);
	
	
	rm = GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);

	dir.Normalize();
	
	return dir;	
}

Vector3 Camera::Get3DCursorPos(float fMouseX,float fMouseY,bool bMouse)
{
	Vector3 dir;
	float x,y;		

	Vector3 kViewSize, kViewCorner;
	kViewSize = GetViewPortSize();
	kViewCorner = GetViewPortCorner();

	if(bMouse)
	{
  		x = -0.5f + (float) (fMouseX - kViewCorner.x) / (float) kViewSize.x;
  		y = -0.5f + (float) ((m_pkRender->GetHeight() - fMouseY) - kViewCorner.y) / (float) kViewSize.y;

		if(GetViewMode() == Camera::CAMMODE_PERSP) 
		{
			x*=2;
			y*=2;
			dir.Set(x,y,0);
		}
		else 
		{			
			dir.Set(x,y,0);
		}
	}
	else
	{
		dir.Set(0,0,0);
	}
	
	dir.x = m_kOrthoSize.x * x * 2;
	dir.y = m_kOrthoSize.y * y * 2;


	return dir;
}
