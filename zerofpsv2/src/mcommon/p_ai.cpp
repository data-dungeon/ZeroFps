#include "p_ai.h"
#include "rulesystem/character/characterfactory.h"
#include "p_charstats.h"
#include "p_item.h"
#include "../zerofpsv2/engine/p_pfpath.h"
#include "../zerofpsv2/engine/entity.h"
#include "../zerofpsv2/engine/zerofps.h"

// ------------------------------------------------------------------------------------------

void P_AI::Update()
{
   
   if ( !m_pkCurrentOrder )
   {
      NextOrder();
      return;
   }
   
   // check if order is finished, and if so, go on to the next one
   if ( m_pkCurrentOrder->m_kOrderType == "MovingTowards" 
        && m_pkObject->GetWorldPosV() == m_pkCurrentOrder->m_kPosition )
   {
      NextOrder();
   }
   else if ( m_pkCurrentOrder->m_kOrderType == "MoveTo" )
   {
      // make a pathfinding
      ((P_PfPath*)m_pkObject->GetProperty ("P_PfPath"))->MakePathFind ( m_pkCurrentOrder->m_kPosition );

      Order *pkMoveOrd = new Order;

      pkMoveOrd->m_kType = "MovingTowards";
      pkMoveOrd->m_kPosition = m_pkCurrentOrder->m_kPosition;

      m_kDynamicOrders.pop_front();
      m_kDynamicOrders.push_front ( pkMoveOrd );

      NextOrder();
   }
   else if ( m_pkCurrentOrder->m_kOrderType == "PickUp" )
   {
      Entity* pkEnt = m_pkObject->m_pkObjectMan->GetObjectByNetWorkID( m_pkCurrentOrder->m_iTargetID );

      // if character has reacher object position
      if ( m_pkCurrentOrder->m_kPosition.DistanceTo (m_pkObject->GetWorldPosV()) > 0.5f )
      {

         // check if object still exists!!
         if ( pkEnt )
         {
            ((P_Item*)m_pkObject->GetProperty("P_Item"))->m_pkItemStats->m_pkContainer->AddObject (m_pkCurrentOrder->m_iTargetID);
            //m_pkCharProp->GetCharStats()->m_pkContainer->AddObject( m_pkCurrentOrder->m_iTargetID );
            NextOrder();
         }
      }
      else
      {
         Order* pkMoveOrder = new Order;
         pkMoveOrder->m_kOrderType = "MoveTo";
         pkMoveOrder->m_kPosition = pkEnt->GetWorldPosV();

         m_kDynamicOrders.push_front ( pkMoveOrder );
         m_pkCurrentOrder = m_kDynamicOrders.front();
      }
   }
   else if ( m_pkCurrentOrder->m_kOrderType == "Use" )
   {
      Entity* pkObj = m_pkObject->m_pkObjectMan->GetObjectByNetWorkID( m_pkCurrentOrder->m_iTargetID );

      if ( pkObj )
      {
         ((P_Item*)pkObj->GetProperty ("P_Item"))->UseOn ( m_pkObject );
         NextOrder();
      }
   }

}

// ------------------------------------------------------------------------------------------

void P_AI::NextOrder()
{
   // always prioriti dynamic orders
   if ( m_kDynamicOrders.size() )
   {
      if ( m_pkCurrentOrder = (m_kDynamicOrders.front()) )
         m_kDynamicOrders.pop_front();

      if ( m_kDynamicOrders.size() )
         m_pkCurrentOrder = (m_kDynamicOrders.front());
//      else
//         m_pkCurrentOrder = 0;

   }
   else if ( m_kStaticOrders.size() )
   {
      if ( m_pkCurrentOrder = (m_kDynamicOrders.front()) )
         m_kDynamicOrders.pop_front();

      if ( m_kDynamicOrders.size() )
         m_pkCurrentOrder = (m_kDynamicOrders.front());
//      else
//         m_pkCurrentOrder = 0;
   }

   cout << "new order:" << m_pkCurrentOrder << endl;
   
}

// ------------------------------------------------------------------------------------------

vector<PropertyValues> P_AI::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	return kReturn;
}

// ------------------------------------------------------------------------------------------

void P_AI::Init()
{
   m_pkCharProp = (CharacterProperty*)m_pkObject->GetProperty ("P_CharStats");
}

// ------------------------------------------------------------------------------------------

P_AI::P_AI()
{
	m_iSide = PROPERTY_SIDE_SERVER;

	strcpy(m_acName,"P_AI");

	bNetwork = false;

   m_pkCurrentOrder = 0;
}

// ------------------------------------------------------------------------------------------

P_AI::P_AI( string kName )
{
	bNetwork = true;

	strcpy(m_acName,"P_AI");
}

// ------------------------------------------------------------------------------------------

bool P_AI::HandleSetValue( string kValueName, string kValue )
{

	return false;
}
// ------------------------------------------------------------------------------------------

void P_AI::Save(ZFIoInterface* pkPackage)
{
}

// ------------------------------------------------------------------------------------------

void P_AI::Load(ZFIoInterface* pkPackage)
{
}

// ------------------------------------------------------------------------------------------

void P_AI::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
}

// ------------------------------------------------------------------------------------------

void P_AI::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
}

// ------------------------------------------------------------------------------------------

void P_AI::AddStaticOrder ( string kOrderType, int iTargetID1, int iTargetID2, Vector3 kPosition )
{
   Order* pkNewOrder = new Order;

   pkNewOrder->m_kOrderType = kOrderType;
   pkNewOrder->m_iTargetID = iTargetID1;
   pkNewOrder->m_iTargetID2 = iTargetID2;
   pkNewOrder->m_kPosition = kPosition;

   cout << "Added stat. order:" << kOrderType << endl;

   m_kStaticOrders.push_back ( pkNewOrder );
}

// ------------------------------------------------------------------------------------------

void P_AI::AddDynamicOrder ( string kOrderType, int iTargetID1, int iTargetID2, Vector3 kPosition )
{
   Order* pkNewOrder = new Order;

   pkNewOrder->m_kOrderType = kOrderType;
   pkNewOrder->m_iTargetID = iTargetID1;
   pkNewOrder->m_iTargetID2 = iTargetID2;
   pkNewOrder->m_kPosition = kPosition;

   cout << "Added dyn. order:" << kOrderType << endl;

   m_kDynamicOrders.push_back ( pkNewOrder );
}

// ------------------------------------------------------------------------------------------

Property* Create_P_AI()
{
	return new P_AI;
}


