#include "p_spell.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/engine_systems/propertys/p_psystem.h"
//#include "../zerofpsv2/engine_systems/propertys/p_physic.h"
#include "rulesystem/rulesystem.h"
#include "p_charstats.h"
#include "p_item.h"

// -----------------------------------------------------------------------------------------------

P_Spell::P_Spell( string kName )
{
   P_Spell();
}

// -----------------------------------------------------------------------------------------------

P_Spell::~P_Spell()
{
   // remove bonuses
   Bonuses(false);
}

// -----------------------------------------------------------------------------------------------

P_Spell::P_Spell()
{
	m_iSide = PROPERTY_SIDE_SERVER;

   m_pkSpellType = 0;
   m_fDamageTimer = 0;

	strcpy(m_acName,"P_Spell");

   m_fAge = 0;
   m_iPSIndex = 0;

   bNetwork = false;

}

// -----------------------------------------------------------------------------------------------

void P_Spell::Update()
{

   // if spelltype hasn't been given, do nothing
   if ( m_pkSpellType )
   {

      // update lived time if spell isn't permanent
      if ( m_pkSpellType->m_fLifeTime != -1 )
	      m_fAge += m_pkZeroFps->m_pkObjectMan->GetSimDelta();


      // TODO!!! Only test for damage if spell MAKES damage and is stuck on CharacterObject

      // if spell i cast upon character, damagecounter
      m_fDamageTimer += m_pkZeroFps->GetFrameTime();


      if ( m_fDamageTimer > m_pkSpellType->m_fDamageEvery )
      {
         m_fDamageTimer = 0;
         DealDamage ( &m_pkSpellType->m_kDamage, m_pkEntity );
      }


      // create new PSystems
      if ( (int) m_pkSpellType->m_kGraphicEffects.size() > m_iPSIndex )
      {
         if ( m_fAge >= m_pkSpellType->m_kGraphicEffects[m_iPSIndex].m_fStartTime )
         {
            // can't have more than one PSystem/object, so create a new object for each new PS *sigh*
            Entity *pkNewObject = m_pkEntity->m_pkEntityManager->CreateEntity();

            // create and attach PS to new object
            P_PSystem *pkPSProp =  (P_PSystem*)pkNewObject->AddProperty ("P_PSystem");
            pkPSProp->SetPSType ( m_pkSpellType->m_kGraphicEffects[m_iPSIndex].m_kPSystemName );

            // add PSystem offset
            pkPSProp->GetPSystem()->m_kPosOffset += m_pkSpellType->m_kGraphicEffects[m_iPSIndex].m_kOffset;

            pkNewObject->SetParent (m_pkEntity);
            pkNewObject->SetRelativeOri(true);

            pkNewObject->AttachToZone();

            m_iPSIndex++;

            // remember created objects so they are destroyed at same time as the spell is removed
            m_kPSystems.push_back (pkNewObject);

         }
      }

      // test collisions vs objects in spell area
      if ( m_pkSpellType->m_fStartRadius || m_pkSpellType->m_fEndRadius )
         DoCollisions();

      // if lifetime is over
      if ( m_fAge >= m_pkSpellType->m_fLifeTime )
      {
         // remove&delete PSystems spell used
         for ( unsigned int i = 0; i < m_kPSystems.size(); i++ )
            m_pkEntity->m_pkEntityManager->Delete( m_kPSystems[i] );

         // delete spell-object or property
//         if ( m_pkSpellType->m_iOnDestruction == eKILL_SELF )
//            m_pkObject->RemoveProperty ( this );
//         else

         //m_pkObject->m_pkObjectMan->Delete( m_pkObject ); 
      }


   }
}

// ------------------------------------------------------------------------------------------
 
// which object the spell is cast upon
bool P_Spell::CastOn ( Entity *pkObject )
{  
   // temp test...i think....

   // if the spell is projectile type

   // temp, for now, all spells are projectiles :)
   //if ( m_pkSpellType->
/*
   // add a physicproperty so we can move the spell
   if ( m_pkSpellType->m_kSpellName == "fire of evil death spell fireballstuff" )
   {
      PhysicProperty* pkPsyProp = (PhysicProperty*)m_pkObject->AddProperty("PhysicProperty");
      m_pkObject->GetAcc().x = 50.f;
   }
*/


   return false;
}

