#ifndef _ENGINE_CHARACTERPROPERTY_H_
#define _ENGINE_CHARACTERPROPERTY_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include "rulesystem/character/characterstats.h"
#include "../zerofpsv2/basic/zfini.h"
#include "rulesystem/sendtype.h"

enum { eATTRIBUTES, eSKILLS, eDATA, eHP, eCONT, eCONTID };

/**	\brief	Da CharacterProperty
		\ingroup Common
*/
class MCOMMON_API CharacterProperty: public Property 
{
	private:
		CharacterStats *m_pkCharStats;
      vector<Entity*>* m_pkInventoryList;

   public:
      list<SendType> m_kSends;       // the clients to recieve data from this property

      void Init();

	   void Update();
		void CloneOf(Property* pkProperty) {}
		vector<PropertyValues> GetPropertyValues(); 


      CharacterStats* GetCharStats()            { return m_pkCharStats; }

		CharacterProperty( string kName );
		CharacterProperty();

      void Save(ZFIoInterface* pkPackage);
      void Load(ZFIoInterface* pkPackage);

      void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		bool HandleSetValue( string kValueName, string kValue );

      void RequestUpdateFromServer( string kData );

      void AddSendsData(SendType kNewSendData);

};

MCOMMON_API Property* Create_P_CharStats();

#endif

















