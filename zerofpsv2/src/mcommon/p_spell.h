#ifndef _ENGINE_SPELLPROPERTY_H_
#define _ENGINE_SPELLPROPERTY_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine_systems/propertys/psystemproperty.h"
#include "mcommon_x.h"
#include "rulesystem/spell/spellstats.h"
//#include "../zerofpsv2/basic/zfini.h"

class MCOMMON_API P_Spell: public Property 
{
	private:

		float 
         m_fAge,
         m_fDamageTimer;

      int m_iPSIndex;
      vector<int> m_kAttackedObjects; // keep track of objects already affected by the spell
      vector<Object*> m_kPSystems; // which PSystems spell has created. If spell vanish, so shall Psystems

      void DoCollisions();

   public:
		SpellStats *m_pkSpellType; // template for spelltype

	   void Update();
		void CloneOf(Property* pkProperty) {}
      P_Spell* Copy();

		vector<PropertyValues> GetPropertyValues(); 

		bool CastOn ( Object *pkObject ); // which object the spell is cast upon

      void SetCaster ( int iNetWorkID )            { m_kAttackedObjects.push_back(iNetWorkID); }

      P_Spell( string kName );
		P_Spell();
      ~P_Spell();

      void Bonuses(bool bAddBonuses); //false = remove bonuses (on end of spell)

      void Save(ZFIoInterface* pkPackage);
      void Load(ZFIoInterface* pkPackage);

      void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

		bool HandleSetValue( string kValueName, string kValue );

      bool operator= ( P_Spell &kP_Spell);

};

MCOMMON_API Property* Create_P_Spell();

#endif





