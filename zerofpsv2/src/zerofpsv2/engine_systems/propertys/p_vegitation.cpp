#include "p_vegitation.h"
#include "p_hmrp2.h"
#include "../common/heightmap.h"
#include "../../engine/camera.h"

P_Vegitation::P_Vegitation()
{
	strcpy(m_acName,"P_Vegitation");
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkTexMan=				static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
	m_pkFps = 				static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkLight=				static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light")); 
	
	m_iSortPlace=2;
	m_bSortDistance = true;
	m_bNetwork = true;
	
	m_CheckedForHM = false;
	m_pkMaterial = new ZFResourceHandle;
	
	Clear();
	
	
	SetMaterial("data/material/forest_grass.zmt");	
	SetScale(Vector3(1,0.7,1));
	m_fWind = 10;	
	
	
	m_iAmount = 400;
	m_iSize = 10;
}

P_Vegitation::~P_Vegitation()
{
	delete m_pkMaterial;
}

void P_Vegitation::Init()
{
	Random();
	
}

void P_Vegitation::Random(P_HMRP2* pkHmrp2)
{
	int size = m_iSize * 100;

	Clear();
	
	if(pkHmrp2)
	{
		float sx = m_pkEntity->GetWorldPosV().x;
		float sz = m_pkEntity->GetWorldPosV().z;
		float sy = m_pkEntity->GetWorldPosV().y;
	
		for(int i=0;i<m_iAmount;i++)
		{
			float x = (rand()%size -(size/2)) /100.0f;
			float z = (rand()%size -(size/2)) /100.0f;
			
			float y = pkHmrp2->m_pkHeightMap->Height(x+sx,z+sz);
			
			AddPos(Vector3(float(x),float(y-sy -0.1),float(z)));
		}
	}
	else
	{
		for(int i=0;i<m_iAmount;i++)
		{
			AddPos(Vector3( (rand()%size -(size/2)) /100.0f,0, (rand()%size -(size/2)) /100.0f));
		}
	}
	
	SetNetUpdateFlag(true);	
}

void P_Vegitation::Update()
{
	//want vegitation
	if(!m_pkZeroFps->GetVegetation())
		return;


	if(m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_SHADOWMAP)
		return;

	//frustum culling
	if(!m_pkFps->GetCam()->GetFrustum()->SphereInFrustum(m_pkEntity->GetWorldPosV(),m_fRadius))
		return;

// 	StartProfileTimer("r___Vegitation");	

	//update light					
	m_pkLight->Update(&m_kLightProfile,GetEntity()->GetWorldPosV());					
	
		
	if(!m_CheckedForHM)
	{
		m_CheckedForHM = true;
	
		//check if theres a heightmap in this zone	
		Entity* hme = m_pkEntity->GetParent();
		if(hme)
		{
			P_HMRP2* pkhmrp = (P_HMRP2*)hme->GetProperty("P_HMRP2");
			if(pkhmrp)
			{
				//cout<<"this zone has a heightmap, using it to get Y values"<<endl;				
				Random(pkhmrp);
			}
		}		
	}
			
			
	static Vector3 kObjectPos;
	kObjectPos = m_pkEntity->GetWorldPosV();			
					
					
// 	//draw a ball on the server
// 	if(m_pkFps->GetDebugGraph())
// 		m_pkRender->Sphere(kObjectPos,0.5,1,Vector3(1,1,1),true);

	
	float fDistance = kObjectPos.DistanceTo(m_pkFps->GetCam()->GetPos()) - m_fRadius;
	if(fDistance > 32)
		return;
							
					
	int iStep = int(fDistance / 5.0);
	if(iStep < 1)
		iStep = 1;

	iStep = PowerOf2(iStep);


	//setup material	
	m_pkZShaderSystem->BindMaterial((ZMaterial*)(m_pkMaterial->GetResourcePtr()));		
	
	float t=m_pkFps->GetTicks();

	if(iStep == 1)
	{
		static Vector3 rot;
		static Vector3 kPos;
		for(unsigned int i=0;i<m_akPositions.size();i++)
		{
			rot = m_akPositions[i].kRot;  
			kPos = m_akPositions[i].kPos + kObjectPos;
			rot.x = float(sin(t + ( kPos.x + kPos.z)) * m_fWind);
			m_pkRender->DrawCross(kPos,rot,m_kScale);			
		}
	}
	else
	{
		static Vector3 kPos;
		for(unsigned int i=0;i<m_akPositions.size();i += iStep)
		{
			kPos = m_akPositions[i].kPos + kObjectPos;
			m_pkRender->DrawCross(kPos,m_akPositions[i].kRot,m_kScale);			
		}		
	}
}

vector<PropertyValues> P_Vegitation::GetPropertyValues()
{
	vector<PropertyValues> kReturn(5);
		
	kReturn[0].kValueName = "material";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_strMaterialFile;

	kReturn[1].kValueName = "m_kScale";
	kReturn[1].iValueType = VALUETYPE_VECTOR3;
	kReturn[1].pkValue    = (void*)&m_kScale;

	kReturn[2].kValueName = "m_fWind";
	kReturn[2].iValueType = VALUETYPE_FLOAT;
	kReturn[2].pkValue    = (void*)&m_fWind;

	kReturn[3].kValueName = "amount";
	kReturn[3].iValueType = VALUETYPE_INT;
	kReturn[3].pkValue    = (void*)&m_iAmount;
	
	kReturn[4].kValueName = "size";
	kReturn[4].iValueType = VALUETYPE_INT;
	kReturn[4].pkValue    = (void*)&m_iSize;


	return kReturn;
}

bool P_Vegitation::HandleSetValue( string kValueName ,string kValue )
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

void P_Vegitation::AddPos(Vector3 kPos)
{
	vegitation temp;
	temp.kPos = kPos;
	temp.kRot = Vector3( float((rand() % 20) -10.0) ,float(rand() % 320),float((rand() % 20) -10.0));
	temp.fWindStart = float((rand() % 2000) / 1000.0);

	m_akPositions.push_back(temp);

	CalculateRadius();
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



