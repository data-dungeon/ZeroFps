#ifndef _STOPEMUP_H_
#define _STOPEMUP_H_

#include "../zerofpsv2/engine/application.h"
#include <vector>

class Camera;
class Entity;

class StopEmUp :public Application , public ZGuiApp 
{
	private:
		enum FuncId_e
		{
			FID_NEWGAME,			
		};		

		enum ePKGTYPE
		{
			eSC_PLAYERID = 0,
			eCS_CONTROLS = 1,
			eSC_LEVELDATA =2,			
		};
		
			
		bool		m_bServerMode;	
		Entity*	m_pkCameraEntity;
		Camera*	m_pkCamera;
		
		int							m_iStartLevel;
		string						m_strMap;
		int							m_iLevel;
		float							m_fLevelWait;
		vector<pair<int,int> >	m_kPlayers;
		vector<int> 				m_kSpawners;
		int							m_iLives;
		
		int							m_iGoalEnt;
				
		//client stuff
		int			m_iPlayerID;
		bool			m_bFire;
		bool			m_bSFire;
		bitset<6>	m_kCharacterControls;
		
		int			m_iEnergy;
		int			m_iMaxEnergy;
		int			m_iScore;
		int			m_iKills;
		int			m_iCurrentLevel;
		int			m_iCurrentLives;
		int			m_iStopers;
		string		m_strGunName;
		
		
		void CreateCamera();
		int CreatePlayerEntity(int iConnID);
		
		
		//player management
		void AddPlayer(int iID,int iEntity);
		void RemovePlayer(int iID);
		int GetPlayerEntity(int iID);
		
		void Input();
		
		void SendControlInfo();
		void SendLevelData();
		
		void SetupLevel(int iLevel);
		bool LevelComplete();
		int  HasGoal();	
		
	public:
		StopEmUp(char* aName,int iWidth,int iHeight,int iDepth);
		
		void OnInit();
		bool StartUp()		{return true;}
		bool ShutDown()	{return true;}
		bool IsValid()		{return true;}	
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void OnSystem(void);
		void OnIdle();
		
		void OnServerStart();
		void OnClientStart();
		void OnServerClientJoin(ZFClient* pkClient,int iConID, char* szLogin, char* szPass, bool bIsEditor);
		void OnServerClientPart(ZFClient* pkClient,int iConID);
		void OnNetworkMessage(NetPacket *PkNetMessage);
		
		
		int GetLevel()		{	return m_iLevel				;};
		int GetPlayers()	{	return m_kPlayers.size()	;};
		int GetGoalEnt()	{	return m_iGoalEnt				;};
		void RemoveLife();
		
};



#endif



