#ifndef _PLAYERDATABASE_H_
#define _PLAYERDATABASE_H_


#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/basic/zfsystem.h"
#include <vector>

using namespace std;

/**	\brief	PlayerDatabase
		\ingroup MistServer

	The part of the server that stores all logins and their rights with what they can do on the server.
	All users are stored in dir /logins and each user get there own dir. In each user dir are a data file
	with things like passwords and user rights.

*/


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

class	LoginData
{
	public:
		char		szName[128];		// Name of this login.
		char		szPass[128];		// Password of this login.
};

class PlayerData
{
	public:
		string	m_strPlayerName;
		string	m_strCharacterName;
		int		m_iCharacterID;
		float		m_fLoginTime;
		int		m_iConnectionID;
};

class PlayerDatabase
{
	private:
		EntityManager*	m_pkEntityMan;
		string			m_strPlayerDirectory;	

		vector<PlayerData>	m_strActivePlayers;		// Names of loged in users.

		
		
	public:
		PlayerDatabase();
		
		bool CreatePlayer(string strPlayer,string strPassword);
		bool VerifyPlayer(string strPlayer,string strPassword);
		
		bool CreateNewCharacter(string strPlayer, string strCharacter);
		Entity* CreateCharacter(string strPlayer, string strCharacter);
		bool SaveCharacter(Entity* pkEntity,string strPlayer);
		
		bool LoginExist(string strLogin);
		bool Login(string strPlayer,string strPassword);
		void Logout(string strPlayer);
		bool IsOnline(string strLogin);

		PlayerData*	GetPlayerData(string	strPlayer);
		PlayerData* GetPlayerData(int iConnection);
		
		vector<string>	GetLoginCharacters(string strLogin);
		void	GetUsers(vector<string>* pkUsers);

};


#endif



