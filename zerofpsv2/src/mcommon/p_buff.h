#ifndef _P_BUFF_H_
#define _P_BUFF_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"


class P_CharacterProperty;

using namespace std;

/**	\brief	Da P_Item
		\ingroup Common
*/


class MCOMMON_API P_Buff: public Property 
{
	private:
		P_CharacterProperty*	m_pkCharacter;
		
		
		void Enable(P_CharacterProperty* pkCP);
		void Disable();
 		
// 		vector<PropertyValues> GetPropertyValues();
			
	public:
		
		P_Buff();
		~P_Buff();
		
		void Init();
		void Update();
		
		P_CharacterProperty*	GetCharacter()				{	return m_pkCharacter;	}
		
// 		void Save(ZFIoInterface* pkPackage);
// 		void Load(ZFIoInterface* pkPackage,int iVersion);


};

MCOMMON_API Property* Create_P_Buff();
MCOMMON_API void Register_P_Buff(ZeroFps* pkZeroFps);
#endif





