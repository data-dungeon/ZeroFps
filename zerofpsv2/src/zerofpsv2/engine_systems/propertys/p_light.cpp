#pragma warning( disable : 4786 )

#include "p_light.h"
#include "../../engine/entity.h"
#include "../../basic/zfsystem.h"

 
P_Light::P_Light()
{
	m_pkLight	= 			static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light"));
	m_pkZeroFps = 			static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	
	strcpy(m_acName,"P_Light");
	m_bNetwork = true;
	m_iVersion = 5;
	m_iSortPlace=10;
	
	m_pkLightSource=new LightSource();

	m_iType = PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_CLIENT;
 
	m_iMode  = 0;
	m_fTimer = 0;
	m_kOffset.Set(0,0,0);
	m_fFlareSize = 0.0;
	
	m_pkMaterial = new ZFResourceHandle;
	SetMaterial("data/material/flare-white.zmt");
	
}

P_Light::~P_Light()
{
	TurnOff();
	delete m_pkLightSource;
	delete m_pkMaterial;
}

void P_Light::Init()
{
	TurnOn();

}

void P_Light::Update() 
{
	if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL))
	{
// 		m_pkLightSource->kPos = m_pkEntity->GetWorldPosV() + m_pkEntity->GetWorldRotM().VectorTransform(m_kOffset);	
		m_pkLightSource->kPos = m_pkEntity->GetIWorldPosV() + m_pkEntity->GetWorldRotM().VectorTransform(m_kOffset);	
	
		if(m_pkLightSource->iType == SPOT_LIGHT)	
			m_pkLightSource->kRot = m_pkEntity->GetWorldRotM().VectorTransform(Vector3(0,0,1));

		UpdateLightMode();		
		
		if((m_pkLightSource->iType == POINT_LIGHT && m_fFlareSize > 0) || (m_pkZeroFps->GetDebugGraph()))
			m_iType = PROPERTY_TYPE_RENDER|PROPERTY_TYPE_NORMAL;
		else
			m_iType = PROPERTY_TYPE_NORMAL;
		
	}
	
	if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER) && m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_NOSHADOWLAST)
	{
		m_pkLightSource->kPos = m_pkEntity->GetIWorldPosV() + m_pkEntity->GetWorldRotM().VectorTransform(m_kOffset);			
		DrawFlare();
	
	
	  	if(m_pkZeroFps->GetDebugGraph())
		{
			m_pkRender->Sphere(m_pkEntity->GetIWorldPosV(),0.3,1,Vector3(1,1,0),false);
		}
	}

	
}

void P_Light::DrawFlare()
{
	//Distance culling
	if(m_pkZeroFps->GetCam()->GetRenderPos().DistanceTo(m_pkLightSource->kPos) > m_pkZeroFps->GetViewDistance())
		return;

	if(!m_pkZeroFps->GetCam()->GetFrustum()->PointInFrustum(m_pkLightSource->kPos))
		return;

	static ZMaterial* pkTestMat=NULL;
	
	if(!pkTestMat)
	{
		pkTestMat = new ZMaterial;
	
		pkTestMat->GetPass(0)->m_kTUs[0]->SetRes("data/textures/clear.tga");
		pkTestMat->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
		pkTestMat->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;
		pkTestMat->GetPass(0)->m_bDepthMask = 				false;
		pkTestMat->GetPass(0)->m_bBlend = 					true;	
		pkTestMat->GetPass(0)->m_iBlendSrc = 				SRC_ALPHA_BLEND_SRC;
		pkTestMat->GetPass(0)->m_iBlendDst = 				ONE_MINUS_SRC_ALPHA_BLEND_DST;	
	}


		
	if(m_pkZShaderSystem->SupportOcculusion())
	{
		//do occulusion test	
		m_pkZShaderSystem->OcculusionBegin();
		
		m_pkRender->DrawBillboardQuad(m_pkZeroFps->GetCam()->GetRotM(),m_pkLightSource->kPos,0.1,pkTestMat);		
		//m_pkRender->Line(m_pkLightSource->kPos-0.02,m_pkLightSource->kPos+0.02,Vector3(1,1,1));		
		int iSamples = m_pkZShaderSystem->OcculusionEnd();
		
		if(iSamples > 0 )
		{
			m_pkRender->DrawBillboardQuad(m_pkZeroFps->GetCam()->GetRotM(),m_pkLightSource->kPos,m_fFlareSize,(ZMaterial*)(m_pkMaterial->GetResourcePtr()));		
		}
	}		
}


