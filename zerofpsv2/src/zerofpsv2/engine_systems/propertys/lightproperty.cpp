#pragma warning( disable : 4786 )

#include "lightproperty.h"
#include "../../engine/object.h"
#include "../../basic/zfsystem.h"
 
LightProperty::LightProperty()
{
	bNetwork = true;
	strcpy(m_acName,"LightProperty");
	
	m_pkLightSource=new LightSource();

	m_pkLight = static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light"));
	m_pkZeroFps = static_cast<I_ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
 
	m_iMode=LMODE_DEFAULT;
	m_fTimer = m_pkZeroFps->GetTicks();
	
	TurnOn();
	
}

LightProperty::~LightProperty()
{
	TurnOff();
	delete m_pkLightSource;
}

void LightProperty::Init()
{

}

void LightProperty::Update() 
{
	m_pkLightSource->kPos = m_pkObject->GetWorldPosV(); 
	m_pkLightSource->kRot = m_pkObject->GetWorldRotV();
	

	switch(m_iMode)
	{
		case LMODE_TORCH:
		{
			if(m_pkZeroFps->GetTicks() - m_fTimer > 0.05)
			{
				m_fTimer = m_pkZeroFps->GetTicks();
				float hora = ((rand() % 1000))/20000.0;
	
				m_pkLightSource->kDiffuse = Vector3(hora + 0.1,hora + 0.1 ,hora + 0.03);
			}
			break;		
		}
	}
}

void LightProperty::PackTo( NetPacket* pkNetPacket ) {
	pkNetPacket->Write( m_pkLightSource->kDiffuse);
	pkNetPacket->Write( m_pkLightSource->kSpecular);	
	pkNetPacket->Write( m_pkLightSource->kAmbient);
	pkNetPacket->Write( m_pkLightSource->iPriority);	
	pkNetPacket->Write( m_pkLightSource->fCutoff);		
	pkNetPacket->Write( m_pkLightSource->fExp);		
	pkNetPacket->Write( m_pkLightSource->fConst_Atten);		
	pkNetPacket->Write( m_pkLightSource->fLinear_Atten);		
	pkNetPacket->Write( m_pkLightSource->fQuadratic_Atten);		
	pkNetPacket->Write( m_iMode);			
}

void LightProperty::PackFrom( NetPacket* pkNetPacket ) {
	pkNetPacket->Read( m_pkLightSource->kDiffuse );		
	pkNetPacket->Read( m_pkLightSource->kSpecular );			
	pkNetPacket->Read( m_pkLightSource->kAmbient);
	pkNetPacket->Read( m_pkLightSource->iPriority);	
	pkNetPacket->Read( m_pkLightSource->fCutoff);		
	pkNetPacket->Read( m_pkLightSource->fExp);		
	pkNetPacket->Read( m_pkLightSource->fConst_Atten);		
	pkNetPacket->Read( m_pkLightSource->fLinear_Atten);		
	pkNetPacket->Read( m_pkLightSource->fQuadratic_Atten);			
	pkNetPacket->Read( m_iMode);				
}

vector<PropertyValues> LightProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(11);

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
	
	kReturn[10].kValueName = "Mode";
	kReturn[10].iValueType = VALUETYPE_INT;
	kReturn[10].pkValue    = (void*)&m_iMode;
	
	
	return kReturn;
}

Property* Create_LightProperty()
{
	return new LightProperty;
}


void LightProperty::Save(ZFIoInterface* pkPackage)
{
	
	pkPackage->Write((void*)m_pkLightSource,sizeof(LightSource),1);
	pkPackage->Write((void*)&m_iMode,sizeof(m_iMode),1);	
}

void LightProperty::Load(ZFIoInterface* pkPackage)
{
	pkPackage->Read((void*)m_pkLightSource,sizeof(LightSource),1);
	pkPackage->Read((void*)&m_iMode,sizeof(m_iMode),1);	

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

