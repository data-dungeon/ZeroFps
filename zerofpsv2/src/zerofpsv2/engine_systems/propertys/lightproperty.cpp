#pragma warning( disable : 4786 )

#include "lightproperty.h"
#include "../../engine/object.h"
#include "../../basic/zfsystem.h"
 
LightProperty::LightProperty()
{
	bNetwork = true;
	strcpy(m_acName,"LightProperty");
	
	m_pkLightSource=new LightSource();
	//pkPos=new Vector3(0,0,0);
	//pkRot=new Vector3(0,0,0);
	m_pkLightSource->kPos=&kPos;
	m_pkLightSource->kRot=&kRot;
	
/*	
	spot->kDiffuse=Vector4(0,0,0,1.0);
	spot->kAmbient=Vector4(0.0,0.0,0.0,1.0);
	spot->iType=POINT_LIGHT;			
	spot->iPriority=0;
	spot->fCutoff=20;	
	spot->fExp=5;
	spot->fConst_Atten=1;
	spot->fLinear_Atten=0;
	spot->fQuadratic_Atten=0;
*/

	m_pkLight = static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light"));

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	TurnOn();
}

LightProperty::~LightProperty()
{
	m_pkLight->Remove(m_pkLightSource);
	delete m_pkLightSource;
	//delete pkPos;
	//delete pkRot;
}

void LightProperty::Init()
{
//	m_pkLightSource->kPos= m_pkObject->GetPosPointer();
//	m_pkLightSource->kRot= m_pkObject->GetRotPointer();
}

void LightProperty::Update() 
{
	kPos = m_pkObject->GetWorldPosV(); 
	kRot = m_pkObject->GetWorldRotV();

	//m_pkLightSource->kPos->Set(pos.x,pos.y,pos.z);
	//m_pkLightSource->kRot->Set(rot.x,rot.y,rot.z);	
}

void LightProperty::PackTo( NetPacket* pkNetPacket ) {
	pkNetPacket->Write( m_pkLightSource->kDiffuse);
	pkNetPacket->Write( m_pkLightSource->kAmbient);
	pkNetPacket->Write( m_pkLightSource->iPriority);	
	pkNetPacket->Write( m_pkLightSource->fCutoff);		
	pkNetPacket->Write( m_pkLightSource->fExp);		
	pkNetPacket->Write( m_pkLightSource->fConst_Atten);		
	pkNetPacket->Write( m_pkLightSource->fLinear_Atten);		
	pkNetPacket->Write( m_pkLightSource->fQuadratic_Atten);		
	pkNetPacket->Write( m_pkLightSource->kConstPos);			
}

void LightProperty::PackFrom( NetPacket* pkNetPacket ) {
	pkNetPacket->Read( m_pkLightSource->kDiffuse );		
	pkNetPacket->Read( m_pkLightSource->kAmbient);
	pkNetPacket->Read( m_pkLightSource->iPriority);	
	pkNetPacket->Read( m_pkLightSource->fCutoff);		
	pkNetPacket->Read( m_pkLightSource->fExp);		
	pkNetPacket->Read( m_pkLightSource->fConst_Atten);		
	pkNetPacket->Read( m_pkLightSource->fLinear_Atten);		
	pkNetPacket->Read( m_pkLightSource->fQuadratic_Atten);			
	pkNetPacket->Read( m_pkLightSource->kConstPos );			
}

