#include "hmrp2.h"
#include "../../engine/zerofps.h"
#include "../../engine/levelmanager.h"

HMRP2::HMRP2(HeightMap* pkHeightMap, string strMapName) 
{
	strcpy(m_acName,"HMRP2");		

	m_pkFrustum	=	static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkTexMan	=	static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps	=	static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender	=	static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		


	SetHeightMap(pkHeightMap,strMapName);
	
	m_iType	=	PROPERTY_TYPE_RENDER;
	m_iSide	=	PROPERTY_SIDE_CLIENT;
	bNetwork	=	true;
}

void HMRP2::SetHeightMap(HeightMap* pkHeightMap, string strMapName)
{
	m_pkHeightMap	=	pkHeightMap;	
	m_strMapName	=	strMapName;

	if(m_pkHeightMap!=NULL)
		m_pkHeightMap->SetPosition(m_pkObject->GetPos());	
}


void HMRP2::Update() 
{	
	if(m_pkHeightMap!=NULL){
		m_pkHeightMap->SetPosition(m_pkObject->GetPos());
		m_pkRender->DrawHMLodSplat(m_pkHeightMap,m_pkZeroFps->GetCam()->GetPos(),int(m_pkZeroFps->m_fFps));
//		m_pkRender->G4DrawHMLodSplat(m_pkHeightMap,m_pkZeroFps->GetCam()->GetPos(),m_pkZeroFps->m_iFps);		
	}
	

}

void HMRP2::PackTo(NetPacket* pkNetPacket)
{
	pkNetPacket->Write_Str(m_strMapName.c_str());
}
 
void HMRP2::PackFrom(NetPacket* pkNetPacket)
{
	char temp[50];
	pkNetPacket->Read_Str(temp);

	if(strcmp(m_strMapName.c_str(), temp) == 0) 
		return;

	LevelManager* pkLev = static_cast<LevelManager*>(g_ZFObjSys.GetObjectPtr("LevelManager"));		
	pkLev->LoadLevelHmapOnly(temp);
	
	m_strMapName = temp;	
	SetHeightMap(pkLev->GetHeightMap(),m_strMapName);

}

Property* Create_HMRP2()
{
	return new HMRP2(NULL, "None");
}

void HMRP2::SetPolyMode(PolygonMode eMode)
{
	m_pkRender->m_eLandscapePolygonMode = eMode;
}