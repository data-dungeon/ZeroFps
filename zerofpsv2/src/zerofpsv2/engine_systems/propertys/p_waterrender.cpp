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
	m_bNetwork	=	true;
	m_bHaveMesh = false;
	
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


P_WaterRender::~P_WaterRender()
{
	delete m_pkMaterial;
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

void P_WaterRender::GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState)
{

	static Vector3 kPos;
	static Vector3 kMin;
	static Vector3 kMax;
	
	kPos = m_pkEntity->GetWorldPosV();
	kMin.Set(kPos.x-m_kSize.x,kPos.y-m_kSize.y,kPos.z-m_kSize.z);
	kMax.Set(kPos.x+m_kSize.x,kPos.y+m_kSize.y,kPos.z+m_kSize.z);
			
	//frustum culling
	if(!kRenderState.m_kFrustum.CubeInFrustum(kMin,kMax))
		return;			
	
	if(kRenderState.m_kCameraPosition.DistanceTo(kPos) - m_kSize.Length() > m_pkZeroFps->GetViewDistance())
		return;

	//setup mesh and renderpackages
	if(!m_bHaveMesh)
		GenerateMesh();


	//setup renderpackage
	m_kRenderPackage.m_pkMaterial = (ZMaterial*)m_pkMaterial->GetResourcePtr();	
	m_kRenderPackage.m_kCenter = kPos;		
	m_kRenderPackage.m_kModelMatrix.Identity();
	m_kRenderPackage.m_kModelMatrix.Translate(kPos);		
	kRenderPackages.push_back(&m_kRenderPackage);
	
	
	//debug render package
	if(m_pkZeroFps->GetDebugGraph())
	{	
		m_kDebugRenderPackage.m_kModelMatrix.Identity();
		m_kDebugRenderPackage.m_kModelMatrix.Translate(kPos);					
		kRenderPackages.push_back(&m_kDebugRenderPackage);
	}
}

void P_WaterRender::GenerateMesh()
{
	
	m_bHaveMesh = true;

	m_kRenderPackage.m_pkLightProfile = &m_kLightProfile;	
	m_kRenderPackage.m_kAABBMin = -m_kSize;
	m_kRenderPackage.m_kAABBMax = m_kSize;
	m_kRenderPackage.m_fRadius = 0;//m_kSize.Length();
	m_kRenderPackage.m_bOcculusionTest = true;

	m_kRenderPackage.m_kMeshData.m_kVertises.clear();
	m_kRenderPackage.m_kMeshData.m_kNormals.clear();
	m_kRenderPackage.m_kMeshData.m_kTexture[0].clear();
	m_kRenderPackage.m_kMeshData.m_kTangents.clear();
	m_kRenderPackage.m_kMeshData.m_kBiTangents.clear();

	for(int x = int(-m_kSize.x);x< m_kSize.x;x+=m_iStep)
	{	
		for(int z = int(-m_kSize.z);z< m_kSize.z;z+=m_iStep)
		{
			m_kRenderPackage.m_kMeshData.m_kVertises.push_back(Vector3(x			,m_kSize.y	,z));
			m_kRenderPackage.m_kMeshData.m_kVertises.push_back(Vector3(x			,m_kSize.y	,z+m_iStep));			
			m_kRenderPackage.m_kMeshData.m_kVertises.push_back(Vector3(x+m_iStep	,m_kSize.y	,z));
			
			m_kRenderPackage.m_kMeshData.m_kVertises.push_back(Vector3(x+m_iStep	,m_kSize.y	,z));
			m_kRenderPackage.m_kMeshData.m_kVertises.push_back(Vector3(x			,m_kSize.y	,z+m_iStep));						
			m_kRenderPackage.m_kMeshData.m_kVertises.push_back(Vector3(x+m_iStep	,m_kSize.y	,z+m_iStep));
		
			static float tw = 0.1;
		
			m_kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(x*tw,z*tw));
			m_kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(x*tw,z*tw+tw));						
			m_kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(x*tw+tw,z*tw));
			
			m_kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(x*tw+tw,z*tw));	
			m_kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(x*tw,z*tw+tw));						
			m_kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(x*tw+tw,z*tw+tw));
			
		
			m_kRenderPackage.m_kMeshData.m_kNormals.push_back(Vector3(0,1,0));
			m_kRenderPackage.m_kMeshData.m_kNormals.push_back(Vector3(0,1,0));
			m_kRenderPackage.m_kMeshData.m_kNormals.push_back(Vector3(0,1,0));
			m_kRenderPackage.m_kMeshData.m_kNormals.push_back(Vector3(0,1,0));
			m_kRenderPackage.m_kMeshData.m_kNormals.push_back(Vector3(0,1,0));
			m_kRenderPackage.m_kMeshData.m_kNormals.push_back(Vector3(0,1,0));		
		}
	}	


	Math::GenerateTangents(	&(m_kRenderPackage.m_kMeshData.m_kVertises[0]),
									&(m_kRenderPackage.m_kMeshData.m_kNormals[0]),
									&(m_kRenderPackage.m_kMeshData.m_kTexture[0][0]),
									m_kRenderPackage.m_kMeshData.m_kTangents,
									m_kRenderPackage.m_kMeshData.m_kBiTangents,
									m_kRenderPackage.m_kMeshData.m_kVertises.size());