void P_Light::UpdateLightMode()
{
	switch(m_iMode)
	{		
		case 1:
		{
			if(m_pkZeroFps->GetEngineTime() - m_fTimer > 0.05)
			{
				m_fTimer = m_pkZeroFps->GetEngineTime();
				//float fRandLight = float( ((rand() % 1000)/400000.0) ) - float( ((rand() % 1000)/400000.0) );
	
				m_pkLightSource->fQuadratic_Atten = 0.05 + Randomf(0.01);
				m_pkLightSource->kDiffuse = Vector3(1,0.8,0.7).Unit();//Vector3(float(fRandLight + 0.04), float(fRandLight + 0.03466) , float(fRandLight + 0.03)).Unit();
				m_pkLightSource->kSpecular = m_pkLightSource->kDiffuse;
				
			}
			break;		
		}		
				
		case 2:
		{
			if(m_pkZeroFps->GetEngineTime() - m_fTimer > 0.05)
			{
				m_fTimer = m_pkZeroFps->GetEngineTime();
				//float fRandLight = float( ((rand() % 1000)/400000.0) ) - float( ((rand() % 1000)/400000.0) );
	
				m_pkLightSource->fQuadratic_Atten = 0.3 + Randomf(0.005);
				m_pkLightSource->kDiffuse = Vector3(1,0.8,0.7).Unit();//Vector3(float(fRandLight + 0.04), float(fRandLight + 0.03466) , float(fRandLight + 0.03)).Unit();
				m_pkLightSource->kSpecular = m_pkLightSource->kDiffuse;
				
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
	pkNetPacket->Write(m_fFlareSize);
	pkNetPacket->Write_Str(m_strMaterial);
	
																						
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
	pkNetPacket->Read(m_fFlareSize);
	
	string strFlareMaterial;
	pkNetPacket->Read_Str(strFlareMaterial);
	SetMaterial(strFlareMaterial);
	
	
}

void P_Light::SetMaterial(const string& strMaterial)
{
	m_pkMaterial->SetRes(strMaterial);
	m_strMaterial = strMaterial;

	SetNetUpdateFlag(true);
}


vector<PropertyValues> P_Light::GetPropertyValues()
{
	vector<PropertyValues> kReturn(14);

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
	
	kReturn[12].kValueName = "FlareSize";
	kReturn[12].iValueType = VALUETYPE_FLOAT;
	kReturn[12].pkValue    = (void*)&m_fFlareSize;
				
	kReturn[13].kValueName = "FlareMaterial";
	kReturn[13].iValueType = VALUETYPE_STRING;
	kReturn[13].pkValue    = (void*)&m_strMaterial;
	
	
	return kReturn;
}

bool P_Light::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(kValueName == "FlareMaterial")
	{
		SetMaterial(kValue);
		return true;
	}
	
	if( kValueName == "FlareSize") 
	{
		SetNetUpdateFlag(true);
		return false;
	}

	return false;
}

void P_Light::Save(ZFIoInterface* pkPackage)
{	
// 	pkPackage->Write(*m_pkLightSource);
	pkPackage->Write( m_pkLightSource->kPos);
	pkPackage->Write( m_pkLightSource->kRot);				
	pkPackage->Write( m_pkLightSource->kDiffuse );		
	pkPackage->Write( m_pkLightSource->kAmbient);			
	pkPackage->Write( m_pkLightSource->kSpecular );						
	pkPackage->Write( m_pkLightSource->fCutoff);		
	pkPackage->Write( m_pkLightSource->fExp);					
	pkPackage->Write( m_pkLightSource->fConst_Atten);		
	pkPackage->Write( m_pkLightSource->fLinear_Atten);		
	pkPackage->Write( m_pkLightSource->fQuadratic_Atten);				
	pkPackage->Write( m_pkLightSource->iType);	
	pkPackage->Write( m_pkLightSource->iPriority);	


	pkPackage->Write(m_iMode);		
	pkPackage->Write(m_kOffset);		
	pkPackage->Write(m_fFlareSize);		
	
	pkPackage->Write_Str(m_strMaterial);		
}

void P_Light::Load(ZFIoInterface* pkPackage,int iVersion)
{
	switch(iVersion)
	{
		case 1:
			pkPackage->Read(*m_pkLightSource);
			pkPackage->Read(m_iMode);		
			break;
			
		case 2:
			pkPackage->Read(*m_pkLightSource);
			pkPackage->Read(m_iMode);		
			pkPackage->Read(m_kOffset);	
			break;
			
		case 3:
			pkPackage->Read(*m_pkLightSource);
			pkPackage->Read(m_iMode);		
			pkPackage->Read(m_kOffset);	
			pkPackage->Read(m_fFlareSize);	
			break;	
			
		case 4:
		{
// 			pkPackage->Read(*m_pkLightSource);
			
			pkPackage->Read( m_pkLightSource->kPos);
			pkPackage->Read( m_pkLightSource->kRot);
			pkPackage->Read( m_pkLightSource->kDiffuse );		
			pkPackage->Read( m_pkLightSource->kAmbient);			
			pkPackage->Read( m_pkLightSource->kSpecular );						
			pkPackage->Read( m_pkLightSource->fCutoff);		
			pkPackage->Read( m_pkLightSource->fExp);				
			pkPackage->Read( m_pkLightSource->fConst_Atten);		
			pkPackage->Read( m_pkLightSource->fLinear_Atten);		
			pkPackage->Read( m_pkLightSource->fQuadratic_Atten);		
			pkPackage->Read( m_pkLightSource->iType);	
			pkPackage->Read( m_pkLightSource->iPriority);	
			pkPackage->Read( m_pkLightSource->fIntensity);	
			
			pkPackage->Read(m_iMode);		
			pkPackage->Read(m_kOffset);	
			pkPackage->Read(m_fFlareSize);	
			
			string strTemp;
			pkPackage->Read_Str(strTemp);	
			SetMaterial(strTemp);
			break;
		}	
		case 5:
		{
			pkPackage->Read( m_pkLightSource->kPos);
			pkPackage->Read( m_pkLightSource->kRot);				
			pkPackage->Read( m_pkLightSource->kDiffuse );		
			pkPackage->Read( m_pkLightSource->kAmbient);			
			pkPackage->Read( m_pkLightSource->kSpecular );						
			pkPackage->Read( m_pkLightSource->fCutoff);		
			pkPackage->Read( m_pkLightSource->fExp);					
			pkPackage->Read( m_pkLightSource->fConst_Atten);		
			pkPackage->Read( m_pkLightSource->fLinear_Atten);		
			pkPackage->Read( m_pkLightSource->fQuadratic_Atten);				
			pkPackage->Read( m_pkLightSource->iType);	
			pkPackage->Read( m_pkLightSource->iPriority);				
			
			pkPackage->Read(m_iMode);		
			pkPackage->Read(m_kOffset);	
			pkPackage->Read(m_fFlareSize);	
			
			string strTemp;
			pkPackage->Read_Str(strTemp);	
			SetMaterial(strTemp);
			break;
		}
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


Property* Create_LightProperty()
{
	return new P_Light;
}





