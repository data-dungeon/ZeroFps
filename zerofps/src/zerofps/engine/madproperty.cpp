#include "madproperty.h"
#include "object.h"
#include "../render/render.h"

MadProperty::MadProperty()
{
	bNetwork = true;
	strcpy(m_acName,"MadProperty");
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
}

MadProperty::MadProperty(Mad_Core* pkModell) 
{
	bNetwork = true;
	strcpy(m_acName,"MadProperty");
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	pkCore = pkModell;
	
	PlayAnimation(0, 0.0);
	m_fScale = 1.0;
	m_bActive = true;
	pkCore->ClearReplaceTexture();
}


float fTestValue;

void MadProperty::Update() 
{
	Input* pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input")); 
	
	if(strcmp(m_kMadFile.c_str(), "../data/mad/dropship.mad") == 0) {
		if(pkInput->Pressed(KEY_F7))
			pkCore->CreateController("lucka", "gunBarrelJoint2",CONTROLL_ANGLE_Z,-90,0);
		if(pkInput->Pressed(KEY_F8))
			fTestValue -= 0.1;
		if(pkInput->Pressed(KEY_F9))
			fTestValue += 0.1;

		if(fTestValue < 0)
			fTestValue = 0.0;
		if(fTestValue > 1.0)
			fTestValue = 1.0;
	
		pkCore->SetControll("lucka",fTestValue);
		}
	
	if(!pkCore)
		return;

	UpdateAnimation(m_pkZeroFps->GetFrameTime());
	
	Vector4 sphere=m_pkObject->GetPos();
	sphere.w = pkCore->GetRadius();

	if(!m_pkFrustum->SphereInFrustum(sphere))
		return;

	glPushMatrix();
		glTranslatef(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
//		glScalef(m_fScale,m_fScale,m_fScale);
		glScalef(0.01,0.01,0.01);
		glRotatef(m_pkObject->GetRot().z ,0,0,1);		
		//glRotatef(m_pkObject->GetRot().x ,1,0,0);
		// FH's Föreningens årsmöte.
			glRotatef(- (m_pkObject->GetRot().y - 90) ,0,1,0);
		Draw_All();
	glPopMatrix();

	m_pkZeroFps->m_iNumOfMadRender++;

}

void MadProperty::SetBase(const char* acName)
{
	SetBasePtr(m_pkZeroFps->GetMADPtr(acName));
}

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


/*
	float fTestValue;

  Input* pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input")); 
	
	if(strcmp(m_kMadFile.c_str(), "../data/mad/dropship.mad") == 0) {
		if(pkInput->Pressed(KEY_F7))
			pkCore->CreateController("lucka", "joint6",CONTROLL_ANGLE_Y,95-84,0-84);
		if(pkInput->Pressed(KEY_F8))
			fTestValue -= 0.1;
		if(pkInput->Pressed(KEY_F9))
			fTestValue += 0.1;
	
		pkCore->SetControll("lucka",fTestValue);
		}*/
