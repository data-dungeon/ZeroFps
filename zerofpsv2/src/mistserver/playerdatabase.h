#ifndef _PLAYERDATABASE_H_
#define _PLAYERDATABASE_H_


#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/basic/zfsystem.h"
#include <vector>

using namespace std;

/**	\brief	Da PlayerDatabase
		\ingroup MistServer
*/


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
		
		vector<string>	GetLoginCharacters(string strLogin);
		vector<string>	GetUsers();

};


#endif



