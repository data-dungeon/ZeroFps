#include "rulesystem/character/characterfactory.h"
#include "p_charstats.h"
#include "p_clientcontrol.h"
#include "../zerofpsv2/engine/entity.h"
#include "../zerofpsv2/engine/zerofps.h"

// ------------------------------------------------------------------------------------------

void CharacterProperty::Update()
{
   if ( m_pkCharStats->m_fReloadTimer > 0 )
      m_pkCharStats->m_fReloadTimer -= m_pkObject->m_pkFps->GetFrameTime();
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

void CharacterProperty::Init()
{
   m_pkCharStats = new CharacterStats( m_pkObject );

   m_pkCharStats->MakeContainer();

   m_pkCharStats->m_fReloadTime = 5;
   m_pkCharStats->m_fReloadTimer = 0;
}
// ------------------------------------------------------------------------------------------

CharacterProperty::CharacterProperty()
{
   m_iSide = PROPERTY_SIDE_SERVER;

	strcpy(m_acName,"P_CharStats");

	bNetwork = true;

	m_pkCharStats = NULL;
}

// ------------------------------------------------------------------------------------------

CharacterProperty::CharacterProperty( string kName )
{
   m_iSide = PROPERTY_SIDE_SERVER;
   
   bNetwork = true;

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
   map<string, Entity*>::iterator kEqIte;
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
   
   // save recal position
   pkPackage->Write ( (void*)&m_pkCharStats->m_kRecalPos, sizeof(m_pkCharStats->m_kRecalPos), 1 );   
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
   map<string, Entity*>::iterator kEqIte;
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


	//load reacal position
   pkPackage->Read ( (void*)&m_pkCharStats->m_kRecalPos, sizeof(m_pkCharStats->m_kRecalPos), 1 );   
}

// ------------------------------------------------------------------------------------------

void CharacterProperty::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
   int iStuff = -1;

   if ( !m_kSends.size() )
      pkNetPacket->Write( &iStuff, sizeof(int) );
   else
   {
      // send character skills
      if ( m_kSends.front().m_iClientID == iConnectionID && m_kSends.front().m_kSendType == "skills" )
      {
         iStuff = eSKILLS;

         pkNetPacket->Write( &iStuff, sizeof(int) );

         // get container vector
         map<string, StatDescriber>* pkSkills = &m_pkCharStats->m_kSkills;

         iStuff = pkSkills->size();

         // write number of skills
         pkNetPacket->Write( &iStuff, sizeof(int) );

         for ( map<string, StatDescriber>::iterator kIte = pkSkills->begin();
               kIte != pkSkills->end(); kIte++ )
         {
            // skill name
            pkNetPacket->Write_Str( (*kIte).first.c_str() );
            // skill value
            pkNetPacket->Write( &(*kIte).second.m_iValue, sizeof(int) );
         }
      }
      // send character attributes
      else if ( m_kSends.front().m_iClientID == iConnectionID && m_kSends.front().m_kSendType == "attributes" )
      {
         iStuff = eATTRIBUTES;

         pkNetPacket->Write( &iStuff, sizeof(int) );

         // get container vector
         map<string, StatDescriber>* pkAttributes = &m_pkCharStats->m_kAttributes;

         iStuff = pkAttributes->size();

         // write number of skills
         pkNetPacket->Write( &iStuff, sizeof(int) );

         for ( map<string, StatDescriber>::iterator kIte = pkAttributes->begin();
               kIte != pkAttributes->end(); kIte++ )
         {
            // attribute name
            pkNetPacket->Write_Str( (*kIte).first.c_str() );
            // attribute value
            pkNetPacket->Write( &(*kIte).second.m_iValue, sizeof(int) );
         }
      }
      // send character data (name, rase, sex...)
      else if ( m_kSends.front().m_iClientID == iConnectionID && m_kSends.front().m_kSendType == "data" )
      {
         iStuff = eDATA;

         pkNetPacket->Write( &iStuff, sizeof(int) );

         // get container vector
         map<string, string>* pkData = &m_pkCharStats->m_kData;

         iStuff = pkData->size();

         // write number of skills
         pkNetPacket->Write( &iStuff, sizeof(int) );

         for ( map<string, string>::iterator kIte = pkData->begin();
               kIte != pkData->end(); kIte++ )
         {
            // attribute name
            pkNetPacket->Write_Str( (*kIte).first.c_str() );
            // attribute value
            pkNetPacket->Write_Str( (*kIte).second.c_str() );
         }
      }
      // send hp
      else if ( m_kSends.front().m_iClientID == iConnectionID && m_kSends.front().m_kSendType == "hp" )
      {
         iStuff = eHP;

         pkNetPacket->Write( &iStuff, sizeof(int) );
          
         int iHP = m_pkCharStats->GetHP(); 
         int iMaxHP = m_pkCharStats->m_kPointStats["hp"].Max();

         // send hp
         pkNetPacket->Write( &iHP, sizeof(int) );
         pkNetPacket->Write( &iMaxHP, sizeof(int) );
      }
      else
      {
         pkNetPacket->Write( &iStuff, sizeof(int) );

         cout << "P_CharStats::PackTo SendOrder:" << m_kSends.front().m_kSendType << " ConnID:" << iConnectionID << " invalid!" << endl;
      }

      m_kSends.erase ( m_kSends.begin() );

   }

   if ( !m_kSends.size() )
  	   SetNetUpdateFlag(iConnectionID, false);
}