// ------------------------------------------------------------------------------------------

 //false = remove bonuses (on end of spell)
void P_Spell::Bonuses(bool bAddBonuses)
{
   CharacterProperty *pkCharStats = (CharacterProperty*)m_pkEntity->GetProperty("P_CharStats");
   P_Item *pkItem = (P_Item*)m_pkEntity->GetProperty("P_Item");


   // if parent is item...
   if ( pkItem )
   {
      int iModi;

      if ( bAddBonuses )
         iModi = 1;
      else 
         iModi = -1;

      map<string, int>::iterator kIte;

      // add skillbonuses
      for ( kIte = m_pkSpellType->m_kSkillBonuses.begin(); 
            kIte != m_pkSpellType->m_kSkillBonuses.end(); kIte++ )
         pkItem->m_pkItemStats->AddToSkillBonus( (*kIte).first, (*kIte).second * iModi );

      // add attributebonuses
      for ( kIte = m_pkSpellType->m_kAttributeBonuses.begin(); 
            kIte != m_pkSpellType->m_kAttributeBonuses.end(); kIte++ )
         pkItem->m_pkItemStats->AddToAttributeBonus( (*kIte).first, (*kIte).second * iModi );

      // add attackbonuses
      for ( kIte = m_pkSpellType->m_kFightBonuses.m_kAttack.begin(); 
            kIte != m_pkSpellType->m_kFightBonuses.m_kAttack.end(); kIte++ )
         pkItem->m_pkItemStats->AddToAttackBonus( (*kIte).first, (*kIte).second * iModi );

      // add defencebonuses
      for ( kIte = m_pkSpellType->m_kFightBonuses.m_kDefence.begin(); 
            kIte != m_pkSpellType->m_kFightBonuses.m_kDefence.end(); kIte++ )
         pkItem->m_pkItemStats->AddToDefenceBonus( (*kIte).first, (*kIte).second * iModi );

   }

   // if parent has character...
   if ( pkCharStats )
   {
      int iModi;

      if ( bAddBonuses )
         iModi = 1;
      else 
         iModi = -1;

      map<string, int>::iterator kIte;

      // add skillbonuses
      for ( kIte = m_pkSpellType->m_kSkillBonuses.begin(); 
            kIte != m_pkSpellType->m_kSkillBonuses.end(); kIte++ )
         pkCharStats->GetCharStats()->AddSkillValue( (*kIte).first, (*kIte).second * iModi );

      // add attributebonuses
      for ( kIte = m_pkSpellType->m_kAttributeBonuses.begin(); 
            kIte != m_pkSpellType->m_kAttributeBonuses.end(); kIte++ )
         pkCharStats->GetCharStats()->AddAttributeValue( (*kIte).first, (*kIte).second * iModi );

      // add attackbonuses
      for ( kIte = m_pkSpellType->m_kFightBonuses.m_kAttack.begin(); 
            kIte != m_pkSpellType->m_kFightBonuses.m_kAttack.end(); kIte++ )
         pkCharStats->GetCharStats()->AddAttackValue( (*kIte).first, (*kIte).second * iModi );

      // add defencebonuses
      for ( kIte = m_pkSpellType->m_kFightBonuses.m_kDefence.begin(); 
            kIte != m_pkSpellType->m_kFightBonuses.m_kDefence.end(); kIte++ )
         pkCharStats->GetCharStats()->AddDefenceValue( (*kIte).first, (*kIte).second * iModi );
   }


}

// ------------------------------------------------------------------------------------------

vector<PropertyValues> P_Spell::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
/*	kReturn[0].kValueName = "PSType";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kPSType;*/

	return kReturn;
}

// ------------------------------------------------------------------------------------------

