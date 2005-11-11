#include "p_linktojoint.h"
#include "p_mad.h"
#include "../../engine/entity.h"
#include "../../render/render.h"
#include "../../basic/zfsystem.h"

P_LinkToJoint::P_LinkToJoint() : Property("P_LinkToJoint")
{	
	m_iType = PROPERTY_TYPE_RENDER|PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_CLIENT;

	m_iVersion = 2;
	
	SetJoint("joint0");
	
	m_iLinkEntityID = -1;
	m_bNetwork = true;
	
	m_iLastFrame = 0;
	
	m_kPropertyValues.push_back(PropertyValues("m_strToJoint",VALUETYPE_STRING,(void*)&m_strToJoint));
}

P_LinkToJoint::~P_LinkToJoint()	
{	
	m_pkEntity->SetInterpolate(true);
}
void P_LinkToJoint::Init()			
{	
	
}

void P_LinkToJoint::Update() 
{
	//this makes sure we dont update when not needed
	if(m_iLastFrame == m_pkZeroFps->GetCurrentFrame())
		return;
	m_iLastFrame = m_pkZeroFps->GetCurrentFrame();

	static Vector3 kPos;
	static Matrix4 kRot;

	Entity* pkHost=NULL;
	
	//get host object
	if(m_iLinkEntityID == -1)
		pkHost = GetEntity()->GetParent();
	else		
		pkHost = m_pkEntityManager->GetEntityByID(m_iLinkEntityID);
	
	
	if(pkHost)
	{
		if(P_Mad* pkMad = (P_Mad*)pkHost->GetProperty("P_Mad"))
		{	
			kPos = pkHost->GetIWorldPosV() + pkMad->GetJointPosition(m_strToJoint);				
			kRot = pkMad->GetJointRotation(m_strToJoint);
			
			m_pkEntity->SetWorldPosV(kPos);
			m_pkEntity->SetLocalRotM(kRot);
			m_pkEntity->SetInterpolate(false);
				
		}
	}		
}


bool P_LinkToJoint::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(strcmp(kValueName.c_str(), "m_strToJoint") == 0) {
		SetJoint(kValue.c_str());
		return true;
	}
	
	return false;
}


void P_LinkToJoint::Save(ZFIoInterface* pkPackage)
{	
	pkPackage->Write_Str(m_strToJoint);
	pkPackage->Write(m_iLinkEntityID);
	
}

void P_LinkToJoint::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 2)
	{
		pkPackage->Read_Str(m_strToJoint);
		pkPackage->Read(m_iLinkEntityID);
	}
	else
	{
		char temp[50];
		pkPackage->Read((void*)temp,50,1);	
		m_strToJoint = temp;
	}
}

void P_LinkToJoint::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{

	pkNetPacket->Write(m_iLinkEntityID);
	pkNetPacket->Write_Str(m_strToJoint);
	
	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_LinkToJoint::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Read(m_iLinkEntityID);

	string temp;
	pkNetPacket->Read_Str(temp);
	SetJoint(temp.c_str());
	
	
}

void P_LinkToJoint::SetJoint(const string& strJoint)
{
	if(m_strToJoint == strJoint)
		return;

	m_strToJoint = strJoint;

	SetNetUpdateFlag(true);
}

void P_LinkToJoint::SetLinkEntity(int iID)
{
	if(m_iLinkEntityID == iID)
		return;

	m_iLinkEntityID = iID;
	SetNetUpdateFlag(true);
}

Property* Create_LinkToJoint()
{
	return new P_LinkToJoint;
}
