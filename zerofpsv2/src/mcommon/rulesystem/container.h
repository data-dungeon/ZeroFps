#ifndef CONTAINER_H
#define CONTAINER_H

#include "../mcommon_x.h"
#include <vector>
   using namespace std;

class Entity;
class Property;
class ItemStats;

class MCOMMON_API Container
{
private:
   vector<int> m_kContainedObjects;
   Property* m_pkParent;

public:
   int m_iCapacity; // number of objets, change to weight??

   unsigned int m_uiVersion;

   // returns true if there was room for the object
   bool AddObject (int iAddToContainer );

   // returns false if the object wasn't found in the container
   bool RemoveObject ( int iRemoveFromContainer );

   vector<int>* GetItemsInContainer()  { return &m_kContainedObjects; }

   bool IsEmpty()                      { return m_kContainedObjects.size(); }

   // empties container
   void Clear()                        { m_kContainedObjects.clear(); }

   // throws out all objects in container, or something :)
   void Empty();

   int Items()                         { return m_kContainedObjects.size(); }

   Container( Property* pkParent );

   void GetAllItemsInContainer( vector<Entity*>* kObjectIDs );

};

#endif