bool P_Spell::HandleSetValue( string kValueName, string kValue )
{
	if( strcmp(kValueName.c_str(), "SpellType") == 0 ) 
	{
      map<string, SpellStats*>::iterator kIte = g_kSpells.find(kValue);

      // check if spelltype is loaded
      if ( kIte != g_kSpells.end() )
      {
         m_pkSpellType = g_kSpells[kValue];

         Bonuses(true);

         CastOn(0);

         return true;
      }
      else
      {
         // if spell not is loaded, check if spell can be loaded
         if ( LoadSpell(kValue) )
         {
            m_pkSpellType = g_kSpells[kValue];

            Bonuses(true);

            CastOn(0);

            return true;
         }
         else
         {
            cout << "Error! Couln't find spell " << kValue << endl;
            return false;
         }
      }
	}

	return false;
}

// ------------------------------------------------------------------------------------------

P_Spell* P_Spell::Copy()
{
   P_Spell *pkNewP = new P_Spell;

   pkNewP->m_pkSpellType = m_pkSpellType;
   pkNewP->m_fAge = m_fAge;

   // don't copy PSIndex since we want every PSystem the spell should use
   //pkNewP->m_iPSIndex = m_iPSIndex;

   for ( unsigned int i = 0; i < m_kAttackedObjects.size(); i++ )
      pkNewP->m_kAttackedObjects.push_back ( m_kAttackedObjects[i] );

   pkNewP->m_kAttackedObjects.push_back ( m_pkEntity->GetEntityID() );

   return pkNewP;
}

// ------------------------------------------------------------------------------------------

