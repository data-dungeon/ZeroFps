// p_ambientsound.h

#include "p_ambientsound.h"
#include "../../engine/object.h"

P_AmbientSound::P_AmbientSound()
{
	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_szFileName = NULL;
	m_bLoop = true;
	m_bStarted = false;
}

P_AmbientSound::~P_AmbientSound()
{
	if(m_szFileName)
	{
		delete[] m_szFileName;
		m_szFileName = NULL;
	}
}

void P_AmbientSound::CloneOf(Property* pkProperty)
{

}

void P_AmbientSound::Update()
{
	Object* pkObject = GetObject();

	Vector3 pos = pkObject->GetWorldPosV();

	// Kolla avståndet till lyssnaren, om det är "tillräckligt" kort
	float fDistanceToListener = (m_pkAudioSystem->GetListnerPos() - pos).Length();

	if(fDistanceToListener < m_fHearableDistance)
	{
		// Är ljudet inte redan startat?
		if(m_bStarted == false)
		{
			if(pkObject && m_szFileName != NULL)
			{
				printf("%f, %f, %f\n", 
					pkObject->GetWorldRotV().x,
					pkObject->GetWorldRotV().y,
					pkObject->GetWorldRotV().z);

				if(m_pkAudioSystem->StartSound(m_szFileName, pkObject->GetWorldPosV(),
					pkObject->GetWorldRotV(), m_bLoop))
				{
					m_bStarted = true;
				}
			}
		}
	}
	else
	{
		if(m_bStarted == true)
		{
			if(pkObject && m_szFileName != NULL)
			{
				// Endast loopade ljud behöver inte stoppas.
				// OBS! Ignorera varning som kommer ifall motorn misslyckas stoppa ljudet.
				// Detta eftersom ljudet i så fall redan har stoppats internt i motorn
				// (dvs. HEARABLE_DISTANCE < fStartDistance)
				if(m_bLoop == true)
					m_pkAudioSystem->StopSound(m_szFileName, pkObject->GetWorldPosV());

				m_bStarted = false;
			}
		}
	}
}

bool P_AmbientSound::SetSound(char *szFileName, bool bPlayOnes, float uiHearableDistance)
{
	if(szFileName == NULL)
		return false;

	if(m_szFileName != NULL)
	{
		delete[] m_szFileName;
		m_szFileName = NULL;
	}

	m_fHearableDistance = uiHearableDistance;
	m_szFileName = new char[strlen(szFileName)+1];
	strcpy(m_szFileName, szFileName);

	m_bLoop = !bPlayOnes;

	return true;
}

Property* Create_AmbientSound()
{
	return new P_AmbientSound();
}