#include "adaptorsniper.h"
#include "../zerofps/engine/cssphere.h"


AdaptorSniper::AdaptorSniper()
{
	strcpy(m_acName,"AdaptorSniper");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
		
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));	
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));	
	m_pkAlSys=static_cast<OpenAlSystem*>(g_ZFObjSys.GetObjectPtr("OpenAlSystem"));				
		
	m_kDir.Set(0,0,0);
	
	m_pkStatusProperty=NULL;
	m_pkPlayer=NULL;
	
	m_fChangeTime=m_pkFps->GetTicks();
	m_fHitTime=m_pkFps->GetTicks();	
	m_fDirUpdate=4;
	m_fRotateSpeed=2;
	m_fWalkSpeed=1;	
	m_bActive=false;;	
	
	walksound=new Sound();
	walksound->m_acFile="file:../data/sound/walk.wav";
	walksound->m_bLoop=true;
	
	firesound=new Sound();
	firesound->m_acFile="file:../data/sound/adaptor_fire.wav";
	firesound->m_bLoop=false;

}

AdaptorSniper::~AdaptorSniper()
{

	m_pkAlSys->RemoveSound(walksound);
	m_pkAlSys->RemoveSound(firesound);	
	delete walksound;
	delete firesound;	
}

void AdaptorSniper::Update()
{
	//update sound possition
	walksound->m_kPos=m_pkObject->GetPos();
	m_pkAlSys->AddSound(walksound);


	//get and update status property
	if(m_pkStatusProperty==NULL)
	{
		m_pkStatusProperty=static_cast<StatusProperty*>(m_pkObject->GetProperty("StatusProperty"));
	}
	else
	{
		if(m_pkStatusProperty->m_fArmor < 100) 
			m_bActive=true;		
	
		if(m_pkStatusProperty->m_fHealth < 0){
			cout<<"DIEEEEEEEEEEEED!"<<endl;
			m_pkObjectMan->Delete(m_pkObject);
		}	
	}

	//check direction every m_fDirUpdate second
	if(m_pkFps->GetTicks() - m_fChangeTime >= m_fDirUpdate) 
	{
		m_fChangeTime=m_pkFps->GetTicks();
		m_kDir.Set(0,rand()%360,0);		
	}
	
	//find the player and go after him
	if(m_pkPlayer==NULL)
	{
		m_pkPlayer=m_pkObjectMan->GetObject("Player");
	}
	else
	{
		if((m_pkObject->GetPos() - m_pkPlayer->GetPos()).Length() < 40){		
			m_kDir.y=GetYawAngle(m_pkObject->GetPos() - m_pkPlayer->GetPos())+90;
			
			//activate adaptor
			m_bActive=true;
			
			//fire at player
			if(m_pkFps->GetTicks() - m_fHitTime >= 4) 
			{
				m_fHitTime=m_pkFps->GetTicks();			
								
				Vector3 kAim = m_pkPlayer->GetPos()-m_pkObject->GetPos();
				kAim.Normalize();
				Fire(kAim);
				//Vector3 kAim=m_pkObject->GetRot().AToU();
			}
		}
	}	
	
	
	if(m_bActive)
	{
		if(m_pkObject->GetRot().y>360)
			m_pkObject->GetRot().y-=360;
	
		if(m_pkObject->GetRot().y<0)
			m_pkObject->GetRot().y+=360;
	
	
		m_pkObject->GetRot().y = BestYawTurnDir(m_pkObject->GetRot().y,m_kDir.y,m_fRotateSpeed);
	
		Vector3 vel(0,m_pkObject->GetVel().y,0);
		vel += GetYawVector2(m_pkObject->GetRot().y) * m_fWalkSpeed;
		m_pkObject->GetVel()=vel;
	}
}

void AdaptorSniper::Touch(Object* pkObject)
{
	if(pkObject->GetName() != "HeightMapObject" && pkObject->GetName() != "Player")
	{		
		m_pkObject->GetRot().y+=180;
	}

	if(pkObject->GetName() == "Player")
	{
		
		if(m_pkFps->GetTicks() - m_fHitTime >= 1) 
		{
			m_fHitTime=m_pkFps->GetTicks();

			cout<<"Swish adaptor kicks players ass"<<endl;
			StatusProperty* sp=static_cast<StatusProperty*>(pkObject->GetProperty("StatusProperty"));
			if(sp!=NULL)
			{
				sp->Damage(20);
			}	
		}
	}

}

void AdaptorSniper::Fire(Vector3 kAim)
{
	firesound->m_kPos=m_pkObject->GetPos();
	m_pkAlSys->AddSound(firesound);
	
	Object* Bullet=new Object;
	Bullet->GetName()="MassDriver_Bullet";
	Bullet->GetVel()=kAim*25;
	Bullet->GetPos()=m_pkObject->GetPos()+Vector3(0,0.5,0) + Bullet->GetVel().Unit();		
	Bullet->AddProperty("MachineGunProjectile");		
	
	CrossRenderProperty* cr=static_cast<CrossRenderProperty*>(Bullet->AddProperty("CrossRenderProperty"));
	cr->SetTexture("../data/textures/adaptor_energyball.tga");
	cr->SetScale(Vector3(.3,.3,.3));

	
//	dynamic_cast<ModelProperty*>(Bullet->AddProperty("ModelProperty"))->m_fRadius=0.1;	
	PhysicProperty* sp = static_cast<PhysicProperty*>(Bullet->AddProperty("PhysicProperty"));
	static_cast<CSSphere*>(sp->GetColSphere())->m_fRadius=0.1;
	sp->m_bGravity = false;
	sp->m_bFloat = false;
	
	Bullet->SetParent(m_pkObjectMan->GetWorldObject());

}

vector<PropertyValues> AdaptorSniper::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	kReturn[0].kValueName="m_bActive";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_bActive;
	
	return kReturn;
};




float AdaptorSniper::BestYawTurnDir(float fStart, float fEnd, float fSpeed)
{
	float fNewAngle = fStart;

	if(fNewAngle == fEnd)	
		return fNewAngle;

	float fMove = fEnd - fStart;
	if (fEnd > fStart)
	{
		if (fMove >= 180)		fMove = fMove - 360;
	}
	else
	{
		if (fMove <= -180)		fMove = fMove + 360;
	}
	if (fMove > 0)
	{
		if (fMove > fSpeed)		fMove = fSpeed;
	}
	else
	{
		if (fMove < -fSpeed)	fMove = -fSpeed;
	}

	fNewAngle += fMove;
	return fNewAngle;
}


void AdaptorSniper::Save(ZFMemPackage* pkPackage)
{
	pkPackage->Write((void*)&m_bActive,4);


}

void AdaptorSniper::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read((void*)&m_bActive,4);

}


Property* Create_AdaptorSniper()
{
	return new AdaptorSniper;

}


/*

Vector3 AdaptorSniper::GetYawVector(float fAngleDeg)
{
	Vector3 kYaw;
	kYaw.x = cos(DegToRad(fAngleDeg));	
	kYaw.y = 0;
	kYaw.z = sin(DegToRad(fAngleDeg));	
	return kYaw;
}
*/
