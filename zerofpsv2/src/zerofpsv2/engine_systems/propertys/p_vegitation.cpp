#include "p_vegitation.h"
#include "p_hmrp2.h"
#include "../common/heightmap.h"
#include "../../engine/camera.h"
#include "../../basic/math.h"

P_Vegitation::P_Vegitation() : Property("P_Vegitation")
{
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkTexMan=				static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
	m_pkFps = 				static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkLight=				static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight")); 
	
	m_iSortPlace=0;
	m_bSortDistance = true;
	m_bNetwork = true;
	
	m_CheckedForHM = false;
	m_pkMaterial = new ZFResourceHandle;
	m_bCastShadow = true;
	
	Clear();
	
	
	SetMaterial("forest_grass.zmt");	
	m_kScale.Set(1,0.7,1);
	m_fWind = 10;	
	
	
	m_iAmount = 400;
	m_iSize = 10;
	
	
	
	m_kPropertyValues.push_back(PropertyValues("material",VALUETYPE_STRING,(void*)&m_strMaterialFile));
	m_kPropertyValues.push_back(PropertyValues("m_kScale",VALUETYPE_VECTOR3,(void*)&m_kScale));
	m_kPropertyValues.push_back(PropertyValues("m_fWind",VALUETYPE_FLOAT,(void*)&m_fWind));
	m_kPropertyValues.push_back(PropertyValues("amount",VALUETYPE_INT,(void*)&m_iAmount));
	m_kPropertyValues.push_back(PropertyValues("size",VALUETYPE_INT,(void*)&m_iSize));
	
	//setup renderpackage
	m_kRenderPackage.m_pkLightProfile = &m_kLightProfile;
	m_kRenderPackage.m_kMeshData.m_ePolygonMode = QUADS_MODE;
	
}

P_Vegitation::~P_Vegitation()
{
	delete m_pkMaterial;
}

void P_Vegitation::Init()
{
	Random();	
}

void P_Vegitation::GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState)
{
	if(!m_pkZeroFps->GetVegetation())
		return;

	//Distance culling
	if(kRenderState.m_kCameraPosition.DistanceTo(m_pkEntity->GetWorldPosV()) > m_pkZeroFps->GetViewDistance()*0.3)
		return;

	//frustum culling sphere
 	if(!kRenderState.m_kFrustum.SphereInFrustum(m_pkEntity->GetWorldPosV(),m_fRadius))
 		return;
	
	
	//setup grass
	if(!m_CheckedForHM)
	{
		m_CheckedForHM = true;
	
		//check if theres a heightmap in this zone	
		Entity* hme = m_pkEntity->GetParent();
		if(hme)
		{
			if(hme->GetProperty("P_Heightmap"))
			{
			
				vector<Entity*> kZones;
				vector<P_Heightmap*> kHMaps;
				
				//get zones			
				m_pkEntityManager->GetAllEntitys(&kZones);	
				
				//get heightmaps
				for(int i =0;i<kZones.size();i++)
				{
					if(P_Heightmap* pkHmap = (P_Heightmap*)kZones[i]->GetProperty("P_Heightmap"))
						kHMaps.push_back(pkHmap);
				}
				
				
				//send heightmaps to random function
				Random(&kHMaps);
			}	
			else				
				Random();
		}		
	}	


	//setup position and material
	m_kRenderPackage.m_kModelMatrix.Identity();
	m_kRenderPackage.m_kModelMatrix.Translate(m_pkEntity->GetWorldPosV());
	m_kRenderPackage.m_pkMaterial = (ZMaterial*)(m_pkMaterial->GetResourcePtr());	

	m_kRenderPackage.m_kAABBMin = m_kAABBMin;
	m_kRenderPackage.m_kAABBMax = m_kAABBMax;
	m_kRenderPackage.m_kCenter = m_pkEntity->GetWorldPosV();
	m_kRenderPackage.m_bOcculusionTest = true;
	
	//add renderpackage
	kRenderPackages.push_back(&m_kRenderPackage);



	//debug render package
	if(m_pkZeroFps->GetDebugGraph())
	{	
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
		m_pkRender->AddAABB(m_kDebugRenderPackage,m_kAABBMin,m_kAABBMax,Vector3(1,1,1));			
		m_pkRender->AddAABB(m_kDebugRenderPackage,Vector3(-0.5,-0.5,-0.5),Vector3(0.5,0.5,0.5),Vector3(1,0.5,0.5));	
		
		m_kDebugRenderPackage.m_kMeshData.m_iNrOfDataElements = m_kDebugRenderPackage.m_kMeshData.m_kVertises.size();
		m_kDebugRenderPackage.m_kMeshData.m_ePolygonMode = QUADS_MODE;
	
		m_kDebugRenderPackage.m_kModelMatrix.Identity();
		m_kDebugRenderPackage.m_kModelMatrix.Translate(m_pkEntity->GetWorldPosV());					

		kRenderPackages.push_back(&m_kDebugRenderPackage);
	}
}

