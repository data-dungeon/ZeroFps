#include "camera.h"


Camera::Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar)
{
	SetView(fFov,fAspect,fNear,fFar);
	SetPos(kPos);
	SetRot(kRot);
		
	m_iX=0;
	m_iY=0;
	m_iWidth=1024;
	m_iHeight=768;
}

void Camera::UpdateAll() 
{
	m_bViewChange=true;
	Update();

}

void Camera::Update() {
	
	if(m_bViewChange){
		m_bViewChange=false;
	
		//load projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((float*)&m_kCamMatrix[0]);
	}
	
	//reset modelview matrix and setup the newone
	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();													
	
	glRotatef(m_kRot.x,1,0,0);	
	glRotatef(m_kRot.y,0,1,0);	
	glRotatef(m_kRot.z,0,0,1);	 	
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



