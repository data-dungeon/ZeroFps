#include "rulesystem/character/characterfactory.h"
#include "p_charstats.h"
#include "../zerofpsv2/engine/object.h"

// ------------------------------------------------------------------------------------------

void CharacterProperty::Update()
{
}

// ------------------------------------------------------------------------------------------

vector<PropertyValues> CharacterProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
/*	kReturn[0].kValueName = "PSType";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kPSType;*/

	return kReturn;
}

// ------------------------------------------------------------------------------------------

CharacterProperty::CharacterProperty()
{
	m_iSide = PROPERTY_SIDE_SERVER;
   
   m_pkCharStats = new CharacterStats( m_pkObject );

	strcpy(m_acName,"P_CharStats");

	bNetwork = false;
}

// ------------------------------------------------------------------------------------------

CharacterProperty::CharacterProperty( string kName )
{
	bNetwork = true;   //

   m_pkCharStats = new CharacterStats ( m_pkObject );

	strcpy(m_acName,"P_CharStats");
}

// ------------------------------------------------------------------------------------------

bool CharacterProperty::HandleSetValue( string kValueName, string kValue )
{

	return false;
}
// ------------------------------------------------------------------------------------------

void CharacterProperty::Save(ZFIoInterface* pkPackage)
{
   char temp[128];
   float fValue;

   // counters
   int iData = 0, iSkills = 0, iAttributes = 0, 
       iEquipment = 0, iAttacks = 0, iDefence = 0, iValue;

   map<string, string>::iterator kDataIte;
   map<string, StatDescriber>::iterator kIte;
   map<string, Object*>::iterator kEqIte;
   map<string, int>::iterator kFightIte;

   iData = m_pkCharStats->m_kData.size();
   iSkills = m_pkCharStats->m_kSkills.size();
   iAttributes = m_pkCharStats->m_kAttributes.size();
   iEquipment = m_pkCharStats->m_kEquipment.size();
   iAttacks = m_pkCharStats->m_kFightStats.m_kAttack.size();
   iDefence = m_pkCharStats->m_kFightStats.m_kDefence.size();

   // save counters, need to know how many when loading data later
   pkPackage->Write ( (void*)&iData, sizeof(int), 1 );
   pkPackage->Write ( (void*)&iSkills, sizeof(int), 1 );
   pkPackage->Write ( (void*)&iAttributes, sizeof(int), 1 );
   pkPackage->Write ( (void*)&iEquipment, sizeof(int), 1 );
   pkPackage->Write ( (void*)&iAttacks, sizeof(int), 1 );
   pkPackage->Write ( (void*)&iDefence, sizeof(int), 1 );

   // save data
   for ( kDataIte = m_pkCharStats->m_kData.begin(); kDataIte != m_pkCharStats->m_kData.end(); kDataIte++ )
   {
     	strcpy( temp, (*kDataIte).first.c_str() ); // name
	   pkPackage->Write((void*)&temp,128,1); 

     	strcpy( temp, (*kDataIte).second.c_str() );	// value
	   pkPackage->Write((void*)&temp,128,1); 
   }

   // save skills
   for ( kIte = m_pkCharStats->m_kSkills.begin(); kIte != m_pkCharStats->m_kSkills.end(); kIte++ )
   {
     	strcpy( temp, (*kIte).first.c_str() ); // name
	   pkPackage->Write((void*)&temp,128,1); 

      iValue = (*kIte).second.m_iValue; // level
      pkPackage->Write ( (void*)&iValue, sizeof(int), 1 );

      fValue = (*kIte).second.m_fExp; // exp
      pkPackage->Write ( (void*)&fValue, sizeof(float), 1 );
   }
   
   // save attributes
   for ( kIte = m_pkCharStats->m_kAttributes.begin(); kIte != m_pkCharStats->m_kAttributes.end(); kIte++ )
   {
     	strcpy( temp, (*kIte).first.c_str() ); // name
	   pkPackage->Write((void*)&temp,128,1); 

      iValue = (*kIte).second.m_iValue; // level
      pkPackage->Write ( (void*)&iValue, sizeof(int), 1 );

      fValue = (*kIte).second.m_fExp; // exp
      pkPackage->Write ( (void*)&fValue, sizeof(float), 1 );
   }
   
   // save equipment
   for ( kEqIte = m_pkCharStats->m_kEquipment.begin(); kEqIte != m_pkCharStats->m_kEquipment.end(); kEqIte++ )
   {
     	strcpy( temp, (*kEqIte).first.c_str() ); // slot
	   pkPackage->Write((void*)&temp,128,1); 

      iValue = (*kEqIte).second->iNetWorkID; // network ID
      pkPackage->Write ( (void*)&iValue, sizeof(int), 1 );
   }
   // save attack stats
   for ( kFightIte = m_pkCharStats->m_kFightStats.m_kAttack.begin(); kFightIte != m_pkCharStats->m_kFightStats.m_kAttack.end(); kFightIte++ )
   {
     	strcpy( temp, (*kFightIte).first.c_str() ); // name
	   pkPackage->Write((void*)&temp,128,1); 

      iValue = (*kFightIte).second; // level
      pkPackage->Write ( (void*)&iValue, sizeof(int), 1 );
   }
   
   // save defence stats
   for ( kFightIte = m_pkCharStats->m_kFightStats.m_kDefence.begin(); kFightIte != m_pkCharStats->m_kFightStats.m_kDefence.end(); kFightIte++ )
   {
     	strcpy( temp, (*kFightIte).first.c_str() ); // name
	   pkPackage->Write((void*)&temp,128,1); 

      iValue = (*kFightIte).second; // level
      pkPackage->Write ( (void*)&iValue, sizeof(int), 1 );
   }

   // save parentID
   if ( m_pkCharStats->m_pkParent )
      iValue = m_pkCharStats->m_pkParent->iNetWorkID;
   else
      iValue = 0;

   pkPackage->Write ( (void*)&iValue, sizeof(int), 1 );
}