void P_Vegitation::Random(vector<P_Heightmap*>* pkHMaps)
{
	int size = m_iSize * 100;

	Clear();
	
	if(pkHMaps)
	{
		float sx = m_pkEntity->GetWorldPosV().x;
		float sz = m_pkEntity->GetWorldPosV().z;
		float sy = m_pkEntity->GetWorldPosV().y;
	
		for(int i=0;i<m_iAmount;i++)
		{
			float x = Math::Randomf(m_iSize) - float(m_iSize) / 2.0;
			float z = Math::Randomf(m_iSize) - float(m_iSize) / 2.0;
			float y = 0;
			
			for(int i = 0;i<pkHMaps->size();i++)
			{
				if((*pkHMaps)[i]->Inside(x+sx,z+sz))
				{
					y = (*pkHMaps)[i]->GetHeight(x+sx,z+sz)-sy;
				}
			}
			
			AddPos(Vector3(float(x),float(y -0.1),float(z)));
		}
	}
	else
	{
		for(int i=0;i<m_iAmount;i++)
		{
			AddPos(Vector3( (rand()%size -(size/2)) /100.0f,0, (rand()%size -(size/2)) /100.0f));
		}
	}
	
	
	CalculateRadius();
	CreateAABB();
 	BuildArrays();
		
	
	ResetAllNetUpdateFlags();	
}


void P_Vegitation::Update()
{
	//want vegitation
	if(!m_pkZeroFps->GetVegetation())
		return;

//  	if(m_pkZeroFps->GetCam()->GetCurrentRenderMode() != RENDER_SHADOWED)
//  		return;

	if(!( (m_pkZeroFps->GetShadowMapRealtime() || m_bCastShadow) && m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_CASTSHADOW ||
		  m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_SHADOWED))
		return;


	static Vector3 kObjectPos;
	kObjectPos = m_pkEntity->GetWorldPosV();

	//Distance culling
	if(m_pkFps->GetCam()->GetPos().DistanceTo(kObjectPos) > m_pkZeroFps->GetViewDistance()*0.3)
		return;

	//frustum culling sphere
 	if(!m_pkFps->GetCam()->GetFrustum()->SphereInFrustum(kObjectPos,m_fRadius))
 		return;

		
	if(!m_CheckedForHM)
	{
		m_CheckedForHM = true;
	
		//check if theres a heightmap in this zone	
		Entity* hme = m_pkEntity->GetParent();
		if(hme)
		{
			if(hme->GetProperty("P_Heightmap"))
			{
			
				vector<Entity*> kZones;
				vector<P_Heightmap*> kHMaps;
				
				//get zones			
// 				if(m_pkEntityManager->IsUpdate(
// 				hme->GetZoneNeighbours(&kZones);
				m_pkEntityManager->GetAllEntitys(&kZones);
	
				
				//get heightmaps
				for(int i =0;i<kZones.size();i++)
				{
					if(P_Heightmap* pkHmap = (P_Heightmap*)kZones[i]->GetProperty("P_Heightmap"))
						kHMaps.push_back(pkHmap);
				}
				
				
				//send heightmaps to random function
				Random(&kHMaps);
			}	
			else				
				Random();
		}		
	}
	

	float fDistance = kObjectPos.DistanceTo(m_pkFps->GetCam()->GetPos()) - m_fRadius;
					
 	//draw a ball on the server
  	if(m_pkFps->GetDebugGraph())
	{
		bool bUse = m_pkZShaderSystem->GetUseDefaultGLSLProgram();
		m_pkZShaderSystem->UseDefaultGLSLProgram(false);
		
  		m_pkRender->Sphere(kObjectPos,0.5,1,Vector3(1,1,1),true);
		
		if(fDistance > 10)
			m_pkRender->DrawAABB(kObjectPos + m_kAABBMin,kObjectPos+m_kAABBMax,Vector3(0.5,0.5,0.5),1);
		else
			m_pkRender->DrawAABB(kObjectPos + m_kAABBMin,kObjectPos+m_kAABBMax,Vector3(1,1,1),1);
		
		m_pkZShaderSystem->UseDefaultGLSLProgram(bUse);
	}
	
					
	
	//frustum culling aabb
	if(!m_pkFps->GetCam()->GetFrustum()->CubeInFrustum(kObjectPos + m_kAABBMin,kObjectPos+m_kAABBMax))
		return;	
	
	
	
	if(m_pkZShaderSystem->SupportOcculusion() && 
		m_pkZeroFps->GetOcculusionCulling() &&
		m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_SHADOWED)
	{						
		static Vector3 kPos;
		kPos = m_pkEntity->GetWorldPosV();
		
		//occulusion test
		if(m_kOCTests[m_pkZeroFps->GetCam()].Visible(kPos+m_kAABBMin,kPos+m_kAABBMax))
		{
			//update lighting
			m_pkLight->Update(&m_kLightProfile,GetEntity()->GetWorldPosV());								
		
			DrawArray();
			
			m_pkZeroFps->m_iNotOcculedObjects++;								
		}
		else
			m_pkZeroFps->m_iOcculedObjects++;		
	}
	else		
	{
		//update lighting
		m_pkLight->Update(&m_kLightProfile,GetEntity()->GetWorldPosV());								
	
		DrawArray();
	}	
	
	
}





