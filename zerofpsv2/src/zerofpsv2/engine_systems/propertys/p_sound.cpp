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
		pkNetPacket->Write_Str( m_akSounds[i].m_strFileName); // filename
		pkNetPacket->Write(m_akSounds[i].m_bLoop); // loop or not
		pkNetPacket->Write(m_akSounds[i].m_bStarted); // loop or not
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

// Serven meddelar objektet med detta property vilka nya ljud som den skall spela upp.
// Kolla av mot den gamla listan och stoppa dom ljud inte längre finns.
// Starta dom ljud som skall spelas upp.
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

			if(bExist == false)
				m_pkAudioSystem->StopSound(m_akSounds[i].m_strFileName, GetEntity()->GetIWorldPosV());
		}
	}

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

	for(int i=0; i<MAX_NUM_PROPERTY_SOUND; i++)
	{
		char szValue[5];
		sprintf(szValue, "%i", i+1);

		kReturn[i*2].kValueName = string("file") + string(szValue);
		kReturn[i*2].iValueType = VALUETYPE_STRING; 
		kReturn[i*2].pkValue    = (void*)&(m_akSounds[i*2].m_strFileName);
		
		kReturn[i*2+1].kValueName = string("loop") + string(szValue);
		kReturn[i*2+1].iValueType = VALUETYPE_BOOL;
		kReturn[i*2+1].pkValue    = (void*)&(m_akSounds[i*2].m_bLoop);
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Version 2 - Funkade konstig med gamla banor
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

P_Sound::P_Sound()
{
	bNetwork = true;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;

	strcpy(m_acName,"P_Sound");
	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pEntityMan = static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));

	m_kPrevpos = Vector3(-9999,-9999,-9999);
}

P_Sound::~P_Sound()
{
	Entity* pkEnt = GetEntity();
	for(int i=0; i<m_kSounds.size(); i++)
		m_pkAudioSystem->StopSound(m_kSounds[i].m_strFileName, pkEnt->GetIWorldPosV());
}

void P_Sound::Update()
{
	// Flytta på loopade ljud
	if(m_pEntityMan->IsUpdate(PROPERTY_SIDE_CLIENT))
	{
		Entity* pkEnt = GetEntity();
		Vector3 currpos = pkEnt->GetWorldPosV();

		if(!m_kPrevpos.NearlyEquals(currpos,0.1f))
		{
			for(int i=0; i<m_kSounds.size(); i++)
			{
				if(m_kSounds[i].m_bLoop)
				{
					m_pkAudioSystem->MoveSound(m_kSounds[i].m_strFileName.c_str(), 
						pkEnt->GetIWorldPosV(), pkEnt->GetIWorldPosV(), pkEnt->GetVel());
				}
			}

			m_kPrevpos = currpos;
		}
	}

}

// körs på serven
void P_Sound::StartSound(string strName, bool bLoop) 
{
	sound_info si;
	si.m_strFileName = strName;
	si.m_bLoop = bLoop;
	m_kSounds.push_back(si); 

	SetNetUpdateFlag(true);
}

// körs på serven
void P_Sound::StopSound(string strName)
{
	for(int i=0; i<m_kSounds.size(); i++)
		if(m_kSounds[i].m_strFileName == strName)
		{
			m_kSounds.erase( m_kSounds.begin() + i);
			break;
		}

	SetNetUpdateFlag(true);
}

// Skicka iväg från serven?
void P_Sound::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	int iNumSounds = m_kSounds.size();
	pkNetPacket->Write(iNumSounds); 

	for(int i=0; i<iNumSounds; i++)
	{
		pkNetPacket->Write_Str( m_kSounds[i].m_strFileName); // filename
		pkNetPacket->Write(m_kSounds[i].m_bLoop); // loop or not
	}

	SetNetUpdateFlag(iConnectionID,false);
	
	if(AllNetUpdateFlagsFalse())
	{
		///// Ta bort alla icke-loopade ljud på serven
		vector<sound_info> temp;

		for(int i=0; i<m_kSounds.size(); i++)
			if(m_kSounds[i].m_bLoop)
				temp.push_back( m_kSounds[i]);

		m_kSounds.clear();
		for(int i=0; i<temp.size(); i++)
			m_kSounds.push_back(temp[i]);
	}

}

// Serven meddelar objektet med detta property vilka nya ljud som den skall spela upp.
// Kolla av mot den gamla listan och stoppa dom ljud inte längre finns.
// Starta dom ljud som skall spelas upp.
void P_Sound::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{

	vector<sound_info> kSoundsOnServer;
	
	int iNumSounds;
	pkNetPacket->Read(iNumSounds); 

	for(int i=0; i<iNumSounds; i++)
	{
		sound_info si;
		pkNetPacket->Read_Str( si.m_strFileName );
		pkNetPacket->Read(si.m_bLoop); 
		kSoundsOnServer.push_back(si); 
	}

	Entity* pkEnt = GetEntity();

	// Stoppa ljud som inte längre finns på serven
	for(int i=0; i<m_kSounds.size(); i++)
	{
		bool bExistOnServer = false;

		for(int j=0; j<kSoundsOnServer.size(); j++)
			if(kSoundsOnServer[j].m_strFileName == m_kSounds[i].m_strFileName)
			{
				bExistOnServer = true;
				break;
			}

		if(bExistOnServer == false)
			m_pkAudioSystem->StopSound(m_kSounds[i].m_strFileName, pkEnt->GetIWorldPosV());
		
	}

	//Spela upp nya ljud som finns på serven men inte klienten
	for(int i=0; i<kSoundsOnServer.size(); i++)
	{
		bool bExistOnClient = false;

		for(int j=0; j<m_kSounds.size(); j++)
			if(m_kSounds[j].m_strFileName == kSoundsOnServer[i].m_strFileName)
			{
				bExistOnClient = true;
				break;
			}

		if(bExistOnClient == false)
			m_pkAudioSystem->StartSound(kSoundsOnServer[i].m_strFileName, 
				pkEnt->GetIWorldPosV(), pkEnt->GetVel(), kSoundsOnServer[i].m_bLoop);
	}

	m_kSounds.clear(); // radera den gamla listan

	// ... och kopiera in dom nya ljuden
	for(int i=0; i<kSoundsOnServer.size(); i++)
		m_kSounds.push_back( kSoundsOnServer[i] );

	/// Ta bort alla icke-loopade ljud på klienten
	vector<sound_info> temp;

	for(int i=0; i<m_kSounds.size(); i++)
		if(m_kSounds[i].m_bLoop)
			temp.push_back( m_kSounds[i]);

	m_kSounds.clear();
	for(int i=0; i<temp.size(); i++)
		m_kSounds.push_back(temp[i]);

}

