#ifndef _ENGINE_ITEMPROPERTY_H_
#define _ENGINE_ITEMPROPERTY_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include "rulesystem/item/itemstats.h"
#include "../zerofpsv2/basic/zfini.h"
#include <vector>
   using namespace std;

class MCOMMON_API P_Item: public Property 
{
	private:
      string m_kObjectScriptname; // which script the object is created from
                                  // needed when splitting items
   public:
      vector<int> m_kSends;       // the clients to recieve data from this property

		ItemStats *m_pkItemStats;

	   void Update();
		void CloneOf(Property* pkProperty) {}
		vector<PropertyValues> GetPropertyValues(); 

		P_Item( string kName );
		P_Item();

      void Save(ZFIoInterface* pkPackage);
      void Load(ZFIoInterface* pkPackage);

      void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		bool HandleSetValue( string kValueName, string kValue );

      Entity* Split ( int iTookens );
      bool Stock ( Entity *pkObject );

      void RequestUpdateFromServer ();
};

MCOMMON_API Property* Create_P_Item();

#endif





