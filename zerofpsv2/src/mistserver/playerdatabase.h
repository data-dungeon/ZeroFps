#ifndef _PLAYERDATABASE_H_
#define _PLAYERDATABASE_H_


#include <vector>
#include <string>

class EntityManager;
class Entity;

using namespace std;

/**	\brief	PlayerDatabase
		\ingroup MistServer

	The part of the server that stores all users and their rights with what they can do on the server.
	All users are stored in dir /logins and each user get there own dir. In each user dir are a data file
	with things like passwords and user rights.

	All users have a user account in the /logins dir.

		Info:	Namn, osv.
		Rätt:	Vad denna login kan göra.
		Kar:	Alla karaktärer kopplade till detta login.

	Login(Name, Password):	Logs in a user.
	Logout(Name):			Logs out a user.
	
	GetOnline(List)			Get a list of users loged in.

*/

#define MAX_LOGIN_STR	128

class	LoginPrivilege
{
public:
	bool	m_bPlayer;			// Can join and play the game.
	bool	m_bBuilder;			// Can join, edit and also save the game.
	bool	m_bGameMaster;		// Can join and act as a game master.
	bool	m_bAdmin;			// Can join and admin the rights of other users.
};

// Data stored on disk about a login.
class	LoginData
{
	public:
		char				szName[MAX_LOGIN_STR];		// Name of this login.
		char				szPass[MAX_LOGIN_STR];		// Password of this login.
		LoginPrivilege	m_kPrivilege;
};

class PlayerData
{
	public:
		string			m_strPlayerName;
		string			m_strCharacterName;
		int				m_iCharacterID;
		float				m_fLoginTime;
		int				m_iConnectionID;
};

class PlayerDatabase
{
	private:
		EntityManager*			m_pkEntityMan;
		string					m_strPlayerDirectory;	

		vector<PlayerData>	m_strActivePlayers;		// Names of loged in users.

		LoginPrivilege			m_bStdPrivilege;			// Prev set on new users.
		
	public:
		PlayerDatabase();

		string PrevToStr(LoginPrivilege kPrev);
		LoginPrivilege StrToPrev(string strPrev);

		// User Mangment
		bool UserExist(string strUser);									// Checks if the user already has a account
		bool UserCreate(string strUser,string strPassword);		// Creates a new account
		bool UserDelete(string strUser);									// Delete a existing account.
		bool VerifyUser(string strPlayer,string strPassword);		// Check users password.
		bool HasPrivilege(string strLoginName, string strPrev);	// Returns true if user as all of the given Privileges.
		void SetUserPrev(string strLoginName, string strPrev);	// Set prev of a user account.
		string GetPrivilege(string strUser);							// Return prev of user as a string.

		// Login/Logout
		bool Login(string strPlayer,string strPassword);
		void Logout(string strPlayer);
		bool IsOnline(string strLogin);
		void GetUsers(vector<string>* pkUsers);

		// Character
		bool CharacterCreate(string strPlayer, string strCharacter, string strModell);
		bool CharacterSave(Entity* pkEntity,string strPlayer);
		void CharacterDelete(string strPlayer, string strCharacter);
		Entity* CharacterSpawn(string strPlayer, string strCharacter);
		vector<string>	GetLoginCharacters(string strLogin);

		// Get PlayerData
		PlayerData*	GetPlayerDataByCharacterID(int iEntityID);
		PlayerData*	GetPlayerData(string	strPlayer);
		PlayerData* GetPlayerData(int iConnection);
		
		void	GetPlayerEntitys(vector<pair<Entity*,int> >* pkEnts);
};


#endif



