#include "p_ai.h"
#include "rulesystem/character/characterfactory.h"
#include "rulesystem/rulesystem.h"
#include "p_event.h"
#include "../zerofpsv2/engine/p_pfpath.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine/entity.h"
#include "../zerofpsv2/engine/zerofps.h"
#include <iostream>
   using namespace std;

// ------------------------------------------------------------------------------------------

void P_AI::Update() 
{
   // TODO!! damn this...
   if ( !m_pkCharProp )
      m_pkCharProp = (CharacterProperty*)m_pkObject->GetProperty ("P_CharStats");
   
   if ( !m_pkCurrentOrder )
   {
      NextOrder();
      return;
   }

   // check if order is finished, and if so, go on to the next one
   if ( m_pkCurrentOrder->m_kOrderType == "MovingTowards" 
        && m_pkObject->GetWorldPosV().DistanceXZTo(m_pkCurrentOrder->m_kPosition) < 0.8f )
   {
      NextOrder();
   }
   else if ( m_pkCurrentOrder->m_kOrderType == "Attack" )
   {
      Entity* pkEnemy = m_pkObject->m_pkObjectMan->GetObjectByNetWorkID( m_pkCurrentOrder->m_iTargetID );

      // if enemy is dead, remove order
      if ( !pkEnemy )
      {
         NextOrder();
         return;
      }

      // if character is to far away, move closer
      Vector3 kPos = m_pkObject->GetWorldPosV();
      if ( pkEnemy->GetWorldPosV().DistanceXZTo(kPos) > 2 )
      {
         Order* pkMoveOrder = new Order;
         pkMoveOrder->m_kOrderType = "MoveTo";
         
         //dvoids fulhack deluxe
         Vector3 kSlot = Vector3(sin(DegToRad(rand()%360))*1.5,0,sin(DegToRad(rand()%360))*1.5);
         
         pkMoveOrder->m_kPosition = pkEnemy->GetWorldPosV() + kSlot;

         pkMoveOrder->m_bDynamic = true;
         m_kDynamicOrders.push_front ( pkMoveOrder );
         m_pkCurrentOrder = m_kDynamicOrders.front();         
      }
      else
      {
         // stop the character
         ((P_PfPath*)m_pkObject->GetProperty ("P_PfPath"))->ClearPath();

         // if character has "reloaded", make attack move
         if ( m_pkCharProp->GetCharStats()->ReadyForAction() )
         {
            string kWhenHit = ((CharacterProperty*)pkEnemy->GetProperty("P_CharStats"))->GetCharStats()->m_strScriptWhenHit;

				//rotate to target
				Vector3 kdiff = pkEnemy->GetWorldPosV() - kPos;
				kdiff.y = 0;
				Matrix4 kRotM;

            // crashed if vector is null
            if ( kdiff != Vector3::ZERO )
            {
				   kRotM.LookDir(kdiff.Unit(),Vector3(0,1,0));
   				kRotM.Transponse();		
            }

				m_pkObject->SetLocalRotM(kRotM);

            // create splattblood PSystem
            if ( kWhenHit.size() )
               m_pkObject->m_pkObjectMan->CreateObjectFromScriptInZone ( kWhenHit.c_str(), pkEnemy->GetWorldPosV() );

            // change animation to attack animation
            ((P_Mad*)m_pkObject->GetProperty("P_Mad"))->SetAnimation ("attack", 0);
            ((P_Mad*)m_pkObject->GetProperty("P_Mad"))->SetNextAnimation ("idle");

            // roll to hit!
            if ( m_pkCharProp->GetCharStats()->RollSkillDice(
               m_pkCharProp->GetCharStats()->m_strPrimSkill, 3) )
            {
               DealDamage( m_pkCharProp->GetCharStats()->GetFightStats(),
                  ((CharacterProperty*)pkEnemy->GetProperty("P_CharStats"))->GetCharStats() );
            }
            else
               cout << "Missed!" << endl;

            m_pkCharProp->GetCharStats()->ResetActionTimer();
         }
      }
   }
   else if ( m_pkCurrentOrder->m_kOrderType == "MoveTo" )
   {
      // make a pathfinding
      ((P_PfPath*)m_pkObject->GetProperty ("P_PfPath"))->MakePathFind ( m_pkCurrentOrder->m_kPosition );

      Order *pkMoveOrd = new Order;

      pkMoveOrd->m_kOrderType = "MovingTowards";
      pkMoveOrd->m_kPosition = m_pkCurrentOrder->m_kPosition;
      pkMoveOrd->m_bDynamic = true;

      if ( m_pkCurrentOrder->m_bDynamic )
         m_kDynamicOrders.pop_front();

      //NextOrder();

      m_kDynamicOrders.push_front ( pkMoveOrd );

      NextOrder();
   }
   else if ( m_pkCurrentOrder->m_kOrderType == "Idle" )
   {
      if ( m_pkCurrentOrder->m_iTargetID <= 0 )
      {
         m_pkCurrentOrder->m_iTargetID = m_pkCurrentOrder->m_iTargetID2;
         NextOrder();
      }
      else
         m_pkCurrentOrder->m_iTargetID -= m_pkZeroFps->GetGameFrameTime() * 10000;
   }
   else if ( m_pkCurrentOrder->m_kOrderType == "Face" )
   {
      // face object
      if ( m_pkCurrentOrder->m_iTargetID )
      {
         Entity* pkFaceObj = m_pkObject->m_pkObjectMan->GetObjectByNetWorkID( m_pkCurrentOrder->m_iTargetID );

		   //rotate to target
		   Vector3 kdiff = pkFaceObj->GetWorldPosV() - m_pkObject->GetWorldPosV();
		   kdiff.y = 0;
		   Matrix4 kRotM;

         // crashed if vector is null
         if ( kdiff != Vector3::ZERO )
         {
			   kRotM.LookDir(kdiff.Unit(),Vector3(0,1,0));
   		   kRotM.Transponse();		
         }

		   m_pkObject->SetLocalRotM(kRotM);
      }
      else
      {
         Matrix4 kM;
			kM= m_pkObject->GetLocalRotM();
         kM.LookDir ( m_pkCurrentOrder->m_kPosition, Vector3(0,1,0) );
         m_pkObject->SetLocalRotM ( kM );
      }

      NextOrder();

   }
   else if ( m_pkCurrentOrder->m_kOrderType == "Action" )
   {
      Entity* pkEnt = m_pkObject->m_pkObjectMan->GetObjectByNetWorkID( m_pkCurrentOrder->m_iTargetID );
	
		Vector3 kPos = m_pkObject->GetWorldPosV();

      // if character has reacher object position
      if ( m_pkCurrentOrder->m_kPosition.DistanceXZTo(kPos) <= 0.8f )
      {
         // stop the character
         ((P_PfPath*)m_pkObject->GetProperty ("P_PfPath"))->ClearPath();

         // check if object still exists!!
         if ( pkEnt )
            ((P_Event*)pkEnt->GetProperty("P_Event"))->SendObjectClickEvent( "PickUp", m_pkObject->iNetWorkID );
         
         NextOrder();
      }
      else
      {
         Order* pkMoveOrder = new Order;
         pkMoveOrder->m_kOrderType = "MoveTo";
         pkMoveOrder->m_kPosition = pkEnt->GetWorldPosV();
         pkMoveOrder->m_bDynamic = true;

         m_kDynamicOrders.push_front ( pkMoveOrder );
         m_pkCurrentOrder = m_kDynamicOrders.front();
      }
   }

}

