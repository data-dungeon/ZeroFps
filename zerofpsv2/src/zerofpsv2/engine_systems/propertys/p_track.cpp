#include "p_track.h"
//#include "../common/zoneobject.h"

/*
ProxyProperty::ProxyProperty() 
{
	strcpy(m_acName,"ProxyProperty");		
	m_fRadius=200;
	m_iAktion=UPDATESTATUS;
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_pkFps			= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
//	m_pkLevelMan	= static_cast<LevelManager*>(g_ZFObjSys.GetObjectPtr("LevelManager"));			
	
//	m_pkTrackers=m_pkLevelMan->GetTrackerList();
}
void ProxyProperty::Update() 
{
	float fDistance;

	//if there is no tracked objects ,,track the camera
//	if(m_pkTrackers->size() <= 0)
//		fDistance= abs((m_pkFps->GetCam()->GetPos() - m_pkObject->GetWorldPosV()).Length());
//	else
		fDistance=GetDistance();

	if(fDistance < m_fRadius){		
		//if camera is inside the proximity
		switch(m_iAktion) {
			case UPDATESTATUS:
				m_pkObject->GetUpdateStatus()=UPDATE_ALL;
				break;				
		}
	} else {
		//if the camera is outside the proximity
		switch(m_iAktion) {
			case UPDATESTATUS:
				m_pkObject->GetUpdateStatus()=UPDATE_DYNAMIC;
				break;				
		}		
	}
}

float ProxyProperty::GetDistance()
{
	float fShortestDistance=99999999;
//	float fDistance;

/*	for(list<Object*>::iterator it=m_pkTrackers->begin();it!=m_pkTrackers->end();it++)
	{
		fDistance = abs(((*it)->GetWorldPosV() - m_pkObject->GetWorldPosV()).Length());		
		if(fDistance < fShortestDistance)
			fShortestDistance = fDistance;
	}

	return fShortestDistance;
}



Property* Create_ProxyProperty()
{
	return new ProxyProperty;
}*/




















P_Track::P_Track() 
{
	m_pkOBjM			= static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));	
	m_pkFps				= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	strcpy(m_acName,"P_Track");		
	m_iType = PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_SERVER;

	m_fNextMove			= m_pkFps->GetGameTime() + 1.0;
	m_iZoneNum			= 0;
	//m_iLastZoneIndex	= -1;
	m_bClosestZone		= true;

	m_iConnectID		= -1;	// No Client.
}

P_Track::~P_Track() 
{
	m_pkOBjM->RemoveTracker(this);
}


void P_Track::Init() 
{
	//cout << "Trackerproperty Init" << endl;
	m_pkOBjM->AddTracker(this);
}

void P_Track::SetClient(int iId)
{
	m_iConnectID = iId;
}


void P_Track::Update() 
{
/*	if(m_pkFps->GetGameTime() < m_fNextMove)
		return;
	m_fNextMove = m_pkFps->GetGameTime() + 1.0;*/

/*	m_iZoneNum++;
	if(m_iZoneNum >= m_pkOBjM->m_kZones.size())
		m_iZoneNum = 0;
	m_pkObject->SetWorldPosV(m_pkOBjM->m_kZones[m_iZoneNum].m_kPos);*/
	
//	ZoneObject* pkZone = m_pkOBjM->GetZone(m_pkObject);
//	if(!pkZone) {
//		return;
//		}
/*
	int iRandDir = rand() % pkZone->m_kZoneLinks.size();
	m_pkObject->SetWorldPosV(m_pkOBjM->m_kZones[m_iZoneNum].m_kPos);
	m_pkObject->SetWorldPosV(pkZone->m_kZoneLinks[iRandDir]->GetWorldPosV());*/
}


Property* Create_TrackProperty()
{
	return new P_Track;
}





