#include "zoneobject.h"

ZoneObject::ZoneObject()
{
	m_kName="ZoneObject";
	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	AddProperty("ProxyProperty");
	
	
	AddProperty("MadProperty");
	MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
	madp->SetBase(m_pkFps->GetMADPtr("../data/mad/zoneobject.mad"));

}


void ZoneObject::SetRadius(float fRadius)
{
	ProxyProperty *proxy = static_cast<ProxyProperty*>(GetProperty("ProxyProperty"));
	proxy->SetRadius(fRadius);	

}





