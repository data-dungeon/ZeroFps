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
	m_pkAlSys=static_cast<OpenAlSystem*>(g_ZFObjSys.GetObjectPtr("OpenAlSystem"));				
		
	m_iActionFire=m_pkInput->RegisterAction("fire_massdriver");
	
	m_iAmmo=100;
	m_kAim.Set(0,0,1);
	
	m_fLastShot=m_pkFps->GetTicks();
	
	firesound=new Sound();
	firesound->m_acFile="file:../data/sound/massdriver_fire.wav";
	firesound->m_bLoop=false;

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
	if(m_pkFps->GetTicks()-m_fLastShot <1)
		return;		
	m_fLastShot=m_pkFps->GetTicks();

	if(m_iAmmo<=0)
	{
		cout<<"klick"<<endl;
		return;
	}
	
//	m_iAmmo--;
	//play sound
	firesound->m_kPos=m_pkObject->GetPos();
	m_pkAlSys->AddSound(firesound);
	
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
//	Bullet->AttachToClosestZone();

}

Property* Create_MassDriverProperty()
{
	return new MassDriverProperty;

}








