#include "p_track.h"

P_Track::P_Track() 
{
	strcpy(m_acName,"P_Track");		
	m_iType = 0;
	m_iSide = 0;

	m_bOneZoneOnly				= false;
	m_iCurrentTrackedZone	= -1;
	m_bClosestZone				= true;
	m_iConnectID				= -1;	// No Client.
	m_iVersion					=	2;
}

P_Track::~P_Track() 
{
	m_pkEntityManager->RemoveTracker(this);
}


void P_Track::Init() 
{
	m_pkEntityManager->AddTracker(this);
}

void P_Track::SetClient(int iId)
{
	m_iConnectID = iId;
}


void P_Track::Update() 
{

}


vector<PropertyValues> P_Track::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
	
	kReturn[0].kValueName = "OneZoneOnly";
	kReturn[0].iValueType = VALUETYPE_BOOL;
	kReturn[0].pkValue    = (void*)&m_bOneZoneOnly;
	
	return kReturn;
}

void P_Track::Save(ZFIoInterface* pkPackage)
{	
	pkPackage->Write(m_bOneZoneOnly);
}

void P_Track::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 2)
	{
		pkPackage->Read(m_bOneZoneOnly);
	}
}

Property* Create_TrackProperty()
{
	return new P_Track;
}





