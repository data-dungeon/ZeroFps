#include "p_clientcontrol.h"
#include "p_container.h"
#include "p_item.h"
#include "../zerofpsv2/engine/entitymanager.h"

// -----------------------------------------------------------------------------------------------

P_Container::P_Container()
{
   m_iCapacity = 500;
   m_uiVersion = 0;
   
   m_iSide = PROPERTY_SIDE_SERVER;

	strcpy(m_acName,"P_Container");

	bNetwork = true;
}

// -----------------------------------------------------------------------------------------------

void P_Container::Init()
{
   m_iContainerID = m_pkObject->GetEntityID();
}

// -----------------------------------------------------------------------------------------------

void P_Container::AddObjectToBeLoaded (int iAddToContainer)
{
   m_kContainedObjects.push_back ( iAddToContainer );
}

// -----------------------------------------------------------------------------------------------

// returns true if there was room for the object
bool P_Container::AddObject ( int iAddToContainer )
{
   if ( (int) m_kContainedObjects.size() < m_iCapacity )
   {

      Entity *pkEntity = m_pkObject->m_pkEntityMan->GetObjectByNetWorkID ( iAddToContainer );

      if ( !pkEntity )
      {
         cout << "Error! Exists no object with ID:" << iAddToContainer << " (container::AddObject)" << endl;
         return false;
      }
      
      P_Item *pkIP = ((P_Item*)pkEntity->GetProperty("P_Item"));

      if ( !pkIP )
      {
         cout << "Error! Tried to put a nonItem object in the container!" << endl;
         return false;
      }

      // objects in containers doesn't need to be updated
      pkEntity->SetUpdateStatus (UPDATE_NONE);

      // Set which container the item is in
      pkIP->m_pkItemStats->m_iCurrentContainer = m_iContainerID;

      // make object child of containers parent
      pkEntity->SetParent ( m_pkObject );

      m_kContainedObjects.push_back ( iAddToContainer );

      m_uiVersion++;

      return true;
   }
   else
      return false;

}
// -----------------------------------------------------------------------------------------------

// returns false if the object wasn't found in the container
bool P_Container::RemoveObject ( int iRemoveFromContainer )
{
   for ( vector<int>::iterator kIte = m_kContainedObjects.begin();
         kIte != m_kContainedObjects.end(); kIte++ )
      if ( (*kIte) == iRemoveFromContainer )
      {

         m_uiVersion++;

         Entity *pkEntity = 
            m_pkObject->m_pkEntityMan->GetObjectByNetWorkID ( (*kIte) );

         // Set which container the item is in
         ((P_Item*)pkEntity->GetProperty("P_Item"))->m_pkItemStats->m_iCurrentContainer = -1;

         // unparent object
         m_pkObject->RemoveChild (pkEntity );

         pkEntity->AttachToZone();

         pkEntity->SetUpdateStatus( UPDATE_ALL );

         // remove from containerlist
         m_kContainedObjects.erase (kIte);

         return true;
      }

   // object wasn't found in the container
   return false;
}

// -----------------------------------------------------------------------------------------------

// throws out all objects in container, or something :)
void P_Container::Empty()
{
/*
   for ( vector<Entity*>::iterator kIte = m_kContainedObjects.begin(); 
         kIte != m_kContainedObjects.end(); kIte++ )
      // do some other fun stuff first...(unparent, and such)
      m_kContainedObjects.erase (kIte);
*/

}

// -----------------------------------------------------------------------------------------------

void P_Container::GetAllItemsInContainer( vector<Entity*>* pkItemList )
{
   // do a request to server for update
   if ( pkItemList )
      for ( unsigned int i = 0; i < m_kContainedObjects.size(); i++ )
      {
         Entity *pkEntity = 
            m_pkObject->m_pkEntityMan->GetObjectByNetWorkID ( m_kContainedObjects[i] );

         // add item to container list
         pkItemList->push_back ( pkEntity );
      
         // get itemstats property
         P_Container *pkC = (P_Container*)pkEntity->GetProperty("P_Container");

         // if item in container is a container itself, request update to that container...
         if ( pkC )
            pkC->GetAllItemsInContainer ( pkItemList );       

      }
   else
      cout << "Error! Got a nullpointer pkItemList! (P_Container::GetAllItemsInContainer)" << endl;
}

// -----------------------------------------------------------------------------------------------

