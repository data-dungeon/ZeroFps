#include "autoparentproperty.h"
#include "../../basic/zfobjectmanger.h"
#include "../../engine/zerofps.h"

AutoParentProperty::AutoParentProperty()
{
	strcpy(m_acName,"AutoParentProperty");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));			

	m_fUpdateTime=1;
	//add a random time so we dont get that many updates at the same frame
	m_fLastUpdate=m_pkFps->GetTicks();// + (rand()%4)/1000;
}

void AutoParentProperty::Update()
{

	float fCurentTime=m_pkFps->GetTicks();

	if(fCurentTime - m_fLastUpdate >= m_fUpdateTime)
	{
		m_fLastUpdate=fCurentTime;// + m_fUpdateTime + (rand()%4)/1000;
		m_fUpdateTime= 1 + (rand()%2000)/1000;

		m_pkObject->AttachToClosestZone();
	}
}

Property* Create_AutoParentProperty()
{
	return new AutoParentProperty;
}







