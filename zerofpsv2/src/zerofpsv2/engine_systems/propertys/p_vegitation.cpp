#include "p_vegitation.h"

P_Vegitation::P_Vegitation()
{
	strcpy(m_acName,"P_Vegitation");
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
		
	m_iSortPlace=9;
	bNetwork = true;
	
	m_pkTexture = new ZFResourceHandle;
	
	Clear();
	
	SetTexture("data/textures/grassp.tga");	
	SetScale(Vector3(1,0.7,1));
	m_fWind = 10;	
	
	
	m_iAmount = 400;
	m_iSize = 10;
}

P_Vegitation::~P_Vegitation()
{
	delete m_pkTexture;

}

void P_Vegitation::Init()
{
	Random();
	
}

void P_Vegitation::Random()
{
	int size = m_iSize * 100;

	Clear();
	for(int i=0;i<m_iAmount;i++)
	{
		AddPos(Vector3( (rand()%size -(size/2)) /100.0f,0, (rand()%size -(size/2)) /100.0f));
	}
	
	SetNetUpdateFlag(true);	
}

void P_Vegitation::Update()
{
	//frustum culling
	if(!m_pkFps->GetCam()->m_kFrustum.SphereInFrustum(m_pkObject->GetWorldPosV(),m_fRadius))
		return;
			
			
	Vector3 ObjectPos = m_pkObject->GetWorldPosV();			
			
	
	float fDistance = (ObjectPos - m_pkFps->GetCam()->GetPos()).Length() - m_fRadius;
	if(fDistance > 40)
		return;
						
					
	//draw a ball on the server
	if(m_pkFps->m_bServerMode)
		m_pkRender->Sphere(ObjectPos,0.5,1,Vector3(1,1,1),true);
					
					
	int iStep = int(fDistance / 5.0);
	if(iStep < 1)
		iStep = 1;

	iStep = PowerOf2(iStep);

//	cout<<"step "<<iStep<<endl;
//	cout<<"grass "<<m_akPositions.size()<<endl;


	ResTexture* pkRt = (ResTexture*)m_pkTexture->GetResourcePtr();
	int iTexture;
		
	if(!pkRt)	
	{
		return;
	}
	else
		iTexture = (pkRt)->m_iTextureID;
	
	
	float t=m_pkFps->GetTicks();

	if(iStep == 1)
	{
		Vector3 rot;
		Vector3 kPos;
		for(unsigned int i=0;i<m_akPositions.size();i++)
		{
			rot = m_akPositions[i].kRot;  
			kPos = m_akPositions[i].kPos + ObjectPos;
			rot.x = float(sin(t + ( kPos.x + kPos.z)) * m_fWind);
			m_pkRender->DrawCross(kPos,rot,m_kScale,iTexture);			
		}
	}
	else
	{
		Vector3 kPos;
		for(unsigned int i=0;i<m_akPositions.size();i += iStep)
		{
			kPos = m_akPositions[i].kPos + ObjectPos;
			m_pkRender->DrawCross(kPos,m_akPositions[i].kRot,m_kScale,iTexture);			
		}		
	}



/*	for(unsigned int i=0;i<m_akPositions.size();i += iStep)
//	for(unsigned int i=0;i<m_akPositions.size();i++)
	{
		Vector3 rot = m_akPositions[i].kRot;  
//		rot.x = float(sin(t + m_akPositions[i].fWindStart) * m_fWind);
		Vector3 kPos = m_akPositions[i].kPos + ObjectPos;
		rot.x = float(sin(t + ( kPos.x + kPos.z)) * m_fWind);
		m_pkRender->DrawCross(kPos,rot,m_kScale,iTexture);			
	}
*/
}

vector<PropertyValues> P_Vegitation::GetPropertyValues()
{
	vector<PropertyValues> kReturn(5);
		
	kReturn[0].kValueName = "m_kTexture";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kTexture;

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
	if(strcmp(kValueName.c_str(), "m_kTexture") == 0) {
		SetTexture(kValue.c_str());
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

void P_Vegitation::SetTexture(const char* acNewTex)//,const char* acTex2)
{
	m_pkTexture->SetRes(acNewTex);
	
	m_kTexture=acNewTex;	
	
	SetNetUpdateFlag(true);	
}

void P_Vegitation::UpdateSet()
{
	SetTexture(m_kTexture.c_str());
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
	char data[256];
	
	strcpy(data,m_kTexture.c_str());		
	pkPackage->Write((void*)&data,256,1);
	
	pkPackage->Write((void*)&m_kScale,sizeof(m_kScale),1);
	pkPackage->Write((void*)&m_fRadius,sizeof(m_fRadius),1);
	pkPackage->Write((void*)&m_fWind,sizeof(m_fWind),1);
	
	pkPackage->Write((void*)&m_iAmount,sizeof(m_iAmount),1);
	pkPackage->Write((void*)&m_iSize,sizeof(m_iSize),1);	
	

}

void P_Vegitation::Load(ZFIoInterface* pkPackage)
{

	char data[256];
	
	pkPackage->Read((void*)&data,256,1);
	m_kTexture = data;
	
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
	pkNetPacket->Write_Str(m_kTexture.c_str());
	
	pkNetPacket->Write(&m_iAmount,sizeof(m_iAmount));
	pkNetPacket->Write(&m_iSize,sizeof(m_iSize));	
	
	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_Vegitation::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	char texture[255];
	pkNetPacket->Read_Str(texture);	
	m_kTexture=texture;
	
	int oldamount = m_iAmount;
	int oldsize = m_iSize;
	
	pkNetPacket->Read(&m_iAmount,sizeof(m_iAmount));
	pkNetPacket->Read(&m_iSize,sizeof(m_iSize));	
	
	if(m_iAmount != oldamount || m_iSize != oldsize)
	{
		Random();
	}
}



Property* Create_VegitationProperty()
{
	return new P_Vegitation();
}