// ------------------------------------------------------------------------------------------

void P_AI::NextOrder()
{
   // always priority dynamic orders
   if ( m_kDynamicOrders.size() )
   {
      if ( m_pkCurrentOrder == (m_kDynamicOrders.front()) )
      {
         delete m_kDynamicOrders.front();
         m_kDynamicOrders.pop_front();
      }

      if ( m_kDynamicOrders.size() )
         m_pkCurrentOrder = (m_kDynamicOrders.front());
      else
         m_pkCurrentOrder = 0;
   }
   else if ( m_kStaticOrders.size() )
   {
      m_kOrderIte++;

      if ( m_kOrderIte == m_kStaticOrders.end() )
         m_kOrderIte = m_kStaticOrders.begin();

      m_pkCurrentOrder = (*m_kOrderIte);

      cout << "Chaned sataic order to:" << m_pkCurrentOrder->m_kPosition.x <<  endl;
   }
   else
      m_pkCurrentOrder = 0;

   
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

   if ( !m_pkCharProp )
      cout << "ERROR!!! P_AI is assigned to a object withour P_CharStats. Crash is certain" << endl;
}

// ------------------------------------------------------------------------------------------

P_AI::P_AI()
{
	m_iSide = PROPERTY_SIDE_SERVER;

	strcpy(m_acName,"P_AI");

	bNetwork = false;
   m_bAIPlayer = false;

   m_pkCurrentOrder = 0;

   m_kOrderIte = m_kStaticOrders.begin();
}

// ------------------------------------------------------------------------------------------

P_AI::P_AI( string kName )
{
	bNetwork = true;
   m_bAIPlayer = false;

	strcpy(m_acName,"P_AI");

   m_kOrderIte = m_kStaticOrders.begin();
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

void P_AI::AddStaticOrder ( string kOrderType, int iTargetID1, int iTargetID2, Vector3 kPosition, string kType )
{
   // is AI is player, return. Players can't have static orders
   if ( m_bAIPlayer )
   {
      cout << "Warning! Tried to give player a static order!" << endl;
      return;
   }

   Order* pkNewOrder = new Order;

   pkNewOrder->m_kOrderType = kOrderType;
   pkNewOrder->m_kType = kType;
   pkNewOrder->m_iTargetID = iTargetID1;
   pkNewOrder->m_iTargetID2 = iTargetID2;
   pkNewOrder->m_kPosition = kPosition;
   pkNewOrder->m_bDynamic = false;


   m_kStaticOrders.push_back ( pkNewOrder );
}

// ------------------------------------------------------------------------------------------

void P_AI::AddDynamicOrder ( string kOrderType, int iTargetID1, int iTargetID2, Vector3 kPosition, string kType )
{
   Order* pkNewOrder = new Order;

   pkNewOrder->m_kOrderType = kOrderType;
   pkNewOrder->m_kType = kType;
   pkNewOrder->m_iTargetID = iTargetID1;
   pkNewOrder->m_iTargetID2 = iTargetID2;
   pkNewOrder->m_kPosition = kPosition;
   pkNewOrder->m_bDynamic = true;

   m_kDynamicOrders.push_back ( pkNewOrder );

}

// ------------------------------------------------------------------------------------------

void P_AI::ClearDynamicOrders()
{
   for (unsigned int i = 0; i < m_kDynamicOrders.size(); i++ )
   {
      delete m_kDynamicOrders.front();
      m_kDynamicOrders.pop_front();
   }

	m_pkCurrentOrder = NULL;
}

// ------------------------------------------------------------------------------------------

P_AI::~P_AI()
{
   unsigned int i;

   for ( i=0; i < m_kDynamicOrders.size(); i++ )
      delete m_kDynamicOrders.at(i);

   for ( i=0; i < m_kStaticOrders.size(); i++ )
      delete m_kStaticOrders.at(i);

}

// ------------------------------------------------------------------------------------------

Property* Create_P_AI()
{
	return new P_AI;
}



