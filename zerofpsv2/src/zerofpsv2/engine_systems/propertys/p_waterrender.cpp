#include "p_waterrender.h"
 
P_WaterRender::P_WaterRender() : Property("P_WaterRender")
{
	m_pkTexMan=				static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps=			static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));		
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));		
	m_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkLight=				static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight")); 
	
 	m_iType = PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_CLIENT;
	
	m_iVersion = 2;

	m_iSortPlace	=	10;
	m_fBlendValue	=	0.7;
	m_bBlendDirUp	=  false;

	m_bNetwork	=	true;
	
	
	m_kSize.Set(0.5,0.5,0.5);
	m_iStep = 1;
	m_fWave = 0.1;
	
	m_pkMaterial = new ZFResourceHandle;
	SetMaterial("water.zmt");	




	m_kPropertyValues.push_back(PropertyValues("size",VALUETYPE_VECTOR3,(void*)&m_kSize));
	m_kPropertyValues.push_back(PropertyValues("step",VALUETYPE_INT,(void*)&m_iStep));
	m_kPropertyValues.push_back(PropertyValues("material",VALUETYPE_STRING,(void*)&m_strMaterial));
	m_kPropertyValues.push_back(PropertyValues("wave",VALUETYPE_FLOAT,(void*)&m_fWave));

}

void P_WaterRender::Init()
{
	GetEntity()->SetLocalAABB(-m_kSize,m_kSize);
}

void P_WaterRender::SetMaterial(const string& strMaterial)
{
	m_pkMaterial->SetRes(strMaterial);
	m_strMaterial = strMaterial;

	SetNetUpdateFlag(true);
}


void P_WaterRender::Update() 
{	
	if(m_pkZeroFps->GetCam()->GetCurrentRenderMode() != RENDER_SHADOWED)
		return;
		


	static Vector3 kPos;
	static Vector3 kMin;
	static Vector3 kMax;
	
	kPos = m_pkEntity->GetWorldPosV();

	kMin.Set(kPos.x-m_kSize.x,kPos.y-m_kSize.y,kPos.z-m_kSize.z);
	kMax.Set(kPos.x+m_kSize.x,kPos.y+m_kSize.y,kPos.z+m_kSize.z);
	
	
	
	//frustum culling
	if(!m_pkZeroFps->GetCam()->GetFrustum()->CubeInFrustum(kMin,kMax))
		return;			
	
	if(m_pkZeroFps->GetCam()->GetRenderPos().DistanceTo(kPos) - m_kSize.Length() > m_pkZeroFps->GetViewDistance()*2)
		return;

		
	//update light					
	m_pkLight->Update(&m_kLightProfile,kPos);	
	
	m_pkZShaderSystem->BindMaterial((ZMaterial*)(m_pkMaterial->GetResourcePtr()));
 	
	DrawSurface();

	
	

 	if(m_pkZeroFps->GetDebugGraph())
	{
		bool bUse = m_pkZShaderSystem->GetUseDefaultGLSLProgram();
		m_pkZShaderSystem->UseDefaultGLSLProgram(false);		
	
 		m_pkRender->Sphere(kPos,0.5,1,Vector3(1,1,1),true);	
		m_pkRender->DrawAABB(kMin,kMax,Vector3(1,1,1));
		
		m_pkZShaderSystem->UseDefaultGLSLProgram(bUse);
	}

}


