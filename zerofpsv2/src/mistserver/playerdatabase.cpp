#include "playerdatabase.h"
#include "../zerofpsv2/basic/zfbasicfs.h"
#include "../zerofpsv2/basic/zfsystem.h"
#include "../zerofpsv2/engine/entitymanager.h"

#include "../mcommon/p_characterproperty.h"

PlayerDatabase::PlayerDatabase()
{
	m_pkEntityMan			= static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));
	ZSSBasicFS* pkBFPS	= static_cast<ZSSBasicFS*>(g_ZFObjSys.GetObjectPtr("ZSSBasicFS"));	

	m_bStdPrivilege.m_bAdmin		= false;
	m_bStdPrivilege.m_bBuilder		= false;
	m_bStdPrivilege.m_bGameMaster = false;
	m_bStdPrivilege.m_bPlayer		= true;


	m_strPlayerDirectory= "logins/";
		
	//make sure that player directory exist
	if(pkBFPS)
		pkBFPS->CreateDir(m_strPlayerDirectory.c_str());
}



string PlayerDatabase::PrevToStr(LoginPrivilege kPrev)
{
	string strPrev;

	if(kPrev.m_bAdmin)		strPrev = strPrev + "A";
	if(kPrev.m_bBuilder)		strPrev = strPrev + "B";
	if(kPrev.m_bGameMaster)	strPrev = strPrev + "G";
	if(kPrev.m_bPlayer)		strPrev = strPrev + "P";


	return strPrev;
}

LoginPrivilege PlayerDatabase::StrToPrev(string strPrev)
{
	LoginPrivilege kPrev;

	kPrev.m_bAdmin = kPrev.m_bBuilder = kPrev.m_bGameMaster = kPrev.m_bPlayer = false;

	if(strPrev.find("A"))		kPrev.m_bAdmin			= true;	
	if(strPrev.find("B"))		kPrev.m_bBuilder		= true;	
	if(strPrev.find("G"))		kPrev.m_bGameMaster	= true;	
	if(strPrev.find("P"))		kPrev.m_bPlayer		= true;	

	return kPrev;
}

// User Mangment ***************************************************************************************************
bool PlayerDatabase::UserExist(string strUser)
{
	ZFVFile kFile;
	string strPlayerDataFile = m_strPlayerDirectory + strUser + "/playerdata.dat"; 
	cout << "strPlayerDataFile " << strPlayerDataFile << endl;

	if(!kFile.Open(strPlayerDataFile.c_str(),0,false))
		return false;

	return true;
}

bool PlayerDatabase::UserCreate(string strUser, string strPassword)
{
	// First check so there is not already a player with this name.
	if(UserExist(strUser)) 
	{
		cout<< "The Login already '" << strUser << "' exist" <<endl;	
		return false;
	}
	
	ZFVFile kFile;
	string strPlayerDataFile = m_strPlayerDirectory + strUser + "/playerdata.dat"; 
	
	if(!kFile.Open(strPlayerDataFile.c_str(),0,true))
	{
		cout<<"Error creating player file " << strPlayerDataFile << endl;
		return false;
	}
	
	LoginData kLogin;
	//writing password
	strcpy(kLogin.szPass, strPassword.c_str());
	strcpy(kLogin.szName, strUser.c_str());
	kLogin.m_kPrivilege = m_bStdPrivilege;

	kFile.Write(&kLogin,sizeof(kLogin),1);
	kFile.Close();

	return true;
}

bool PlayerDatabase::UserDelete(string strUser)
{
	// Don't delete players that are online.
	if(IsOnline(strUser))
	{
		cout << "The user '" << strUser << "' is online and can not be removed." << endl;
		return false;
	}

	cout << "Delete Login '" << strUser << "'" << endl;
	ZSSVFileSystem* pkVFS	= static_cast<ZSSVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	

	string strLoginDir = m_strPlayerDirectory + strUser; 
	string strLoginDataFile = strLoginDir + string("/playerdata.dat"); 

	// Delete Player Data files
	cout << "TargetFile: "<< strLoginDataFile << endl;
	return pkVFS->RemoveFile(strLoginDataFile.c_str());
}

