#include "p_item.h"
#include "p_clientcontrol.h"
#include "../zerofpsv2/engine/entity.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "p_charstats.h"

// ------------------------------------------------------------------------------------------

void P_Item::Update()
{
}

// ------------------------------------------------------------------------------------------

vector<PropertyValues> P_Item::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
/*	kReturn[0].kValueName = "PSType";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_kPSType;*/

	return kReturn;
}

// ------------------------------------------------------------------------------------------

P_Item::P_Item()
{
	m_iSide = PROPERTY_SIDE_CLIENT;

   m_pkItemStats = new ItemStats(this);

	bNetwork = true;

   m_pkInventoryList = 0;

	strcpy(m_acName,"P_Item");

   m_pkItemStats->MakeContainer();
}

// ------------------------------------------------------------------------------------------

P_Item::P_Item( string kName )
{
	m_iSide = PROPERTY_SIDE_CLIENT;

   m_pkItemStats = new ItemStats(this);

	bNetwork = true;

   m_pkInventoryList = 0;
      
	strcpy(m_acName,"P_Item");

}

// ------------------------------------------------------------------------------------------

bool P_Item::HandleSetValue( string kValueName, string kValue )
{
	if( strcmp(kValueName.c_str(), "Type") == 0 ) 
	{
//      m_pkItemStats = ItemDataBase::GetItem ( kValue );

		return true;
	}

	return false;
}

// ------------------------------------------------------------------------------------------

void P_Item::Save(ZFIoInterface* pkPackage)
{
   int iSkillBonuses = 0, iAttributeBonuses = 0, iAttackBonuses = 0, iDefenceBonuses = 0,
       iEquipOn = 0, iValue;

   char temp[128];

   map<string, int>::iterator kIte;

   
   iSkillBonuses = m_pkItemStats->m_kSkillBonus.size();
   iAttributeBonuses = m_pkItemStats->m_kAttributeBonus.size();
   iAttackBonuses = m_pkItemStats->m_kFightStats.m_kAttack.size();
   iDefenceBonuses = m_pkItemStats->m_kFightStats.m_kDefence.size();
   iEquipOn = m_pkItemStats->m_kEquippableOn.size();
   
   pkPackage->Write((void*)m_pkItemStats->m_kItemName.c_str(),128,1); // save itemname

   pkPackage->Write((void*)&m_pkItemStats->m_iQuantity,sizeof(int),1); // save quantity
   pkPackage->Write((void*)&m_pkItemStats->m_iValue,sizeof(int),1); // save value
   pkPackage->Write((void*)&m_pkItemStats->m_fQuality,sizeof(float),1); // save quality
   pkPackage->Write((void*)&m_pkItemStats->m_fWeight,sizeof(float),1); // save weight


   // save counters
   pkPackage->Write((void*)&iSkillBonuses,sizeof(int),1);
   pkPackage->Write((void*)&iAttributeBonuses,sizeof(int),1);
   pkPackage->Write((void*)&iAttackBonuses,sizeof(int),1);
   pkPackage->Write((void*)&iDefenceBonuses,sizeof(int),1);
   pkPackage->Write((void*)&iEquipOn,sizeof(int),1);
  
   
   // save skillbonuses
   for ( kIte = m_pkItemStats->m_kSkillBonus.begin(); kIte != m_pkItemStats->m_kSkillBonus.end(); kIte++ )
   {
     	strcpy( temp, (*kIte).first.c_str() ); // name
	   pkPackage->Write((void*)&temp,128,1); 

      iValue = (*kIte).second; // bonus
      pkPackage->Write ( (void*)&iValue, sizeof(int), 1 );
   }
   
   // save attributebonuses
   for ( kIte = m_pkItemStats->m_kAttributeBonus.begin(); kIte != m_pkItemStats->m_kAttributeBonus.end(); kIte++ )
   {
     	strcpy( temp, (*kIte).first.c_str() ); // name
	   pkPackage->Write((void*)&temp,128,1); 

      iValue = (*kIte).second; // bonus
      pkPackage->Write ( (void*)&iValue, sizeof(int), 1 );
   }

   // save attackbonuses
   for ( kIte = m_pkItemStats->m_kFightStats.m_kAttack.begin(); kIte != m_pkItemStats->m_kFightStats.m_kAttack.end(); kIte++ )
   {
     	strcpy( temp, (*kIte).first.c_str() ); // name
	   pkPackage->Write((void*)&temp,128,1); 

      iValue = (*kIte).second; // bonus
      pkPackage->Write ( (void*)&iValue, sizeof(int), 1 );
   }

   // save defencebonuses
   for ( kIte = m_pkItemStats->m_kFightStats.m_kDefence.begin(); kIte != m_pkItemStats->m_kFightStats.m_kDefence.end(); kIte++ )
   {
     	strcpy( temp, (*kIte).first.c_str() ); // name
	   pkPackage->Write((void*)&temp,128,1); 

      iValue = (*kIte).second; // bonus
      pkPackage->Write ( (void*)&iValue, sizeof(int), 1 );
   }

   // save equipable on
   for ( unsigned int i = 0; i < m_pkItemStats->m_kEquippableOn.size(); i++ )
   {
     	strcpy( temp, m_pkItemStats->m_kEquippableOn[i].c_str() ); // name
	   pkPackage->Write((void*)&temp,128,1);       
   }


}

