#include "camera.h"
#include "../render/render.h"
#include "../ogl/zfpsgl.h"

bool Camera::m_bDrawOrthoGrid(false);
float	Camera::m_fGridSpace(1.0);
bool Camera::m_bGridSnap(false);

Camera::Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar)
{
	m_pkRender = dynamic_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));

	SetView(fFov,fAspect,fNear,fFar);
	SetViewPort(0,0,m_pkRender->GetWidth(),m_pkRender->GetHeight());
	SetPos(kPos);
	SetRot(kRot);
	m_kRotM.Identity();
	
	m_strName = "A Camera";
	
	m_kOrthoSize.Set(15,15,0);				// Defualt Size is 15 x 15 meters

	m_bRender		= true;
	m_bSelected		= false;
	m_iEntity		= -1;
	m_eMode			= CAMMODE_PERSP;		//	just initiating it
	m_fGridSpace	= 1.0;					// Defualt grid space is one meter.
	m_pkWnd			= NULL;

	m_bForceFullScreen = false;
}

void Camera::UpdateAll(int iWidth,int iHeight) 
{
	m_bViewChange		=	true;
	m_bViewPortChange	=	true;
}

void Camera::Update(int iWidth,int iHeight) 
{
	m_fAppWidth  = iWidth;
	m_fAppHeight = iHeight;

	if(m_bViewChange)   
	{
		m_bViewChange = false;
	
		// Load projection.
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((float*) &m_kCamProjectionMatrix.data);
	}
	
	if(m_bViewPortChange)
	{
		m_bViewPortChange = false;

		if(m_pkWnd) 
		{
			Rect kJagVillSpelUT2k4 = m_pkWnd->GetScreenRect();
			m_kViewPortCorner.x = kJagVillSpelUT2k4.Left;
			m_kViewPortCorner.y = 600 - kJagVillSpelUT2k4.Top - kJagVillSpelUT2k4.Height();
			m_kViewPortSize.x  = kJagVillSpelUT2k4.Width(); 
			m_kViewPortSize.y  = kJagVillSpelUT2k4.Height(); 
		
			m_kViewPortCorner.x	= float(m_kViewPortCorner.x)	/ 800.0 * m_pkRender->GetWidth();
			m_kViewPortCorner.y	= float(m_kViewPortCorner.y)  / 600.0 * m_pkRender->GetHeight();
			m_kViewPortSize.x		= float(m_kViewPortSize.x)		/ 800.0 * m_pkRender->GetWidth();
			m_kViewPortSize.y		= float(m_kViewPortSize.y)		/ 600.0 * m_pkRender->GetHeight();
		}

		if(m_bForceFullScreen)
		{
			m_kViewPortCorner.x = 0;
			m_kViewPortCorner.y = 0;
			m_kViewPortSize.x = m_pkRender->GetWidth();
			m_kViewPortSize.y = m_pkRender->GetHeight();
		}

		glScissor  ( m_kViewPortCorner.x, m_kViewPortCorner.y,	m_kViewPortSize.x, m_kViewPortSize.y );
		glViewport ( m_kViewPortCorner.x, m_kViewPortCorner.y,	m_kViewPortSize.x, m_kViewPortSize.y  );		
	}
	
	//reset modelview matrix and setup the newone
	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();													
	
	glMultMatrixf(&m_kRotM[0]);
	 	
	glTranslatef(-m_kPos.x,-m_kPos.y,-m_kPos.z);

	//get modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&m_kCamModelViewMatrix.data);
	
	//update the frustum
	m_kFrustum.GetFrustum();
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
	m_bViewChange	= true;

	m_fFov		= fFov;
	m_fAspect	= fAspect;
	m_fNear		= fNear;
	m_fFar		= fFar;
	
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();
	 	glLoadIdentity();													
		gluPerspective(fFov, fAspect,fNear,fFar);	
		glGetFloatv(GL_PROJECTION_MATRIX,(float*)&m_kCamProjectionMatrix.data);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
}


void Camera::SetOrthoView()
{
	m_bViewChange	= true;

	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();
		glLoadIdentity();													
		glOrtho(-m_kOrthoSize.x, m_kOrthoSize.x, -m_kOrthoSize.y, m_kOrthoSize.y, -500, 500); 	
		glGetFloatv(GL_PROJECTION_MATRIX,(float*)&m_kCamProjectionMatrix.data);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
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
	m_bViewPortChange = true;

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

	iStart = fFromX / m_fGridSpace;
	fStart = m_fGridSpace * (float)iStart;

	// Draw X-Grid Lines
	for(float x = fStart; x < fToX; x += m_fGridSpace) {
		kStart	= m_kOrthoAxisX * x + m_kOrthoAxisY * fFromY;	
		kEnd		= m_kOrthoAxisX * x + m_kOrthoAxisY * fToY;	
		glVertex3f(kStart.x,kStart.y,kStart.z);
		glVertex3f(kEnd.x,kEnd.y,kEnd.z);
		}

	// Draw Y-Grid Lines
	iStart = fFromY / m_fGridSpace;
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

	kPos += Vector3(m_fGridSpace / 2.0, m_fGridSpace / 2.0, m_fGridSpace / 2.0);

	iStart = kPos.x / m_fGridSpace;
	kNewPos.x = m_fGridSpace * (float)iStart;
	iStart = kPos.y / m_fGridSpace;
	kNewPos.y = m_fGridSpace * (float)iStart;
	iStart = kPos.z / m_fGridSpace;
	kNewPos.z = m_fGridSpace * (float)iStart;

	kNewPos.x *= kSign.x;
	kNewPos.y *= kSign.y;
	kNewPos.z *= kSign.z;

	return kNewPos;
}


void Camera::ClearViewPort()
{
	if(m_bSelected)
		glClearColor(0.680, 0.631, 0.631,0.0);
	else
		glClearColor(0.631, 0.631, 0.631,0.0);
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
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


void Camera::SetRotM(Matrix4 kRotM)	
{	
	if(m_eMode != CAMMODE_PERSP)
		return;
	m_kRotM = kRotM;
}


void Camera::RotateV(Vector3 kRot)			
{	
	if(m_eMode != CAMMODE_PERSP)
		return;
	m_kRotM.Rotate(kRot);
}


void Camera::MultRotM(Matrix4 kRotM)
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