void P_Spell::DoCollisions()
{
   // get zone spell is in
   ZoneData* pkZone = m_pkEntity->m_pkEntityManager->GetZone( m_pkEntity->GetWorldPosV() );

   // get all objects in zone
   vector<Entity*> kObjects;
   pkZone->m_pkZone->GetAllEntitys(&kObjects);

   for ( unsigned int i = 0; i < kObjects.size(); i++ )
   {
      
      bool bOk = true;
      
      // don't care if spell collides with own ps or self
      // don't test against the zone object :) We don't want to burn that away
      if ( kObjects[i] == m_pkEntity || pkZone->m_pkZone == kObjects[i] )
         bOk = false;


      // don't test against own psystems
      for ( unsigned int j = 0; j < m_kPSystems.size(); j++ )
         if ( kObjects[i] == m_kPSystems[j] )
            bOk = false;

      // check if the object and the spell hasn't already collided
      for ( unsigned int h = 0; h < m_kAttackedObjects.size(); h++ )
         if ( kObjects[i]->GetEntityID() == m_kAttackedObjects[h] )
            bOk = false;

      // if object was ok to test..
      if ( bOk )
      {
         bOk = false;

         // check if object really is affected by the spell
         if ( m_pkSpellType->m_kAffectedObjects == "all" )
            bOk = true;

         if ( m_pkSpellType->m_kAffectedObjects == "characters" )
            if ( kObjects[i]->GetProperty("P_CharStats") )
               bOk = true;
            else
               m_kAttackedObjects.push_back (kObjects[i]->GetEntityID());

         if ( m_pkSpellType->m_kAffectedObjects == "items" )
            if ( kObjects[i]->GetProperty("P_Item") )
               bOk = true;
            else
               m_kAttackedObjects.push_back (kObjects[i]->GetEntityID());

         if ( m_pkSpellType->m_kAffectedObjects == "characters_and_items" || 
              m_pkSpellType->m_kAffectedObjects == "items_and_characters" )
            if ( kObjects[i]->GetProperty("P_Item") || kObjects[i]->GetProperty("P_CharStats") )
               bOk = true;
            else
               m_kAttackedObjects.push_back (kObjects[i]->GetEntityID());

         // if the object had right to be affacted by the spell
         if ( bOk )
         {
				Vector3 Pos = m_pkEntity->GetLocalPosV();
            float fDist = float( kObjects[i]->GetLocalPosV().DistanceXZTo ( Pos ) );

            // calculate collision radius
            float fColRad = ((m_fAge / m_pkSpellType->m_fLifeTime) * m_pkSpellType->m_fEndRadius) +
                            ((1-(m_fAge / m_pkSpellType->m_fLifeTime)) * m_pkSpellType->m_fStartRadius);

            // test is a object was hit
            if ( fDist < fColRad )
            {
               // check which event concur when something is hit

               // attach new spell to hit object
               if ( m_pkSpellType->m_kOnHit[0] == "attachnewspell" )
               {
                  Entity *pkNewSpell =
                  m_pkEntity->m_pkEntityManager->CreateEntityFromScript ( m_pkSpellType->m_kOnHit[1].c_str() );

                  if ( pkNewSpell )
                  {
                     pkNewSpell->SetLocalPosV ( Vector3(0,0,0) );

                     // set hit object as parent
                     pkNewSpell->SetParent ( kObjects[i] );
                     pkNewSpell->SetRelativeOri(true);

                     pkNewSpell->AttachToZone();
                     
                     // It's annoying to hit the new spell with the spell just casted so....
                     m_kAttackedObjects.push_back (pkNewSpell->GetEntityID());  
                  }

               }

               // destroy spell (TODO!!!: after dealt damage!!! )
               if ( m_pkSpellType->m_kOnHit[0] == "destroyspell" ) 
                  m_pkEntity->m_pkEntityManager->Delete ( m_pkEntity );

               // creates a new spell at the hit location and removes the old one
               if ( m_pkSpellType->m_kOnHit[0] == "createnewspell" ) 
               {
                  Entity *pkNewSpell =
                  m_pkEntity->m_pkEntityManager->CreateEntityFromScriptInZone (m_pkSpellType->m_kOnHit[1].c_str(), 
                                                                           kObjects[i]->GetWorldPosV() );
                  pkNewSpell->SetWorldPosV ( kObjects[i]->GetWorldPosV() );
                  /*
                  Object *pkNewSpellObject = m_pkObject->m_pkObjectMan->CreateObject();
            
                  P_Spell *pkNewSpell = new P_Spell;
                  pkNewSpellObject->AddProperty( pkNewSpell );
                  pkNewSpell->SetValue ( "SpellType", m_pkSpellType->m_kOnHit[1] );
            
                  pkNewSpellObject->SetWorldPosV ( m_pkObject->GetWorldPosV() );
                  pkNewSpellObject->SetWorldRotV ( m_pkObject->GetWorldRotV() );

                  pkNewSpellObject->AttachToZone();
                  */

                  m_pkEntity->m_pkEntityManager->Delete ( m_pkEntity );
               }

               // creates a psystem at the hit location and removes the spell
               if ( m_pkSpellType->m_kOnHit[0] == "createpsystem" ) 
               {
                  Entity *pkNewPSystem = m_pkEntity->m_pkEntityManager->CreateEntity();
            
                  P_PSystem *pkNewPSProp = new P_PSystem;
                  kObjects[i]->AddProperty( pkNewPSProp );
                  pkNewPSProp->SetValue ( "PSType", m_pkSpellType->m_kOnHit[1] );
            
                  pkNewPSystem->SetWorldPosV ( m_pkEntity->GetWorldPosV() );
                  pkNewPSystem->SetWorldRotV ( m_pkEntity->GetWorldRotV() );

                  pkNewPSystem->AttachToZone();

                  m_pkEntity->m_pkEntityManager->Delete ( m_pkEntity );
               }

               // the spell maskes a copy of itself that attaches on the hit object
               if ( m_pkSpellType->m_kOnHit[0] == "spread" ) 
                   kObjects[i]->AddProperty ( Copy() );

               // remember which objects is hit by the spell so it can't hit same object twice
               m_kAttackedObjects.push_back (kObjects[i]->GetEntityID());

               CharacterProperty* pkCP = (CharacterProperty*)kObjects[i]->GetProperty("P_CharStats");

               if ( pkCP )
                  DealDamage( &m_pkSpellType->m_kDamage, pkCP->GetCharStats() );

            }
         }
      }



   }
}

// ------------------------------------------------------------------------------------------

void P_Spell::Save(ZFIoInterface* pkPackage)
{
  
}

// ------------------------------------------------------------------------------------------

void P_Spell::Load(ZFIoInterface* pkPackage,int iVersion)
{

}

// ------------------------------------------------------------------------------------------

void P_Spell::PackTo(NetPacket* pkNetPacket)
{
}

// ------------------------------------------------------------------------------------------

void P_Spell::PackFrom(NetPacket* pkNetPacket)
{

}

// ------------------------------------------------------------------------------------------



Property* Create_P_Spell()
{
	return new P_Spell;
}

