#ifndef _GAME_H_
#define _GAME_H_

#define _MAINAPPLICATION_

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"
#include "itembox.h"

#include "../common/common.pkg"

/*
#include "playercontrolproperty.h"
#include "ballobject.h"
#include "playerobject.h"
#include "bunnyobject.h"
#include "teleportproperty.h"
#include "adaptorsniper.h"
#include "treeidleproperty.h"
*/

class GameScript;
class GameScriptInterface;

enum GAME_STATE
{
	GAME_STATE_INGAME,
	GAME_STATE_MENU,
};

class GameScript;

class Game :public Application {
	private:
		
//		HeightMap *m_pkMap;
//		HeightMapObject *m_pkHeightMapObject;

		Camera *m_pkCamera;;

//		string m_kMapBaseDir;

		int m_iGameState;

		void RegisterPropertys();	
		void CreateZones();
		void Init();
		void SetUpMenuScreen();

		void InitScript();
		void InitGui();
		void Input();				
		void SetupLevel();		
//		bool LoadLevel(const char* acFile);				

//		void Clear();		
//		void CreateNew(int iSize);
		
		PlayerObject* m_pkPlayer;
		float* m_pfPlayerHealth;
		float* m_pfPlayerArmor;

		GameScript* m_pkScript;
		GameScriptInterface* m_pkGameScriptInterface;

		int m_iActionOpenInventory, m_iActionCloseInventory;

	public:
		
//		Core	m_kTestCore;

		ItemBox* m_pkPlayerInventoryBox;
		ItemBox* m_pkContainerBox;

		enum FuncId_e
		{
			FID_LOAD,
			FID_UNLOAD,
		};
	
		Game(char* aName,int iWidth,int iHeight,int iDepth);
		~Game() {};
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);

		void OnServerStart(void);
		void OnClientStart(void);
		
		void RunCommand(int cmdid, const CmdArgument* kCommand);

		friend class GameScriptInterface;
};



#endif