// ------------------------------------------------------------------------------------------

void CharacterProperty::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
   if ( iConnectionID != -1 )
      cout << "iConnectionID wasn't -1 in P_CharStats::PackFrom!!! The end is near!" << endl;

   // read type of data
   int iType;

	pkNetPacket->Read( &iType, sizeof(int) );

   if ( iType == eSKILLS )
   {
      int iCount, iValue;

      // read number of skills
      pkNetPacket->Read( &iCount, sizeof(int) );

      char caSkillName[128];

      for ( int i = 0; i < iCount; i++ )
      {
         // read skill name
         pkNetPacket->Read_Str( caSkillName );
         // read skill value
         pkNetPacket->Read( &iValue, sizeof(int) );

         m_pkCharStats->m_kSkills[string(caSkillName)].m_iValue = iValue;
      }

   }
   else if ( iType == eATTRIBUTES )
   {
      int iCount, iValue;

      // read number of attributes
      pkNetPacket->Read( &iCount, sizeof(int) );

      char caAttrName[128];

      for ( int i = 0; i < iCount; i++ )
      {
         // read attribute name
         pkNetPacket->Read_Str( caAttrName );
         // read attribute value
         pkNetPacket->Read( &iValue, sizeof(int) );

         m_pkCharStats->m_kAttributes[string(caAttrName)].m_iValue = iValue;
      }
   }
   else if ( iType == eDATA )
   {
      int iCount;

      // read number of datas
      pkNetPacket->Read( &iCount, sizeof(int) );

      char caAttrName[128], caAttrValue[128];

      for ( int i = 0; i < iCount; i++ )
      {
         // read data name
         pkNetPacket->Read_Str( caAttrName );
         // read data value
         pkNetPacket->Read_Str( caAttrValue );

         m_pkCharStats->m_kData[string(caAttrName)] = caAttrValue;

         cout << "Data from server:" << m_pkCharStats->m_kData[string(caAttrName)] << endl;
      }
   }
   else if ( iType == eHP )
   {
      int iHP, iMaxHP;

      pkNetPacket->Read ( &iHP, sizeof(int) );
      pkNetPacket->Read ( &iMaxHP, sizeof(int) );

      m_pkCharStats->m_kPointStats["hp"].SetMaxValue(iMaxHP);
      m_pkCharStats->m_kPointStats["hp"] = iHP;
   }
   else
      cout << "Got unknown data in p_charstats::packfrom, type:" << iType << endl;
}

// ------------------------------------------------------------------------------------------

