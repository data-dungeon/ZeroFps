#include "massdriverproperty.h"
#include "../zerofps/engine/modelproperty.h"


MassDriverProperty::MassDriverProperty()
{
	cout<<"MASS DRIVER CREATED:...mohaha"<<endl;

	strcpy(m_acName,"MassDriverProperty");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkInput= static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
		
	m_iActionFire=m_pkInput->RegisterAction("fire_massdriver");
	
	m_iAmmo=100;
	m_kAim.Set(0,0,1);
	
	m_fLastShot=m_pkFps->GetTicks();
}


void MassDriverProperty::Update()
{
	m_kAim=m_pkObject->GetRot().AToU();

	if(m_pkInput->Action(m_iActionFire))
	{
		Fire();
	}
}

void MassDriverProperty::Fire()
{
	if(m_pkFps->GetTicks()-m_fLastShot <0.2)
		return;		
	m_fLastShot=m_pkFps->GetTicks();

	
//	cout<<"BANG!!"<<endl;
	
	Object* Bullet=new Object;
	Bullet->GetName()="MassDriver_Bullet";
	Bullet->GetVel()=m_kAim*PROJECTILE_SPEED;
	Bullet->GetPos()=m_pkObject->GetPos();//+Vector3(3,0,0);		
	Bullet->AddProperty("MassDriverProjectile");
	
	Bullet->AddProperty("ModelProperty");
	ModelProperty* mp = dynamic_cast<ModelProperty*>(Bullet->GetProperty("ModelProperty"));
	mp->m_fRadius=0.1;

	Bullet->AddProperty("PhysicProperty");	
	
	Bullet->SetParent(m_pkObjectMan->GetWorldObject());

}

Property* Create_MassDriverProperty()
{
	return new MassDriverProperty;

}








