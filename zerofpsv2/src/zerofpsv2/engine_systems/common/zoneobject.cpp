#include "zoneobject.h"
#include "../propertys/proxyproperty.h"
 
ZoneObject::ZoneObject()
{
	m_strType	= "ZoneObject";
	m_kName		= "A ZoneObject";

	m_bSave=false;	
	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

//	AddProperty("P_Primitives3D");

	m_eRole			= NETROLE_AUTHORITY;
//	m_eRemoteRole	= NETROLE_NONE;
	m_eRemoteRole	= NETROLE_PROXY;

	m_kSize.Set(10,10,10);
//	m_bActive = true;
}
/*
bool ZoneObject::IsInside(Vector3 kPos)
{
	Vector3 kMin = GetWorldPosV() - (m_kSize * 0.5);
	Vector3 kMax = GetWorldPosV() + (m_kSize * 0.5);

	if(kPos.x < kMin.x)	return false;
	if(kPos.y < kMin.y)	return false;
	if(kPos.z < kMin.z)	return false;

	if(kPos.x > kMax.x)	return false;
	if(kPos.y > kMax.y)	return false;
	if(kPos.z > kMax.z)	return false;

	return true;
}
*/

void ZoneObject::SetRadius(float fRadius)
{
	ProxyProperty *proxy = static_cast<ProxyProperty*>(GetProperty("ProxyProperty"));
	proxy->SetRadius(fRadius);	

}





