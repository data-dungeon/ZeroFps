#include "madproperty.h"
#include "object.h"
#include "../render/render.h"

extern int g_iMadLODLock;
extern float g_fMadLODScale;
 
MadProperty::MadProperty()
{
	strcpy(m_acName,"MadProperty");
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	bNetwork		= true;
	m_bIsVisible	= true;

	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
}

MadProperty::MadProperty(string strResName) 
{
	bNetwork = true;
	strcpy(m_acName,"MadProperty");
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	ZFResourceDB* pkResDB = static_cast<ZFResourceDB*>(g_ZFObjSys.GetObjectPtr("ZFResourceDB"));
	pkResDB->GetResource(kMadHandle, strResName);

	//pkCore = pkModell;
	
	PlayAnimation(0, 0.0);
	m_fScale = 1.0;
	m_bActive = true;
//	pkCore->ClearReplaceTexture();
}


//float fTestValue;

void MadProperty::Update() 
{
//	return;

/*	Input* pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input")); 
	
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
		}*/
	
//	Vector4 sphere=m_pkObject->GetPos();
//	sphere.w = GetRadius();

//	if(!m_pkFrustum->SphereInFrustum(sphere))
//		return;

	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return;

	UpdateAnimation(m_pkZeroFps->GetFrameTime());
	if(!m_bIsVisible)
		return;

	if(!m_pkFrustum->SphereInFrustum(m_pkObject->GetPos(),GetRadius()))
		return;


	// Set Object LOD.
	if(g_iMadLODLock == 0) {
		Vector3 kDiff = m_pkZeroFps->GetCam()->GetPos() - m_pkObject->GetPos();
		float fDist = fabs(kDiff.Length());
		m_fLod = 1 - (fDist / 300);
		//cout << "fDist: " << fDist << " / " << "m_fLod: " << m_fLod << endl;
		
		
		//dvoid yber loding deluxe
		float blub = GetRadius() / fDist;		
		if(blub < 0.010)
			return;
		
	}

	g_fMadLODScale = m_fLod;


	glPushMatrix();
		glTranslatef(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
		glScalef(m_fScale, m_fScale, m_fScale);
		glRotatef(- m_pkObject->GetRot().y ,0,1,0);		
		glRotatef(- m_pkObject->GetRot().z ,0,0,1);		
		glRotatef(- m_pkObject->GetRot().x ,1,0,0);

		Draw_All(m_pkZeroFps->m_iMadDraw);
	glPopMatrix();

	if(m_pkZeroFps->m_iMadDraw & MAD_DRAW_SPHERE) {
		glPushMatrix();
			glTranslatef(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
			glRotatef(90 ,1,0,0);
			Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render")); 
			pkRender->DrawBoundSphere(GetRadius(),Vector3::ZERO);		
		glPopMatrix();
	}

	m_pkZeroFps->m_iNumOfMadRender++;
}

void MadProperty::SetBase(const char* acName)
{
	SetBasePtr(string(acName));
//	SetBasePtr(m_pkZeroFps->GetMADPtr(acName));
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

	g_ZFObjSys.Logf("net", "Mad: %s, Scale: %f", temp, scale);
	
}

void MadProperty::PackTo(NetPacket* pkNetPacket)
{
	pkNetPacket->Write_Str(m_kMadFile.c_str());
	pkNetPacket->Write(m_fScale);
}
 
void MadProperty::PackFrom(NetPacket* pkNetPacket)
{
	char temp[50];
	pkNetPacket->Read_Str(temp);
	SetBase(temp);

	pkNetPacket->Read(m_fScale);
}


vector<PropertyValues> MadProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);

	kReturn[0].kValueName = "m_fScale";
	kReturn[0].iValueType = VALUETYPE_FLOAT;
	kReturn[0].pkValue    = (void*)&m_fScale;

	kReturn[1].kValueName = "m_kMadFile";
	kReturn[1].iValueType = VALUETYPE_STRING;
	kReturn[1].pkValue    = (void*)&m_kMadFile;

//	string	m_kMadFile;

	return kReturn;
}

bool MadProperty::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_kMadFile") == 0) {
		SetBase(kValue.c_str());
		return true;
		}

	return false;
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
