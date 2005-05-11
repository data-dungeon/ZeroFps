#include "p_ost.h"


P_Ost::P_Ost()
{
	strcpy(m_acName,"P_Ost");		
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_bNetwork = true;	
	m_iVersion = 2;
	m_iSortPlace=4;
	
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));				
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	
	m_fSize = 4;
	
		
	m_pkFogMaterial = new ZMaterial;
	m_pkFogMaterial->GetPass(0)->m_kTUs[0]->SetRes("data/textures/fogplane.tga");
	m_pkFogMaterial->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
	m_pkFogMaterial->GetPass(0)->m_iCullFace = 				CULL_FACE_NONE;		
	m_pkFogMaterial->GetPass(0)->m_bLighting = 				true;		
	m_pkFogMaterial->GetPass(0)->m_bFog = 						true;		
	m_pkFogMaterial->GetPass(0)->m_bBlend = 					true;	
	m_pkFogMaterial->GetPass(0)->m_bDepthMask =				false;
		
	
	m_pkFogMaterial->GetPass(0)->m_iBlendSrc = SRC_ALPHA_BLEND_SRC;
	m_pkFogMaterial->GetPass(0)->m_iBlendDst = ONE_MINUS_SRC_ALPHA_BLEND_DST;


}

P_Ost::~P_Ost()
{

	delete m_pkFogMaterial;
}

void P_Ost::Update()
{
	if(!m_pkZeroFps->GetCam()->GetFrustum()->SphereInFrustum(m_pkEntity->GetWorldPosV(),m_fSize*0.5))
		return;


//   	if(m_pkZeroFps->GetDebugGraph())
// 	{
//   		m_pkRender->Sphere(m_pkEntity->GetWorldPosV(),m_fSize*0.5,1,Vector3(1,1,1),false);
// 	}


	static float afVerts[] = {	-0.5,	0,	0.5,	
							 			-0.5,	0,	-0.5,	
							 			0.5,	0,	-0.5,	
										0.5,	0,	0.5};
										
	static float afNormals[] = {	0,1,0,	
							 				0,1,0,	
							 				0,1,0,	
											0,1,0,};										
	
	static float afUvs[] = {	0,1,
							 			1,1,
										1,0,
										0,0};	

	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetNormal(Vector3(0,1,0));
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,afVerts);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);
// 	m_pkZShaderSystem->SetPointer(NORMAL_POINTER,afNormals);
	m_pkZShaderSystem->SetNrOfVertexs(4);
	
										
										
	m_pkZShaderSystem->MatrixPush();	
	
	m_pkZShaderSystem->MatrixTranslate(GetEntity()->GetWorldPosV());
	//m_pkZShaderSystem->MatrixRotate(Vector3(0,m_pkZeroFps->GetTicks()*50,0));
	m_pkZShaderSystem->MatrixScale(m_fSize);
	
	m_pkZShaderSystem->BindMaterial(m_pkFogMaterial);	
	m_pkZShaderSystem->DrawArray(QUADS_MODE);										
	
	
	m_pkZShaderSystem->MatrixPop();

	
}

vector<PropertyValues> P_Ost::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
	kReturn[0].kValueName = "size";
	kReturn[0].iValueType = VALUETYPE_FLOAT;
	kReturn[0].pkValue    = (void*)&m_fSize;

	return kReturn;
}


void P_Ost::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(m_fSize);

}

void P_Ost::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 2)
		pkPackage->Read(m_fSize);
}


void P_Ost::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write(m_fSize);
	
	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_Ost::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Read(m_fSize);
}


Property* Create_P_FogPlane()
{
	return new P_Ost;
}

void Register_P_FogPlane(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Ost", Create_P_FogPlane);					

	// Register Property Script Interface

}
