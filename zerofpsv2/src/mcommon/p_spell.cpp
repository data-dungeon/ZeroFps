#include "p_spell.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/engine_systems/propertys/psystemproperty.h"
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
	m_iSide = PROPERTY_SIDE_CLIENT;

	bNetwork = true;

   m_pkSpellType = 0;

	strcpy(m_acName,"P_Spell");

   m_fAge = 0;
   m_iPSIndex = 0;
}

// -----------------------------------------------------------------------------------------------

void P_Spell::Update()
{
   // if spelltype hasn't been given, do nothing
   if ( m_pkSpellType )
   {

      // update lived time if spell isn't permanent
      if ( m_pkSpellType->m_fLifeTime != -1 )
	      m_fAge += m_pkZeroFps->GetFrameTime();


      // create new PSystems
      if ( m_pkSpellType->m_kGraphicEffects.size() > m_iPSIndex )
      {
         if ( m_fAge >= m_pkSpellType->m_kGraphicEffects[m_iPSIndex].m_fStartTime )
         {
            // can't have more than one PSystem/object, so create a new object for each new PS *sigh*
            Object *pkNewObject = m_pkObject->m_pkObjectMan->CreateObject();

            // create and attach PS to new object
            pkNewObject->AddProperty ("PSystemProperty");
            PSystemProperty *pkPSProp = (PSystemProperty*)pkNewObject->GetProperty("PSystemProperty");
            pkPSProp->SetPSType ( m_pkSpellType->m_kGraphicEffects[m_iPSIndex].m_kPSystemName );

            // add PSystem offset
            pkPSProp->GetPSystem()->m_kPosOffset += m_pkSpellType->m_kGraphicEffects[m_iPSIndex].m_kOffset;

            pkNewObject->SetParent (m_pkObject);
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
            m_pkObject->m_pkObjectMan->Delete( m_kPSystems[i] );

         // delete spell-object or property
         if ( m_pkSpellType->m_iOnDestruction == eKILL_SELF )
            m_pkObject->RemoveProperty ( this );
         else
            m_pkObject->m_pkObjectMan->Delete( m_pkObject ); 
      }


   }
}

// ------------------------------------------------------------------------------------------
 
// which object the spell is cast upon
bool P_Spell::CastOn ( Object *pkObject )
{  
   // remove from parent
   m_pkObject->RemoveProperty (this);

   // remove the parent from the objectsystem
   m_pkObject->m_pkObjectMan->Delete (m_pkObject);

   // connect to new parent
   pkObject->AddProperty (this);

   // add bonuses
   Bonuses(true);
   
   return false;
}

// ------------------------------------------------------------------------------------------

 //false = remove bonuses (on end of spell)
void P_Spell::Bonuses(bool bAddBonuses)
{
   CharacterProperty *pkCharStats = (CharacterProperty*)m_pkObject->GetProperty("P_CharStats");
   P_Item *pkItem = (P_Item*)m_pkObject->GetProperty("P_Item");


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

         return true;
      }
      else
      {
         // if spell not is loaded, check if spell can be loaded
         if ( LoadSpell(kValue) )
         {
            m_pkSpellType = g_kSpells[kValue];

            Bonuses(true);

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

   pkNewP->m_kAttackedObjects.push_back ( m_pkObject->iNetWorkID );

   return pkNewP;
}

// ------------------------------------------------------------------------------------------

void P_Spell::DoCollisions()
{
   // get zone spell is in
   ZoneData* pkZone = m_pkObject->GetObjectMan()->GetZone( m_pkObject->GetWorldPosV() );

   // get all objects in zone
   vector<Object*> kObjects;
   pkZone->m_pkZone->GetAllObjects(&kObjects);

   for ( unsigned int i = 0; i < kObjects.size(); i++ )
   {
      
      bool bOk = true;
      
      // don't care if spell collides with own ps or self
      if ( kObjects[i] == m_pkObject )
         bOk = false;

      // don't test against the zone object :) We don't want to burn that away
      if ( pkZone->m_pkZone == kObjects[i] )
         bOk = false;

      for ( unsigned int j = 0; j < m_kPSystems.size(); j++ )
         if ( kObjects[i] == m_kPSystems[j] )
            bOk = false;

      // check if the object and the spell hasn't already collided
      for ( unsigned int h = 0; h < m_kAttackedObjects.size(); h++ )
         if ( kObjects[i]->iNetWorkID == m_kAttackedObjects[h] )
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
               m_kAttackedObjects.push_back (kObjects[i]->iNetWorkID);

         if ( m_pkSpellType->m_kAffectedObjects == "items" )
            if ( kObjects[i]->GetProperty("P_Item") )
               bOk = true;
            else
               m_kAttackedObjects.push_back (kObjects[i]->iNetWorkID);

         if ( m_pkSpellType->m_kAffectedObjects == "characters_and_items" || 
              m_pkSpellType->m_kAffectedObjects == "items_and_characters" )
            if ( kObjects[i]->GetProperty("P_Item") || kObjects[i]->GetProperty("P_CharStats") )
               bOk = true;
            else
               m_kAttackedObjects.push_back (kObjects[i]->iNetWorkID);

         // if the object had right to be affacted by the spell
         if ( bOk )
         {
				Vector3 Pos = m_pkObject->GetLocalPosV();
            float fDist = kObjects[i]->GetLocalPosV().DistanceTo (Pos );

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
                  P_Spell *pkNewSpell = new P_Spell;
                  kObjects[i]->AddProperty( pkNewSpell );
                  pkNewSpell->SetValue ( "SpellType", m_pkSpellType->m_kOnHit[1] );
               }

               // destroy spell (TODO!!!: after dealt damage!!! )
               if ( m_pkSpellType->m_kOnHit[0] == "destroyspell" ) 
                  m_pkObject->m_pkObjectMan->Delete ( m_pkObject );

               // creates a new spell at the hit location and removes the old one
               if ( m_pkSpellType->m_kOnHit[0] == "createnewspell" ) 
               {
                  Object *pkNewSpellObject = m_pkObject->m_pkObjectMan->CreateObject();
            
                  P_Spell *pkNewSpell = new P_Spell;
                  pkNewSpellObject->AddProperty( pkNewSpell );
                  pkNewSpell->SetValue ( "SpellType", m_pkSpellType->m_kOnHit[1] );
            
                  pkNewSpellObject->SetWorldPosV ( m_pkObject->GetWorldPosV() );
                  pkNewSpellObject->SetWorldRotV ( m_pkObject->GetWorldRotV() );

                  pkNewSpellObject->AttachToZone();

                  m_pkObject->m_pkObjectMan->Delete ( m_pkObject );
               }

               // creates a psystem at the hit location and removes the spell
               if ( m_pkSpellType->m_kOnHit[0] == "createpsystem" ) 
               {
                  Object *pkNewPSystem = m_pkObject->m_pkObjectMan->CreateObject();
            
                  PSystemProperty *pkNewPSProp = new PSystemProperty;
                  kObjects[i]->AddProperty( pkNewPSProp );
                  pkNewPSProp->SetValue ( "PSType", m_pkSpellType->m_kOnHit[1] );
            
                  pkNewPSystem->SetWorldPosV ( m_pkObject->GetWorldPosV() );
                  pkNewPSystem->SetWorldRotV ( m_pkObject->GetWorldRotV() );

                  pkNewPSystem->AttachToZone();

                  m_pkObject->m_pkObjectMan->Delete ( m_pkObject );
               }

               // the spell maskes a copy of itself that attaches on the hit object
               if ( m_pkSpellType->m_kOnHit[0] == "spread" ) 
                   kObjects[i]->AddProperty ( Copy() );

               // remember which objects is hit by the spell so it can't hit same object twice
               m_kAttackedObjects.push_back (kObjects[i]->iNetWorkID);

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

void P_Spell::Load(ZFIoInterface* pkPackage)
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

