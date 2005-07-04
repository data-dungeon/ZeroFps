#include "p_sound.h"
#include "../../engine/entity.h"
#include "../script_interfaces/si_objectmanager.h" 

P_Sound::P_Sound()
{
	m_bNetwork = true;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;

	strcpy(m_acName,"P_Sound");
	m_pkAudioSystem = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pEntityMan = static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));

	m_fGain = 1.0f;
	m_strFileName = "";
	m_strStopFileName = "";
	m_bLoop=false;
	m_fStartTime = -1;

	m_bStarted = false;
	m_kPrevpos = Vector3(-9999,-9999,-9999);

	m_iVersion = 2; // lade till gain
}

P_Sound::~P_Sound()
{
	//if(!m_strFileName.empty())
	//{
	//	Entity* pkEnt = GetEntity();
	//	m_pkAudioSystem->StopAudio(m_strFileName, pkEnt->GetIWorldPosV());
	//	m_strStopFileName = "";
	//	m_strFileName = "";

	//	m_bStarted = false;
	//	m_kPrevpos = Vector3(-9999,-9999,-9999);
	//}

	if(m_iID > 0)
	{
		m_pkAudioSystem->StopAudio(m_iID);
		m_strStopFileName = "";
		m_strFileName = "";
		m_iID = -1;

		m_bStarted = false;
		m_kPrevpos = Vector3(-9999,-9999,-9999);
	}
}

void P_Sound::Update()
{
	if(!m_strStopFileName.empty())
	{
		if(m_bStarted)
		{
			Entity* pkEnt = GetEntity();
			m_pkAudioSystem->StopAudio(m_iID);
			m_strStopFileName = "";
			m_strFileName = "";
	
			m_bStarted = false;
			m_kPrevpos = Vector3(-9999,-9999,-9999);
		}
	}	

	if(!m_strFileName.empty())
	{	
		Entity* pkEnt = GetEntity();

		if(m_bLoop == false)
		{
			m_pkAudioSystem->StopAudio(m_iID);	//DVOID fix, ljud kunde lÃ¤cka ibland, gjorde en failsafe =D
						
			m_iID = m_pkAudioSystem->PlayAudio(m_strFileName, 
				pkEnt->GetIWorldPosV(), pkEnt->GetVel(), 0, m_fGain);
			m_strFileName = "";
		}
		else
		{
			Vector3 currpos = pkEnt->GetWorldPosV();

			if(m_bStarted == false)
			{
				m_pkAudioSystem->StopAudio(m_iID);	//DVOID fix, ljud kunde lÃ¤cka ibland, gjorde en failsafe =D
								
				m_iID = m_pkAudioSystem->PlayAudio(m_strFileName, 
					pkEnt->GetIWorldPosV(), pkEnt->GetVel(), ZFAUDIO_LOOP, m_fGain);
				m_bStarted = true;
			}
			else
			{
				//if(!m_kPrevpos.NearlyEquals(currpos,0.1f))
				//{
					m_pkAudioSystem->MoveAudio(m_iID, pkEnt->GetIWorldPosV(), pkEnt->GetVel(), m_fGain);
					m_kPrevpos = pkEnt->GetIWorldPosV();
				//}
			}
		}
	}
}

void P_Sound::StartSound(string strName, bool bLoop, float fGain) 
{
	if(m_strFileName != strName)
		m_strStopFileName = m_strFileName;
	else
		m_strStopFileName = "";

	m_strFileName = strName;
	m_bLoop = bLoop;
	m_fGain = fGain;
	m_fStartTime = m_pkZeroFps->GetEngineTime();
	
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
	//if 2s has passed since non looping sound was started, dont send it to client
	if(!m_bLoop && 
		!m_strFileName.empty() && 
		m_pkZeroFps->GetEngineTime() > m_fStartTime + 2.0)
	{
		m_strFileName = "";
		//cout<<"P_Sound: none looping sound timed out ,starttime:"<<m_fStartTime<<"  current time:"<<m_pkZeroFps->GetEngineTime()<<endl;
	}
	


	pkNetPacket->Write_Str( m_strFileName); // filename
	pkNetPacket->Write(m_bLoop); // loop or not
	pkNetPacket->Write_Str( m_strStopFileName); // filename
	pkNetPacket->Write( m_fGain); // gain
	
	SetNetUpdateFlag(iConnectionID,false);
	
	
// 	//check if all clients got sound, if so clear it
// 	if(!m_strFileName.empty() && !m_bLoop)
// 	{
// 		int iClients = m_pkZeroFps->m_pkNetWork->GetNumOfClients();
// 		bool bComplete = true;
// 		for(int i =0;i<iClients;i++)
// 		{
// 			if(m_pkZeroFps->m_pkNetWork->IsConnected(i) &&
// 				m_pkEntity->GetExistOnClient(i) &&
// 				GetNetUpdateFlag(i))
// 			{
// 				bComplete = false;
// 				break;
// 			}
// 		}
// 		
// 		if(bComplete)
// 		{
// 			//cout<<"everyone seems to have the sound,clearing it: "<<m_pkEntity->GetEntityID()<<endl;
// 			m_strFileName = "";
// 		}
// 	}
}

