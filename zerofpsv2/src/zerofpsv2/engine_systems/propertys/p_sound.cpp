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
	{
		m_strStopFileName = m_strFileName;
	}
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
	//int iNumElements = m_kSounds.size();
	//pkFile->Write( &iNumElements, sizeof(int), 1); // number of elements

	//char temp[128];

	//for(int i=0; i<m_kSounds.size(); i++)
	//{
	//	strcpy(temp,m_kSounds[i].m_strFileName.c_str());
	//	pkFile->Write( temp,128,1 ); // filename
	//	pkFile->Write( &m_kSounds[i].m_bLoop, sizeof(bool), 1); // loop
	//	pkFile->Write( &m_kSounds[i].m_bPlaying, sizeof(bool), 1); // playing or not
	//}
}

void P_Sound::Load(ZFIoInterface* pkFile,int iVersion)
{
	//char temp[128];

	//int iNumElements;
	//pkFile->Read( &iNumElements, sizeof(int), 1); // number of elements

	//for(int i=0; i<iNumElements; i++)
	//{
	//	sound_info sf;

	//	pkFile->Read(temp,128,1); sf.m_strFileName = temp; // filename
	//	pkFile->Read( &sf.m_bLoop, sizeof(bool), 1); // loop
	//	pkFile->Read( &sf.m_bPlaying, sizeof(bool), 1); // playing or not

	//	sf.m_iAction = 0;

	//	m_kSounds.push_back(sf); 
	//}
	//	
//	SetNetUpdateFlag(true);		
}



Property* Create_SoundProperty()
{
	return new P_Sound();
}
