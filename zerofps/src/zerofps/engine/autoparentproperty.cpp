#include "autoparentproperty.h"

AutoParentProperty::AutoParentProperty()
{
	strcpy(m_acName,"AutoParentProperty");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));			

	m_fUpdateTime=1;
	m_fLastUpdate=m_pkFps->GetTicks() + ((rand()%1) /1000.0);
}

void AutoParentProperty::Update()
{
	float fCurentTime=m_pkFps->GetTicks();

	if(fCurentTime - m_fLastUpdate >= m_fUpdateTime)
	{
		m_fLastUpdate=fCurentTime;

		m_pkObject->AttachToClosestZone();
	}
}

Property* Create_AutoParentProperty()
{
	return new AutoParentProperty;
}







