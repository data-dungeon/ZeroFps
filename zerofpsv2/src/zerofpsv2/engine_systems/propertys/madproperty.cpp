#include "madproperty.h"
#include "../../engine/object.h"
#include "../../render/render.h"
#include "../../basic/zfsystem.h"
 
extern int g_iMadLODLock;
extern float g_fMadLODScale;
 
MadProperty::MadProperty()
{
	m_pkZeroFps  = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	strcpy(m_acName,"MadProperty");
	bNetwork	 = true;

	m_iType = PROPERTY_TYPE_RENDER | PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;
	
	m_bIsVisible = true;
	m_fScale	 = 1.0;
}

void MadProperty::Update() 
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return;

	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_NORMAL ) /* &  m_pkObjMan->IsUpdate(PROPERTY_SIDE_SERVER)*/ ) {
		UpdateAnimation(m_pkZeroFps->GetGameFrameTime());
		return;
		}
	
	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER) ) {
		if(!m_bIsVisible)
			return;
		
		if(!m_pkZeroFps->GetCam()->m_kFrustum.SphereInFrustum(m_pkObject->GetWorldPosV(),GetRadius()))
			return;
		
		// Set Object LOD.
		if(g_iMadLODLock == 0) {
			Vector3 kDiff = m_pkZeroFps->GetCam()->GetPos() - m_pkObject->GetWorldPosV();
			float fDist = float( fabs(kDiff.Length()) );
			m_fLod = 1 - (fDist / 300);
			//cout << "fDist: " << fDist << " / " << "m_fLod: " << m_fLod << endl;
			
			
			//dvoid yber loding deluxe
			float blub = GetRadius() / fDist;		
			if(blub < 0.010)
				return;
		}

		g_fMadLODScale = m_fLod;
		Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render")); 

		glPushMatrix();
			Matrix4 ori = m_pkObject->GetWorldOriM();
			glMultMatrixf(&ori[0]);
			glScalef(m_fScale, m_fScale, m_fScale);
			Draw_All(m_pkZeroFps->m_iMadDraw);
		glPopMatrix();

		if(m_pkZeroFps->m_iMadDraw & MAD_DRAW_SPHERE) {
			glPushMatrix();
				glTranslatef(m_pkObject->GetWorldPosV().x,m_pkObject->GetWorldPosV().y,m_pkObject->GetWorldPosV().z);
				glRotatef(90 ,1,0,0);
				pkRender->DrawBoundSphere(GetRadius(),Vector3::ZERO);		
			glPopMatrix();
		}

		m_pkZeroFps->m_iNumOfMadRender++;
	
		return;
		}
}

void MadProperty::SetBase(const char* acName)
{
	SetBasePtr(string(acName));
	m_iNetUpdateFlags = 1;
}

void MadProperty::Save(ZFIoInterface* pkPackage)
{	
	char temp[50];
	strcpy(temp,m_kMadFile.c_str());

	pkPackage->Write((void*)temp,50,1);
	pkPackage->Write((void*)&m_fScale,4,1);
}

void MadProperty::Load(ZFIoInterface* pkPackage)
{
	char temp[50];
	pkPackage->Read((void*)temp,50,1);	
	SetBase(temp);
	
	float scale;
	pkPackage->Read((void*)&scale,4,1);
	SetScale(scale);
	
	//update object radius
	m_pkObject->GetRadius() = GetRadius();
}

void MadProperty::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write_NetStr(m_kMadFile.c_str());
	pkNetPacket->Write( m_fScale );
	pkNetPacket->Write( iActiveAnimation );
	
	m_iNetUpdateFlags = 0;
}
 
void MadProperty::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	char temp[50];
	pkNetPacket->Read_NetStr(temp);
	SetBase(temp);
	pkNetPacket->Read(m_fScale);

	int iNewAnim;
	pkNetPacket->Read( iNewAnim );
	if(iNewAnim != iActiveAnimation)
		PlayAnimation(iNewAnim, 0);

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

	return kReturn;
}

bool MadProperty::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_kMadFile") == 0) {
		SetBase(kValue.c_str());
		return true;
	}
	
	if(strcmp(kValueName.c_str(), "m_fScale") == 0) {
		m_fScale = atof(kValue.c_str());
		m_pkObject->GetRadius() = GetRadius();
		return true;
	}

	return false;
}

Property* Create_MadProperty()
{
	return new MadProperty;
}




LinkToJoint::LinkToJoint() 
{
	strcpy(m_acName,"LinkToJoint");		
	m_iType = PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;

	m_strToJoint = "joint0";
}

LinkToJoint::~LinkToJoint()		{ }
void LinkToJoint::Init()		{ }

void LinkToJoint::Update() 
{
	MadProperty* pkMad = dynamic_cast<MadProperty*>(m_pkObject->GetParent()->GetProperty("MadProperty"));
	if(!pkMad)
		return;

	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return;

 	pkCore->SetBoneAnimationTime(pkMad->iActiveAnimation, pkMad->fCurrentTime,pkMad->m_bLoop);
	pkCore->SetupBonePose();
	
	/*
		Vector3 kPos = pkCore->GetBonePosition(pkCore->GetJointID("joint12"));	// s_Goblin_joint12
		cout << "Joint Pos: <" << kPos.x << "," << kPos.y << "," << kPos.z << ">" << endl;
		m_pkObject->SetLocalPosV(kPos);
	*/

	Matrix4 kMat;
	Vector3 kPos;
	kMat = pkCore->GetBoneTransform(pkCore->GetJointID(m_strToJoint.c_str()));
	kPos = kMat.GetPos() * pkMad->m_fScale;
	kMat.SetPos(Vector3(0,0,0));
	m_pkObject->SetLocalRotM(kMat);
	m_pkObject->SetLocalPosV( kPos );
}

vector<PropertyValues> LinkToJoint::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
	
	kReturn[0].kValueName = "m_strToJoint";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_strToJoint;

	return kReturn;
}

void LinkToJoint::Save(ZFIoInterface* pkPackage)
{	
	char temp[50];
	strcpy(temp,m_strToJoint.c_str());

	pkPackage->Write((void*)temp,50,1);
}

void LinkToJoint::Load(ZFIoInterface* pkPackage)
{
	char temp[50];
	pkPackage->Read((void*)temp,50,1);	
	m_strToJoint = temp;

}

Property* Create_LinkToJoint()
{
	return new LinkToJoint;
}




