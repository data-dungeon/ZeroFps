// p_ambientsound.h

#include "p_ambientsound.h"
#include "../../engine/object.h"

P_AmbientSound::P_AmbientSound()
{
	strcpy(m_acName,"P_AmbientSound");

	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	//m_szFileName = NULL;
	m_bStarted = false;
	m_bSoundHaveBeenSaved = false;
	SetSound("/data/sound/dummy.wav");
}

P_AmbientSound::~P_AmbientSound()
{
/*	if(m_szFileName)
	{
		delete[] m_szFileName;
		m_szFileName = NULL;
	}*/
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
		// Eftersom ett P_AmbientSound objekt kan ha förstörts.


		// Är ljudet inte redan startat?
		if(m_bStarted == false)
		{
			if(pkObject && !m_strFileName.empty())//m_szFileName != NULL)
			{
				if(m_pkAudioSystem->StartSound(m_strFileName/*m_szFileName*/, pkObject->GetWorldPosV(),
					pkObject->GetVel(), m_bLoop))
				{
					m_bStarted = true;
					//printf("--------- Sound have started ---------- \n");
				}
			}
		}
	}
	else
	{
		if(m_bStarted == true)
		{
			if(pkObject && !m_strFileName.empty())//m_szFileName != NULL)
			{
				// Endast loopade ljud behöver inte stoppas.
				// OBS! Ignorera varning som kommer ifall motorn misslyckas stoppa ljudet.
				// Detta eftersom ljudet i så fall redan har stoppats internt i motorn
				// (dvs. HEARABLE_DISTANCE < fStartDistance)
				if(m_bLoop == true)
					m_pkAudioSystem->StopSound(m_strFileName/*m_szFileName*/, pkObject->GetWorldPosV());

				m_bStarted = false;
			}
		}
	}
}

bool P_AmbientSound::SetSound(char *szFileName, bool bPlayOnes, float uiHearableDistance)
{
	if(szFileName == NULL)
		return false;
/*
	if(m_szFileName != NULL)
	{
		delete[] m_szFileName;
		m_szFileName = NULL;
	}*/

	m_fHearableDistance = uiHearableDistance;
	//m_szFileName = new char[strlen(szFileName)+1];

	m_strFileName = szFileName;
	//strcpy(m_szFileName, szFileName);

	m_bLoop = !bPlayOnes;

	return true;
}

Property* Create_AmbientSound()
{
	return new P_AmbientSound();
}

vector<PropertyValues> P_AmbientSound::GetPropertyValues()
{
	vector<PropertyValues> kReturn(3);

	kReturn[0].kValueName = "FileName";
	kReturn[0].iValueType = VALUETYPE_STRING; //VALUETYPE_CHARS;
	kReturn[0].pkValue    = (void*)&m_strFileName;//m_szFileName;
	
	kReturn[1].kValueName = "AreaSize";
	kReturn[1].iValueType = VALUETYPE_FLOAT;
	kReturn[1].pkValue    = (void*)&m_fHearableDistance;

	kReturn[2].kValueName = "Loop";
	kReturn[2].iValueType = VALUETYPE_BOOL;
	kReturn[2].pkValue    = (void*)&m_bLoop;
	
	return kReturn;
}

void P_AmbientSound::Save(ZFIoInterface* pkFile)
{
	//printf("P_AmbientSound::Save!\n");
	m_bSoundHaveBeenSaved = true;

	pkFile->Write( &m_bSoundHaveBeenSaved,sizeof(bool),1); // have been saved or not
	
	unsigned int uiSize = m_strFileName.length();
	char* szFileName = (char*) m_strFileName.c_str();

	pkFile->Write( &uiSize, sizeof(unsigned int), 1); // number of characters in filename
	pkFile->Write( szFileName, sizeof(char), m_strFileName.size()); // filename
	pkFile->Write( &m_fHearableDistance, sizeof(float), 1); // hearable distance
	pkFile->Write( &m_bLoop, sizeof(bool), 1); // loop
	pkFile->Write( &m_bStarted, sizeof(bool), 1); // started or not
	pkFile->Write( m_pkAudioSystem, sizeof(ZFAudioSystem), 1); // ZFAudioSystem object
}

void P_AmbientSound::Load(ZFIoInterface* pkFile)
{
	//printf("P_AmbientSound::Load!\n");

	char* szFileName = NULL;
	unsigned int uiFileNameSize;
	
	pkFile->Read( &m_bSoundHaveBeenSaved,sizeof(bool),1); // have been saved or not
	pkFile->Read( &uiFileNameSize, sizeof(int), 1); // nummber of characters in filename

	szFileName = new char[uiFileNameSize+1];

	pkFile->Read( szFileName, sizeof(char), uiFileNameSize); // filename
	szFileName[uiFileNameSize] = '\0';
	
	m_strFileName = szFileName;
	
	pkFile->Read( &m_fHearableDistance, sizeof(float), 1); // hearable distance
	pkFile->Read( &m_bLoop, sizeof(bool), 1); // loop
	pkFile->Read( &m_bStarted, sizeof(bool), 1); // started or not
	pkFile->Read( m_pkAudioSystem, sizeof(ZFAudioSystem), 1); // ZFAudioSystem object

	if(m_bSoundHaveBeenSaved)
	{
		Object* pkObject = GetObject();
		m_pkAudioSystem->StopSound(m_strFileName, pkObject->GetWorldPosV());
		m_bSoundHaveBeenSaved = false;
		m_bStarted = false;
	}	

	if(szFileName)
		delete[] szFileName;
}