bool PlayerDatabase::VerifyUser(string strPlayer,string strPassword)
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

bool PlayerDatabase::HasPrivilege(string strLoginName, string strPrev)
{
	ZFVFile kFile;

	string strPlayerDataFile = m_strPlayerDirectory + strLoginName + "/playerdata.dat"; 
	
	if(!kFile.Open(strPlayerDataFile.c_str(),0,false))
	{	
		//cout<<"Player "<<strPlayer<< " dont exist"<<endl;
		return false;
	}
		
	LoginData kLogin;
	kFile.Read(&kLogin,sizeof(kLogin),1);
	kFile.Close();

	if(strstr(strPrev.c_str(), "A") && kLogin.m_kPrivilege.m_bAdmin == false)			return false;
	if(strstr(strPrev.c_str(), "B") && kLogin.m_kPrivilege.m_bBuilder == false)		return false;
	if(strstr(strPrev.c_str(), "G") && kLogin.m_kPrivilege.m_bGameMaster == false)	return false;
	if(strstr(strPrev.c_str(), "P") && kLogin.m_kPrivilege.m_bPlayer == false)		return false;

	return true;
}


void PlayerDatabase::SetUserPrev(string strLoginName, string strPrev)
{
	ZFVFile kFile;

	string strPlayerDataFile = m_strPlayerDirectory + strLoginName + "/playerdata.dat"; 
	
	if(!kFile.Open(strPlayerDataFile.c_str(),0,false))
	{	
		return ;
	}
		
	LoginData kLogin;
	kFile.Read(&kLogin,sizeof(kLogin),1);
	kFile.Close();

	if(strstr(strPrev.c_str(), "-A"))	kLogin.m_kPrivilege.m_bAdmin = false;
	if(strstr(strPrev.c_str(), "+A") || strstr(strPrev.c_str(), "=A"))	kLogin.m_kPrivilege.m_bAdmin = true;

	if(strstr(strPrev.c_str(), "-B"))	kLogin.m_kPrivilege.m_bBuilder = false;
	if(strstr(strPrev.c_str(), "+B") || strstr(strPrev.c_str(), "=B"))	kLogin.m_kPrivilege.m_bBuilder = true;

	if(strstr(strPrev.c_str(), "-G"))	kLogin.m_kPrivilege.m_bGameMaster = false;
	if(strstr(strPrev.c_str(), "+G") || strstr(strPrev.c_str(), "=G"))	kLogin.m_kPrivilege.m_bGameMaster = true;

	if(strstr(strPrev.c_str(), "-P"))	kLogin.m_kPrivilege.m_bPlayer = false;
	if(strstr(strPrev.c_str(), "+P") || strstr(strPrev.c_str(), "=P"))	kLogin.m_kPrivilege.m_bPlayer = true;

	if(!kFile.Open(strPlayerDataFile.c_str(),0,true))
	{
		cout<<"Error creating player file " << strPlayerDataFile << endl;
		return;
	}
	
	kFile.Write(&kLogin,sizeof(kLogin),1);
	kFile.Close();
}

string  PlayerDatabase::GetPrivilege(string strUser)
{
	ZFVFile kFile;

	string strPlayerDataFile = m_strPlayerDirectory + strUser + "/playerdata.dat"; 
	
	if(!kFile.Open(strPlayerDataFile.c_str(),0,false))
	{	
		return string("");
	}
		
	LoginData kLogin;
	kFile.Read(&kLogin,sizeof(kLogin),1);
	kFile.Close();

	string strPrev = PrevToStr(kLogin.m_kPrivilege);
	return strPrev;
}


