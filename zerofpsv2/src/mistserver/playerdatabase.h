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
	Sk�ter logins och hanteringen av karakt�rer. Alla som kan ansluta har ett login
	som inneh�ller information, password, r�ttigheter och karakt�rer.

		Info:	Namn, osv.
		R�tt:	Vad denna login kan g�ra.
		Kar:	Alla karakt�rer kopplade till detta login.

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

class PlayerDatabase
{
	private:
		EntityManager*	m_pkEntityMan;
		string			m_strPlayerDirectory;	

		vector<string>	m_strActiveUsers;		// Names of loged in users.

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

		vector<string>	GetLoginCharacters(string strLogin);
		vector<string>	GetUsers();

};


#endif



