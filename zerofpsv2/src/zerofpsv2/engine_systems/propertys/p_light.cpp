#pragma warning( disable : 4786 )

#include "p_light.h"
#include "../../engine/entity.h"
#include "../../basic/zfsystem.h"

 
P_Light::P_Light()
{
	m_pkLight	= static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light"));
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			

	strcpy(m_acName,"P_Light");
	bNetwork = true;
	m_iVersion = 2;
		
	m_pkLightSource=new LightSource();

	m_iType = PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;
 
	m_iMode  = LMODE_DEFAULT;
	m_fTimer = 0;
	m_kOffset.Set(0,0,0);
}

P_Light::~P_Light()
{
	TurnOff();
	delete m_pkLightSource;
}

void P_Light::Init()
{
	TurnOn();

}

void P_Light::Update() 
{
	//draw ball on the server
//	if(m_pkZeroFps->GetDebugGraph())
//		m_pkRender->Sphere(m_pkEntity->GetIWorldPosV(),0.1,1,Vector3(1,0,1),true);


	m_pkLightSource->kPos = m_pkEntity->GetWorldPosV() + m_pkEntity->GetWorldRotM().VectorTransform(m_kOffset);
	
	if(m_pkLightSource->iType == SPOT_LIGHT)	
		m_pkLightSource->kRot = m_pkEntity->GetWorldRotM().VectorTransform(Vector3(0,0,1));

		

	UpdateLightMode();		
}

void P_Light::UpdateLightMode()
{
	switch(m_iMode)
	{
		
		case LMODE_TORCH:
		{
			if(m_pkZeroFps->GetTicks() - m_fTimer > 0.05)
			{
				m_fTimer = m_pkZeroFps->GetTicks();
				float hora = float( ((rand() % 1000)/200000.0) );
	
				m_pkLightSource->kDiffuse = Vector3(float(hora + 0.02), float(hora + 0.02) , float(hora + 0.01) );
			}
			break;		
		}
		
	}	
}

void P_Light::PackTo( NetPacket* pkNetPacket, int iConnectionID ) 
{
	pkNetPacket->Write( m_pkLightSource->kDiffuse);						//12
	pkNetPacket->Write( m_pkLightSource->kSpecular);					//12
	pkNetPacket->Write( m_pkLightSource->kAmbient);						//12
	pkNetPacket->Write( m_pkLightSource->iPriority);					//4
	pkNetPacket->Write( m_pkLightSource->fCutoff);						//4
	pkNetPacket->Write( m_pkLightSource->fExp);							//4
	pkNetPacket->Write( m_pkLightSource->fConst_Atten);				//4
	pkNetPacket->Write( m_pkLightSource->fLinear_Atten);				//4
	pkNetPacket->Write( m_pkLightSource->fQuadratic_Atten);			//4
	pkNetPacket->Write( m_iMode);												//4
																						//sum 64 bytes
	
	pkNetPacket->Write(m_kOffset);
																						
	SetNetUpdateFlag(iConnectionID,false);
}

void P_Light::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
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
	
	pkNetPacket->Read(m_kOffset);
	
}

vector<PropertyValues> P_Light::GetPropertyValues()
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
	
	kReturn[10].kValueName = "Mode";
	kReturn[10].iValueType = VALUETYPE_INT;
	kReturn[10].pkValue    = (void*)&m_iMode;

	kReturn[11].kValueName = "Offset";
	kReturn[11].iValueType = VALUETYPE_VECTOR3;
	kReturn[11].pkValue    = (void*)&m_kOffset;
	
			
	return kReturn;
}

Property* Create_LightProperty()
{
	return new P_Light;
}

void P_Light::Save(ZFIoInterface* pkPackage)
{	
	pkPackage->Write(*m_pkLightSource);
	pkPackage->Write(m_iMode);		
	pkPackage->Write(m_kOffset);		
}

void P_Light::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 1)
	{
		pkPackage->Read(*m_pkLightSource);
		pkPackage->Read(m_iMode);		
	}
	
	if(iVersion == 2)
	{
		pkPackage->Read(*m_pkLightSource);
		pkPackage->Read(m_iMode);		
		pkPackage->Read(m_kOffset);	
	}
	
	
	SetNetUpdateFlag(true);	
}

void P_Light::TurnOn()
{
	m_pkLight->Add(m_pkLightSource);
}

void P_Light::TurnOff()
{
	m_pkLight->Remove(m_pkLightSource);
}

void P_Light::OnEvent(GameMessage& Msg)
{
	Property::OnEvent(Msg);

	if(Msg.m_Name == "on")
		TurnOn();
	if(Msg.m_Name == "off")
		TurnOff();


/*	cout << "[LightProperty]: Recv GM: " << m_acName << ", ";
	cout << Msg.m_ToObject << ", ";
	cout << Msg.m_FromObject << ", ";
	cout << Msg.m_Name;
	cout << endl;*/
	
}

