#include "massdriverproperty.h"
#include "../zerofps/engine/modelproperty.h"
#include "../zerofps/engine/cssphere.h"

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
	
	m_fFireRate=0.1;
	m_iAmmo=100;
	m_kAim.Set(0,0,1);
	
	m_fLastShot=m_pkFps->GetTicks();
	
	firesound=new Sound();
	firesound->m_acFile="file:../data/sound/massdriver_autofire.wav";
	firesound->m_bLoop=false;

}


void MassDriverProperty::Update()
{


	if(m_pkInput->Action(m_iActionFire))
	{
		Fire();
		
		//play sound
		firesound->m_kPos=m_pkObject->GetPos();
		m_pkAlSys->AddSound(firesound);
	
	}else
	{
		m_pkAlSys->RemoveSound(firesound);
	}
}

void MassDriverProperty::Fire()
{
	if(m_pkFps->GetTicks()-m_fLastShot < m_fFireRate)
		return;		
	m_fLastShot=m_pkFps->GetTicks();

	if(m_iAmmo<=0)
	{
		cout<<"klick"<<endl;
		return;
	}
	
	//m_iAmmo--;
	
	
	m_kAim=m_pkObject->GetRot().AToU();	
	
	Object* Bullet=new Object;
	Bullet->GetName()="MassDriver_Bullet";
	Bullet->GetVel()=m_kAim*PROJECTILE_SPEED;
//	Vector3 bla=Bullet->GetVel().Unit();
	Bullet->GetPos()=m_pkObject->GetPos()+Vector3(0,0.5,0) + Bullet->GetVel().Unit();		
	Bullet->AddProperty("MassDriverProjectile");	
//	static_cast<MassDriverProjectile*>(Bullet->AddProperty("MassDriverProjectile"));//->shoterid = m_pkObject->iNetWorkID;
	
//	Bullet->AddProperty("ModelProperty");
//	ModelProperty* mp = dynamic_cast<ModelProperty*>(Bullet->GetProperty("ModelProperty"));
//	mp->m_fRadius=0.01;

	static_cast<CSSphere*>(static_cast<PhysicProperty*>(Bullet->AddProperty("PhysicProperty"))->GetColSphere())->m_fRadius=0.01;	
	
	Bullet->SetParent(m_pkObjectMan->GetWorldObject());

}

Property* Create_MassDriverProperty()
{
	return new MassDriverProperty;

}








