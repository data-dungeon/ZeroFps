#include "madproperty.h"
#include "object.h"

/*
float fGameTime;

void SetGameTime(void)
{

	fGameTime = SDL_GetTicks() / 1000.0;
}*/


MadProperty::MadProperty()
{
	
	bNetwork = true;
	strcpy(m_acName,"MadProperty");
//	m_pkPropertyFactory->Register("MadProperty", Create_MadProperty);
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
}

MadProperty::MadProperty(Mad_Core* pkModell) {
	strcpy(m_acName,"MadProperty");

	pkCore = pkModell;
	
	PlayAnimation(0, 0.0);
	m_fScale = 1.0;
	m_bActive = true;
//	bFlipFace = false;
	pkCore->ClearReplaceTexture();

}

void DrawBoundSphere(float fRadius)
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glColor3f(1,1,1);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D );

	float x,y;
	glBegin(GL_LINE_LOOP );
	for(int i=0; i<360; i+=(int)12.25) {
		x = cos(DegToRad(i)) * fRadius;
		y = sin(DegToRad(i)) * fRadius;
		glVertex3f(x,y,0);
	}
	
	glEnd();

	glPopAttrib();
}

float fTestValue;

void MadProperty::Update() 
{
	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
/*	Input* pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input")); 
	
	if(strcmp(m_kMadFile.c_str(), "../data/mad/dropship.mad") == 0) {
		if(pkInput->Pressed(KEY_F7))
			pkCore->CreateController("lucka", "joint6",CONTROLL_ANGLE_Y,95-84,0-84);
		if(pkInput->Pressed(KEY_F8))
			fTestValue -= 0.1;
		if(pkInput->Pressed(KEY_F9))
			fTestValue += 0.1;
	
		pkCore->SetControll("lucka",fTestValue);
		}*/

//	return;
	float fRadius = pkCore->GetRadius();
 
	if(!pkCore)
		return;

	UpdateAnimation(m_pkZeroFps->GetFrameTime());

	glPushMatrix();
		glTranslatef(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
		glScalef(m_fScale,m_fScale,m_fScale);
		glRotatef(m_pkObject->GetRot().z ,0,0,1);		
		glRotatef(m_pkObject->GetRot().x ,1,0,0);
		glRotatef(m_pkObject->GetRot().y ,0,1,0);		
		
		Vector4 sphere=m_pkObject->GetPos();
		sphere.w = fRadius;

		if(m_pkFrustum->SphereInFrustum(sphere)) {
			m_pkZeroFps->m_iNumOfMadRender++;
			//DrawBoundSphere(sphere.w * (1 / m_fScale));
//			DrawBoundSphere(fRadius * (1 / m_fScale));
//			DrawBoundSphere(fRadius);
			
			Draw_All();
			}
	
	glPopMatrix();
}

void MadProperty::SetBase(const char* acName)
{
	SetBasePtr(m_pkZeroFps->GetMADPtr(acName));
//	m_kMadFile=acName;
}

//void MadProperty::SetBase(Mad_Core* pkModell)
//{
//	pkCore = pkModell;
	
//	PlayAnimation(0, 0.0);
//	m_fScale = 1.0;
//	m_bActive = true;
//	bFlipFace = false;
//	pkCore->ClearReplaceTexture();
//}

/*

void MadProperty::PlayAnimation(int iAnimNum, float fStartTime)
{
	SetGameTime();

	iActiveAnimation = iAnimNum;
	fCurrentTime = fStartTime;
	fLastUpdate = fGameTime; 
}*/

/*
void MadProperty::UpdateAnimation(void)
{
	if(!m_bActive)
		return;

	SetGameTime();

	float fDelta = fGameTime - fLastUpdate;

	fCurrentTime += fDelta;

	float fAnimLength = pkCore->GetAnimationLengthInS(iActiveAnimation);

	if(fCurrentTime >= fAnimLength)

		fCurrentTime -= fAnimLength;

	fLastUpdate = fGameTime; 
}*/

/*
void MadProperty::SetScale(float fScale)
{
	m_fScale = fScale;
}


void	MadProperty::SetNextAnimation(int iAnimNum)
{
	m_iNextAnimation = iAnimNum;
}

void	MadProperty::PlayNextAnimations(void)
{

}

int		MadProperty::GetNextAnimation()
{
	return m_iNextAnimation;
}


int		MadProperty::GetCurrentAnimation()
{
	return iActiveAnimation;
}

void	MadProperty::SetLoopedStatus(bool bLoop)
{
	m_bLoop = bLoop;
}

bool	MadProperty::IsLooped()
{
	return m_bLoop;
}


void	MadProperty::StopAnimation(void)
{
	m_bActive = false;
}

void	MadProperty::StartAnimation(void)
{
	m_bActive = true;
}

void	MadProperty::NextCoreAnimation(void)
{}

void MadProperty::SetReplaceTexture(char* szName)
{
	pkCore->SetReplaceTexture(szName);
}*/

void MadProperty::Save(ZFMemPackage* pkPackage)
{	
	char temp[50];
	strcpy(temp,m_kMadFile.c_str());

	pkPackage->Write((void*)temp,50);
//	pkPackage->Write(m_fScale);
	pkPackage->Write((void*)&m_fScale,4);
}

void MadProperty::Load(ZFMemPackage* pkPackage)
{
	pkPackage->SetPos(0);
	char temp[50];
	pkPackage->Read((void*)temp,50);	
	SetBase(temp);
	
	float scale;
//	pkPackage->Read(scale);
	pkPackage->Read((void*)&scale,4);
	SetScale(scale);
}



Property* Create_MadProperty()
{
	return new MadProperty;
}