void P_WaterRender::DrawSurface()
{
	static float fY1,fY2,fY3,fY4;
	static Vector3 kNormal = Vector3(0,1,0);
	static float t;										
	
	static Vector3 V1,V2;
	
	m_pkZShaderSystem->ClearGeometry();
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetWorldPosV());
	
	t = m_pkZeroFps->GetEngineTime();
	
	
	//first
	float xo = m_pkEntity->GetWorldPosV().x;
	float zo = m_pkEntity->GetWorldPosV().z;
	for(int x = int(-m_kSize.x);x< m_kSize.x;x+=m_iStep)
	{	
		for(int z = int(-m_kSize.z);z< m_kSize.z;z+=m_iStep)
		{
// 			fY1 = sin((x+xo)+t)   			*m_fWave  + m_kSize.y;
// 			fY2 = sin((x+xo+m_iStep)+t)	*m_fWave + m_kSize.y;
			
			fY1 = sin((x+xo)+t)*sin((z+zo)+t)   *m_fWave  + m_kSize.y;
			fY2 = sin((x+xo+m_iStep)+t)*sin((z+zo)+t)	*m_fWave + m_kSize.y;
			
			fY3 = sin((x+xo+m_iStep)+t)*sin((z+zo+m_iStep)+t)  *m_fWave  + m_kSize.y;
			fY4 = sin((x+xo)+t)*sin((z+zo+m_iStep)+t)	*m_fWave + m_kSize.y;
			
			
			
			m_pkZShaderSystem->AddQuadUV(	Vector2(x*0.1,z*0.1),
													Vector2(x*0.1+0.1,z*0.1),
													Vector2(x*0.1+0.1,z*0.1+0.1),
													Vector2(x*0.1,z*0.1+0.1));
													
			m_pkZShaderSystem->AddQuadN(	Vector3(0,fY4-fY1,1).Cross(Vector3(1,fY2-fY1,0)),
													Vector3(-1,fY1-fY2,0).Cross(Vector3(0,fY3-fY2,1)),
													Vector3(0,fY2-fY3,-1).Cross(Vector3(-1,fY4-fY3,0)),
													Vector3(1,fY3-fY4,0).Cross(Vector3(0,fY1-fY4,-1)));													
													
//  			m_pkZShaderSystem->AddQuadN(kNormal,kNormal,kNormal,kNormal);
			
			m_pkZShaderSystem->AddQuadV(	Vector3(x,fY1,z),Vector3(x+m_iStep,fY2,z),
													Vector3(x+m_iStep,fY3,z+m_iStep),Vector3(x,fY4,z+m_iStep));		
		}
	}
	m_pkZShaderSystem->DrawGeometry(QUADS_MODE);

	
	
	m_pkZShaderSystem->MatrixPop();
}

void P_WaterRender::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write(m_kSize);
	pkNetPacket->Write(m_iStep);	
	pkNetPacket->Write_Str(m_strMaterial);
	
	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_WaterRender::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Read(m_kSize);
	pkNetPacket->Read(m_iStep);
	pkNetPacket->Read_Str(m_strMaterial);	
	SetMaterial(m_strMaterial);
	GetEntity()->SetLocalAABB(-m_kSize,m_kSize);
}


void P_WaterRender::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(m_kSize);
	pkPackage->Write(m_iStep);
	pkPackage->Write(m_fWave);
	
	pkPackage->Write_Str(m_strMaterial);

}

void P_WaterRender::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 2)
	{
		pkPackage->Read(m_kSize);
		pkPackage->Read(m_iStep);
		pkPackage->Read(m_fWave);
		pkPackage->Read_Str(m_strMaterial);	
		SetMaterial(m_strMaterial);
		GetEntity()->SetLocalAABB(-m_kSize,m_kSize);
	}
	else
	{
		pkPackage->Read(m_kSize);
		pkPackage->Read(m_iStep);
		pkPackage->Read_Str(m_strMaterial);	
		SetMaterial(m_strMaterial);		
		GetEntity()->SetLocalAABB(-m_kSize,m_kSize);		
	}

}


bool P_WaterRender::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(kValueName == "material")
	{
		SetMaterial(kValue);		
		return true;
	}

	return false;
}

void P_WaterRender::HaveSetValue( const string& kValueName)
{
	if(kValueName == "size")
		GetEntity()->SetLocalAABB(-m_kSize,m_kSize);

}


Property* Create_WaterRenderProperty()
{
	return new P_WaterRender;
}




/*
vector<PropertyValues> P_WaterRender::GetPropertyValues()
{
	vector<PropertyValues> kReturn(4);
			
	kReturn[0].kValueName="size";
	kReturn[0].iValueType=VALUETYPE_VECTOR3;
	kReturn[0].pkValue=(void*)&m_kSize;
	
	kReturn[1].kValueName="step";
	kReturn[1].iValueType=VALUETYPE_INT;
	kReturn[1].pkValue=(void*)&m_iStep;
	
	kReturn[2].kValueName="material";
	kReturn[2].iValueType=VALUETYPE_STRING;
	kReturn[2].pkValue=(void*)&m_strMaterial;

	kReturn[3].kValueName="wave";
	kReturn[3].iValueType=VALUETYPE_FLOAT;
	kReturn[3].pkValue=(void*)&m_fWave;
	
	
	return kReturn;
}*/