#ifndef _ENGINE_ITEMPROPERTY_H_
#define _ENGINE_ITEMPROPERTY_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include "rulesystem/item/itemstats.h"
#include "rulesystem/sendtype.h"
#include "../zerofpsv2/basic/zfini.h"
#include <list>
   using namespace std;

/**	\brief	Da P_Item
		\ingroup Common
*/
class MCOMMON_API P_Item: public Property 
{
	private:
		string m_kObjectScriptname; // which script the object is created from
                                  // needed when splitting items
	public:

		list<SendType> m_kSends;       // the clients to recieve data from this property

		ItemStats *m_pkItemStats;
		vector<Entity*>* m_pkInventoryList;

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

		void RequestUpdateFromServer (string kType);

		void AddSendsData(SendType);

		bool UseOn ( Entity *pkCharacterObject );

		friend class Container;
};

MCOMMON_API Property* Create_P_Item();

#endif





