#ifndef _P_POWERUP_H_
#define _P_POWERUP_H_

#include "../zerofpsv2/engine/property.h"

class StopEmUp;
class Render;

class P_Powerup : public Property
{
	private:
		StopEmUp*	m_pkStopEmUp;
		ZSSRender*	m_pkRender;	
			
		//over head text
		ZMaterial*	m_pkTextMaterial;
		ZGuiFont*	m_pkFont;				
		
	public:
		int		m_iPowerupType;
		string	m_strPowerupName;
		
		P_Powerup();
		~P_Powerup();	
	
		void Update();		
		void Touch(int iID);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;				
};


Property* Create_P_Powerup();

#endif

