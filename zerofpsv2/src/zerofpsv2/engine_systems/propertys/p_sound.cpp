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
	//if(!m_strFileName.empty())
	//{
	//	Entity* pkEnt = GetEntity();
	//	m_pkAudioSystem->StopSound(m_strFileName, pkEnt->GetIWorldPosV());
	//	m_strStopFileName = "";
	//	m_strFileName = "";

	//	m_bStarted = false;
	//	m_kPrevpos = Vector3(-9999,-9999,-9999);
	//}

	if(m_iID > 0)
	{
		m_pkAudioSystem->StopSound(m_iID);
		m_strStopFileName = "";
		m_strFileName = "";
		m_iID = -1;

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
				m_iID = m_pkAudioSystem->StartSound(m_strFileName, 
					pkEnt->GetIWorldPosV(), pkEnt->GetVel(), false);
				m_strFileName = "";
			}
			else
			{
				Vector3 currpos = pkEnt->GetWorldPosV();

				if(m_bStarted == false)
				{
					m_iID = m_pkAudioSystem->StartSound(m_strFileName, 
						pkEnt->GetIWorldPosV(), pkEnt->GetVel(), true);
					m_bStarted = true;
				}
				else
				{
					if(!m_kPrevpos.NearlyEquals(currpos,0.1f))
					{
						m_pkAudioSystem->MoveSound(m_iID, pkEnt->GetIWorldPosV(), pkEnt->GetVel());
						m_kPrevpos = pkEnt->GetIWorldPosV();
					}
				}
			}
		}

		if(!m_strStopFileName.empty())
		{
			Entity* pkEnt = GetEntity();
			m_pkAudioSystem->StopSound(m_iID);
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

	kReturn[0].kValueName = "filename";
	kReturn[0].iValueType = VALUETYPE_STRING; 
	kReturn[0].pkValue    = (void*)&m_strFileName;
	
	kReturn[1].kValueName = "loop";
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

bool P_Sound::HandleSetValue( string kValueName ,string kValue )
{
	bool res = true;

	if(strcmp(kValueName.c_str(), "filename") == 0) 
	{
		res = false;
		m_strFileName = kValue;
	}
	
	if(strcmp(kValueName.c_str(), "loop") == 0) 
	{
		res = false;
		if(kValue == "true")
			m_bLoop = 1;
		else
			m_bLoop = 0;
	}

	SetNetUpdateFlag(true);

	return res;
}

Property* Create_SoundProperty()
{
	return new P_Sound();
}

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
	vector<PropertyValues> kReturn(2);

	kReturn[0].kValueName = "filename";
	kReturn[0].iValueType = VALUETYPE_STRING; 
	kReturn[0].pkValue    = (void*)&m_strFileName;
	
	kReturn[1].kValueName = "loop";
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