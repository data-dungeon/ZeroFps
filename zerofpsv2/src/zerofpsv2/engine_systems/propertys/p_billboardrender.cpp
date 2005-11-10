#include "p_billboardrender.h"
#include "../../engine/zerofps.h"
 
P_BillBoardRender::P_BillBoardRender() : Property("P_BillBoardRender")
{
	m_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));		
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkMaterial = new ZFResourceHandle;
	m_strMaterial = "";
	m_fScale=1;	
		
		
	m_kPropertyValues.push_back(PropertyValues("material",VALUETYPE_STRING,&m_strMaterial));
	m_kPropertyValues.push_back(PropertyValues("scale",VALUETYPE_FLOAT,(void*)&m_fScale));		

}

P_BillBoardRender::~P_BillBoardRender()
{
	delete m_pkMaterial;
}


void P_BillBoardRender::Update()
{
	if(!m_pkMaterial->IsValid())
		return;

	if(m_pkZeroFps->GetCam()->GetCurrentRenderMode() != RENDER_NOSHADOWLAST)
		return;

	if(!m_pkZeroFps->GetCam()->GetFrustum()->SphereInFrustum(m_pkEntity->GetWorldPosV(),m_fScale))
		return;
		
// 	m_pkRender->DrawBillboard(	m_pkFps->GetCam()->GetModelViewMatrix(),
// 										m_pkEntity->GetIWorldPosV(),m_fScale,
// 										m_iTexture,false,false,true);

	m_pkRender->DrawBillboardQuad(m_pkZeroFps->GetCam()->GetRotM(),m_pkEntity->GetIWorldPosV(),m_fScale,(ZMaterial*)m_pkMaterial->GetResourcePtr());		
// 	DrawBillboard();
}



void P_BillBoardRender::SetMaterial(const string& strMaterial)
{
	m_pkMaterial->SetRes(strMaterial);
 	m_strMaterial = strMaterial;

	SetNetUpdateFlag(true);
}

// vector<PropertyValues> P_BillBoardRender::GetPropertyValues()
// {
// 	vector<PropertyValues> kReturn(2);
//  
// 	kReturn[0].kValueName = "material";
// 	kReturn[0].iValueType = VALUETYPE_STRING;
// 	kReturn[0].pkValue    = &m_strMaterial;
// 
// 	kReturn[1].kValueName = "scale";
// 	kReturn[1].iValueType = VALUETYPE_FLOAT;
// 	kReturn[1].pkValue    = (void*)&m_fScale;
// 	
// 	return kReturn;
// }

bool P_BillBoardRender::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(strcmp(kValueName.c_str(), "material") == 0) 
	{
		SetMaterial(kValue);
		return true;
	}
	
	if(strcmp(kValueName.c_str(), "scale") == 0) 
	{
		SetNetUpdateFlag(true);
	}

	return false;
}


void P_BillBoardRender::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(m_strMaterial);
	pkPackage->Write(m_fScale);

}

void P_BillBoardRender::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read(m_strMaterial);
	pkPackage->Read(m_fScale);	
	
	
	if(m_strMaterial.empty())
		m_pkMaterial->FreeRes();
	else
		m_pkMaterial->SetRes(m_strMaterial);	
}


Property* Create_BillBoardRenderProperty()
{
	return new P_BillBoardRender;

}


