#include "camera.h"
#include "../ogl/zfpsgl.h"

/*#include "GL/gl.h"
#include "GL/glu.h"*/
 
Camera::Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar)
{
	SetView(fFov,fAspect,fNear,fFar);
	SetViewPort(0,0,1,1);
	SetPos(kPos);
	SetRot(kRot);
	m_kRotM.Identity();
	
	m_strName = "A Camera";
	
}

void Camera::UpdateAll(int iWidth,int iHeight) 
{
	m_bViewChange=true;
	m_bViewPortChange=true;
//	Update(iWidth,iHeight);
}

void Camera::Update(int iWidth,int iHeight) 
{

	if(m_bViewChange)
	{
		m_bViewChange=false;
	
		//load projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((float*) &m_kCamProjectionMatrix.data);
	}
	
	if(m_bViewPortChange){	
		m_bViewPortChange=false;
		
		glScissor(	int(iWidth*m_fX),
						int(iHeight*m_fY),
						int(iWidth*m_fWidth),
						int(iHeight*m_fHeight));
		
		glViewport( int(iWidth*m_fX), 
						int(iHeight*m_fY),
						int(iWidth*m_fWidth),
						int(iHeight*m_fHeight));		
	
	}

	
	//reset modelview matrix and setup the newone
	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();													
	
	glMultMatrixf(&m_kRotM[0]);
	//glRotatef(m_kRot.z,0,0,1);	
	//glRotatef(m_kRot.x,1,0,0);	
	//glRotatef(m_kRot.y,0,1,0);		
	 	
	glTranslatef(-m_kPos.x,-m_kPos.y,-m_kPos.z);
	
	//get modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&m_kCamModelViewMatrix.data);
	
	//update the frustum
	m_kFrustum.GetFrustum();
}

void Camera::SetView(float fFov,float fAspect,float fNear,float fFar)
{
	m_bViewChange=true;

	m_fFov		= fFov;
	m_fAspect	= fAspect;
	m_fNear		= fNear;
	m_fFar		= fFar;
	
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();
	 	glLoadIdentity();													
		gluPerspective(fFov, fAspect,fNear,fFar);	
		//get projection matrix
		glGetFloatv(GL_PROJECTION_MATRIX,(float*)&m_kCamProjectionMatrix.data);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
	
}

void Camera::SetFov(float fFov)
{
	m_fFov = fFov;
	SetView(fFov, m_fAspect, m_fNear, m_fFar);
}


void Camera::SetViewPort(float fX,float fY,float fW,float fH) 
{

	m_bViewPortChange=true;

	m_fX=fX;
	m_fY=fY;
	
	m_fWidth=fW;
	m_fHeight=fH;

}


void Camera::ClearViewPort() {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
}

string Camera::GetCameraDesc()
{
	char desc[512];
	string strDesc;

//	sprintf(desc, "Cam '%s': <%.3f, %.3f, %.3f>, <%.3f, %.3f, %.3f>", m_strName.c_str(), m_kPos.x,m_kPos.y,m_kPos.z,
//		m_kRot.x,m_kRot.y,m_kRot.z);
	sprintf(desc, "No Cam Desc");
	strDesc = desc;

	return strDesc;
}