void P_Sound::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Read_Str( m_strFileName);
	pkNetPacket->Read(m_bLoop); 
	pkNetPacket->Read_Str(m_strStopFileName);
	pkNetPacket->Read(m_fGain);
}

vector<PropertyValues> P_Sound::GetPropertyValues()
{
	vector<PropertyValues> kReturn(3);

	kReturn[0].kValueName = "filename";
	kReturn[0].iValueType = VALUETYPE_STRING; 
	kReturn[0].pkValue    = (void*)&m_strFileName;
	
	kReturn[1].kValueName = "loop";
	kReturn[1].iValueType = VALUETYPE_BOOL;
	kReturn[1].pkValue    = (void*)&m_bLoop;

	kReturn[2].kValueName = "gain";
	kReturn[2].iValueType = VALUETYPE_FLOAT;
	kReturn[2].pkValue    = (void*)&m_fGain;

	SetNetUpdateFlag(true);
	return kReturn;
}

void P_Sound::Save(ZFIoInterface* pkFile)
{
 	//liten snabbfix så inte ljud som inte loopar sparas, (blev fel när man joina med en karaktär som t.ex dött innan man gick ut, då hördes döds ljudet en gång när man joina)
	if(!m_bLoop)
 		m_strFileName = "";

	pkFile->Write_Str( m_strFileName );
	pkFile->Write( &m_bLoop, sizeof(m_bLoop), 1 ); 
	pkFile->Write( &m_fGain, sizeof(m_fGain), 1 ); 
}

void P_Sound::Load(ZFIoInterface* pkFile,int iVersion)
{
	switch(iVersion)
	{
	case 1:
		pkFile->Read_Str( m_strFileName ); 
		pkFile->Read( m_bLoop );
		break;

	case 2:
		pkFile->Read_Str( m_strFileName ); 
		pkFile->Read( m_bLoop );
		pkFile->Read( m_fGain ); 
		break;
	}

	m_fStartTime = m_pkZeroFps->GetEngineTime();
	
	
	SetNetUpdateFlag(true);
}

bool P_Sound::HandleSetValue( string kValueName ,string kValue )
{
	bool res = true;

	if(strcmp(kValueName.c_str(), "filename") == 0) 
	{
		res = false;
		m_strFileName = kValue;
		m_fStartTime = m_pkZeroFps->GetEngineTime();
	}
	
	if(strcmp(kValueName.c_str(), "loop") == 0) 
	{
		res = false;
		if(kValue == "true")
			m_bLoop = 1;
		else
			m_bLoop = 0;
	}

	if(strcmp(kValueName.c_str(), "gain") == 0) 
	{
		char szValue[25];
		strcpy(szValue, kValue.c_str());
		m_fGain = (float) atof(szValue);
	}

	SetNetUpdateFlag(true);

	return res;
}

Property* Create_SoundProperty()
{
	return new P_Sound();
}


// SCRIPT INTERFACE FOR P_Sound
using namespace ObjectManagerLua;

namespace SI_P_Sound
{
	int StartSoundLua(lua_State* pkLua)
	{
		int args = 0;
		if((args = g_pkScript->GetNumArgs(pkLua)) > 4)
			return 0;
					
		int id;
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		id = (int)dTemp;

		//char	acSound[256];
		string strSound;
		g_pkScript->GetArgString(pkLua, 1, strSound);	

		double dGain = 1.0f, dLoop = 0;

		if(args > 2)
			g_pkScript->GetArgNumber(pkLua, 2, &dGain);

		if(args > 3)
			g_pkScript->GetArgNumber(pkLua, 4, &dLoop);
								
		Entity* pkObject = g_pkObjMan->GetEntityByID(id);
		if(pkObject)
		{
			if(P_Sound* pe = (P_Sound*)pkObject->GetProperty("P_Sound"))
			{
				pe->StartSound(strSound,(dLoop==1) ? true : false, (float) dGain);			
			}
			else
				cout<<"WARNING: StartSoundLua on entity without P_Sound: "<<pkObject->GetType()<<endl;
		}
		else
			cout<<"WARNING: StartSoundLua could not find entity "<<id<<endl;

		return 0;
	}	
}

void Register_P_Sound(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Sound", Create_SoundProperty);					

	// Register Property Script Interface
	g_pkScript->ExposeFunction("StartSound",	SI_P_Sound::StartSoundLua);

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
				m_pkAudioSystem->PlayAudio(m_strFileName, 
					pkEnt->GetIWorldPosV(), pkEnt->GetVel());
				m_strFileName = "";
			}
			else
			{
				Vector3 currpos = pkEnt->GetWorldPosV();

				if(m_bStarted == false)
				{
					m_pkAudioSystem->PlayAudio(m_strFileName, 
						pkEnt->GetIWorldPosV(), pkEnt->GetVel(), ZFAUDIO_LOOP);
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
