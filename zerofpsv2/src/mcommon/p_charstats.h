#ifndef _ENGINE_CHARACTERPROPERTY_H_
#define _ENGINE_CHARACTERPROPERTY_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include "rulesystem/character/characterstats.h"
#include "../zerofpsv2/basic/zfini.h"
#include "rulesystem/sendtype.h"

class MCOMMON_API CharacterProperty: public Property 
{
	private:
		CharacterStats *m_pkCharStats;

   public:
      vector<SendType> m_kSends;       // the clients to recieve data from this property

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

};

MCOMMON_API Property* Create_P_CharStats();

#endif