bool P_Vegitation::HandleSetValue( const string& kValueName ,const string& kValue )
{
	if(strcmp(kValueName.c_str(), "material") == 0) {
		SetMaterial(kValue.c_str());
		return true;
	}
		
	if(strcmp(kValueName.c_str(), "amount") == 0) {
		m_iAmount = atoi(kValue.c_str());
		Random();
		return true;
	}
		
	if(strcmp(kValueName.c_str(), "size") == 0) {
		m_iSize = atoi(kValue.c_str());
		Random();
		return true;
	}
		
		
	return false;
}

void P_Vegitation::SetMaterial(const char* acNewTex)//,const char* acTex2)
{
	m_pkMaterial->SetRes(acNewTex);	
	m_strMaterialFile=acNewTex;	
	
	
	SetNetUpdateFlag(true);	
}

void P_Vegitation::UpdateSet()
{
	SetMaterial(m_strMaterialFile.c_str());
}

void P_Vegitation::SetScale(Vector3 kScale)
{
	m_kScale=kScale;
 
 	CalculateRadius();
	CreateAABB();
}

void P_Vegitation::Clear()
{
	m_akPositions.clear();
}

void P_Vegitation::CalculateRadius()
{
	float maxDist = 0;

	for(unsigned int i=0;i<m_akPositions.size();i++)
	{
		float Distance = (m_akPositions[i].kPos).Length();
		
		if(Distance > maxDist)
			maxDist = Distance;
	}
	
	float maxscale =0;
	
	if(m_kScale.x > maxscale)
		maxscale = m_kScale.x;
	if(m_kScale.y > maxscale)
		maxscale = m_kScale.y;
	if(m_kScale.z > maxscale)
		maxscale = m_kScale.z;
	
	
	m_fRadius = maxDist + (maxscale /2);	
}

void P_Vegitation::CreateAABB()
{
	m_kAABBMax.Set(-99999999,-99999999,-99999999);
	m_kAABBMin.Set(99999999,99999999,99999999);

	static Vector3* pkVert;
	int iSize = m_akPositions.size();
	for(unsigned int i=0;i<iSize;i++)
	{
		pkVert = &m_akPositions[i].kPos;

		//max
		if(pkVert->x > m_kAABBMax.x)
			m_kAABBMax.x = pkVert->x;

		if(pkVert->y > m_kAABBMax.y)
			m_kAABBMax.y = pkVert->y;
		
		if(pkVert->z > m_kAABBMax.z)
			m_kAABBMax.z = pkVert->z;	
			
		//min	
		if(pkVert->x < m_kAABBMin.x)
			m_kAABBMin.x = pkVert->x;

		if(pkVert->y < m_kAABBMin.y)
			m_kAABBMin.y = pkVert->y;
		
		if(pkVert->z < m_kAABBMin.z)
			m_kAABBMin.z = pkVert->z;	
	}		

	float maxwidth =0;

	if(m_kScale.x > maxwidth)
		maxwidth = m_kScale.x;
	if(m_kScale.z > maxwidth)
		maxwidth = m_kScale.z;

	maxwidth/=2;

	m_kAABBMax.y += m_kScale.y;

	m_kAABBMin.x -= maxwidth;
	m_kAABBMin.z -= maxwidth;
	m_kAABBMax.x += maxwidth;
	m_kAABBMax.z += maxwidth;



	GetEntity()->SetLocalAABB(m_kAABBMin,m_kAABBMax);
}



