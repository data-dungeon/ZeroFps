#include "camera.h"
#include "../render/render.h"
#include "../ogl/zfpsgl.h"

bool Camera::m_bDrawOrthoGrid(false);
float	Camera::m_fGridSpace(1.0);
bool Camera::m_bGridSnap(false);

Camera::Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar)
 : I_Camera(kPos,kRot,fFov,fAspect,fNear,fFar) 
{
	m_pkRender = 			dynamic_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));
	m_pkZShaderSystem =	dynamic_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkEntityMan = 		dynamic_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_pkZeroFps =			dynamic_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkZShadow = 			dynamic_cast<ZShadow*>(g_ZFObjSys.GetObjectPtr("ZShadow"));
	m_pkLight	=			dynamic_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light"));
	m_pkTexMan	=			dynamic_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	
	SetView(fFov,fAspect,fNear,fFar);
	SetViewPort( 0, 0, float(m_pkRender->GetWidth()), float(m_pkRender->GetHeight()));
	SetPos(kPos);
	m_kRotM.Identity();
	m_kRenderPos.Set(0,0,0);	
	
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
	
	m_kClearColor	=		Vector4(0.5,0.5,0.5,0);
	m_kFogColor		=		Vector4(0.5,0.5,0.5,0);
	m_fFogNear		=		50;
	m_fFogFar		=		100;
	m_bFogEnabled	=		false;
	
	m_bDebugGraphs	=		true;
	m_bShadowMap	=		true;
	m_iCurrentRenderMode=RENDER_NONE;
	
	
	if(	(!m_pkZShaderSystem->HaveExtension("GL_ARB_shadow")) || 
			(!m_pkZShaderSystem->HaveExtension("GL_ARB_depth_texture")) )
	{
		cout<<"WARNING: GL_ARB_shadow not supported shadows disabled"<<endl;
		m_bShadowMap = false;
	}
		
	//SHADOW HACK
	
	//find shadowtexture size
	m_iShadowTexWidth = GetMaxSize(m_pkRender->GetWidth());
	m_iShadowTexHeight = GetMaxSize(m_pkRender->GetHeight());
		
	cout<<"Using shadow texture size:"<<	m_iShadowTexWidth<<" "<<m_iShadowTexHeight<<endl;
	
	m_iShadowTexture = -1;
	m_fShadowArea = 30;
	
	glGenTextures(1, &m_iShadowTexture);
	glBindTexture(GL_TEXTURE_2D, m_iShadowTexture);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_iShadowTexWidth, m_iShadowTexHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