// Login/Logout ***************************************************************************************************
bool PlayerDatabase::Login(string strPlayer,string strPassword)
{
	//check if player is already connected
	if(IsOnline(strPlayer))
		return false;

	//check player password
	if(VerifyUser(strPlayer, strPassword)) 
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

// Character ***************************************************************************************************
bool PlayerDatabase::CharacterCreate(string strPlayer, string strCharacter, string strModell)
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
	
	//wich character script to use?
	string strScipt = "data/script/objects/characters/playable/"+strModell+".lua";
	
	//create new caracter and save it	
	//Entity* pkEntity = m_pkEntityMan->CreateEntityFromScript("data/script/objects/game objects/hosplayer.lua");		// t_player
	Entity* pkEntity = m_pkEntityMan->CreateEntityFromScript(strScipt.c_str());		// t_player
	if(!pkEntity)
	{
		cout<<"could not create character entity"<<endl;
		return false;
	}
	
// 	P_Mad* pkMad = dynamic_cast<P_Mad*>(pkEntity->GetProperty("P_Mad"));
// 	if(!pkMad)
// 		return false;
/*
	if(strModell == "Good guy")
      pkMad->SetBase("/data/mad/player.mad");
	if(strModell == "Bad guy")
      pkMad->SetBase("/data/mad/player2.mad");*/

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

Entity* PlayerDatabase::CharacterSpawn(string strPlayer, string strCharacter)
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


bool PlayerDatabase::CharacterSave(Entity* pkEntity,string strPlayer)
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

void PlayerDatabase::CharacterDelete(string strPlayer, string strCharacter)
{
	cout << "Delete Character '" << strCharacter << "' from Login '" << strPlayer << "'" << endl;
	ZSSVFileSystem* pkVFS	= static_cast<ZSSVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	


	string strCharacterDataDir = m_strPlayerDirectory + (strPlayer + "/") + strCharacter; 
	string strCharacterDataFile = strCharacterDataDir + string("/character.dat"); 

	// Delete Player Data files
	cout << "TargetFile: "<< strCharacterDataFile << endl;
	pkVFS->RemoveFile(strCharacterDataFile.c_str());

	// Delete player dir
	cout << "TargetDir: "<< strCharacterDataDir << endl;
	pkVFS->RemoveDir(strCharacterDataDir);
}

vector<string> PlayerDatabase::GetLoginCharacters(string strLogin)
{
	vector<string>	kCharNames;
	//string strPlayerDataFile = "bin/" + m_strPlayerDirectory + strLogin; 
	string strPlayerDataFile = m_strPlayerDirectory + strLogin; 
	cout << "***Scaning in" << strPlayerDataFile << endl;

	ZSSVFileSystem* pkVFS	= static_cast<ZSSVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	
	pkVFS->ListDir(kCharNames, strPlayerDataFile, true);

	
 	//dvoids hax för o ta bort ".."	och "."
	vector<string>::iterator it =kCharNames.begin();
	while(it != kCharNames.end())
	{
		if( *it == ".." || *it == ".") 
		{
			kCharNames.erase(it);
			it = kCharNames.begin();
			continue;
		}			
		it++;
	}
	
	
/*	cout << strPlayerDataFile <<endl;
	for(int i=0; i<kCharNames.size(); i++)
		cout << "Name: " << i << " " <<  kCharNames[i] << endl;*/

	return kCharNames;
}

// Get PlayerData ***************************************************************************************************
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

void	PlayerDatabase::GetPlayerEntitys(vector<pair<Entity*,int> >* pkEnts)
{
	for(int i = 0 ;i<m_strActivePlayers.size();i++)
	{	
		if(Entity* pkEnt = m_pkEntityMan->GetEntityByID(m_strActivePlayers[i].m_iCharacterID))	
			pkEnts->push_back(pair<Entity*,int>(pkEnt,m_strActivePlayers[i].m_iConnectionID));	
	}
}
















