#ifndef _ENGINE_P_AI_H_
#define _ENGINE_P_AI_H_

#include "../zerofpsv2/engine/property.h"
#include "p_charstats.h"
#include "mcommon_x.h"
#include "../zerofpsv2/basic/zfini.h"
#include <deque>
#include <vector>
#include <string>
#include <map>
#include "../zerofpsv2/engine_systems/propertys/p_sound.h"

   using namespace std;

struct Order
{
   string 
      m_kOrderType,
      m_kType;
   
   int 
      m_iTargetID,
      m_iTargetID2;

   bool m_bDynamic;

   Vector3 m_kPosition;
   
};

enum { eIDLEMODE, eATTACKMODE };

/**	\brief	Da P_AI
		\ingroup Common
*/
class MCOMMON_API P_AI: public Property 
{
	private:
      deque<Order*> m_kStaticOrders;   // once list is finished, it is started over..idle orders
      deque<Order*> m_kDynamicOrders;  // pop order once executed
      deque<Order*> m_kConstantOrders; // check order every frame

      deque<Order*>::iterator m_kOrderIte;

      int m_eAI_Mode; // attackmode, idlemode...

      Order* m_pkCurrentOrder;

      // pointer to the characters charprop.
      CharacterProperty* m_pkCharProp;
		P_Sound* m_pkSound;

		map<int, bool> m_kEnemies; // objectID of enemies

      bool m_bAIPlayer;

		// if enemy lists gets to big, clean up all non existing or active objects in it
		void CleanEnemies ();

   public:

      void NextOrder();

		void AddEnemy ( int iEnemyID );
		void RemoveEnemy ( int iEnemyID );

      void AddStaticOrder ( string kOrderType, int iTargetID1, int iTargetID2, Vector3 kPosition, string kType );
      void AddDynamicOrder ( string kOrderType, int iTargetID1, int iTargetID2, Vector3 kPosition, string kType );

	   void Update();
		vector<PropertyValues> GetPropertyValues(); 

      void Init();

		P_AI( string kName );
		P_AI();
      ~P_AI();

      void Save(ZFIoInterface* pkPackage);
      void Load(ZFIoInterface* pkPackage,int iVersion);

      void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		bool HandleSetValue( string kValueName, string kValue );

      bool HaveTarget()                               { return m_eAI_Mode == eATTACKMODE; }

      void ClearDynamicOrders();

      void SetAIIsPlayer (bool bIsPlayer)             { m_bAIPlayer = bIsPlayer; }
      bool PlayerAI ()                                { return m_bAIPlayer; }
};

MCOMMON_API Property* Create_P_AI();

#endif