// 	for(int i = 0;i<m_kRenderPackage.m_kMeshData.m_kVertises.size()*2	;i++)
// 	{
// 		m_kRenderPackage.m_kMeshData.m_kTangents.push_back(Vector3(1,0,0));
// 		m_kRenderPackage.m_kMeshData.m_kBiTangents.push_back(Vector3(0,0,1));
// 	}


	m_kRenderPackage.m_kMeshData.m_iNrOfDataElements = m_kRenderPackage.m_kMeshData.m_kVertises.size();
	m_kRenderPackage.m_kMeshData.m_ePolygonMode = TRIANGLES_MODE;
	
	
	
	//debug package	
	static ZMaterial* pkLine = NULL;
	if(!pkLine)
	{
		pkLine = new ZMaterial;
		pkLine->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkLine->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
		pkLine->GetPass(0)->m_iCullFace = CULL_FACE_NONE;		
		pkLine->GetPass(0)->m_bLighting = false;
		pkLine->GetPass(0)->m_bColorMaterial = true;
		pkLine->GetPass(0)->m_bUseShader = false;
	}					
	m_kDebugRenderPackage.m_pkMaterial = pkLine;	
	
	
	m_kDebugRenderPackage.m_kMeshData.m_kVertises.clear();
	m_kDebugRenderPackage.m_kMeshData.m_kColors.clear();	
	m_pkRender->AddAABB(m_kDebugRenderPackage,-m_kSize,m_kSize,Vector3(1,1,1));	
	m_pkRender->AddAABB(m_kDebugRenderPackage,Vector3(-0.5,-0.5,-0.5),Vector3(0.5,0.5,0.5),Vector3(1,0.5,0.5));		
	m_kDebugRenderPackage.m_kMeshData.m_iNrOfDataElements = m_kDebugRenderPackage.m_kMeshData.m_kVertises.size();
	m_kDebugRenderPackage.m_kMeshData.m_ePolygonMode = QUADS_MODE;

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
	
	if(m_pkZeroFps->GetCam()->GetRenderPos().DistanceTo(kPos) - m_kSize.Length() > m_pkZeroFps->GetViewDistance())
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

	m_bHaveMesh = false;
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
		m_bHaveMesh = false;
	}
	else
	{
		pkPackage->Read(m_kSize);
		pkPackage->Read(m_iStep);
		pkPackage->Read_Str(m_strMaterial);	
		SetMaterial(m_strMaterial);		
		GetEntity()->SetLocalAABB(-m_kSize,m_kSize);		
		m_bHaveMesh = false;
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
	{
		GetEntity()->SetLocalAABB(-m_kSize,m_kSize);
		m_bHaveMesh = false;
	}
}


Property* Create_WaterRenderProperty()
{
	return new P_WaterRender;
}



