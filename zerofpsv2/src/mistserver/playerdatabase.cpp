#include "playerdatabase.h"

PlayerDatabase::PlayerDatabase()
{
	m_pkEntityMan = static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_strPlayerDirectory="players/";
}

bool PlayerDatabase::CreatePlayer(string strPlayer,string strPassword)
{
	ZFVFile kFile;

	string strPlayerDataFile = m_strPlayerDirectory + strPlayer + "/playerdata.dat"; 
	
	if(!kFile.Open(strPlayerDataFile.c_str(),0,false))
	{
		//cout<<"player does not exist <-- this is actualy good =)"<<endl;	
		
		if(!kFile.Open(strPlayerDataFile.c_str(),0,true))
		{
			cout<<"Error creating player file "<<strPlayer<<endl;
			return false;
		}
		
		//writing password
		char czPwd[128];
		strcpy(czPwd,strPassword.c_str());
		kFile.Write(czPwd,128,1);
		
		kFile.Close();
		return true;
	}
	else
	{	
		cout<<"player already exist"<<endl;	
		kFile.Close();
		return false;
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
	pkEntity->GetName() = strCharacter;
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
	
	Entity* pkEntity = m_pkEntityMan->CreateObject();
	pkEntity->Load(&kFile);
	kFile.Close();
	
	//set save to false so it wont we saved whit the rest of the world
	pkEntity->GetSave() = false;
	pkEntity->SetUseZones(true);
	
	return pkEntity;
}


bool PlayerDatabase::SaveCharacter(Entity* pkEntity,string strPlayer)
{
	ZFVFile kFile;
	string strCharacter = pkEntity->GetName();


	string strCharacterDataFile = m_strPlayerDirectory + (strPlayer + "/") + strCharacter + "/character.dat"; 
	
	if(!kFile.Open(strCharacterDataFile.c_str(),0,false))
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