#ifndef _ENGINE_ITEMPROPERTY_H_
#define _ENGINE_ITEMPROPERTY_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include "rulesystem/item/itemstats.h"
#include "../zerofpsv2/basic/zfini.h"

class MCOMMON_API P_Item: public Property 
{
	private:
		ItemStats *m_pkItemStats;

   public:
	   void Update();
		void CloneOf(Property* pkProperty) {}
		vector<PropertyValues> GetPropertyValues(); 


		P_Item( string kName );
		P_Item();

		bool HandleSetValue( string kValueName, string kValue );

};

MCOMMON_API Property* Create_P_Item();

#endif