vector<PropertyValues> P_Sound::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
	SetNetUpdateFlag(true);
	return kReturn;
}

void P_Sound::Save(ZFIoInterface* pkFile)
{
}

void P_Sound::Load(ZFIoInterface* pkFile,int iVersion)
{
}

Property* Create_SoundProperty()
{
	return new P_Sound();
}


*/







/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Version 1 - Funkade bra, men klarade bara 1 ljud åt gången
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

P_Sound::P_Sound()
{
	bNetwork = true;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;

	strcpy(m_acName,"P_Sound");
	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pEntityMan = static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));

	m_strFileName = "";
	m_strStopFileName = "";
	m_bLoop=false;

	m_bStarted = false;
	m_kPrevpos = Vector3(-9999,-9999,-9999);
}

P_Sound::~P_Sound()
{
	if(!m_strFileName.empty())
	{
		Entity* pkEnt = GetEntity();
		m_pkAudioSystem->StopSound(m_strFileName, pkEnt->GetIWorldPosV());
		m_strStopFileName = "";
		m_strFileName = "";

		m_bStarted = false;
		m_kPrevpos = Vector3(-9999,-9999,-9999);
	}
}

void P_Sound::Update()
{
	if(m_pEntityMan->IsUpdate(PROPERTY_SIDE_CLIENT))
	{
		if(!m_strFileName.empty())
		{
			Entity* pkEnt = GetEntity();

			if(m_bLoop == false)
			{
				m_pkAudioSystem->StartSound(m_strFileName, 
					pkEnt->GetIWorldPosV(), pkEnt->GetVel(), false);
				m_strFileName = "";
			}
			else
			{
				Vector3 currpos = pkEnt->GetWorldPosV();

				if(m_bStarted == false)
				{
					m_pkAudioSystem->StartSound(m_strFileName, 
						pkEnt->GetIWorldPosV(), pkEnt->GetVel(), true);
					m_bStarted = true;
				}
				else
				{
					if(!m_kPrevpos.NearlyEquals(currpos,0.1f))
					{
						m_pkAudioSystem->MoveSound(m_strFileName.c_str(), 
							pkEnt->GetIWorldPosV(), pkEnt->GetIWorldPosV(), pkEnt->GetVel());
						m_kPrevpos = pkEnt->GetIWorldPosV();
					}
				}
			}
		}

		if(!m_strStopFileName.empty())
		{
			Entity* pkEnt = GetEntity();
			m_pkAudioSystem->StopSound(m_strStopFileName, pkEnt->GetIWorldPosV());
			m_strStopFileName = "";
			m_strFileName = "";

			m_bStarted = false;
			m_kPrevpos = Vector3(-9999,-9999,-9999);
		}
	}
}

void P_Sound::StartSound(string strName, bool bLoop) 
{
	if(m_strFileName != strName)
		m_strStopFileName = m_strFileName;
	else
		m_strStopFileName = "";

	m_strFileName = strName;
	m_bLoop = bLoop;
	SetNetUpdateFlag(true);
}

void P_Sound::StopSound(string strName)
{
	m_strFileName = "";
	m_strStopFileName = strName;
	SetNetUpdateFlag(true);
}

void P_Sound::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write_Str( m_strFileName.c_str()); // filename
	pkNetPacket->Write(&m_bLoop, sizeof(m_bLoop)); // loop or not
	pkNetPacket->Write_Str( m_strStopFileName.c_str()); // filename
	SetNetUpdateFlag(iConnectionID,false);
}

void P_Sound::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	char file_name[128];
	pkNetPacket->Read_Str( file_name ); m_strFileName = file_name;
	pkNetPacket->Read(&m_bLoop, sizeof(m_bLoop)); 
	pkNetPacket->Read_Str( file_name ); m_strStopFileName = file_name;
}

vector<PropertyValues> P_Sound::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);

	kReturn[0].kValueName = "FileName";
	kReturn[0].iValueType = VALUETYPE_STRING; 
	kReturn[0].pkValue    = (void*)&m_strFileName;
	
	kReturn[1].kValueName = "Loop";
	kReturn[1].iValueType = VALUETYPE_BOOL;
	kReturn[1].pkValue    = (void*)&m_bLoop;

	SetNetUpdateFlag(true);
	return kReturn;
}

void P_Sound::Save(ZFIoInterface* pkFile)
{
	pkFile->Write_Str( m_strFileName );
	pkFile->Write( &m_bLoop, sizeof(m_bLoop), 1 ); 
}

void P_Sound::Load(ZFIoInterface* pkFile,int iVersion)
{
	pkFile->Read_Str( m_strFileName ); 
	pkFile->Read( m_bLoop );
	SetNetUpdateFlag(true);
}

Property* Create_SoundProperty()
{
	return new P_Sound();
}

*/