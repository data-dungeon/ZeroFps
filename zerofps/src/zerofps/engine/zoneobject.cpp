#include "zoneobject.h"

ZoneObject::ZoneObject()
{
	m_kName="ZoneObject";
	m_bSave=false;	
	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	AddProperty("LightUpdateProperty");

	m_eRole			= NETROLE_AUTHORITY;
	m_eRemoteRole	= NETROLE_NONE;
}


void ZoneObject::SetRadius(float fRadius)
{
	ProxyProperty *proxy = static_cast<ProxyProperty*>(GetProperty("ProxyProperty"));
	proxy->SetRadius(fRadius);	

}





