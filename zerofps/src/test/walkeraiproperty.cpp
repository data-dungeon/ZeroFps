#include "walkeraiproperty.h"


WalkerAIProperty::WalkerAIProperty()
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkMap = static_cast<HeightMap*>(g_ZFObjSys.GetObjectPtr("HeightMap"));	

	strcpy(m_acName,"WalkerAiPropery");	
	m_pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));

	m_iState = AIWALKER_IDLE;
	m_kWaitTime = m_pkFps->GetGameTime() + rand() % 10 + 2;
}

Vector3 GetYawVector(float fAngleDeg)
{
	Vector3 kYaw;
	kYaw.x = sin(DegToRad(fAngleDeg));			
	kYaw.z = cos(DegToRad(fAngleDeg));	
	return kYaw;
}

float GetYawAngle(Vector3 kDir)
{
	float fAngle =  atan2(kDir.x, - kDir.z);
	return RadToDeg(fAngle);
}

float GetYawAngleToTarget(Vector3 kPosition, Vector3 kTargetPos)
{
	Vector3 diff = kPosition - kTargetPos;
	return -GetYawAngle(diff);
}

float BestYawTurnDir(float fStart, float fEnd, float fSpeed)
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


void WalkerAIProperty::Update() 
{
	float speed = 4;
	Vector3 kShowDirYaw = GetYawVector(m_pkObject->GetRot().y);
	Render *pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));
	pkRender->Line(m_pkObject->GetPos() + Vector3(0,0.7,0), (m_pkObject->GetPos() + kShowDirYaw * 1) + Vector3(0,0.7,0));

	/*if(m_pkInput->Pressed(KEY_Z)){
		m_pkObject->GetRot().y -= 2;
	}
	if(m_pkInput->Pressed(KEY_C)){
		m_pkObject->GetRot().y += 2;
	}

	m_pkFps->DevPrintf("Walker: State = %i Wait = %f", m_iState, m_kWaitTime - m_pkFps->GetGameTime());

	MadProperty* madp = dynamic_cast<MadProperty*>(m_pkObject->GetProperty("MadProperty"));
	if(m_pkInput->Pressed(KEY_X)){
		m_pkObject->GetPos() += kYaw * m_pkFps->GetFrameTime()*speed;
		if(m_iState == AIWALKER_IDLE) {
			madp->PlayAnimation(4,0);
			m_iState = AIWALKER_WALK;
			}
	}
	else if(m_iState == AIWALKER_WALK) {
		m_iState = AIWALKER_IDLE;
		madp->PlayAnimation(0,0);
		}


	m_pkFps->DevPrintf("Walker: <%.3f,%.3f,%.3f>, <%.3f,%.3f,%.3f>", 
		m_pkObject->GetPos().x, m_pkObject->GetPos().y,m_pkObject->GetPos().z,
		m_pkObject->GetRot().x, m_pkObject->GetRot().y,m_pkObject->GetRot().z);
	*/

	if(m_iState == AIWALKER_IDLE) {
		if(m_pkFps->GetGameTime() < m_kWaitTime)
			return;

		// Move to walk state.
		m_kTargetPos.Set(rand()%100 + 300,0,rand()%100 + 720);
		m_kTargetPos.y = m_pkMap->Height(m_kTargetPos.x,m_kTargetPos.z);
		m_kTargetPos.Print();

		m_iState = AIWALKER_WALK;
		MadProperty* madp = dynamic_cast<MadProperty*>(m_pkObject->GetProperty("MadProperty"));
		madp->PlayAnimation(4,0);
	}

	Vector3 diff = m_pkObject->GetPos() - m_kTargetPos;
	if(diff.Length() < 5 ) {
		m_iState = AIWALKER_IDLE;
		m_kWaitTime = m_pkFps->GetGameTime() + rand() % 10 + 2;
		return;
		}


	m_pkFps->DevPrintf("Dist: %f", diff.Length());
	
	WalkerObject *bunny = dynamic_cast<WalkerObject*>(m_pkObject);

	float fGoalAngle = GetYawAngleToTarget(m_pkObject->GetPos(), m_kTargetPos);
	m_pkObject->GetRot().y = BestYawTurnDir(m_pkObject->GetRot().y, fGoalAngle, 3 );
	Vector3 kYaw = GetYawVector(m_pkObject->GetRot().y);
	m_pkObject->GetPos() += kYaw * m_pkFps->GetFrameTime()*speed;
}