/*	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	

	//Light projection matrix
 	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);		
	m_pkZShaderSystem->MatrixGenerateOrtho(-m_fShadowArea,m_fShadowArea,-m_fShadowArea,m_fShadowArea,m_fNear,m_fFar);
 	//m_pkZShaderSystem->MatrixGeneratePerspective(25,1,m_fNear,m_fFar);
 	m_pkZShaderSystem->MatrixSave(&m_kLightProjMatrix);			
}

Camera::~Camera()
{
	if(m_iShadowTexture != -1)
		glDeleteTextures(1,&m_iShadowTexture);

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


void Camera::MakeShadowTexture(const Vector3& kLightPos,const Vector3& kCenter,unsigned int iTexture)
{
	//modelview matrix
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);	
	m_pkZShaderSystem->MatrixIdentity();
	gluLookAt( kLightPos.x, kLightPos.y, kLightPos.z,
					kCenter.x,kCenter.y,kCenter.z,
					0.0f, 1.0f, 0.0f);
					
	m_pkZShaderSystem->MatrixSave(&m_kLightViewMatrix);								
					

	
	//m_pkZShaderSystem->SetClearColor(m_kClearColor);
	//m_pkZShaderSystem->SetFog(m_kFogColor,m_fFogNear,m_fFogFar,m_bFogEnabled);
	
	m_pkZShaderSystem->ClearBuffer(COLOR_BUFFER|DEPTH_BUFFER);

 	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
 	m_pkZShaderSystem->MatrixLoad(&m_kLightProjMatrix);
 
 	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
 	m_pkZShaderSystem->MatrixLoad(&m_kLightViewMatrix);
	
	
	
	m_kRenderPos = kLightPos;
	m_kFrustum.GetFrustum(m_kLightProjMatrix,m_kLightViewMatrix);	
			
	
	
	glViewport(0, 0, m_iShadowTexWidth, m_iShadowTexHeight);
	glScissor(0, 0, m_iShadowTexWidth, m_iShadowTexHeight);

	//Draw back faces into the shadow map
	
	//Disable color writes, and use flat shading for speed
  	m_pkZShaderSystem->ForceColorMask(0);
// 	m_pkZShaderSystem->ForceCullFace(CULL_FACE_FRONT);
	glDepthRange (0.003, 1.0);
	
	//reload last material
	m_pkZShaderSystem->ReloadMaterial();		
	
	
 	m_pkLight->SetLighting(false);
 	glShadeModel(GL_FLAT);
	
	//update all render propertys that shuld cast a shadow
	Entity* pkRootEntity = m_pkEntityMan->GetEntityByID(m_iRootEntity);
	m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly);
	
	
	glBindTexture(GL_TEXTURE_2D, m_iShadowTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_iShadowTexWidth, m_iShadowTexHeight);
	//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, 1024,1024,0);
	
	
	glDepthRange (0.0, 1.0);
	
 	m_pkLight->SetLighting(true);
 	glShadeModel(GL_SMOOTH);
	m_pkZShaderSystem->ForceColorMask(-1);
	m_pkZShaderSystem->ForceCullFace(-1);


	//reload last material
	m_pkZShaderSystem->ReloadMaterial();			
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
	m_pkZShaderSystem->MatrixIdentity();									
	m_pkZShaderSystem->MatrixMult(m_kRotM);	
	m_pkZShaderSystem->MatrixTranslate(-m_kPos);	 	

	//save render position (camera position may change during renderpass)
	m_kRenderPos = m_kPos;
	
	//save modelview matrix for future use =)
	m_pkZShaderSystem->MatrixSave(&m_kCamModelViewMatrix);
	
	//update the frustum
	m_kFrustum.GetFrustum(m_kCamProjectionMatrix,m_kCamModelViewMatrix);
	
	
	//last but not least setup fog and clear color
	m_pkZShaderSystem->SetClearColor(m_kClearColor);
	m_pkZShaderSystem->SetFog(m_kFogColor,m_fFogNear,m_fFogFar,m_bFogEnabled);


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

	if(strcmp(strName.c_str(), "persp") == 0)	eMode = CAMMODE_PERSP;
	if(strcmp(strName.c_str(), "top") == 0)		eMode = CAMMODE_ORTHO_TOP;
	if(strcmp(strName.c_str(), "front") == 0)	eMode = CAMMODE_ORTHO_FRONT;
	if(strcmp(strName.c_str(), "left") == 0)	eMode = CAMMODE_ORTHO_LEFT;
	if(strcmp(strName.c_str(), "bot") == 0)		eMode = CAMMODE_ORTHO_BOT;
	if(strcmp(strName.c_str(), "back") == 0)	eMode = CAMMODE_ORTHO_BACK;
	if(strcmp(strName.c_str(), "right") == 0)	eMode = CAMMODE_ORTHO_RIGHT;
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

	m_kViewPortCorner.Set(fX,fY,0);
	m_kViewPortSize.Set(fW,fH,0);
}


void Camera::DrawGrid()
{
	if(m_bDrawOrthoGrid == false)	return;
	if(m_eMode == CAMMODE_PERSP)	return;

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
	glEnable(GL_LIGHTING);
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
/*	if(m_bSelected)
		glClearColor(0.680, 0.631, 0.631,0.0);
	else
		glClearColor(0.631, 0.631, 0.631,0.0);
*/
	if(bColor)
		m_pkZShaderSystem->ClearBuffer(COLOR_BUFFER);
		
	m_pkZShaderSystem->ClearBuffer(DEPTH_BUFFER);
	
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
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
	//set current camera in engine ( render propertys wants to know this)
	m_pkZeroFps->m_pkCamera=this;			

	//if this camera is attached with any camera property run an update on that 
	//property to make sure the camera position is up to date
	if(m_pkCameraProp)
	{
		if(!m_pkEntityMan->GetEntityByID(m_iEntity))
			cout<<"entity does not exist"<<endl;
		
		m_pkCameraProp->Update();
	}
							
	//first make this camera matrises the current ones
	InitView();
	
	//return if no rendering shuld be done
	if( (!m_bRender) || (!m_pkZeroFps->GetRenderOn()) || m_pkZeroFps->GetMinimized() )
		return;


	//get root entity		
	Entity* pkRootEntity = m_pkEntityMan->GetEntityByID(m_iRootEntity);
		
	
	//shuld be render shadowmaps?		
	if((m_bShadowMap && m_pkZeroFps->GetShadowMap()))
	{	
		//scene center (use entity pos if any)
		Vector3 kCenter;	
		if(Entity* pkEnt = m_pkEntityMan->GetEntityByID(m_iEntity))
			kCenter = pkEnt->GetIWorldPosV();		
		else
			kCenter = m_kPos;
			
		//setup light
		LightSource* pkLight= m_pkLight->GetFirstDirectionalLight();		
		
		Vector3 kLightPos;
		if(pkLight)
			kLightPos = (kCenter + (pkLight->kRot.Unit() * 100));
		else
			kLightPos =  kCenter + Vector3(0,100,0);		

		
		//create shadow map	
		//cout<<"pos:"<<kLightPos.x<<" "<<kLightPos.y<<" "<<kLightPos.z<<"   "<< kCenter.DistanceTo(kLightPos)<<endl;
		m_iCurrentRenderMode = RENDER_SHADOWMAP;
		MakeShadowTexture(kLightPos,kCenter,m_iShadowTexture);
				
	
		//draw LIT light				
		InitView();	
		m_iCurrentRenderMode = RENDER_NORMAL;
		m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly,false);		
		
		//draw shadowed scene
		if(pkLight)
		{
			//disable diffuse and specular lighting for the shadowcasting lightsource
			Vector4 kDiffuseBak  = pkLight->kDiffuse;
			Vector4 kSpecularBak = pkLight->kSpecular;;			
			pkLight->kDiffuse.Set(0,0,0,0);
			pkLight->kSpecular.Set(0,0,0,0);
		
			m_iCurrentRenderMode = RENDER_SHADOW;
			DrawShadowedScene();			
			
			pkLight->kDiffuse = kDiffuseBak;
			pkLight->kSpecular = kSpecularBak;		
		}
		else
		{
			m_pkLight->SetAmbientOnly(true);		
			m_iCurrentRenderMode = RENDER_SHADOW;
			DrawShadowedScene();
			m_pkLight->SetAmbientOnly(false);				
		}
		
			
		//Draw unshadows scene
		m_iCurrentRenderMode = RENDER_NOSHADOWED;
		m_pkEntityMan->Update(PROPERTY_TYPE_RENDER_NOSHADOW,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly);
	
		m_iCurrentRenderMode = RENDER_NONE;		
		
	}
	else
	{		
		//update all render propertys that shuld be shadowed
		m_iCurrentRenderMode = RENDER_NORMAL;
		m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly);
	
		//update shadow map
		//m_pkZShadow->Update();
	
		//update all render propertys that shuld NOT be shadowed
		m_iCurrentRenderMode = RENDER_NOSHADOWED;
		m_pkEntityMan->Update(PROPERTY_TYPE_RENDER_NOSHADOW,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly);
		
		m_iCurrentRenderMode = RENDER_NONE;	
	}
		
	if(m_bDebugGraphs) 
	{
		m_pkEntityMan->DrawZones();				
		m_pkZeroFps->m_pkApp->RenderInterface();	
			
		//draw axes icon
		if(m_pkZeroFps->GetDrawAxesIcon())
			m_pkRender->Draw_AxisIcon(5);	
	}
		
	
	//reset camera
	m_pkZeroFps->m_pkCamera=NULL;

}