// ------------------------------------------------------------------------------------------

void CharacterProperty::Load(ZFIoInterface* pkPackage)
{
   char temp[128], temp2[128];
   float fValue;
   int i;

   // counters
   int iData = 0, iSkills = 0, iAttributes = 0, 
       iEquipment = 0, iAttacks = 0, iDefence = 0, iValue;

   map<string, string>::iterator kDataIte;
   map<string, StatDescriber>::iterator kIte;
   map<string, Object*>::iterator kEqIte;
   map<string, int>::iterator kFightIte;

    // load counters 
   pkPackage->Read ( (void*)&iData, sizeof(int), 1 );
   pkPackage->Read ( (void*)&iSkills, sizeof(int), 1 );
   pkPackage->Read ( (void*)&iAttributes, sizeof(int), 1 );
   pkPackage->Read ( (void*)&iEquipment, sizeof(int), 1 );
   pkPackage->Read ( (void*)&iAttacks, sizeof(int), 1 );
   pkPackage->Read ( (void*)&iDefence, sizeof(int), 1 );

   
   // load data
   for ( i = 0; i < iData; i++ )
   {
	   pkPackage->Read((void*)&temp,128,1); 
	   pkPackage->Read((void*)&temp2,128,1);
      
      m_pkCharStats->SetData ( string(temp), string(temp2) );
   }

   // load skills
   for ( i = 0; i < iSkills; i++ )
   {
	   pkPackage->Read ( (void*)&temp,128,1); // name
      pkPackage->Read ( (void*)&iValue, sizeof(int), 1 ); // level
      pkPackage->Read ( (void*)&fValue, sizeof(float), 1 ); // exp

      m_pkCharStats->SetSkill ( string(temp), iValue );
      m_pkCharStats->SetSkillExp ( string(temp), fValue );
   }
   
   // load attributes
   for ( i = 0; i < iAttributes; i++ )
   {
	   pkPackage->Read ( (void*)&temp,128,1); // name
      pkPackage->Read ( (void*)&iValue, sizeof(int), 1 ); // level
      pkPackage->Read ( (void*)&fValue, sizeof(float), 1 ); // exp

      m_pkCharStats->SetAttribute ( string(temp), iValue );
      m_pkCharStats->SetAttributeExp ( string(temp), fValue );
   }

   // load equipment
   for ( i = 0; i < iEquipment; i++ )
   {
	   pkPackage->Read((void*)&temp,128,1);  // slot
      pkPackage->Read ( (void*)&iValue, sizeof(int), 1 ); // network ID

      // TODO!!: equip the item. Make sure it exist first!
   }
    
   // load attack stats
   for ( i = 0; i < iAttacks; i++ )
   {
	   pkPackage->Read ( (void*)&temp,128,1);  // name
      pkPackage->Read ( (void*)&iValue, sizeof(int), 1 ); // level

      m_pkCharStats->SetAttackValue ( string(temp), iValue );
   }

   // load defence stats
   for ( i = 0; i < iDefence; i++ )
   {
	   pkPackage->Read ( (void*)&temp,128,1);  // name
      pkPackage->Read ( (void*)&iValue, sizeof(int), 1 ); // level

      m_pkCharStats->SetDefenceValue ( string(temp), iValue );
   }

   // load parentID
   pkPackage->Read ( (void*)&iValue, sizeof(int), 1 );


}

// ------------------------------------------------------------------------------------------

void CharacterProperty::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
   // update life

}

// ------------------------------------------------------------------------------------------

void CharacterProperty::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
   // get life
}

// ------------------------------------------------------------------------------------------

Property* Create_P_CharStats()
{
	return new CharacterProperty;
}


