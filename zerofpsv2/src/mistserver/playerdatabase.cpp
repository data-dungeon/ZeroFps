#include "playerdatabase.h"
#include "../zerofpsv2/basic/zfbasicfs.h"
#include "../zerofpsv2/basic/zfsystem.h"
#include "../zerofpsv2/engine/entitymanager.h"

#include "../mcommon/p_characterproperty.h"

PlayerDatabase::PlayerDatabase()
{
	m_pkEntityMan		= static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	ZFBasicFS* pkBFPS	= static_cast<ZFBasicFS*>(g_ZFObjSys.GetObjectPtr("ZFBasicFS"));	

	m_strPlayerDirectory= "logins/";
		
	//make sure that player directory exist
	if(pkBFPS)
		pkBFPS->CreateDir(m_strPlayerDirectory.c_str());
}

bool PlayerDatabase::CreatePlayer(string strPlayer,string strPassword)
{
	// First check so there is not already a player with this name.
	if(LoginExist(strPlayer)) 
	{
		cout<< "The Login already '" << strPlayer << "' exist" <<endl;	
		return false;
	}
	
	ZFVFile kFile;
	string strPlayerDataFile = m_strPlayerDirectory + strPlayer + "/playerdata.dat"; 
	
	if(!kFile.Open(strPlayerDataFile.c_str(),0,true))
	{
		cout<<"Error creating player file " << strPlayerDataFile << endl;
		return false;
	}
	
	LoginData kLogin;
	

	//writing password
	strcpy(kLogin.szPass, strPassword.c_str());
	strcpy(kLogin.szName, strPlayer.c_str());
	kFile.Write(&kLogin,sizeof(kLogin),1);
	
	kFile.Close();
	return true;
}

vector<string> PlayerDatabase::GetLoginCharacters(string strLogin)
{
	vector<string>	kCharNames;
	//string strPlayerDataFile = "bin/" + m_strPlayerDirectory + strLogin; 
	string strPlayerDataFile = m_strPlayerDirectory + strLogin; 
	cout << "***Scaning in" << strPlayerDataFile << endl;

	ZFVFileSystem* pkVFS	= static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	
	pkVFS->ListDir(&kCharNames, strPlayerDataFile, true);

/*	cout << strPlayerDataFile <<endl;
	for(int i=0; i<kCharNames.size(); i++)
		cout << "Name: " << i << " " <<  kCharNames[i] << endl;*/

	return kCharNames;
}

bool PlayerDatabase::LoginExist(string strLogin)
{
	ZFVFile kFile;
	string strPlayerDataFile = m_strPlayerDirectory + strLogin + "/playerdata.dat"; 
	cout << "strPlayerDataFile " << strPlayerDataFile << endl;

	if(!kFile.Open(strPlayerDataFile.c_str(),0,false))
		return false;

	return true;
}

PlayerData* PlayerDatabase::GetPlayerDataByCharacterID(int iEntityID)
{
	for(int i = 0 ;i<m_strActivePlayers.size();i++)
	{
		if(m_strActivePlayers[i].m_iCharacterID == iEntityID)
		{
			return &m_strActivePlayers[i];
		}
	}	
	
	return NULL;	
}

PlayerData*	PlayerDatabase::GetPlayerData(string	strPlayer)
{
	for(int i = 0 ;i<m_strActivePlayers.size();i++)
	{
		if(m_strActivePlayers[i].m_strPlayerName == strPlayer)
		{
			return &m_strActivePlayers[i];
		}
	}	
	
	return NULL;
}

PlayerData*	PlayerDatabase::GetPlayerData(int iConnection)
{
	for(int i = 0 ;i<m_strActivePlayers.size();i++)
	{
		if(m_strActivePlayers[i].m_iConnectionID == iConnection)
		{
			return &m_strActivePlayers[i];
		}
	}	
	
	return NULL;
}


bool PlayerDatabase::Login(string strPlayer,string strPassword)
{
	//check if player is already connected
	if(IsOnline(strPlayer))
		return false;

	//check player password
	if(VerifyPlayer(strPlayer, strPassword)) 
	{
		//m_strActiveUsers.push_back( strPlayer );
		PlayerData kNewPlayer;		
		kNewPlayer.m_strPlayerName = 		strPlayer;
		kNewPlayer.m_strCharacterName = 	"NONE";
		kNewPlayer.m_iCharacterID = 		-1;
		kNewPlayer.m_fLoginTime = 			-1;
		kNewPlayer.m_iConnectionID = 		-1;
		
		m_strActivePlayers.push_back(kNewPlayer);
		
		return true;
	}

	return false;
}

void PlayerDatabase::Logout(string strPlayer)
{
	for(vector<PlayerData>::iterator it = m_strActivePlayers.begin();it != m_strActivePlayers.end(); it++)
	{
		if( (*it).m_strPlayerName == strPlayer)
		{
			m_strActivePlayers.erase(it);
			return;
		}
	}

	//m_strActiveUsers.erase(find(m_strActiveUsers.begin(), m_strActiveUsers.end(), strPlayer));
	cout << "Logout: " << strPlayer << endl;
}