// ------------------------------------------------------------------------------------------

void P_Item::Load(ZFIoInterface* pkPackage)
{
   int iSkillBonuses, iAttributeBonuses, iAttackBonuses, iDefenceBonuses, iEquipOn, iValue, i;

   char temp[128];

   map<string, int>::iterator kIte;
   
	pkPackage->Read((void*)temp,128,1); // save itemname
   m_pkItemStats->m_kItemName = temp;

 
   pkPackage->Read((void*)&m_pkItemStats->m_iQuantity,sizeof(int),1); // save quantity
   pkPackage->Read((void*)&m_pkItemStats->m_iValue,sizeof(int),1); // save value
   pkPackage->Read((void*)&m_pkItemStats->m_fQuality,sizeof(float),1); // save quality
   pkPackage->Read((void*)&m_pkItemStats->m_fWeight,sizeof(float),1); // save weight
    
   // load counters
   pkPackage->Read((void*)&iSkillBonuses,sizeof(int),1);
   pkPackage->Read((void*)&iAttributeBonuses,sizeof(int),1);
   pkPackage->Read((void*)&iAttackBonuses,sizeof(int),1);
   pkPackage->Read((void*)&iDefenceBonuses,sizeof(int),1);
   pkPackage->Read((void*)&iEquipOn,sizeof(int),1);

   
   // load skillbonuses
   for ( i = 0; i < iSkillBonuses; i++ )
   {
	   pkPackage->Read((void*)&temp, 128, 1);  // name
      pkPackage->Read((void*)&iValue, sizeof(int), 1); // bonus

      m_pkItemStats->SetSkillBonus ( string(temp), iValue );
   }
   
   // load attributebonuses
   for ( i = 0; i < iAttributeBonuses; i++ )
   {
	   pkPackage->Read((void*)&temp,128,1);  // name
      pkPackage->Read((void*)&iValue, sizeof(int), 1 ); // bonus

      m_pkItemStats->SetAttributeBonus (string(temp), iValue);
   }

   // load attackbonuses
   for ( i = 0; i < iAttackBonuses; i++ )
   {
	   pkPackage->Read((void*)&temp,128,1); // name
      pkPackage->Read ( (void*)&iValue, sizeof(int), 1 ); // bonus

      m_pkItemStats->SetAttackBonus (string(temp), iValue);
   }

   // load defencebonuses
   for ( i = 0; i < iDefenceBonuses; i++ )
   {
	   pkPackage->Read((void*)&temp,128,1); // name
      pkPackage->Read ( (void*)&iValue, sizeof(int), 1 ); // bonus

      m_pkItemStats->SetDefenceBonus (string(temp), iValue);
   }

   // load equipable on
   for ( i = 0; i < iEquipOn; i++ )
   {
	   pkPackage->Read((void*)&temp,128,1); // name
      m_pkItemStats->AddCanEquipOn (string(temp));
   }

}

// ------------------------------------------------------------------------------------------

