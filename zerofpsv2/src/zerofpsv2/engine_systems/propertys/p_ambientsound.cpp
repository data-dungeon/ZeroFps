// p_ambientsound.h

#include "p_ambientsound.h"
#include "../../engine/entity.h"


P_AmbientSound::P_AmbientSound()
{
	strcpy(m_acName,"P_AmbientSound");

	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	m_bManagedByAudioSystem = true;
	m_bStarted = false;
	m_bSoundHaveBeenSaved = false;
	SetSound("/data/sound/dummy.wav");
	bNetwork = true;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;
}

P_AmbientSound::~P_AmbientSound()
{
	if(m_bManagedByAudioSystem == false)
		Stop();
}

void P_AmbientSound::CloneOf(Property* pkProperty)
{

}

void P_AmbientSound::Update()
{
	if(m_pkFps->m_bServerMode)
		return;

	Entity* pkObject = GetObject();

	Vector3 pos = pkObject->GetWorldPosV();

	// Kolla avståndet till lyssnaren, om det är "tillräckligt" kort
	float fDistanceToListener = (m_pkAudioSystem->GetListnerPos() - pos).Length();

	if(fDistanceToListener < m_fHearableDistance)
	{
		// Eftersom ett P_AmbientSound objekt kan ha förstörts.

		// Är ljudet inte redan startat?
		if(m_bStarted == false)
		{
			if(pkObject && !m_strFileName.empty())
			{
				if(m_pkAudioSystem->StartSound(m_strFileName, pos,
					pkObject->GetVel(), m_bLoop))
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
			if(pkObject && !m_strFileName.empty())
			{
				// Endast loopade ljud behöver inte stoppas.
				// OBS! Ignorera varning som kommer ifall motorn misslyckas stoppa ljudet.
				// Detta eftersom ljudet i så fall redan har stoppats internt i motorn
				// (dvs. HEARABLE_DISTANCE < fStartDistance)
				if(m_bLoop == true)
					m_pkAudioSystem->StopSound(m_strFileName, pkObject->GetWorldPosV());

				m_bStarted = false;
			}
		}
	}
}

bool P_AmbientSound::SetSound(char *szFileName, bool bPlayOnes, float uiHearableDistance)
{
	if(szFileName == NULL)
		return false;

	m_fHearableDistance = uiHearableDistance;
	m_strFileName = szFileName;
	m_bLoop = !bPlayOnes;

	return true;
}

Property* Create_AmbientSound()
{
	return new P_AmbientSound();
}

vector<PropertyValues> P_AmbientSound::GetPropertyValues()
{
	vector<PropertyValues> kReturn(4);

	kReturn[0].kValueName = "FileName";
	kReturn[0].iValueType = VALUETYPE_STRING; 
	kReturn[0].pkValue    = (void*)&m_strFileName;
	
	kReturn[1].kValueName = "AreaSize";
	kReturn[1].iValueType = VALUETYPE_FLOAT;
	kReturn[1].pkValue    = (void*)&m_fHearableDistance;

	kReturn[2].kValueName = "Loop";
	kReturn[2].iValueType = VALUETYPE_BOOL;
	kReturn[2].pkValue    = (void*)&m_bLoop;

	kReturn[3].kValueName = "ManagedByAudioSys";
	kReturn[3].iValueType = VALUETYPE_BOOL;
	kReturn[3].pkValue    = (void*)&m_bManagedByAudioSystem;
	
	return kReturn;
}

void P_AmbientSound::Save(ZFIoInterface* pkFile)
{
	m_bSoundHaveBeenSaved = true;

	pkFile->Write( &m_bSoundHaveBeenSaved,sizeof(bool),1); // have been saved or not
	
	unsigned int uiSize = m_strFileName.length();
	char* szFileName = (char*) m_strFileName.c_str();

	pkFile->Write( &uiSize, sizeof(unsigned int), 1); // number of characters in filename
	pkFile->Write( szFileName, sizeof(char), m_strFileName.size()); // filename
	pkFile->Write( &m_fHearableDistance, sizeof(float), 1); // hearable distance
	pkFile->Write( &m_bLoop, sizeof(bool), 1); // loop
	pkFile->Write( m_pkAudioSystem, sizeof(ZFAudioSystem), 1); // ZFAudioSystem object
	pkFile->Write( m_pkFps, sizeof(ZeroFps), 1); 
	pkFile->Write( &m_bManagedByAudioSystem, sizeof(bool), 1); 
}

void P_AmbientSound::Load(ZFIoInterface* pkFile)
{
	char* szFileName = NULL;
	unsigned int uiFileNameSize;
	
	pkFile->Read( &m_bSoundHaveBeenSaved,sizeof(bool),1); // have been saved or not
	pkFile->Read( &uiFileNameSize, sizeof(int), 1); // number of characters in filename

	szFileName = new char[uiFileNameSize+1];

	pkFile->Read( szFileName, sizeof(char), uiFileNameSize); // filename
	szFileName[uiFileNameSize] = '\0';
	
	m_strFileName = szFileName;
	
	pkFile->Read( &m_fHearableDistance, sizeof(float), 1); // hearable distance
	pkFile->Read( &m_bLoop, sizeof(bool), 1); // loop
	pkFile->Read( m_pkAudioSystem, sizeof(ZFAudioSystem), 1); // ZFAudioSystem object
	pkFile->Read( m_pkFps, sizeof(ZeroFps), 1); 
	pkFile->Read( &m_bManagedByAudioSystem, sizeof(bool), 1); 
	
	if(szFileName)
		delete[] szFileName;
}

void P_AmbientSound::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
   pkNetPacket->Write_Str( m_strFileName.c_str()); // write filename
	pkNetPacket->Write(&m_fHearableDistance,sizeof(m_fHearableDistance)); // write hearable distance
	pkNetPacket->Write(&m_bLoop,sizeof(m_bLoop)); // loop or not
	pkNetPacket->Write(&m_bManagedByAudioSystem, sizeof(m_bManagedByAudioSystem)); 
	pkNetPacket->Write(&m_bSoundHaveBeenSaved, sizeof(m_bSoundHaveBeenSaved)); 
}

