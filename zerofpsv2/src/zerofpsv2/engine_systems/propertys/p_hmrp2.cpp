#include "p_hmrp2.h"
#include "../../engine/zerofps.h"
#include "../common/heightmap.h"

P_HMRP2::P_HMRP2() 
{
	strcpy(m_acName,"P_HMRP2");		

	m_pkTexMan	=	static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps	=	static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender	=	static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		
	m_pkLight   =	static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light"));	

	
	m_iType	=	PROPERTY_TYPE_RENDER;
	m_iSide	=	PROPERTY_SIDE_CLIENT;
	m_bNetwork	=	true;
	
	SetHeightMap(NULL,string(""));
}

P_HMRP2::~P_HMRP2()
{
	if(m_pkHeightMap)
		delete m_pkHeightMap;
}

void P_HMRP2::SetHeightMap(HeightMap* pkHeightMap, string strMapName)
{
	cout<<"SETTING HMAP"<<endl;

	m_pkHeightMap	=	pkHeightMap;	
	m_strMapName	=	strMapName;
}

void P_HMRP2::Init()
{
	cout << "Run P_HMRP2 Init" << endl;
	if(!m_pkHeightMap)
		m_pkHeightMap = new HeightMap;

	m_pkHeightMap->SetID( m_pkEntity->GetEntityID() );
	char szFileName[256];
	sprintf(szFileName, "%s/hm%d", m_pkEntity->m_pkEntityManager->GetWorldDir().c_str(), m_pkEntity->GetEntityID());
	m_strMapName = szFileName;
	
}

void P_HMRP2::Update() 
{	
	
	//cout << "Render HMAP" << endl; 
	if(m_pkHeightMap)
	{
		
		m_pkHeightMap->SetPosition(m_pkEntity->GetWorldPosV());
		
		m_pkLight->Update(&m_kLightProfile,GetEntity()->GetWorldPosV());					
// 		m_pkLight->Update(m_pkEntity->GetWorldPosV());
		m_pkRender->DrawHMLodSplat(m_pkHeightMap,m_pkZeroFps->GetCam()->GetPos(),int(m_pkZeroFps->m_fFps));
		//m_pkRender->DrawNormals(m_pkHeightMap,m_pkEntity->GetWorldPosV(),int(m_pkZeroFps->m_fFps));
	}
}

void P_HMRP2::PackTo(NetPacket* pkNetPacket, int iConnectionID)
{
	pkNetPacket->Write_Str(m_strMapName.c_str());
}
 
void P_HMRP2::PackFrom(NetPacket* pkNetPacket, int iConnectionID)
{
	char temp[50];
	pkNetPacket->Read_Str(temp);

	m_strMapName = temp;	
	char hmapname[256];
	sprintf(hmapname, "%s/hm%d", m_pkEntity->m_pkEntityManager->GetWorldDir().c_str(), m_pkEntity->GetEntityID());
	cout << "Should Load HMRP2: " << hmapname << endl;

	m_pkHeightMap = new HeightMap;
	m_pkHeightMap->Load(hmapname);
}

void P_HMRP2::Save(ZFIoInterface* pkPackage)
{
	char hmapname[256];
	sprintf(hmapname, "%s/hm%d",m_pkEntity->m_pkEntityManager->GetWorldDir().c_str() ,m_pkEntity->GetEntityID());

/*	if(m_pkHeightMap)
		m_pkHeightMap->Save(hmapname);*/

	//dvoid haxa in så det sparas i propertyt istället *EEEEEEEVIL* =D
	if(m_pkHeightMap)
		m_pkHeightMap->Save(pkPackage);
}

void P_HMRP2::Load(ZFIoInterface* pkPackage,int iVersion)
{
	char hmapname[256];
	sprintf(hmapname, "%s/hm%d", m_pkEntity->m_pkEntityManager->GetWorldDir().c_str(), m_pkEntity->GetEntityID());
	cout << "Should Load HMRP2: " << hmapname << endl;

	m_pkHeightMap = new HeightMap;
 	m_pkHeightMap->Load(pkPackage);

// 	m_pkHeightMap->Load(hmapname);

}

float P_HMRP2::GetRadius()
{
	return m_pkHeightMap->GetSize();
}

vector<PropertyValues> P_HMRP2::GetPropertyValues()
{
	vector<PropertyValues> kReturn(3);

	kReturn[0].kValueName="invert";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_pkHeightMap->m_bInverted;

	kReturn[1].kValueName="tilesize";
	kReturn[1].iValueType=VALUETYPE_FLOAT;
	kReturn[1].pkValue=(void*)&m_pkHeightMap->m_fTileSize;

	kReturn[2].kValueName="tilesides";
	kReturn[2].iValueType=VALUETYPE_INT;
	kReturn[2].pkValue=(void*)&m_pkHeightMap->m_iTilesSide;

	return kReturn;
}


/* ********************************** SCRIPT INTERFACE ****************************************/
Property* Create_HMRP2()
{
	return new P_HMRP2();
}

void ENGINE_SYSTEMS_API Register_PHmrp(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_HMRP2", Create_HMRP2);					
}