void CharacterProperty::RequestUpdateFromServer( string kData )
{
 
   int iClientObjectID = m_pkZeroFps->GetClientObjectID();
   Entity* pkClientObj = m_pkObjMan->GetObjectByNetWorkID(iClientObjectID);

   if ( pkClientObj )
   {
      // get ClientControlProperty
      P_ClientControl* pkCP = (P_ClientControl*)pkClientObj->GetProperty("P_ClientControl");

      ClientOrder kOrder;

      // order type
      if ( kData == "container" )
      {
         // get client object
         kOrder.m_sOrderName = "(rq)cont";
         kOrder.m_iObjectID = m_pkObject->iNetWorkID;
         kOrder.m_iClientID = m_pkZeroFps->GetConnectionID();
         kOrder.m_iCharacter = pkCP->m_iActiveCaracterObjectID;

         // use this useless variabel to send which version of the item this prop. has
         kOrder.m_iUseLess = m_pkCharStats->m_pkContainer->m_uiVersion;

         pkCP->AddOrder (kOrder);
      }
      else if ( kData == "attributes" )
      {
         // get client object
         kOrder.m_sOrderName = "(rq)attr";
         kOrder.m_iObjectID = m_pkObject->iNetWorkID;
         kOrder.m_iClientID = m_pkZeroFps->GetConnectionID();
         kOrder.m_iCharacter = pkCP->m_iActiveCaracterObjectID;

         // use this useless variabel to send which version of the item this prop. has
         kOrder.m_iUseLess = m_pkCharStats->m_pkContainer->m_uiVersion;

         pkCP->AddOrder (kOrder);
      }
      else if ( kData == "skills" )
      {
         // get client object
         kOrder.m_sOrderName = "(rq)skil";
         kOrder.m_iObjectID = m_pkObject->iNetWorkID;
         kOrder.m_iClientID = m_pkZeroFps->GetConnectionID();
         kOrder.m_iCharacter = pkCP->m_iActiveCaracterObjectID;

         // use this useless variabel to send which version of the item this prop. has
         kOrder.m_iUseLess = m_pkCharStats->m_pkContainer->m_uiVersion;

         pkCP->AddOrder (kOrder);
      }
      else if ( kData == "data" )
      {
         // get client object
         kOrder.m_sOrderName = "(rq)cdat";
         kOrder.m_iObjectID = m_pkObject->iNetWorkID;
         kOrder.m_iClientID = m_pkZeroFps->GetConnectionID();
         kOrder.m_iCharacter = pkCP->m_iActiveCaracterObjectID;

         // use this useless variabel to send which version of the item this prop. has
         kOrder.m_iUseLess = m_pkCharStats->m_pkContainer->m_uiVersion;

         pkCP->AddOrder (kOrder);
      }
      // attribute and skills, change to only skills or attributes?
      else if ( kData == "charstats" )
      {
         // get client object
         kOrder.m_sOrderName = "(rq)char";
         kOrder.m_iObjectID = m_pkObject->iNetWorkID;
         kOrder.m_iClientID = m_pkZeroFps->GetConnectionID();
         kOrder.m_iCharacter = pkCP->m_iActiveCaracterObjectID;

         // use this useless variabel to send which version of the item this prop. has
         kOrder.m_iUseLess = m_pkCharStats->m_uiVersion;

         pkCP->AddOrder (kOrder);
      }
      // life and mana
      else if ( kData == "life" )
      {
         // get client object
         kOrder.m_sOrderName = "(rq)life";
         kOrder.m_iObjectID = m_pkObject->iNetWorkID;
         kOrder.m_iClientID = m_pkZeroFps->GetConnectionID();
         kOrder.m_iCharacter = pkCP->m_iActiveCaracterObjectID;

         // use this useless variabel to send which version of the item this prop. has
         kOrder.m_iUseLess = m_pkCharStats->m_uiVersion;

         pkCP->AddOrder (kOrder);
      }
      else
         cout << "Unknown request:" << kData << " made in p_charstats::RequestUpdateFromServer" << endl;
   }
   else
      cout << "P_Item::RequestUpdateFromServer(): no client object found!" << endl;

}

// ------------------------------------------------------------------------------------------

void CharacterProperty::AddSendsData(SendType kNewSendData)
{
	SetNetUpdateFlag(true);		
	
	m_kSends.push_back(kNewSendData);
}

// ------------------------------------------------------------------------------------------

Property* Create_P_CharStats()
{
	return new CharacterProperty;
}


