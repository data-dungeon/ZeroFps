#include "camera.h"


Camera::Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar)
{
	SetView(fFov,fAspect,fNear,fFar);
	SetViewPort(0,0,1,1);
	SetPos(kPos);
	SetRot(kRot);

}

void Camera::UpdateAll(int iWidth,int iHeight) 
{
	m_bViewChange=true;
	m_bViewPortChange=true;
	Update(iWidth,iHeight);
}

void Camera::Update(int iWidth,int iHeight) {

	if(m_bViewChange)
	{
		m_bViewChange=false;
	
		//load projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((float*)&m_kCamMatrix[0]);
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
	
	glRotatef(m_kRot.z,0,0,1);	
	glRotatef(m_kRot.x,1,0,0);	
	glRotatef(m_kRot.y,0,1,0);		
	 	
	glTranslatef(-m_kPos.x,-m_kPos.y,-m_kPos.z);
	
	
}

void Camera::SetView(float fFov,float fAspect,float fNear,float fFar)
{
	m_bViewChange=true;
	
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();
		gluPerspective(fFov,fAspect,fNear,fFar);	
		glGetFloatv(GL_PROJECTION_MATRIX,(float*)&m_kCamMatrix[0]);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
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

Matrix4 Camera::GetModelMatrix() {
	Matrix4 temp;

	glGetFloatv(GL_MODELVIEW_MATRIX, &temp[0]);
	return temp;
}

string Camera::GetCameraDesc()
{
	char desc[256];
	string strDesc;

	sprintf(desc, "Cam: <%.3f, %.3f, %.3f>, <%.3f, %.3f, %.3f>", m_kPos.x,m_kPos.y,m_kPos.z,
		m_kRot.x,m_kRot.y,m_kRot.z);
	strDesc = desc;

	return strDesc;
}
