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
	{
		if(m_kSounds[i].m_strFileName == strName)
		{
			m_kSounds.erase( m_kSounds.begin() + i);
			break;
		}
	}

	SetNetUpdateFlag(true);
}

// Skicka iväg från serven?
void P_Sound::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	int iNumSounds = m_kSounds.size();

	pkNetPacket->Write(&iNumSounds, sizeof(iNumSounds)); 

	for(int i=0; i<iNumSounds; i++)
	{
		pkNetPacket->Write_Str( m_kSounds[i].m_strFileName.c_str()); // filename
		pkNetPacket->Write(&m_kSounds[i].m_bLoop, sizeof(m_kSounds[i].m_bLoop)); // loop or not
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
	pkNetPacket->Read(&iNumSounds, sizeof(iNumSounds)); 

	char file_name[128];

	for(int i=0; i<iNumSounds; i++)
	{
		sound_info si;
		pkNetPacket->Read_Str( file_name ); si.m_strFileName = file_name;
		pkNetPacket->Read(&si.m_bLoop, sizeof(si.m_bLoop)); 
		kSoundsOnServer.push_back(si); 
	}

	// Stoppa ljud som inte längre finns på serven
	for(int i=0; i<m_kSounds.size(); i++)
	{
		bool bExistOnServer = false;

		for(int j=0; j<kSoundsOnServer.size(); j++)
		{
			if(kSoundsOnServer[j].m_strFileName == m_kSounds[i].m_strFileName)
			{
				bExistOnServer = true;
				break;
			}
		}

		if(bExistOnServer == false)
		{
			Entity* pkEnt = GetEntity();
			m_pkAudioSystem->StopSound(m_kSounds[i].m_strFileName, pkEnt->GetIWorldPosV());
		}
	}

	//Spela upp nya ljud som finns på serven men inte klienten
	for(int i=0; i<kSoundsOnServer.size(); i++)
	{
		bool bExistOnClient = false;

		for(int j=0; j<m_kSounds.size(); j++)
		{
			if(m_kSounds[i].m_strFileName == kSoundsOnServer[j].m_strFileName)
			{
				bExistOnClient = true;
				break;
			}
		}

		if(bExistOnClient == false)
		{
			Entity* pkEnt = GetEntity();
			m_pkAudioSystem->StartSound(kSoundsOnServer[i].m_strFileName, 
				pkEnt->GetIWorldPosV(), pkEnt->GetVel(), kSoundsOnServer[i].m_bLoop);
		}
	}

	m_kSounds.clear(); // radera den gamla listan

	// ... och kopiera in dom nya ljuden
	for(int i=0; i<kSoundsOnServer.size(); i++)
	{
		m_kSounds.push_back( kSoundsOnServer[i] );
	}

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










/////////////// OLD Code (do not remove jet)
/*
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
*///////////////////////////////////////