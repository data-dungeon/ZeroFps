#include "fh.h"

FHObject::FHObject() {
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));


/*	AddProperty("MadProperty");
	MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
	madp->SetBasePtr(m_pkFps->GetMADPtr("../data/mad/tree.mad"));*/

	m_iObjectType=OBJECT_TYPE_DYNAMIC;
}

FHObject::FHObject(const char* szMadName)
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	if(szMadName) {
		AddProperty("MadProperty");
		MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
		madp->SetBasePtr(m_pkFps->GetMADPtr(szMadName));
		//szMadName
	}
		
	GetPos()		= Vector3(0,0,0);
	m_iObjectType	= OBJECT_TYPE_STATIC;
}


void FHObject::HandleCollision(Object* pkOther,Vector3 kPos,bool bContinue)
{

}

