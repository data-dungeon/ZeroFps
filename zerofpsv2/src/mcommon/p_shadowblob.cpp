#include "p_shadowblob.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"

P_ShadowBlob::P_ShadowBlob()
{
	strcpy(m_acName,"P_ShadowBlob");		
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkZShaderSystem=static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
	
	m_bNetwork = true;
	m_iSortPlace	=	11;
	
	m_kOffset.Set(0,0.01,0);
	m_kScale.Set(1,1,1);
	m_bHaveSet = false;
}

P_ShadowBlob::~P_ShadowBlob()
{
	delete m_pkMaterial;
}

void P_ShadowBlob::Update()
{
	if(!m_bHaveSet)
	{
		m_bHaveSet = true;
		
		if(P_Mad* pkMad = (P_Mad*)m_pkEntity->GetProperty("P_Mad"))
		{
			m_kScale.Set(pkMad->GetRadius(),pkMad->GetRadius(),pkMad->GetRadius());
		}
	}
	
	
	m_pkZShaderSystem->BindMaterial(m_pkMaterial);
	m_pkZShaderSystem->ClearGeometry();
	
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetIWorldPosV()+m_kOffset);
	
	float x = m_kScale.x/2.0;
	float z = m_kScale.z/2.0;
	
	m_pkZShaderSystem->AddQuadV(	Vector3(-x,0, z),Vector3( x,0, z),
											Vector3( x,0,-z),Vector3(-x,0,-z));												

	m_pkZShaderSystem->AddQuadUV(	Vector2(0,0),Vector2(1,0),Vector2(1,1),Vector2(0,1));
												
	m_pkZShaderSystem->DrawGeometry(QUADS_MODE);	
	m_pkZShaderSystem->MatrixPop();

}

void P_ShadowBlob::Init()
{

	m_pkMaterial = new ZMaterial;
	m_pkMaterial->GetPass(0)->m_kTUs[0]->SetRes("data/textures/shadowblob.bmp");
	m_pkMaterial->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;
	m_pkMaterial->GetPass(0)->m_bLighting = 			false;		
	m_pkMaterial->GetPass(0)->m_bBlend = 				true;
	m_pkMaterial->GetPass(0)->m_iBlendDst = 			ZERO_BLEND_DST;
	m_pkMaterial->GetPass(0)->m_iBlendSrc = 			SRC_COLOR_BLEND_DST;
}

void P_ShadowBlob::Save(ZFIoInterface* pkPackage)
{	

	pkPackage->Write(&m_kOffset,sizeof(m_kOffset),1);
	pkPackage->Write(&m_kScale,sizeof(m_kScale),1);
	pkPackage->Write(&m_bHaveSet,sizeof(m_bHaveSet),1);


}

void P_ShadowBlob::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read(&m_kOffset,sizeof(m_kOffset),1);		
	pkPackage->Read(&m_kScale,sizeof(m_kScale),1);		
	pkPackage->Read(&m_bHaveSet,sizeof(m_bHaveSet),1);		
	
}


vector<PropertyValues> P_ShadowBlob::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
	kReturn[0].kValueName = "scale";
	kReturn[0].iValueType = VALUETYPE_FLOAT;
	kReturn[0].pkValue    = (void*)&m_kScale.x;


	return kReturn;
}

bool P_ShadowBlob::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "scale") == 0) 
	{
		float fScale = float(atof(kValue.c_str()));	
		m_kScale.Set(fScale,fScale,fScale);
		
		m_bHaveSet = true;
		return true;
	}
	
	return false;
}

Property* Create_P_ShadowBlob()
{
	return new P_ShadowBlob;
}








