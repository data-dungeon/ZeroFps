#ifndef _ENGINE_P_AI_H_
#define _ENGINE_P_AI_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"


class P_CharacterControl;
class P_CharacterProperty;

using namespace std;


/**	\brief	Da P_AI
		\ingroup Common
*/
class MCOMMON_API P_AI: public Property 
{
	private:
		P_CharacterControl*	m_pkCharacterControl;
		P_CharacterProperty*	m_pkCharacterProperty;
		
		int	m_iState;
		
		
		float m_fSeeDistance;
		float m_fAttackDistance;
		
		//random walk
		bool 	m_bWalk;
		float m_fTime;
		
		//look at
		int	m_iTarget;
		
		
		
		
		vector<PropertyValues> GetPropertyValues(); 
 		bool HandleSetValue( string kValueName, string kValue );
   
		int FindClosestEnemy(float fMaxRange);
		
	public:

		P_AI();
      ~P_AI();
      
		void Init();
	   void Update();
		void Touch(int iID);		

       void Save(ZFIoInterface* pkPackage);
       void Load(ZFIoInterface* pkPackage,int iVersion);
//       void PackTo(NetPacket* pkNetPacket, int iConnectionID );
// 		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

MCOMMON_API Property* Create_P_CharacterProperty();
MCOMMON_API void Register_P_AI(ZeroFps* pkZeroFps);

#endif





