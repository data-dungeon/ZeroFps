#include "proxyproperty.h"

ProxyProperty::ProxyProperty() 
{
	strcpy(m_acName,"ProxyProperty");		
	m_fRadius=200;
	m_iAktion=UPDATESTATUS;
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkLevelMan=static_cast<LevelManager*>(g_ZFObjSys.GetObjectPtr("LevelManager"));			
	
	m_pkTrackers=m_pkLevelMan->GetTrackerList();
}

void ProxyProperty::Update() 
{
	float fDistance;

	//if there is no tracked objects ,,track the camera
	if(m_pkTrackers->size() <= 0)
		fDistance= abs((m_pkFps->GetCam()->GetPos() - m_pkObject->GetPos()).Length());
	else
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
	float fDistance;

	for(list<Object*>::iterator it=m_pkTrackers->begin();it!=m_pkTrackers->end();it++)
	{
		fDistance = abs(((*it)->GetPos() - m_pkObject->GetPos()).Length());		
		if(fDistance < fShortestDistance)
			fShortestDistance = fDistance;
	}

	return fShortestDistance;
}



Property* Create_ProxyProperty()
{
	return new ProxyProperty;
}