void P_Item::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{

   if ( m_kSends.size() )
   {
      for (list<SendType>::iterator kIte = m_kSends.begin(); kIte != m_kSends.end(); kIte++ )
      {
         //  if a object which has requested info was found...
         if ( (*kIte).m_iClientID == iConnectionID && (*kIte).m_kSendType == "itemdata" )
         {

            pkNetPacket->Write_NetStr( "data" );

            // icon
            pkNetPacket->Write_NetStr( m_pkItemStats->m_szPic );

            // category
            pkNetPacket->Write( &m_pkItemStats->m_eEquipmentCategory, 
					sizeof(m_pkItemStats->m_eEquipmentCategory) );

            // item name
            pkNetPacket->Write_NetStr( m_pkItemStats->m_kItemName.c_str() );

            // to add: tooltip

            // send version
            pkNetPacket->Write( &m_pkItemStats->m_uiVersion, sizeof(unsigned int) );


            m_kSends.erase ( kIte++ );

         }
         else if ( (*kIte).m_iClientID == iConnectionID && (*kIte).m_kSendType == "container" )
         {
            // if object isn't a container, don't send anything
            if ( m_pkItemStats->m_pkContainer )
            {
               pkNetPacket->Write_NetStr( "cont" );

               // get container vector
               vector<int>* pkItems = m_pkItemStats->m_pkContainer->GetItemsInContainer();

               // container ID
               pkNetPacket->Write( &m_pkItemStats->m_iContainerID, sizeof(int) );

               int iSize = pkItems->size();

               // size of container
               pkNetPacket->Write( &iSize, sizeof(int) );

               // send itemIDs in container
               for ( int i = 0; i < pkItems->size(); i++ )
                  pkNetPacket->Write( &pkItems->at(i), sizeof(int) );

               // version
               pkNetPacket->Write( &m_pkItemStats->m_pkContainer->m_uiVersion, sizeof(unsigned int) );

            }

            m_kSends.erase ( kIte++ );

         }
      }
   }
   else
   {
      pkNetPacket->Write_NetStr( "foo" );

      // container id (if it's a container)
      pkNetPacket->Write( &m_pkItemStats->m_iContainerID, sizeof(int) );

      // which container the object is in
      pkNetPacket->Write( &m_pkItemStats->m_iCurrentContainer, sizeof(int) );

   }

   
   SetNetUpdateFlag(iConnectionID, false);
}

// ------------------------------------------------------------------------------------------

void P_Item::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
//   if ( iConnectionID == -1 )
//      return;

	char temp[128];

	pkNetPacket->Read_NetStr(temp);

	string kDataType = temp;


   if ( kDataType == "data" )
   {
      // get icon
      pkNetPacket->Read_NetStr(m_pkItemStats->m_szPic);

      // category
      pkNetPacket->Read( &m_pkItemStats->m_eEquipmentCategory, 
			sizeof(m_pkItemStats->m_eEquipmentCategory) );

      // item name
      pkNetPacket->Read_NetStr( (char*)m_pkItemStats->m_kItemName.c_str() );

      // to add: tooltip

      // get version
      pkNetPacket->Read( &m_pkItemStats->m_uiVersion, sizeof(unsigned int) );

      if ( m_pkInventoryList )
         m_pkInventoryList->push_back ( m_pkObject );

   }
   else if ( kDataType == "cont" )
   {
      // if object isn't a container, better make it one, or it crashes!
      if ( !m_pkItemStats->m_pkContainer )
         m_pkItemStats->MakeContainer();

      // container ID
      pkNetPacket->Read( &m_pkItemStats->m_iContainerID, sizeof(int) );

      int iContSize, iObjID;

      // size of container
      pkNetPacket->Read( &iContSize, sizeof(int) );

      // clear container first, so we don't get double versions of the items
      m_pkItemStats->m_pkContainer->Clear();

      // read itemIDs in container
      for ( int i = 0; i < iContSize; i++ )
      {
         pkNetPacket->Read( &iObjID, sizeof(int) );
         m_pkItemStats->m_pkContainer->AddObject ( iObjID );

         // request for a update of the item at the same time
         ((P_Item*)m_pkObjMan->GetObjectByNetWorkID( iObjID )->GetProperty("P_Item"))->RequestUpdateFromServer ("data");
      }

      // version
      pkNetPacket->Read( &m_pkItemStats->m_pkContainer->m_uiVersion, sizeof(unsigned int) );

      if ( m_pkInventoryList )
         m_pkItemStats->m_pkContainer->GetAllItemsInContainer( m_pkInventoryList );
   }
   else
   {
      // contatiner id (if it's a container)
      pkNetPacket->Read( &m_pkItemStats->m_iContainerID, sizeof(int) );

      // which container the object is in
      pkNetPacket->Read( &m_pkItemStats->m_iCurrentContainer, sizeof(int) );

      // tjoff
      if ( m_pkItemStats->m_iContainerID != -1 )
         m_pkItemStats->MakeContainer();
   }
	

}

// ------------------------------------------------------------------------------------------