bool PlayerDatabase::IsOnline(string strLogin)
{
	for(int i = 0 ;i<m_strActivePlayers.size();i++)
	{
		if(m_strActivePlayers[i].m_strPlayerName == strLogin)
			return true;
	}

	return false;
}


void PlayerDatabase::GetUsers(vector<string>* pkUsers)
{
	for(int i = 0 ;i<m_strActivePlayers.size();i++)
	{
		pkUsers->push_back(m_strActivePlayers[i].m_strPlayerName);
	}
}

bool PlayerDatabase::VerifyPlayer(string strPlayer,string strPassword)
{
	ZFVFile kFile;

	string strPlayerDataFile = m_strPlayerDirectory + strPlayer + "/playerdata.dat"; 
	
	if(!kFile.Open(strPlayerDataFile.c_str(),0,false))
	{	
		//cout<<"Player "<<strPlayer<< " dont exist"<<endl;
		return false;
	}
		
	//cout<<"player found verrifying password"<<endl;	
	
	LoginData kLogin;
	kFile.Read(&kLogin,sizeof(kLogin),1);
	
	//read password
	kFile.Close();
		
	//is the password valid ?
	if(strPassword == kLogin.szPass)
		return true;
		
	return false;

}


bool PlayerDatabase::CreateNewCharacter(string strPlayer, string strCharacter)
{
	ZFVFile kFile;

	string strCharacterDataFile = m_strPlayerDirectory + (strPlayer + "/") + strCharacter + "/character.dat"; 
	
	if(kFile.Open(strCharacterDataFile.c_str(),0,false))
	{	
		cout<<"character "<<strPlayer<<" -> "<<strCharacter<<" already exist"<<endl;
		kFile.Close();
		return false;
	}	
	
	if(!kFile.Open(strCharacterDataFile.c_str(),0,true))
	{	
		cout<<"error creating character file "<<strCharacterDataFile<<endl;
		return false;
	}	
	
	//create new caracter and save it	
	Entity* pkEntity = m_pkEntityMan->CreateEntityFromScript("data/script/objects/game objects/hosplayer.lua");		// t_player
	if(!pkEntity)
	{
		cout<<"could not create character entity"<<endl;
		return false;
	}
	
	//setup entity
	pkEntity->SetName(strCharacter);
	pkEntity->SetSave(true);
	pkEntity->SetUseZones(true);

	//setup propertys
	if(!pkEntity->GetProperty("P_Track"))	//check if theres a tracker property
		 pkEntity->AddProperty("P_Track");	//else create one
		
	if(P_CharacterProperty* cp = (P_CharacterProperty*)pkEntity->GetProperty("P_CharacterProperty"))
	{
		cp->SetName(strCharacter);
		cp->SetOwnedByPlayer(strPlayer);
		cp->SetIsPlayerCharacter(true);
	}
	
 
	//save it
	pkEntity->Save(&kFile);
	
	//delete it
	m_pkEntityMan->Delete(pkEntity);
	
	//cout<<"created charater "<<strPlayer<<" -> "<<strCharacter<<endl;
	kFile.Close();

	return true;	
}

Entity* PlayerDatabase::CreateCharacter(string strPlayer, string strCharacter)
{
	ZFVFile kFile;

	string strCharacterDataFile = m_strPlayerDirectory + (strPlayer + "/") + strCharacter + "/character.dat"; 
	
	if(!kFile.Open(strCharacterDataFile.c_str(),0,false))
	{	
		cout<<"did't find character"<<strPlayer<< " -> "<<strCharacter<<endl;
		kFile.Close();
		return false;
	}	
	
	Entity* pkEntity = m_pkEntityMan->CreateEntity(false);
	pkEntity->Load(&kFile,false);
	kFile.Close();
	
	//set save to false so it wont we saved whit the rest of the world
	pkEntity->SetSave(false);
	pkEntity->SetUseZones(true);
	
	cout<<"Loaded Character "<<strPlayer<< " -> "<<strCharacter<<endl;
	
	return pkEntity;
}


bool PlayerDatabase::SaveCharacter(Entity* pkEntity,string strPlayer)
{
	ZFVFile kFile;
	string strCharacter = pkEntity->GetName();


	string strCharacterDataFile = m_strPlayerDirectory + (strPlayer + "/") + strCharacter + "/character.dat"; 
	
	if(!kFile.Open(strCharacterDataFile.c_str(),0,true))
	{	
		cout<<"did't find character"<<strPlayer<< " -> "<<strCharacter<<endl;
		return false;
	}	

	//set save
	pkEntity->SetSave(true);
	
	//save it
	pkEntity->Save(&kFile);	
	
	//set save to false
	pkEntity->SetSave(false);
	
	kFile.Close();
	
	cout<<"Saved Character "<<strPlayer<< " -> "<<strCharacter<<endl;
	
	return true;
}

void PlayerDatabase::DeleteCharacter(string strPlayer, string strCharacter)
{
	ZFVFileSystem* pkVFS	= static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	
	string strCharacterDataFile = m_strPlayerDirectory + (strPlayer + "/") + strCharacter; 
	pkVFS->RemoveDir(strCharacterDataFile);
}









