#include "zoneobject.h"

ZoneObject::ZoneObject()
{
	m_kName="ZoneObject";
	m_bSave=false;	
	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	AddProperty("LightUpdateProperty");

	AddProperty("MadProperty");
	MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
	madp->SetBasePtr(m_pkFps->GetMADPtr("../data/mad/zoneobject.mad"));


	AddProperty("ProxyProperty");

}


void ZoneObject::SetRadius(float fRadius)
{
	ProxyProperty *proxy = static_cast<ProxyProperty*>(GetProperty("ProxyProperty"));
	proxy->SetRadius(fRadius);	

}





