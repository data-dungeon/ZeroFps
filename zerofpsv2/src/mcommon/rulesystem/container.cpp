#include "container.h"

// -----------------------------------------------------------------------------------------------

Container::Container( Property* pkParent )
{
   m_pkParent = pkParent;
   m_iCapacity = 5;
}

// -----------------------------------------------------------------------------------------------

// returns true if there was room for the object
bool Container::AddObject ( Entity* pkAddToContainer )
{
   if ( m_kContainedObjects.size() < m_iCapacity )
   {
      // todo: some other stuff, objects in containers are speciall :)
      m_kContainedObjects.push_back ( pkAddToContainer );
      return true;
   }
   else
      return false;

}

// -----------------------------------------------------------------------------------------------

// returns false if the object wasn't found in the container
bool Container::RemoveObject ( Entity* pkRemoveFromContainer )
{
   for ( vector<Entity*>::iterator kIte = m_kContainedObjects.begin(); 
         kIte != m_kContainedObjects.end(); kIte++ )
      if ( (*kIte) == pkRemoveFromContainer )
      {
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
   for ( vector<Entity*>::iterator kIte = m_kContainedObjects.begin(); 
         kIte != m_kContainedObjects.end(); kIte++ )
      // do some other fun stuff first...(unparent, and such)
      m_kContainedObjects.erase (kIte);


}

// -----------------------------------------------------------------------------------------------
