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
		
		string	m_strName;
		string	m_strIcon;
		float		m_fTimeOut;
		char		m_cType;
		bool		m_bShow;		
		
		
		float		m_fAddTime;
		
		
		void Enable(P_CharacterProperty* pkCP);
		void Disable();
		
		vector<PropertyValues> GetPropertyValues();
			
	public:
	
			
		P_Buff();
		~P_Buff();
		
		void Init();
		void Update();
		
		P_CharacterProperty*	GetCharacter()				{	return m_pkCharacter;	}
	
		
		friend class P_CharacterProperty;	
};

MCOMMON_API Property* Create_P_Buff();
MCOMMON_API void Register_P_Buff(ZeroFps* pkZeroFps);
#endif