void P_Vegitation::AddPos(const Vector3& kPos)
{
	static vegitation temp;
	temp.kPos = kPos;
	temp.kRot.Set(float((rand() % 20) -10.0) ,float(rand() % 320),float((rand() % 20) -10.0));
	temp.fWindStart = float((rand() % 2000) / 1000.0);

	m_akPositions.push_back(temp);

}




void P_Vegitation::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write_Str(m_strMaterialFile);
	
	pkPackage->Write((void*)&m_kScale,sizeof(m_kScale),1);
	pkPackage->Write((void*)&m_fRadius,sizeof(m_fRadius),1);
	pkPackage->Write((void*)&m_fWind,sizeof(m_fWind),1);
	
	pkPackage->Write((void*)&m_iAmount,sizeof(m_iAmount),1);
	pkPackage->Write((void*)&m_iSize,sizeof(m_iSize),1);	
	

}

void P_Vegitation::Load(ZFIoInterface* pkPackage,int iVersion)
{

	pkPackage->Read_Str(m_strMaterialFile);
	
	pkPackage->Read((void*)&m_kScale,12,1);
	pkPackage->Read((void*)&m_fRadius,4,1);
	pkPackage->Read((void*)&m_fWind,4,1);
	
	pkPackage->Read((void*)&m_iAmount,sizeof(m_iAmount),1);
	pkPackage->Read((void*)&m_iSize,sizeof(m_iSize),1);	

	Random();

	UpdateSet();


}

void P_Vegitation::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write_Str(m_strMaterialFile);
	
	pkNetPacket->Write(m_iAmount);
	pkNetPacket->Write(m_iSize);	
	pkNetPacket->Write(m_kScale);	
	
	
	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_Vegitation::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Read_Str(m_strMaterialFile);	
	
	int oldamount = m_iAmount;
	int oldsize = m_iSize;
	
	pkNetPacket->Read(m_iAmount);
	pkNetPacket->Read(m_iSize);
	pkNetPacket->Read(m_kScale);			
	
	if(m_iAmount != oldamount || m_iSize != oldsize)
	{
		Random();
	}
	
	UpdateSet();
}



Property* Create_VegitationProperty()
{
	return new P_Vegitation();
}






void P_Vegitation::BuildArrays()
{
	m_kVertexArray.clear();
	m_kTextureArray.clear();
	m_kColorArray.clear();
	m_kNormalArray.clear();

	static Vector2 kUV1(0,1);
	static Vector2 kUV2(1,1);
	static Vector2 kUV3(1,0);
	static Vector2 kUV4(0,0);

	Vector3 kScale(m_kScale.x/2,m_kScale.y,m_kScale.z/2);// = m_kScale * 0.5;

	Vector3 topleft(-kScale.x,kScale.y,0);
	Vector3 topright(kScale.x,kScale.y,0);
	Vector3 bottomleft(-kScale.x,0,0);
	Vector3 bottomright(kScale.x,0,0);

	Vector3 topback(0,kScale.y,-kScale.z);
	Vector3 topfront(0,kScale.y,kScale.z);
	Vector3 bottomback(0,0,-kScale.z);
	Vector3 bottomfront(0,0,kScale.z);

	int iPositions = m_akPositions.size();
	for(int i =0;i<iPositions;i++ )
	{
		//Vector3 off(Randomf(kScale.x*1.5)+0.25,0,Randomf(kScale.z*1.5)+0.25);
		float r = Math::DegToRad(Math::Randomf(360));
		float cosr = cos(r);
		float sinr = sin(r);
		float cosrd2 = cos(r+Math::HALF_PI);	// PId2
		float sinrd2 = sin(r+Math::HALF_PI);
		
		float s = Math::Randomf(0.4);
		float h = kScale.y*s + kScale.y*0.6;
		float w = kScale.x*s + kScale.x*0.6;
		
	
		//cross vertises
		m_kVertexArray.push_back(m_akPositions[i].kPos + Vector3(-sinr*w,h,-cosr*w));
		m_kVertexArray.push_back(m_akPositions[i].kPos + Vector3(sinr*w,h,cosr*w));
		m_kVertexArray.push_back(m_akPositions[i].kPos + Vector3(sinr*w,0,cosr*w));		
		m_kVertexArray.push_back(m_akPositions[i].kPos + Vector3(-sinr*w,0,-cosr*w));		
		
		m_kVertexArray.push_back(m_akPositions[i].kPos + Vector3(-sinrd2*w,h,-cosrd2*w));
		m_kVertexArray.push_back(m_akPositions[i].kPos + Vector3(sinrd2*w,h,cosrd2*w));
		m_kVertexArray.push_back(m_akPositions[i].kPos + Vector3(sinrd2*w,0,cosrd2*w));		
		m_kVertexArray.push_back(m_akPositions[i].kPos + Vector3(-sinrd2*w,0,-cosrd2*w));		
		
		//uv's
		m_kTextureArray.push_back(kUV1);
		m_kTextureArray.push_back(kUV2);
		m_kTextureArray.push_back(kUV3);
		m_kTextureArray.push_back(kUV4);
	
		m_kTextureArray.push_back(kUV1);
		m_kTextureArray.push_back(kUV2);
		m_kTextureArray.push_back(kUV3);
		m_kTextureArray.push_back(kUV4);
	
		m_kColorArray.push_back(Vector4(1,1,1,1));
		m_kColorArray.push_back(Vector4(1,1,1,1));
		m_kColorArray.push_back(Vector4(1,1,1,0.1));
		m_kColorArray.push_back(Vector4(1,1,1,0.1));
		
		m_kColorArray.push_back(Vector4(1,1,1,1));
		m_kColorArray.push_back(Vector4(1,1,1,1));
		m_kColorArray.push_back(Vector4(1,1,1,0.1));
		m_kColorArray.push_back(Vector4(1,1,1,0.1));
	
		//normals
		for(int j = 0;j<8;j++)
			m_kNormalArray.push_back(Vector3(0,1,0));
	}
	
	
	//setup renderpackage
	m_kRenderPackage.m_kMeshData.m_kDataPointers.push_back(DataPointer(VERTEX_POINTER,&m_kVertexArray[0]));
	m_kRenderPackage.m_kMeshData.m_kDataPointers.push_back(DataPointer(TEXTURE_POINTER0,&m_kTextureArray[0]));
	m_kRenderPackage.m_kMeshData.m_kDataPointers.push_back(DataPointer(COLOR_POINTER,&m_kColorArray[0]));
	m_kRenderPackage.m_kMeshData.m_kDataPointers.push_back(DataPointer(NORMAL_POINTER,&m_kNormalArray[0]));
	m_kRenderPackage.m_kMeshData.m_iNrOfDataElements = m_kVertexArray.size();
}

