#include "playerdatabase.h"
#include "../zerofpsv2/basic/zfbasicfs.h"


/*
	Sköter logins och hanteringen av karaktärer. Alla som kan ansluta har ett login
	som innehåller information, password, rättigheter och karaktärer.

		Info:	Namn, osv.
		Rätt:	Vad denna login kan göra.
		Kar:	Alla karaktärer kopplade till detta login.

	Login(Name, Password):	Logs in a user.
	Logout(Name):			Logs out a user.
	
	GetOnline(List)			Get a list of users loged in.



	

*/

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
	if(LoginExist(strPlayer)) {
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
	
	//writing password
	char czPwd[128];
	strcpy(czPwd,strPassword.c_str());
	kFile.Write(czPwd,128,1);
	
	kFile.Close();
	return true;
}

vector<string> PlayerDatabase::GetLoginCharacters(string strLogin)
{
	vector<string>	kCharNames;
	string strPlayerDataFile = "bin/" + m_strPlayerDirectory + strLogin; 

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


bool PlayerDatabase::Login(string strPlayer,string strPassword)
{
	if(VerifyPlayer(strPlayer, strPassword)) {
		m_strActiveUsers.push_back( strPlayer );

		return true;
		}

	return false;
}

void PlayerDatabase::Logout(string strPlayer)
{
	m_strActiveUsers.erase(find(m_strActiveUsers.begin(), m_strActiveUsers.end(), strPlayer));
	cout << "Logout: " << strPlayer << endl;
}

vector<string> PlayerDatabase::GetUsers()
{
	return m_strActiveUsers;
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
	
	//read password
	char czPwd[128];
	kFile.Read(czPwd,128,1);
	kFile.Close();
		
	//is the password valid ?
	if(strPassword == czPwd)
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
	Entity* pkEntity = m_pkEntityMan->CreateObjectFromScript("data/script/objects/t_player.lua");	
	pkEntity->SetName(strCharacter);
	pkEntity->GetSave() = true;
	pkEntity->SetUseZones(true);
	
	if(!pkEntity->GetProperty("P_Track"))	//check if theres a tracker property
		pkEntity->AddProperty("P_Track");	//else create one
	
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
	
	Entity* pkEntity = m_pkEntityMan->CreateObject(false);
	pkEntity->Load(&kFile,false);
	kFile.Close();
	
	//set save to false so it wont we saved whit the rest of the world
	pkEntity->GetSave() = false;
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
	pkEntity->GetSave() = true;
	
	//save it
	pkEntity->Save(&kFile);	
	
	//set save to false
	pkEntity->GetSave() = false;
	
	kFile.Close();
	
	cout<<"Saved Character "<<strPlayer<< " -> "<<strCharacter<<endl;
	
	return true;
}