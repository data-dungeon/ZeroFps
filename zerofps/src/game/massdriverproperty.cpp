#include "massdriverproperty.h"
#include "../zerofps/engine/modelproperty.h"
#include "../zerofps/engine/cssphere.h"
#include "playercontrolproperty.h"

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
	
	m_fFireRate=2;
	m_iAmmo=100;
	m_kAim.Set(0,0,1);
	m_bAdded=false;
	m_bFiring=false;
	
	m_fLastShot=m_pkFps->GetTicks();
	
	firesound=new Sound();
	firesound->m_acFile="file:../data/sound/massdriver_fire.wav";
	firesound->m_bLoop=false;

}

MassDriverProperty::~MassDriverProperty()
{
/*	
	if(m_bAdded)
	{
		PlayerControlProperty* pc=static_cast<PlayerControlProperty*>(m_pkObject->GetProperty("PlayerControlProperty"));
		
		if(pc!=NULL)
		{
			pc->RemoveObject(this);
			m_bAdded=false;
		}
	}
*/	
}

void MassDriverProperty::Update()
{
	if(!m_bAdded)
	{
		PlayerControlProperty* pc=static_cast<PlayerControlProperty*>(m_pkObject->GetProperty("PlayerControlProperty"));
		
		if(pc!=NULL)
		{
			pc->AddObject(this);
			m_bAdded=true;
		}
	}


	if(m_bUse)
		Use();
//	else
//		m_pkAlSys->RemoveSound(firesound);
		
	m_bUse=false;		
}

void MassDriverProperty::Use()
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
	Bullet->GetPos()=m_pkObject->GetPos()+Vector3(0,0.6,0) + Bullet->GetVel().Unit();		
	Bullet->AddProperty("MassDriverProjectile");	
	
//	Bullet->AddProperty("ModelProperty");
//	ModelProperty* mp = dynamic_cast<ModelProperty*>(Bullet->GetProperty("ModelProperty"));
//	mp->m_fRadius=0.01;


	PhysicProperty* mp = dynamic_cast<PhysicProperty*>(Bullet->AddProperty("PhysicProperty"));
	mp->m_bGravity=false;
	mp->m_bFloat=false;
	static_cast<CSSphere*>(mp->GetColSphere())->m_fRadius=0.01;	
	
	Bullet->SetParent(m_pkObjectMan->GetWorldObject());

	//play sound
	firesound->m_kPos=m_pkObject->GetPos();
	m_pkAlSys->AddSound(firesound);

}

Property* Create_MassDriverProperty()
{
	return new MassDriverProperty;

}