void P_AmbientSound::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	char file_name[128];

   pkNetPacket->Read_Str( file_name); // write filename
	pkNetPacket->Read(&m_fHearableDistance,sizeof(m_fHearableDistance)); // write hearable distance
	pkNetPacket->Read(&m_bLoop,sizeof(m_bLoop)); // loop or not
	pkNetPacket->Read(&m_bManagedByAudioSystem,sizeof(m_bManagedByAudioSystem)); // loop or not
	pkNetPacket->Read(&m_bSoundHaveBeenSaved, sizeof(m_bSoundHaveBeenSaved)); 
	
	m_strFileName = file_name;
}

void P_AmbientSound::Stop()
{
	Entity* pkObject = GetObject();
	m_pkAudioSystem->StopSound(m_strFileName, pkObject->GetWorldPosV());
	m_bStarted = false;
}




/////////////////////// P_Sound

P_Sound::P_Sound()
{
	bNetwork = true;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;

	strcpy(m_acName,"P_Sound");
	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_strFileName = "";
}

P_Sound::~P_Sound()
{

}

void P_Sound::Update()
{
	if(!m_pkFps->m_bServerMode)
	{
		if(!m_strFileName.empty())
		{
			Entity* pkObject = GetObject();

			m_pkAudioSystem->StartSound(m_strFileName, 
				pkObject->GetWorldPosV(), pkObject->GetVel(), false);

			m_strFileName = "";
		}
	}
}

void P_Sound::Play(string strName) // körs endast på server sidan
{
	m_strFileName = strName;
}

void P_Sound::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write_Str( m_strFileName.c_str()); // write filename
	m_strFileName = "";
}

void P_Sound::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	char file_name[128];
	pkNetPacket->Read_Str(file_name); // read filename
	m_strFileName = file_name;
}

Property* Create_SoundProperty()
{
	return new P_Sound();
}