void P_Container::RequestUpdateFromServer()
{
   int iClientObjectID = m_pkZeroFps->GetClientObjectID();
   Entity* pkClientObj = m_pkObjMan->GetObjectByNetWorkID(iClientObjectID);

   if ( pkClientObj )
   {
      // get ClientControlProperty
      P_ClientControl* pkCP = (P_ClientControl*)pkClientObj->GetProperty("P_ClientControl");

      ClientOrder kOrder;

      // get client object
      kOrder.m_sOrderName = "(rq)cont";
      kOrder.m_iObjectID = m_pkObject->GetEntityID();
      kOrder.m_iClientID = m_pkZeroFps->GetConnectionID();
      kOrder.m_iCharacter = pkCP->m_iActiveCaracterObjectID;

      // use this useless variabel to send which version of the item this prop. has
      kOrder.m_iUseLess = m_uiVersion;

      pkCP->AddOrder (kOrder);
   }
   else
      cout << "ERROR! Couldn't find clientobject! (P_Container::RequestUpdateFromServer)" << endl;
}

// -----------------------------------------------------------------------------------------------

void P_Container::AddSendsData ( int iClientID )
{
	SetNetUpdateFlag(true);		
}

// -----------------------------------------------------------------------------------------------

void P_Container::Update()
{
}

// -----------------------------------------------------------------------------------------------

void P_Container::Save(ZFIoInterface* pkPackage)
{/*
   // save capacity
   pkPackage->Write ( (void*)&m_iCapacity, sizeof(int), 1 );

   // items in container   
   int iContSize = m_kContainedObjects.size();
   pkPackage->Write ( (void*)&iContSize, sizeof(int), 1 );

   // save version
   pkPackage->Write ( (void*)&m_uiVersion, sizeof(unsigned int), 1 );

   // save ID
   pkPackage->Write ( (void*)&m_iContainerID, sizeof(int), 1 );

   // save contained items ID
   for ( int i = 0; i < iContSize; i++ )
      pkPackage->Write( (void*)&m_kContainedObjects[i], sizeof(int), 1); */
}

// -----------------------------------------------------------------------------------------------

void P_Container::Load(ZFIoInterface* pkPackage,int iVersion)
{/*
   // load capacity
   pkPackage->Read ( (void*)&m_iCapacity, sizeof(int), 1 );

   // items in container   
   int iContSize;
   pkPackage->Read ( (void*)&iContSize, sizeof(int), 1 );

   // load version
   pkPackage->Read ( (void*)&m_uiVersion, sizeof(unsigned int), 1 );

   // load ID
   pkPackage->Read ( (void*)&m_iContainerID, sizeof(int), 1 );

   // Read contained item IDs
   for ( int i = 0; i < iContSize; i++ )
   {
      int iID;
      pkPackage->Read ( (void*)&iID, sizeof(int), 1);

      AddObject (iID);
   }*/
}

// -----------------------------------------------------------------------------------------------

void P_Container::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
   // container ID
   pkNetPacket->Write( &m_iContainerID, sizeof(int) );

   int iSize = m_kContainedObjects.size();

   // nr of items in container
   pkNetPacket->Write( &iSize, sizeof(int) );

   // container capacity
   pkNetPacket->Write( &m_iCapacity, sizeof(int) );

   // send itemIDs in container
   for ( int i = 0; i < iSize; i++ )
      pkNetPacket->Write( &m_kContainedObjects[i], sizeof(int) );

   // version
   pkNetPacket->Write( &m_uiVersion, sizeof(unsigned int) );

   SetNetUpdateFlag(iConnectionID, false);
}

// -----------------------------------------------------------------------------------------------

void P_Container::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
   // container ID
   pkNetPacket->Read( &m_iContainerID, sizeof(int) );

   int iSize;

   // nr of items in container
   pkNetPacket->Read( &iSize, sizeof(int) );

   // container capacity
   pkNetPacket->Read( &m_iCapacity, sizeof(int) );

   // clear container so we don't get dubbel
   m_kContainedObjects.clear();

   int iID;
   // send itemIDs in container
   for ( int i = 0; i < iSize; i++ )
   {
      pkNetPacket->Read( &iID, sizeof(int) );
      AddObject (iID);

      // request update for item at the same time
      Entity* pkObj = m_pkObject->m_pkEntityMan->GetObjectByNetWorkID(iID);

      cout << "GOT ITEM IN CONT" << endl;

      if ( pkObj )
         ((P_Item*)pkObj->GetProperty("P_Item"))->RequestUpdateFromServer("data");
      else
         cout << "Error! Object in container doesn't exist! (P_Container::PackFrom)" << endl;
   }

   // version
   pkNetPacket->Read( &m_uiVersion, sizeof(unsigned int) );
}

// -----------------------------------------------------------------------------------------------

vector<PropertyValues> P_Container::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
	return kReturn;
}

// ------------------------------------------------------------------------------------------

Property* Create_P_Container()
{
	return new P_Container;
}
