#include "p_item.h"
#include "../zerofpsv2/engine/object.h"
#include "../zerofpsv2/engine/objectmanager.h"

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

   m_pkItemStats = new ItemStats;

	bNetwork = true;

	strcpy(m_acName,"P_Item");
}

// ------------------------------------------------------------------------------------------

P_Item::P_Item( string kName )
{
	m_iSide = PROPERTY_SIDE_CLIENT;

   m_pkItemStats = new ItemStats;

	bNetwork = true;

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
}

// ------------------------------------------------------------------------------------------

void P_Item::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
}

// ------------------------------------------------------------------------------------------

Object* P_Item::Split ( int iTookens )
{
   // only works if object was created from script 
   if ( m_pkObject->m_strCreatedFromScript.size() && iTookens > 0 && iTookens < m_pkItemStats->GetQuantity() )
   {
      m_pkItemStats->AddQuantity( -iTookens );      

      // in zone???
      Object *pkNewObject = m_pkObjMan->CreateObjectFromScript ( m_pkObject->m_strCreatedFromScript.c_str() );

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

bool P_Item::Stock ( Object *pkObject )
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

Property* Create_P_Item()
{
	return new P_Item;
}

