#include "vegitationproperty.h"

VegitationProperty::VegitationProperty()
{
	strcpy(m_acName,"VegitationProperty");
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
		
	m_iSortPlace=9;
	
	Clear();
	
	SetTexture("grass.tga");	
	SetScale(Vector3(1,1,1));
	m_fWind = 10;	
	
	
	
}

void VegitationProperty::Init()
{
/*
	for(int i=0;i<100;i++)
	{
		AddPos(Vector3( (rand()%1000 -500.0) /100.0,0, (rand()%1000 -500.0) /100.0));
	}
*/	
}

void VegitationProperty::Update()
{
	//frustum culling
	if(!m_pkFps->GetCam()->m_kFrustum.SphereInFrustum(m_pkObject->GetWorldPosV(),m_fRadius))
		return;
			
	Vector3 ObjectPos = m_pkObject->GetWorldPosV();			
			
	float fDistance = (ObjectPos - m_pkFps->GetCam()->GetPos()).Length() - m_fRadius;
	if(fDistance > 50)
		return;
					
	int iStep = int(fDistance / 8.0);
	if(iStep < 1)
		iStep = 1;

	iStep = PowerOf2(iStep);

//	cout<<"step "<<iStep<<endl;
//	cout<<"grass "<<m_akPositions.size()<<endl;

	float t=m_pkFps->GetTicks();

	

	for(unsigned int i=0;i<m_akPositions.size();i += iStep){
		Vector3 rot = m_akPositions[i].kRot; 
		rot.x = float(sin(t + m_akPositions[i].fWindStart) * m_fWind);
		m_pkRender->DrawCross(m_akPositions[i].kPos + ObjectPos,rot,m_kScale,m_iTexture);			
	}
}

vector<PropertyValues> VegitationProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(3);
		
	kReturn[0].kValueName = "m_kTexture";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kTexture;

	kReturn[1].kValueName = "m_kScale";
	kReturn[1].iValueType = VALUETYPE_VECTOR3;
	kReturn[1].pkValue    = (void*)&m_kScale;

	kReturn[2].kValueName = "m_fWind";
	kReturn[2].iValueType = VALUETYPE_FLOAT;
	kReturn[2].pkValue    = (void*)&m_fWind;

	return kReturn;
}

bool VegitationProperty::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_kTexture") == 0) {
		SetTexture(kValue.c_str());
		return true;
	}
		
	return false;
}

void VegitationProperty::SetTexture(const char* acNewTex)//,const char* acTex2)
{
	m_kTexture=acNewTex;	
	m_iTexture=m_pkTexMan->Load(acNewTex,0);
}

void VegitationProperty::UpdateSet()
{
	SetTexture(m_kTexture.c_str());
}

void VegitationProperty::SetScale(Vector3 kScale)
{
	m_kScale=kScale;
 
 	CalculateRadius();
}

void VegitationProperty::Clear()
{
	m_akPositions.clear();
}

void VegitationProperty::CalculateRadius()
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
	

//	cout<<"Setting radius to "<<m_fRadius<<endl;
}

void VegitationProperty::AddPos(Vector3 kPos)
{
	vegitation temp;
	temp.kPos = kPos;
	temp.kRot = Vector3( float((rand() % 20) -10.0) ,float(rand() % 320),float((rand() % 20) -10.0));
	temp.fWindStart = float((rand() % 2000) / 1000.0);

	m_akPositions.push_back(temp);

	CalculateRadius();
}




void VegitationProperty::Save(ZFMemPackage* pkPackage)
{
	char data[256];
	
	strcpy(data,m_kTexture.c_str());		
	pkPackage->Write((void*)&data,256,1);
	
	pkPackage->Write((void*)&m_kScale,sizeof(m_kScale),1);
	pkPackage->Write((void*)&m_fRadius,sizeof(m_fRadius),1);
	pkPackage->Write((void*)&m_fWind,sizeof(m_fWind),1);
	
	
	int nrofpos = m_akPositions.size();
	pkPackage->Write((void*)&nrofpos,sizeof(nrofpos),1);
	
	for(int i=0;i<nrofpos;i++)
	{			
		pkPackage->Write((void*)&m_akPositions[i],sizeof(vegitation),1);			
	}

}

void VegitationProperty::Load(ZFMemPackage* pkPackage)
{

	char data[256];
	
	pkPackage->Read((void*)&data,256,1);
	m_kTexture = data;
	

	
	pkPackage->Read((void*)&m_kScale,12,1);
	pkPackage->Read((void*)&m_fRadius,4,1);
	pkPackage->Read((void*)&m_fWind,4,1);
	

	Clear();
	
	int nrofpos;

	pkPackage->Read((void*)&nrofpos,sizeof(nrofpos),1);
	
	//cout<<"grass found :"<<nrofpos<<endl;	
	
	for(int i=0;i<nrofpos;i++)
	{			
		vegitation temp;
		pkPackage->Read((void*)&temp,sizeof(vegitation),1);			
		
		m_akPositions.push_back(temp);
		
	}


	UpdateSet();
}


Property* Create_VegitationProperty()
{
	return new VegitationProperty();
}