void Camera::DrawShadowedScene()
{
	//setup some textures
	glActiveTextureARB(GL_TEXTURE3_ARB);

	static Matrix4 biasMatrix(	0.5f, 0.0f, 0.0f, 0.0f,
										0.0f, 0.5f, 0.0f, 0.0f,
										0.0f, 0.0f, 0.5f, 0.0f,
										0.5f, 0.5f, 0.5f, 1.0f); //bias from [-1, 1] to [0, 1]


	Matrix4 Proj = m_kLightProjMatrix;
	Matrix4 View = m_kLightViewMatrix;
	Matrix4 Bias = biasMatrix;	

	View.Transponse();
	Proj.Transponse();
	Bias.Transponse();

	Matrix4 textureMatrix=Bias*(Proj*View);									

	
	
	//Bind & enable shadow map texture
	//int iTexture = m_pkTexMan->GetTextureID(m_pkTexMan->GetIndex("data/textures/coke.tga"));
	//m_pkTexMan->BindTexture("c#data/textures/hora.tga",0);
	glBindTexture(GL_TEXTURE_2D, m_iShadowTexture);
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
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_ALPHA);

	
	//Set alpha test to discard false comparisons
	m_pkZShaderSystem->ForceAlphaTest(2);
		
	
	//reload last material
	m_pkZShaderSystem->ReloadMaterial();		
	
	Entity* pkRootEntity = m_pkEntityMan->GetEntityByID(m_iRootEntity);
	m_pkEntityMan->Update(PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT,true,pkRootEntity,m_bRootOnly,false);	
	
	
	glActiveTextureARB(GL_TEXTURE3_ARB);
	
	m_pkZShaderSystem->ForceAlphaTest(-1);
	
	//Disable textures and texgen
	glDisable(GL_TEXTURE_2D);
	
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	
	//Restore other states
	//glDisable(GL_LIGHTING);
	glActiveTextureARB(GL_TEXTURE0_ARB);
}



void Camera::SetFog(const Vector4& kColor,float fStart,float fStop,bool bEnabled)
{
	m_kFogColor = 		kColor;
	m_fFogNear = 		fStart;
	m_fFogFar = 		fStop;	
	m_bFogEnabled =	bEnabled;
}


