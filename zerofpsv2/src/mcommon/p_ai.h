#ifndef _ENGINE_P_AI_H_
#define _ENGINE_P_AI_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"


class P_CharacterControl;
class P_CharacterProperty;

using namespace std;

enum MCOMMON_API AI_STATES
{
	eAI_STATE_RANDOMWALK =	1,
	eAI_STATE_LOOKAT		=	2,
	eAI_STATE_CHASE		=	3,
	eAI_STATE_ATTACK 		=	5,
	eAI_STATE_GUARD 		=	4,
	eAI_STATE_DEAD			=	6,


};


/**	\brief	Da P_AI
		\ingroup Common
*/
class MCOMMON_API P_AI: public Property 
{
	private:
		Render*					m_pkRender;	

		P_CharacterControl*	m_pkCharacterControl;
		P_CharacterProperty*	m_pkCharacterProperty;
		
		int	m_iState;
		int	m_iNextState;
		bool	m_bStateChanged;
		
		
		float m_fSeeDistance;
		float m_fAttackDistance;
		float	m_fStrikeRange;
		
		//random walk
		bool 	m_bWalk;
		float m_fTime;
		
		float	m_fFindTime;
		
		//look at
		int	m_iTarget;		
		
		vector<PropertyValues> GetPropertyValues(); 
 		bool HandleSetValue( string kValueName, string kValue );
   
		int FindClosestEnemy(float fMaxRange);
		void UseOffensiveSkill();
		float GetOffensiveRange();
		
		bool ValidTarget(int iTarget);
		
	public:

		P_AI();
      ~P_AI();

		void ChangeState(int iNewState);
		void SetState (int iNewState);
      void SetTarget (int iNewTarget)		{ m_iTarget = iNewTarget; }
		
		bool States(int iEvent, int iState);
		void Process(int iEvent);

		void Init();
	   void Update();
		void DrawEditor();
		void DrawCircle(float fRadius, char* szEditColor);
		void Touch(int iID);		

       void Save(ZFIoInterface* pkPackage);
       void Load(ZFIoInterface* pkPackage,int iVersion);
//       void PackTo(NetPacket* pkNetPacket, int iConnectionID );
// 		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

MCOMMON_API Property* Create_P_CharacterProperty();
MCOMMON_API void Register_P_AI(ZeroFps* pkZeroFps);

#endif





