#ifndef _ENGINE_CHARACTERPROPERTY_H_
#define _ENGINE_CHARACTERPROPERTY_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include "rulesystem/character/characterstats.h"
#include "../zerofpsv2/basic/zfini.h"

class MCOMMON_API CharacterProperty: public Property 
{
	private:
		CharacterStats *m_pkCharStats;

   public:
	   void Update();
		void CloneOf(Property* pkProperty) {}
		vector<PropertyValues> GetPropertyValues(); 


      CharacterStats* GetCharStats()            { return m_pkCharStats; }

		CharacterProperty( string kName );
		CharacterProperty();

		bool HandleSetValue( string kValueName, string kValue );

};

MCOMMON_API Property* Create_P_CharStats();

#endif





