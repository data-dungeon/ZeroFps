#include "camera.h"


Camera::Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar)
{
	SetView(fFov,fAspect,fNear,fFar);
	SetPos(kPos);
	SetRot(kRot);
		
	m_fX=0;
	m_fY=0;
	m_fWidth=1;
	m_fHeight=1;
	
}

void Camera::UpdateAll() 
{
	m_bViewChange=true;
	Update();

}

void Camera::Update() {
	
	if(m_bViewChange){
		m_bViewChange=false;
	
//		cout<<"balle"<<endl;
	
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

void Camera::SetViewPort(float fX,float fY,float fW,float fH) 
{
	m_fX=fX;
	m_fY=fY;
	
	m_fWidth=fW;
	m_fHeight=fH;

}


