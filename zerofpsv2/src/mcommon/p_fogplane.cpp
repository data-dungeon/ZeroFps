#include "p_fogplane.h"


P_FogPlane::P_FogPlane() : Property("P_FogPlane")
{
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_bNetwork = true;	
	m_iVersion = 3;
	m_iSortPlace=4;
	
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));				
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));			
	
	m_fSize = 4;
	
		
	m_pkMaterial = new ZFResourceHandle;
	SetMaterial("fogplane.zlm");		


	m_kPropertyValues.push_back(PropertyValues("size",VALUETYPE_FLOAT,(void*)&m_fSize));
	m_kPropertyValues.push_back(PropertyValues("material",VALUETYPE_STRING,(void*)&m_strMaterial));

}

P_FogPlane::~P_FogPlane()
{
	if(m_pkMaterial)
		delete m_pkMaterial;
}

void P_FogPlane::SetMaterial(const string& strMaterial)
{
	m_pkMaterial->SetRes(strMaterial);
	m_strMaterial = strMaterial;

	SetNetUpdateFlag(true);
}

void P_FogPlane::Update()
{
	if(m_pkZeroFps->GetCam()->GetCurrentRenderMode() != RENDER_SHADOWED)
		return;

	if(!m_pkZeroFps->GetCam()->GetFrustum()->SphereInFrustum(m_pkEntity->GetWorldPosV(),m_fSize*0.5))
		return;


	static float afVerts[] = {	-0.5,	0,	0.5,	
							 			-0.5,	0,	-0.5,	
							 			0.5,	0,	-0.5,	
										0.5,	0,	0.5};
																	
	
	static float afUvs[] = {	0,1,
							 			1,1,
										1,0,
										0,0};	

	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetNormal(Vector3(0,1,0));
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,afVerts);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);
	m_pkZShaderSystem->SetNrOfVertexs(4);
	
										
										
	m_pkZShaderSystem->MatrixPush();	
	
	m_pkZShaderSystem->MatrixTranslate(GetEntity()->GetWorldPosV());
	m_pkZShaderSystem->MatrixScale(m_fSize);
	
	m_pkZShaderSystem->BindMaterial((ZMaterial*)m_pkMaterial->GetResourcePtr());	
	m_pkZShaderSystem->DrawArray(QUADS_MODE);										
	
	
	m_pkZShaderSystem->MatrixPop();

	
}



bool P_FogPlane::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(kValueName == "material")
	{
		SetMaterial(kValue);
		return true;
	}
	
	if( kValueName == "size") 
	{
		SetNetUpdateFlag(true);
		return false;
	}

	return false;
}

void P_FogPlane::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(m_fSize);
	pkPackage->Write_Str(m_strMaterial);
}

void P_FogPlane::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 2)
	{
		pkPackage->Read(m_fSize);		
	}
	
	if(iVersion == 3)
	{
		pkPackage->Read(m_fSize);
		pkPackage->Read_Str(m_strMaterial);
		SetMaterial(m_strMaterial);
	}
}


void P_FogPlane::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write(m_fSize);
	pkNetPacket->Write(m_strMaterial);
	
	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_FogPlane::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Read(m_fSize);
	pkNetPacket->Read(m_strMaterial);
	
	SetMaterial(m_strMaterial);
}


Property* Create_P_FogPlane()
{
	return new P_FogPlane;
}

void Register_P_FogPlane(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_FogPlane", Create_P_FogPlane);					

	// Register Property Script Interface

}



// vector<PropertyValues> P_FogPlane::GetPropertyValues()
// {
// 	vector<PropertyValues> kReturn(2);
// 		
// 	kReturn[0].kValueName = "size";
// 	kReturn[0].iValueType = VALUETYPE_FLOAT;
// 	kReturn[0].pkValue    = (void*)&m_fSize;
// 
// 	kReturn[1].kValueName = "material";
// 	kReturn[1].iValueType = VALUETYPE_STRING;
// 	kReturn[1].pkValue    = (void*)&m_strMaterial;
// 
// 	return kReturn;
// }