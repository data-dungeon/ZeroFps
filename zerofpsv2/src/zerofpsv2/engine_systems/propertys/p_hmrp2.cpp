#include "p_hmrp2.h"
#include "../../engine/zerofps.h"
//#include "../../engine/levelmanager.h"

P_HMRP2::P_HMRP2(HeightMap* pkHeightMap, string strMapName) 
{
	strcpy(m_acName,"P_HMRP2");		

	m_pkTexMan	=	static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps	=	static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender	=	static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		
	m_pkLight   =	static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light"));	


	SetHeightMap(pkHeightMap,strMapName);
	
	m_iType	=	PROPERTY_TYPE_RENDER;
	m_iSide	=	PROPERTY_SIDE_CLIENT;
	bNetwork	=	true;
}

void P_HMRP2::SetHeightMap(HeightMap* pkHeightMap, string strMapName)
{
	m_pkHeightMap	=	pkHeightMap;	
	m_strMapName	=	strMapName;

//	if(m_pkHeightMap!=NULL)
//		m_pkHeightMap->SetPosition(m_pkObject->GetWorldPosV());	
}

void P_HMRP2::Init()
{
	cout << "Run P_HMRP2 Init" << endl;
	if(!m_pkHeightMap)
		m_pkHeightMap = new HeightMap;

	m_pkHeightMap->SetID( m_pkObject->iNetWorkID );
}

void P_HMRP2::Update() 
{	
	if(m_pkHeightMap!=NULL){
		m_pkHeightMap->SetPosition(m_pkObject->GetWorldPosV());
		m_pkLight->Update(m_pkObject->GetWorldPosV());
		m_pkRender->DrawHMLodSplat(m_pkHeightMap,m_pkZeroFps->GetCam()->GetPos(),int(m_pkZeroFps->m_fFps));
		//m_pkRender->DrawNormals(m_pkHeightMap,m_pkObject->GetWorldPosV(),int(m_pkZeroFps->m_fFps));
//		m_pkRender->G4DrawHMLodSplat(m_pkHeightMap,m_pkZeroFps->GetCam()->GetPos(),m_pkZeroFps->m_iFps);		
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

	if(strcmp(m_strMapName.c_str(), temp) == 0) 
		return;

//	LevelManager* pkLev = static_cast<LevelManager*>(g_ZFObjSys.GetObjectPtr("LevelManager"));		
//	pkLev->LoadLevelHmapOnly(temp);
	
	m_strMapName = temp;	
//	SetHeightMap(pkLev->GetHeightMap(),m_strMapName);

}

Property* Create_HMRP2()
{
	return new P_HMRP2(NULL, "None");
}

void P_HMRP2::SetPolyMode(PolygonMode eMode)
{
	m_pkRender->m_eLandscapePolygonMode = eMode;
}


void P_HMRP2::Save(ZFIoInterface* pkPackage)
{
	char hmapname[256];
	sprintf(hmapname, "%s/hm%d",m_pkObject->m_pkObjectMan->GetWorldDir().c_str() ,m_pkObject->iNetWorkID);

	cout << "Should Save HMRP2: " << hmapname << endl;
	if(m_pkHeightMap)
		m_pkHeightMap->Save(hmapname);
}

void P_HMRP2::Load(ZFIoInterface* pkPackage)
{
	char hmapname[256];
	sprintf(hmapname, "%s/hm%d", m_pkObject->m_pkObjectMan->GetWorldDir().c_str(), m_pkObject->iNetWorkID);
	cout << "Should Load HMRP2: " << hmapname << endl;

	m_pkHeightMap = new HeightMap;
	m_pkHeightMap->Load(hmapname);
}



