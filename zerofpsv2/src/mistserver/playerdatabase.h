#ifndef _PLAYERDATABASE_H_
#define _PLAYERDATABASE_H_

using namespace std;

#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/basic/zfsystem.h"

class PlayerDatabase
{
	private:
		EntityManager* m_pkEntityMan;
		string			m_strPlayerDirectory;	

	public:
		PlayerDatabase();
		
		bool CreatePlayer(string strPlayer,string strPassword);
		bool VerifyPlayer(string strPlayer,string strPassword);
		
		bool CreateNewCharacter(string strPlayer, string strCharacter);
		Entity* CreateCharacter(string strPlayer, string strCharacter);
		bool SaveCharacter(Entity* pkEntity,string strPlayer);
		

};


#endif



