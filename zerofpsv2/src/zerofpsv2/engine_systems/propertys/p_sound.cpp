#include "p_sound.h"
#include "../../engine/entity.h"

P_Sound::P_Sound()
{
	bNetwork = true;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;

	strcpy(m_acName,"P_Sound");
	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
}

P_Sound::~P_Sound()
{
	Entity* pkEnt = GetEntity();

	// stoppa alla ljud när propertyt förstörs
	for(int i=0; i<m_kSounds.size(); i++) 
	{
		if(m_kSounds[i].m_bPlaying)
		{
			m_pkAudioSystem->StopSound(m_kSounds[i].m_strFileName, 
				pkEnt->GetIWorldPosV());
		}
	}
}

void P_Sound::Update()
{
	Entity* pkEnt = GetEntity();

	for(int i=0; i<m_kSounds.size(); i++) // kolla av actions (starta, stoppa ljud)
	{
		if(m_kSounds[i].m_eAction == SA_START_SOUND)
		{
			if(!m_pkFps->m_bServerMode)
				m_pkAudioSystem->StartSound(m_kSounds[i].m_strFileName, 
					pkEnt->GetIWorldPosV(), pkEnt->GetVel(), m_kSounds[i].m_bLoop);

			m_kSounds[i].m_eAction = SA_DO_NOTHING;

			if(m_kSounds[i].m_bLoop)
				m_kSounds[i].m_bPlaying = true;

			SetNetUpdateFlag(false);
		}
		else
		if(m_kSounds[i].m_eAction == SA_STOP_SOUND)
		{
			if(!m_pkFps->m_bServerMode)
				m_pkAudioSystem->StopSound(m_kSounds[i].m_strFileName, 
					pkEnt->GetIWorldPosV());

			m_kSounds[i].m_eAction = SA_DO_NOTHING;
			m_kSounds[i].m_bPlaying = false;

			SetNetUpdateFlag(false);
		}
	}

	static Vector3 prevpos = Vector3(-9999,-9999,-9999);
	Vector3 currpos = pkEnt->GetWorldPosV();

	if(!prevpos.NearlyEquals(currpos,0.1f))
	//if(!pkEnt->GetVel().NearlyZero(1)) // om objektet har rört sig, flytta ljudet
	{
		for(int i=0; i<m_kSounds.size(); i++)
			if(m_kSounds[i].m_bPlaying)
			{
				if(!m_pkFps->m_bServerMode)
					m_pkAudioSystem->MoveSound(m_kSounds[i].m_strFileName.c_str(), 
						pkEnt->GetIWorldPosV(), pkEnt->GetIWorldPosV(), pkEnt->GetVel());
			}
	}	

	prevpos = currpos;
}

void P_Sound::StartSound(string strName, bool bLoop) 
{
	bool bCreateNew = true;

	for(int i=0; i<m_kSounds.size(); i++)
		if(m_kSounds[i].m_strFileName == strName)
		{
			m_kSounds[i].m_eAction = SA_START_SOUND;
			m_kSounds[i].m_bLoop = bLoop;
			bCreateNew = false;
			break;
		}

	if(bCreateNew)
	{
		sound_info kNewSound;
		kNewSound.m_strFileName = strName;
		kNewSound.m_bLoop = bLoop;
		kNewSound.m_bPlaying = false;
		kNewSound.m_eAction = SA_START_SOUND;
		m_kSounds.push_back(kNewSound);
	}

	SetNetUpdateFlag(true);
}

void P_Sound::StopSound(string strName)
{
	for(int i=0; i<m_kSounds.size(); i++)
		if(m_kSounds[i].m_strFileName == strName)
		{
			m_kSounds[i].m_eAction = SA_STOP_SOUND;
			break;
		}

	SetNetUpdateFlag(true);
}

void P_Sound::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	SetNetUpdateFlag(iConnectionID,false);

	int iNumElements = m_kSounds.size();
	pkNetPacket->Write(&iNumElements, sizeof(int)); // number of sounds

	for(int i=0; i<m_kSounds.size(); i++)
	{
		pkNetPacket->Write_Str( m_kSounds[i].m_strFileName.c_str()); // write filename
		pkNetPacket->Write(&m_kSounds[i].m_bLoop, sizeof(m_kSounds[i].m_bLoop)); // loop or not
		pkNetPacket->Write(&m_kSounds[i].m_bPlaying, sizeof(m_kSounds[i].m_bPlaying)); // playing or not
		pkNetPacket->Write(&m_kSounds[i].m_eAction, sizeof(SoundAction)); // playing or not
	}
}

void P_Sound::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	m_kSounds.clear(); 

	int iNumElements;
	pkNetPacket->Read(&iNumElements, sizeof(int)); // number of sounds

	char file_name[128];

	for(int i=0; i<iNumElements; i++)
	{
		sound_info sf;

		pkNetPacket->Read_Str( file_name ); // read filename
		sf.m_strFileName = file_name;

		pkNetPacket->Read(&sf.m_bLoop, sizeof(sf.m_bLoop)); // loop or not
		pkNetPacket->Read(&sf.m_bPlaying, sizeof(sf.m_bPlaying)); // playing or not
		pkNetPacket->Read(&sf.m_eAction, sizeof(SoundAction)); // playing or not

		m_kSounds.push_back(sf); 
	}
}

vector<PropertyValues> P_Sound::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
	SetNetUpdateFlag(true);
	return kReturn;
}

void P_Sound::Save(ZFIoInterface* pkFile)
{
	int iNumElements = m_kSounds.size();
	pkFile->Write( &iNumElements, sizeof(int), 1); // number of elements

	char temp[128];

	for(int i=0; i<m_kSounds.size(); i++)
	{
		strcpy(temp,m_kSounds[i].m_strFileName.c_str());
		pkFile->Write( temp,128,1 ); // filename
		pkFile->Write( &m_kSounds[i].m_bLoop, sizeof(bool), 1); // loop
		pkFile->Write( &m_kSounds[i].m_bPlaying, sizeof(bool), 1); // playing or not
	}
}

void P_Sound::Load(ZFIoInterface* pkFile,int iVersion)
{
	char temp[128];

	int iNumElements;
	pkFile->Read( &iNumElements, sizeof(int), 1); // number of elements

	for(int i=0; i<iNumElements; i++)
	{
		sound_info sf;

		pkFile->Read(temp,128,1); sf.m_strFileName = temp; // filename
		pkFile->Read( &sf.m_bLoop, sizeof(bool), 1); // loop
		pkFile->Read( &sf.m_bPlaying, sizeof(bool), 1); // playing or not

		sf.m_eAction = SA_DO_NOTHING;

		m_kSounds.push_back(sf); 
	}
		
	SetNetUpdateFlag(true);		
}


Property* Create_SoundProperty()
{
	return new P_Sound();
}
