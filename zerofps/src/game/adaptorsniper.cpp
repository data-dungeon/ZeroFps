#include "adaptorsniper.h"


AdaptorSniper::AdaptorSniper()
{
	strcpy(m_acName,"AdaptorSniper");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
		
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));	
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));	
		
	m_kDir.Set(0,0,0);
	
	m_pkStatusProperty=NULL;
	m_pkPlayer=NULL;
	
	m_fChangeTime=m_pkFps->GetTicks();
	m_fHitTime=m_pkFps->GetTicks();	
	m_fDirUpdate=4;
	m_fRotateSpeed=2;
	m_fWalkSpeed=3;	
}


void AdaptorSniper::Update()
{
	if(m_pkStatusProperty==NULL)
	{
		m_pkStatusProperty=static_cast<StatusProperty*>(m_pkObject->GetProperty("StatusProperty"));
	}
	else
	{
		if(m_pkStatusProperty->m_fHealth < 0){
			cout<<"DIEEEEEEEEEEEED!"<<endl;
			m_pkObjectMan->Delete(m_pkObject);
		}	
	}

	if(m_pkFps->GetTicks() - m_fChangeTime >= m_fDirUpdate) 
	{
		m_fChangeTime=m_pkFps->GetTicks();
		m_kDir.Set(0,rand()%360,0);		
	}
	
	if(m_pkPlayer==NULL)
	{
		m_pkPlayer=m_pkObjectMan->GetObject("Player");
	}
	else
	{
		if((m_pkObject->GetPos() - m_pkPlayer->GetPos()).Length() < 30){		
			m_kDir.y=GetYawAngle(m_pkObject->GetPos() - m_pkPlayer->GetPos())+90;
		}
	}	
	
	
	if(m_pkObject->GetRot().y>360)
		m_pkObject->GetRot().y-=360;
	
	if(m_pkObject->GetRot().y<0)
		m_pkObject->GetRot().y+=360;
	
	
	m_pkObject->GetRot().y = BestYawTurnDir(m_pkObject->GetRot().y,m_kDir.y,m_fRotateSpeed);
	
	Vector3 vel(0,m_pkObject->GetVel().y,0);
	vel += GetYawVector2(m_pkObject->GetRot().y) * m_fWalkSpeed;
	m_pkObject->GetVel()=vel;
	


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
