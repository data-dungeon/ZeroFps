#include "massdriverprojectile.h"

MassDriverProjectile::MassDriverProjectile()
{
	strcpy(m_acName,"MassDriverProjectile");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
}



void MassDriverProjectile::Touch(Object* pkObject)
{
	m_pkObjectMan->Delete(m_pkObject);	
}


Property* Create_MassDriverProjectile()
{
	return new MassDriverProjectile;

}


