#include "p_linktojoint.h"
#include "p_mad.h"
#include "../../engine/entity.h"
#include "../../render/render.h"
#include "../../basic/zfsystem.h"

P_LinkToJoint::P_LinkToJoint() 
{
	strcpy(m_acName,"P_LinkToJoint");		
	m_iType = PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;

	SetJoint("joint0");
	
	bNetwork = true;
}

P_LinkToJoint::~P_LinkToJoint()	
{	
	m_pkObject->SetInterpolate(true);
}
void P_LinkToJoint::Init()			
{	
	//turn off interpolation of current objects mad
	m_pkObject->SetInterpolate(false);
}

void P_LinkToJoint::Update() 
{
	P_Mad* pkMad = dynamic_cast<P_Mad*>(m_pkObject->GetParent()->GetProperty("P_Mad"));
	if(!pkMad)
		return;

	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return;



 	pkCore->SetBoneAnimationTime(pkMad->iActiveAnimation, pkMad->fCurrentTime,pkMad->m_bLoop);
	pkCore->SetupBonePose();
	
	
	//dvoid ultra hax deluxe  ..dont mess whit this code
	Matrix4 kMat;
	Vector3 kPos;
	kMat = pkCore->GetBoneTransform(pkCore->GetJointID(m_strToJoint.c_str()));
	kPos = kMat.GetPos() * pkMad->m_fScale;
	kMat.SetPos(Vector3(0,0,0));
	kMat *= m_pkObject->GetParent()->GetWorldRotM();	//add parents rotation , cos where not using realtive orientation anymore
	m_pkObject->SetLocalRotM(kMat);
	
	kPos = m_pkObject->GetParent()->GetWorldRotM().VectorRotate(kPos);	//apply object rotation to joint offset
	kPos += m_pkObject->GetParent()->GetIWorldPosV();							//apply interpolatet parent position 
	
	m_pkObject->SetLocalPosV(kPos);
}

vector<PropertyValues> P_LinkToJoint::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
	
	kReturn[0].kValueName = "m_strToJoint";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_strToJoint;

	return kReturn;
}

bool P_LinkToJoint::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_strToJoint") == 0) {
		SetJoint(kValue.c_str());
		return true;
	}
	
	return false;
}


void P_LinkToJoint::Save(ZFIoInterface* pkPackage)
{	
	char temp[50];
	strcpy(temp,m_strToJoint.c_str());

	pkPackage->Write((void*)temp,50,1);
}

void P_LinkToJoint::Load(ZFIoInterface* pkPackage)
{
	char temp[50];
	pkPackage->Read((void*)temp,50,1);	
	m_strToJoint = temp;
}

void P_LinkToJoint::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write_Str(m_strToJoint.c_str());
	
	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_LinkToJoint::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	char temp[128];
	pkNetPacket->Read_Str(temp);
	SetJoint(temp);
	
	
}

void P_LinkToJoint::SetJoint(const char* czJoint)
{
	if(m_strToJoint == czJoint)
		return;

	m_strToJoint = czJoint;

	SetNetUpdateFlag(true);
}

Property* Create_LinkToJoint()
{
	return new P_LinkToJoint;
}
