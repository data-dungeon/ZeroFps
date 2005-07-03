#include "p_linktojoint.h"
#include "p_mad.h"
#include "../../engine/entity.h"
#include "../../render/render.h"
#include "../../basic/zfsystem.h"

P_LinkToJoint::P_LinkToJoint() 
{
	strcpy(m_acName,"P_LinkToJoint");		
	m_iType = PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_CLIENT;

	m_iVersion = 2;
	
	SetJoint("joint0");
	
	m_iLinkEntityID = -1;
	m_bNetwork = true;
}

P_LinkToJoint::~P_LinkToJoint()	
{	
	m_pkEntity->SetInterpolate(true);
}
void P_LinkToJoint::Init()			
{	
	//turn off interpolation of current objects madss
	
}

void P_LinkToJoint::Update() 
{
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
			kPos = pkHost->GetIWorldPosV();
			if(pkMad->IsCulled())
			{
				m_pkEntity->SetWorldPosV(kPos);
				return;
			}
		
			kPos +=pkMad->GetJointPosition(m_strToJoint);				
			kRot = pkMad->GetJointRotation(m_strToJoint);
			
			m_pkEntity->SetWorldPosV(kPos);
			m_pkEntity->SetLocalRotM(kRot);
			m_pkEntity->SetInterpolate(false);
				
		}
	}		
		
/*			if(Mad_Core* pkCore = (Mad_Core*)pkMad->kMadHandle.GetResourcePtr())
			{
*/	
	
				//pkCore->SetBoneAnimationTime(pkMad->iActiveAnimation, pkMad->fCurrentTime,pkMad->m_bLoop);
				//pkCore->SetupBonePose();
				
				

				
				
				/*
				//dvoid ultra hax deluxe  ..dont mess whit this code
				Matrix4 kMat;
				Matrix4 kParentMat;
				Vector3 kPos;
				kMat = pkCore->GetBoneTransform(pkCore->GetJointID(m_strToJoint.c_str()));
				kPos = kMat.GetPos() * pkMad->m_fScale;
				kMat.SetPos(Vector3(0,0,0));
				kParentMat = pkHost->GetWorldRotM();
				kMat *= kParentMat;	//add parents rotation , cos where not using realtive orientation anymore
			
				Matrix3 kMat3;
				kMat3 = kMat;
				m_pkEntity->SetLocalRotM(kMat3);
				
				kMat = pkHost->GetWorldRotM();
				kPos = kMat.VectorRotate(kPos);	//apply object rotation to joint offset
				kPos += pkHost->GetIWorldPosV();							//apply interpolatet parent position 
				
				m_pkEntity->SetLocalPosV(kPos);
				*/
//			}
//		}
//	}
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
	/*
	char temp[50];
	strcpy(temp,m_strToJoint.c_str());
	pkPackage->Write((void*)temp,50,1);
	*/
	
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
