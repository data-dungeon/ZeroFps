#include "p_waterrender.h"
 
P_WaterRender::P_WaterRender()
{
	strcpy(m_acName,"P_WaterRender");		

	m_pkTexMan=				static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps=			static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		
	m_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkLight=				static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light")); 
	
 	m_iType = PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_CLIENT;
	

	m_iSortPlace	=	2;
	m_fBlendValue	=	0.7;
	m_bBlendDirUp	=  false;

	m_bNetwork	=	true;
	
	
	m_kSize.Set(0.5,0.5,0.5);
	
	m_pkMaterial = new ZFResourceHandle;
	SetMaterial("data/material/water.zmt");	
}

void P_WaterRender::SetMaterial(const string& strMaterial)
{
	m_pkMaterial->SetRes(strMaterial);
	m_strMaterial = strMaterial;

	SetNetUpdateFlag(true);
}


void P_WaterRender::Update() 
{	
	if(m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_SHADOWMAP)
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
	
		
	//update light					
	m_pkLight->Update(&m_kLightProfile,kPos);	
	
	m_pkZShaderSystem->BindMaterial((ZMaterial*)(m_pkMaterial->GetResourcePtr()));
	m_pkRender->DrawAABB(kMin,kMax);


 	if(m_pkZeroFps->GetDebugGraph())
 		m_pkRender->Sphere(kPos,0.5,1,Vector3(1,1,1),true);	


}

void P_WaterRender::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write(m_kSize);
	pkNetPacket->Write(m_iStep);	
	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_WaterRender::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Read(m_kSize);
	pkNetPacket->Read(m_iStep);
}


void P_WaterRender::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(m_kSize);
	pkPackage->Write(m_iStep);
	
	pkPackage->Write_Str(m_strMaterial);

}

void P_WaterRender::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read(m_kSize);
	pkPackage->Read(m_iStep);

	pkPackage->Read_Str(m_strMaterial);
	
	SetMaterial(m_strMaterial);
}

vector<PropertyValues> P_WaterRender::GetPropertyValues()
{
	vector<PropertyValues> kReturn(3);
			
	kReturn[0].kValueName="size";
	kReturn[0].iValueType=VALUETYPE_VECTOR3;
	kReturn[0].pkValue=(void*)&m_kSize;
	
	kReturn[1].kValueName="step";
	kReturn[1].iValueType=VALUETYPE_INT;
	kReturn[1].pkValue=(void*)&m_iStep;
	
	kReturn[2].kValueName="material";
	kReturn[2].iValueType=VALUETYPE_STRING;
	kReturn[2].pkValue=(void*)&m_strMaterial;

	return kReturn;
}

bool P_WaterRender::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "material") == 0) {
		SetMaterial(kValue);		
		return true;
	}

	return false;
}



Property* Create_WaterRenderProperty()
{
	return new P_WaterRender;
}

