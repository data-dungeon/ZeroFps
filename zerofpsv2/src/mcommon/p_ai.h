#ifndef _ENGINE_P_AI_H_
#define _ENGINE_P_AI_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"


class P_CharacterControl;

using namespace std;


/**	\brief	Da P_AI
		\ingroup Common
*/
class MCOMMON_API P_AI: public Property 
{
	private:
		P_CharacterControl*	m_pkCharacterControl;

		int	m_iState;
		
		//random walk
		bool 	m_bWalk;
		float m_fTime;
		
		vector<PropertyValues> GetPropertyValues(); 
 		bool HandleSetValue( string kValueName, string kValue );
   
	public:

		P_AI();
      ~P_AI();
      
		void Init();
	   void Update();
		void Touch(int iID);		

//       void Save(ZFIoInterface* pkPackage);
//       void Load(ZFIoInterface* pkPackage,int iVersion);
//       void PackTo(NetPacket* pkNetPacket, int iConnectionID );
// 		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

MCOMMON_API Property* Create_P_CharacterProperty();
MCOMMON_API void Register_P_AI(ZeroFps* pkZeroFps);

#endif





