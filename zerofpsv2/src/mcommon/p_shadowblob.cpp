#include "p_shadowblob.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"

P_ShadowBlob::P_ShadowBlob()
{
	strcpy(m_acName,"P_ShadowBlob");		
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkZShaderSystem=		static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
	m_pkTcs =					static_cast<Tcs*>(g_ZFObjSys.GetObjectPtr("Tcs"));			
	
	m_pkMad			= NULL;
	m_bNetwork 		= true;
	m_iSortPlace	=	11;
	
	m_kOffset.Set(0,0,0);
	m_fScale 		=		1;
	m_bHaveSet 		= 	false;
}

P_ShadowBlob::~P_ShadowBlob()
{
	delete m_pkMaterial;
}

void P_ShadowBlob::Update()
{
	if(m_pkZeroFps->GetCam()->GetCurrentRenderMode() != RENDER_NOSHADOWLAST)
		return;


	if(!m_pkMad)
		m_pkMad = (P_Mad*)m_pkEntity->GetProperty("P_Mad");

	if(!m_bHaveSet)
	{
		m_bHaveSet = true;		
		m_fScale = m_pkMad->GetRadius();
	}
	
	if(m_pkMad)
		if(m_pkMad->IsCulled())
			return;

	Vector3 kShadowPos = Vector3::ZERO;
	float fScale = m_fScale;
	
	

	kShadowPos = GetShadowPos();
	fScale = m_fScale - ( kShadowPos.DistanceTo(GetEntity()->GetIWorldPosV() + m_kOffset)/2.0 );	
	if(fScale > m_fScale)
		fScale = m_fScale;
	
	if(fScale < 0)
		return;
	
	
	m_pkZShaderSystem->BindMaterial(m_pkMaterial);
	m_pkZShaderSystem->ClearGeometry();
	
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(kShadowPos + Vector3(0,0.05,0));
	
	float x = fScale/2.0;
	float z = fScale/2.0;
	
	m_pkZShaderSystem->AddQuadV(	Vector3(-x,0, z),Vector3( x,0, z),
											Vector3( x,0,-z),Vector3(-x,0,-z));												

	m_pkZShaderSystem->AddQuadUV(	Vector2(0,0),Vector2(1,0),Vector2(1,1),Vector2(0,1));
												
	m_pkZShaderSystem->DrawGeometry(QUADS_MODE);	
	m_pkZShaderSystem->MatrixPop();

}

void P_ShadowBlob::PackTo( NetPacket* pkNetPacket, int iConnectionID ) 
{
	pkNetPacket->Write( m_bHaveSet);
	pkNetPacket->Write( m_kOffset);
	pkNetPacket->Write( m_fScale);
	
																						
	SetNetUpdateFlag(iConnectionID,false);
}

void P_ShadowBlob::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read( m_bHaveSet);
	pkNetPacket->Read( m_kOffset);
	pkNetPacket->Read( m_fScale);
}

Vector3 P_ShadowBlob::GetShadowPos()
{
	Vector3 start = GetEntity()->GetIWorldPosV();
	Vector3 dir = Vector3(0,-1,0);

	vector<Entity*> kObjects;
	kObjects.clear();
	
	
	m_pkEntityManager->GetWorldEntity()->GetAllEntitys(&kObjects);
	
//	cout<<"nr of targets: "<<kObjects.size()<<endl;
	
	float closest = 999999999;
	Vector3 kPos = Vector3::ZERO;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{		
		if(!kObjects[i]->IsZone())
			continue;
		
		if(kObjects[i] == GetEntity())
			continue;
			
		
		//get mad property and do a linetest
		P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad");
		if(mp)
		{
			if(mp->TestLine(start,dir))
			{	
				float d = (start - kObjects[i]->GetIWorldPosV()).Length();
	
				if(d < closest)
				{
					closest = d;
					pkClosest = kObjects[i];
					kPos = mp->GetLastColPos();
				}				
			}
		}		
		
	}
	
	return kPos;
}

void P_ShadowBlob::Init()
{

	m_pkMaterial = new ZMaterial;
	m_pkMaterial->GetPass(0)->m_kTUs[0]->SetRes("data/textures/shadowblob.tga");
	m_pkMaterial->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;
	m_pkMaterial->GetPass(0)->m_bLighting = 			false;		
	m_pkMaterial->GetPass(0)->m_bBlend = 				true;
	m_pkMaterial->GetPass(0)->m_bFog =	 				true;
	m_pkMaterial->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
	m_pkMaterial->GetPass(0)->m_iBlendDst = ONE_MINUS_SRC_ALPHA_BLEND_DST;	
}

void P_ShadowBlob::Save(ZFIoInterface* pkPackage)
{	

	pkPackage->Write(&m_kOffset,sizeof(m_kOffset),1);
	pkPackage->Write(m_fScale);
	pkPackage->Write(&m_bHaveSet,sizeof(m_bHaveSet),1);


}

void P_ShadowBlob::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read(&m_kOffset,sizeof(m_kOffset),1);		
	pkPackage->Read(m_fScale);		
	pkPackage->Read(&m_bHaveSet,sizeof(m_bHaveSet),1);		
	
}


vector<PropertyValues> P_ShadowBlob::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
		
	kReturn[0].kValueName = "scale";
	kReturn[0].iValueType = VALUETYPE_FLOAT;
	kReturn[0].pkValue    = (void*)&m_fScale;

	kReturn[1].kValueName = "offset";
	kReturn[1].iValueType = VALUETYPE_VECTOR3;
	kReturn[1].pkValue    = (void*)&m_kOffset;
	
	return kReturn;
}

bool P_ShadowBlob::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(strcmp(kValueName.c_str(), "scale") == 0) 
	{
		m_fScale = float(atof(kValue.c_str()));	
		m_bHaveSet = true;
		return true;
	}
	
	return false;
}

Property* Create_P_ShadowBlob()
{
	return new P_ShadowBlob;
}








