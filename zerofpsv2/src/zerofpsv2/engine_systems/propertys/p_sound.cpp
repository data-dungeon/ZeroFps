#include "p_sound.h"
#include "../../engine/entity.h"

P_Sound::P_Sound()
{
	bNetwork = true;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;

	strcpy(m_acName,"P_Sound");
	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pEntityMan = static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));

	m_kPrevpos = Vector3(-9999,-9999,-9999);

	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		m_akSounds[i].m_strFileName = "";
		m_akSounds[i].m_bStarted = false;
	}
}

P_Sound::~P_Sound()
{
	Entity* pkEnt = GetEntity();
	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		if(!m_akSounds[i].m_strFileName.empty())
			m_pkAudioSystem->StopSound(m_akSounds[i].m_strFileName, pkEnt->GetIWorldPosV());
	}
}

void P_Sound::Update()
{
	if(m_pEntityMan->IsUpdate(PROPERTY_SIDE_CLIENT))
	{
		Entity* pkEnt = GetEntity();
		Vector3 currpos = pkEnt->GetWorldPosV();

		// Flytta på loopade ljud
		if(!m_kPrevpos.NearlyEquals(currpos,0.1f))
		{
			for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
			{
				if(!m_akSounds[i].m_strFileName.empty())
				{
					if(m_akSounds[i].m_bStarted && m_akSounds[i].m_bLoop)
						m_pkAudioSystem->MoveSound(m_akSounds[i].m_strFileName.c_str(), 
							pkEnt->GetIWorldPosV(), pkEnt->GetIWorldPosV(), pkEnt->GetVel());
				}
			}

			m_kPrevpos = currpos;
		}

		// Starta ljud
		for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
		{
			if(!m_akSounds[i].m_strFileName.empty())
			{
				if(m_akSounds[i].m_bStarted == false)
				{
					m_akSounds[i].m_bStarted = true;
					m_pkAudioSystem->StartSound(m_akSounds[i].m_strFileName, 
						pkEnt->GetIWorldPosV(), pkEnt->GetVel(), m_akSounds[i].m_bLoop);
				}
			}
		}
	}
}

// körs på serven
void P_Sound::StartSound(string strName, bool bLoop) 
{
	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		if(m_akSounds[i].m_strFileName.empty())
		{
			m_akSounds[i].m_strFileName = strName;
			m_akSounds[i].m_bLoop = bLoop;
			m_akSounds[i].m_bStarted = false;
			break;
		}
	}

	SetNetUpdateFlag(true);
}

// körs på serven
void P_Sound::StopSound(string strName)
{
	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		if(m_akSounds[i].m_strFileName == strName)
		{
			m_akSounds[i].m_strFileName = "";
			break;
		}
	}

	SetNetUpdateFlag(true);
}

// Skicka iväg från serven?
void P_Sound::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		pkNetPacket->Write_Str( m_akSounds[i].m_strFileName); 
		pkNetPacket->Write(m_akSounds[i].m_bLoop); 
		pkNetPacket->Write(m_akSounds[i].m_bStarted); 
	}

	SetNetUpdateFlag(iConnectionID,false);
	
	// Alla klienter har fått sina ljud startade/stoppade.
	// Behåll alla loopade ljud och markera att dom har blivit startade.
	// För om dom tas bort så kommer dom stoppas nästa gång klienten får ett PackFrom
	// meddelande (när StopSound körs). Registeras dom inte som startade kommer dom
	// starta pånytt och då blir det dubbla loopade ljud som spelas.
	if(AllNetUpdateFlagsFalse())
	{
		for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
		{
			if(!m_akSounds[i].m_strFileName.empty())
			{
				if(m_akSounds[i].m_bLoop)
				{
					m_akSounds[i].m_bStarted = true;
				}
				else
				{
					m_akSounds[i].m_strFileName = "";
				}
			}
		}
	}

}

void P_Sound::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	sound_info akSoundsOnServer[MAX_NUM_PROPERTY_SOUND];

	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		pkNetPacket->Read_Str(akSoundsOnServer[i].m_strFileName);
		pkNetPacket->Read(akSoundsOnServer[i].m_bLoop); 
		pkNetPacket->Read(akSoundsOnServer[i].m_bStarted); 
	}

	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		if(!m_akSounds[i].m_strFileName.empty())
		{
			bool bExist = false;

			for(int j=0; j<MAX_NUM_PROPERTY_SOUND; j++)
			{
				if(!akSoundsOnServer[j].m_strFileName.empty())
				{
					if(akSoundsOnServer[j].m_strFileName == m_akSounds[i].m_strFileName)
					{
						bExist = true;
						break;
					}
				}
			}

			// Detta ljud finns inte längre på serven. Det har stoppats. Därför skall det stoppas oxå på klienten.
			if(bExist == false)
				m_pkAudioSystem->StopSound(m_akSounds[i].m_strFileName, GetEntity()->GetIWorldPosV());
		}
	}

	// Uppdatera klientens ljudlista så den stämmer överens med servens.
	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		m_akSounds[i].m_strFileName = akSoundsOnServer[i].m_strFileName;
		m_akSounds[i].m_bLoop = akSoundsOnServer[i].m_bLoop;
		m_akSounds[i].m_bStarted = akSoundsOnServer[i].m_bStarted;
	}

}

vector<PropertyValues> P_Sound::GetPropertyValues()
{
	vector<PropertyValues> kReturn(MAX_NUM_PROPERTY_SOUND*2);

	char szValue[5];
	int counter=0;

	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		sprintf(szValue, "%i", i+1);

		kReturn[counter].kValueName = string("file") + string(szValue);
		kReturn[counter].iValueType = VALUETYPE_STRING; 
		kReturn[counter].pkValue    = (void*)&(m_akSounds[counter].m_strFileName);
		counter++;
		
		kReturn[counter].kValueName = string("loop") + string(szValue);
		kReturn[counter].iValueType = VALUETYPE_BOOL;
		kReturn[counter].pkValue    = (void*)&(m_akSounds[counter].m_bLoop);
		counter++;
	}

	SetNetUpdateFlag(true);
	return kReturn;
}

void P_Sound::Save(ZFIoInterface* pkFile)
{
	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		pkFile->Write_Str( m_akSounds[i].m_strFileName );
		pkFile->Write( &m_akSounds[i].m_bLoop, sizeof(m_akSounds[i].m_bLoop), 1 ); 
	}
}

void P_Sound::Load(ZFIoInterface* pkFile,int iVersion)
{
	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		pkFile->Read_Str( m_akSounds[i].m_strFileName ); 
		pkFile->Read( m_akSounds[i].m_bLoop );
	}
	SetNetUpdateFlag(true);
}

Property* Create_SoundProperty()
{
	return new P_Sound();
}