#include "container.h"
//#include "../../zerofpsv2/engine/entity.h"
#include "../p_item.h"
#include "../../zerofpsv2/engine/entitymanager.h"


// -----------------------------------------------------------------------------------------------

Container::Container( Property* pkParent )
{
   m_pkParent = pkParent;
   m_iCapacity = 5;
   m_uiVersion = 0;
}

// -----------------------------------------------------------------------------------------------

// returns true if there was room for the object
bool Container::AddObject ( int iAddToContainer )
{
   if ( m_kContainedObjects.size() < m_iCapacity )
   {

      Entity *pkEntity = m_pkParent->GetObject()->m_pkObjectMan->GetObjectByNetWorkID ( iAddToContainer );

      P_Item *pkIP = ((P_Item*)pkEntity->GetProperty("P_Item"));

      if ( !pkIP )
      {
         cout << "Error! Tried to put a nonItem object in the container!" << endl;
         return false;
      }


      // objects in containers doesn't need to be updated
      pkEntity->SetUpdateStatus (UPDATE_NONE);

      // Set which container the item is in
      pkIP->m_pkItemStats->m_iCurrentContainer = ((P_Item*)m_pkParent)->m_pkItemStats->m_iContainerID;
      pkIP->m_pkItemStats->m_pkIsInContainer = this;

      // make object child of containers parent
      pkEntity->SetParent ( m_pkParent->GetObject() );

      m_kContainedObjects.push_back ( iAddToContainer );

      m_uiVersion++;

      return true;
   }
   else
      return false;

}

// -----------------------------------------------------------------------------------------------

// returns false if the object wasn't found in the container
bool Container::RemoveObject ( int iRemoveFromContainer )
{
   for ( vector<int>::iterator kIte = m_kContainedObjects.begin();
         kIte != m_kContainedObjects.end(); kIte++ )
      if ( (*kIte) == iRemoveFromContainer )
      {

         m_uiVersion++;

         Entity *pkEntity = 
            m_pkParent->GetObject()->m_pkObjectMan->GetObjectByNetWorkID ( (*kIte) );

         // Set which container the item is in
         ((P_Item*)pkEntity->GetProperty("P_Item"))->m_pkItemStats->m_iCurrentContainer = 0;
         ((P_Item*)pkEntity->GetProperty("P_Item"))->m_pkItemStats->m_pkIsInContainer = 0;


         // unparent object
         m_pkParent->GetObject()->RemoveChild (pkEntity );

         pkEntity->AttachToZone();

         pkEntity->SetUpdateStatus( UPDATE_ALL );

         // do some other fun stuff first...(unparent, and such)
         m_kContainedObjects.erase (kIte);


         return true;
      }

   // object wasn't found in the container
   return false;
}

// -----------------------------------------------------------------------------------------------

// throws out all objects in container, or something :)
void Container::Empty()
{
/*
   for ( vector<Entity*>::iterator kIte = m_kContainedObjects.begin(); 
         kIte != m_kContainedObjects.end(); kIte++ )
      // do some other fun stuff first...(unparent, and such)
      m_kContainedObjects.erase (kIte);
*/

}

// -----------------------------------------------------------------------------------------------

void Container::GetAllItemsInContainer( vector<Entity*>* pkItemList )
{
   if ( pkItemList )
      for ( int i = 0; i < m_kContainedObjects.size(); i++ )
      {
         Entity *pkEntity = 
            m_pkParent->GetObject()->m_pkObjectMan->GetObjectByNetWorkID ( m_kContainedObjects[i] );

         // get itemstats property
         P_Item *pkItem = (P_Item*)pkEntity->GetProperty("P_Item");

         // add item to container list
         pkItemList->push_back ( pkEntity );

         // if item in container is a container itself, request update to that container...
   //      if ( pkItem->m_pkItemStats->m_pkContainer )
     //       ((P_Item*)m_pkParent)->m_pkItemStats->m_pkContainer->GetAllItemsInContainer ( pkItemList );         

      }
   else
      cout << "Error! Got a nullpointer pkItemList! (container::GetAllItemsInContainer)" << endl;
}

// -----------------------------------------------------------------------------------------------