vector<PropertyValues> LightProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(12);

	kReturn[0].kValueName = "Ambient";
	kReturn[0].iValueType = VALUETYPE_VECTOR4;
	kReturn[0].pkValue    = (void*)&m_pkLightSource->kAmbient;
	
	kReturn[1].kValueName = "Diffuse";
	kReturn[1].iValueType = VALUETYPE_VECTOR4;
	kReturn[1].pkValue    = (void*)&m_pkLightSource->kDiffuse;

	kReturn[2].kValueName = "Specular";
	kReturn[2].iValueType = VALUETYPE_VECTOR4;
	kReturn[2].pkValue    = (void*)&m_pkLightSource->kSpecular;
	
	kReturn[3].kValueName = "Cutoff";
	kReturn[3].iValueType = VALUETYPE_FLOAT;
	kReturn[3].pkValue    = (void*)&m_pkLightSource->fCutoff;

	kReturn[4].kValueName = "Exp";
	kReturn[4].iValueType = VALUETYPE_FLOAT;
	kReturn[4].pkValue    = (void*)&m_pkLightSource->fExp;

	kReturn[5].kValueName = "Const_Atten";
	kReturn[5].iValueType = VALUETYPE_FLOAT;
	kReturn[5].pkValue    = (void*)&m_pkLightSource->fConst_Atten;

	kReturn[6].kValueName = "Linear_Atten";
	kReturn[6].iValueType = VALUETYPE_FLOAT;
	kReturn[6].pkValue    = (void*)&m_pkLightSource->fLinear_Atten;

	kReturn[7].kValueName = "Quadratic_Atten";
	kReturn[7].iValueType = VALUETYPE_FLOAT;
	kReturn[7].pkValue    = (void*)&m_pkLightSource->fQuadratic_Atten;
	
	kReturn[8].kValueName = "Priority";
	kReturn[8].iValueType = VALUETYPE_INT;
	kReturn[8].pkValue    = (void*)&m_pkLightSource->iPriority;
	
	kReturn[9].kValueName = "Type";
	kReturn[9].iValueType = VALUETYPE_INT;
	kReturn[9].pkValue    = (void*)&m_pkLightSource->iType;
	
	kReturn[10].kValueName = "ConstPosition";
	kReturn[10].iValueType = VALUETYPE_VECTOR3;
	kReturn[10].pkValue    = (void*)&m_pkLightSource->kConstPos;
	
	kReturn[11].kValueName = "ConstRotation";
	kReturn[11].iValueType = VALUETYPE_VECTOR3;
	kReturn[11].pkValue    = (void*)&m_pkLightSource->kConstRot;
	return kReturn;
}

Property* Create_LightProperty()
{
	return new LightProperty;
}


void LightProperty::Save(ZFIoInterface* pkPackage)
{
	
	pkPackage->Write((void*)m_pkLightSource,sizeof(LightSource),1);
	//pkPackage->Write((void*)pkPos, sizeof(Vector3));
	//pkPackage->Write((void*)pkRot, sizeof(Vector3));
	//pkPackage->Write((void*)&m_pkLight,sizeof(Light));
}

void LightProperty::Load(ZFIoInterface* pkPackage)
{
	pkPackage->Read((void*)m_pkLightSource,sizeof(LightSource),1);
	//pkPackage->Read((void*)pkPos, sizeof(Vector3));
	//pkPackage->Read((void*)pkRot,sizeof(Vector3));
	//pkPackage->Read((void*)&m_pkLight,sizeof(Light));
	


	//m_pkLightSource->kPos= m_pkObject->GetPosPointer();
	//pkLightSource->kRot= m_pkObject->GetRotPointer();
	//delete pkPos;
	//delete pkRot;
	//cout<<"m_pkObject->GetPos():" <<m_pkObject->GetPos().x <<endl;
}

void LightProperty::TurnOn()
{
	m_pkLight->Add(m_pkLightSource);
}

void LightProperty::TurnOff()
{
	m_pkLight->Remove(m_pkLightSource);
}



void LightProperty::HandleGameMessage(GameMessage& Msg)
{
	if(Msg.m_Name == "on")
		TurnOn();
	if(Msg.m_Name == "off")
		TurnOff();


	cout << "[LightProperty]: Recv GM: " << m_acName << ", ";
	cout << Msg.m_ToObject << ", ";
	cout << Msg.m_FromObject << ", ";
	cout << Msg.m_Name;
	cout << endl;
	
}

