#ifndef _P_ML_H_
#define _P_ML_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>
#include "p_spawn.h"

using namespace std;

/**	\brief	Da P_Ml
		\ingroup Common
*/
class MCOMMON_API P_Ml: public Property {
	private:
		Render*				m_pkRender;	
		ZShaderSystem*		m_pkZShaderSystem;
		
		vector<string>		m_kActions;				///> Det skall finnas en 32x32 ikon med samma namn i mappen "data/textures/gui/actions"
				
		bool					m_bShowText;
		string				m_strText;


		vector<PropertyValues> GetPropertyValues();
	public:

		void GetActions(vector<string>& actions) {	actions = m_kActions; }

		P_Ml();
		~P_Ml();
		
		void Update();
		void Init();
		
		void AddAction(const char* csAction);
		
		void SetText(string strText) 					{	m_strText = strText;	};
		string GetText() 									{	return m_strText;		};
				
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

MCOMMON_API Property* Create_P_Ml();

#endif