Entity* P_Item::Split ( int iTookens )
{
   // only works if object was created from script 
   if ( m_pkObject->m_strCreatedFromScript.size() && iTookens > 0 && iTookens < m_pkItemStats->GetQuantity() )
   {
      m_pkItemStats->AddQuantity( -iTookens );      

      // in zone???
      Entity *pkNewObject = m_pkObjMan->CreateObjectFromScript ( m_pkObject->m_strCreatedFromScript.c_str() );

      // copy all object data
      P_Item *pkNewItemProp = (P_Item*)pkNewObject->GetProperty("P_Item");

      *pkNewItemProp->m_pkItemStats = *m_pkItemStats;

      pkNewItemProp->m_pkItemStats->SetQuantity ( iTookens );

      return pkNewObject;
   }
   else 
      return 0;
}

// ------------------------------------------------------------------------------------------

bool P_Item::Stock ( Entity *pkObject )
{
   P_Item *pkIP = (P_Item*)pkObject->GetProperty("P_Item");

   if ( pkIP != 0 )
      if ( *pkIP->m_pkItemStats == *m_pkItemStats )
      {
         pkIP->m_pkItemStats->m_iQuantity += m_pkItemStats->m_iQuantity;

         // remove the stacked object
         m_pkObjMan->Delete ( pkObject );

         return true;
      }

   return false;
}

// ---------------------------------------------------------------------------------------------

void P_Item::RequestUpdateFromServer (string kType)
{
   
   int iClientObjectID = m_pkZeroFps->GetClientObjectID();
   Entity* pkClientObj = m_pkObjMan->GetObjectByNetWorkID(iClientObjectID);

   if ( pkClientObj )
   {
      // get ClientControlProperty
      P_ClientControl* pkCC = (P_ClientControl*)pkClientObj->GetProperty("P_ClientControl");

      ClientOrder kOrder;

      if ( kType == "container" && m_pkItemStats->m_pkContainer )
      {
         // get client object
         kOrder.m_sOrderName = "(rq)cont";
         kOrder.m_iObjectID = m_pkObject->iNetWorkID;
         kOrder.m_iClientID = m_pkZeroFps->GetConnectionID();
         kOrder.m_iCharacter = pkCC->m_iActiveCaracterObjectID;

         // use this useless variabel to send which version of the item this prop. has
         kOrder.m_iUseLess = m_pkItemStats->m_pkContainer->m_uiVersion; 
         
         pkCC->AddOrder (kOrder);

      }
      // for now, the only data the client need is icon, item name and tooltip
      else if ( kType == "data" )
      {
         // hmm...will this work? Problem if object changes name..ok otherwise..i think
         //if object already has a name and such, already got his info from the server...
         //if ( m_pkItemStats->m_kItemName.size() )
         //   return;

         // get client object
         kOrder.m_sOrderName = "(rq)item";
         kOrder.m_iObjectID = m_pkObject->iNetWorkID;
         kOrder.m_iClientID = m_pkZeroFps->GetConnectionID();
         kOrder.m_iCharacter = pkCC->m_iActiveCaracterObjectID;

         // use this useless variabel to send which version of the item this prop. has
         kOrder.m_iUseLess = m_pkItemStats->m_uiVersion;     

         pkCC->AddOrder (kOrder);        
      }
      else
         cout << "The request:" << kType << " in P_Item::RequestUpdateFromServer was illegal" << endl;
   }
   else
      cout << "P_Item::RequestUpdateFromServer(): no client object found!" << endl;
   
}

// ---------------------------------------------------------------------------------------------

void P_Item::GetAllItemsInContainer( vector<Entity*>* pkContainerList )
{
   if ( !pkContainerList )
   {
      cout << "error! Null pointer in P_Item::GetAllItemsInContainer!" << endl;
      return;
   }

   m_pkInventoryList = pkContainerList;

   RequestUpdateFromServer ( "container" );

}

// ---------------------------------------------------------------------------------------------

void P_Item::AddSendsData(SendType ns)
{
	SetNetUpdateFlag(true);		
	
	m_kSends.push_back(ns);
}

// ---------------------------------------------------------------------------------------------

bool P_Item::UseOn ( Entity *pkCharacterObject )
{
   if ( !pkCharacterObject )
   {
      cout << "Error! Tried to use a object on a null pointer. Character wanted!" << endl;
      return false;
   }

   // check if target object is character
   CharacterProperty* pkChar = (CharacterProperty*)pkCharacterObject->GetProperty ("P_CharStats");

   if ( pkChar )
   {
      m_pkItemStats->EquipOn ( pkChar->GetCharStats() );

      // Object is destroyed when used
      m_pkObject->m_pkObjectMan->Delete ( m_pkObject );

      return true;
   }
   else
   {
      cout << "Tried to use a item on a object without characterproperty." << endl;
      return false;
   }
}

// ---------------------------------------------------------------------------------------------

Property* Create_P_Item()
{
	return new P_Item;
}