void P_Vegitation::DrawArray()
{
	m_pkZShaderSystem->BindMaterial((ZMaterial*)(m_pkMaterial->GetResourcePtr()));
	
	m_pkZShaderSystem->MatrixPush();
	
		m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetWorldPosV()+Vector3(0,-0.01,0));		
	
		m_pkZShaderSystem->ResetPointers();
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,	&(m_kTextureArray[0]));
		m_pkZShaderSystem->SetPointer(VERTEX_POINTER,  	&(m_kVertexArray[0]));
		m_pkZShaderSystem->SetPointer(COLOR_POINTER,		&(m_kColorArray[0]));
		m_pkZShaderSystem->SetNormal(Vector3(0,1,0));
// 		m_pkZShaderSystem->SetPointer(NORMAL_POINTER,  &(m_kNormalArray[0]));		
		m_pkZShaderSystem->SetNrOfVertexs(m_kVertexArray.size());
		
		m_pkZShaderSystem->DrawArray(QUADS_MODE);			
	
	m_pkZShaderSystem->MatrixPop();
}









// vector<PropertyValues> P_Vegitation::GetPropertyValues()
// {
// 	vector<PropertyValues> kReturn(5);
// 		
// 	kReturn[0].kValueName = "material";
// 	kReturn[0].iValueType = VALUETYPE_STRING;
// 	kReturn[0].pkValue    = (void*)&m_strMaterialFile;
// 
// 	kReturn[1].kValueName = "m_kScale";
// 	kReturn[1].iValueType = VALUETYPE_VECTOR3;
// 	kReturn[1].pkValue    = (void*)&m_kScale;
// 
// 	kReturn[2].kValueName = "m_fWind";
// 	kReturn[2].iValueType = VALUETYPE_FLOAT;
// 	kReturn[2].pkValue    = (void*)&m_fWind;
// 
// 	kReturn[3].kValueName = "amount";
// 	kReturn[3].iValueType = VALUETYPE_INT;
// 	kReturn[3].pkValue    = (void*)&m_iAmount;
// 	
// 	kReturn[4].kValueName = "size";
// 	kReturn[4].iValueType = VALUETYPE_INT;
// 	kReturn[4].pkValue    = (void*)&m_iSize;
// 
// 
// 	return kReturn;
// }