#ifndef CONTAINER_H
#define CONTAINER_H

#include "../mcommon_x.h"
#include <vector>
   using namespace std;

class Entity;
class Property;

class MCOMMON_API Container
{
private:
   vector<Entity*> m_kContainedObjects;
   Property* m_pkParent;

public:
   int m_iCapacity; // number of objets, change to weight??

   // returns true if there was room for the object
   bool AddObject ( Entity* pkAddToContainer );

   // returns false if the object wasn't found in the container
   bool RemoveObject ( Entity* pkRemoveFromContainer );

   vector<Entity*>* GetItemsInContiner()  { return &m_kContainedObjects; }

   bool IsEmpty()                         { return m_kContainedObjects.size(); }

   // throws out all objects in container, or something :)
   void Empty();

   Container( Property* pkParent );

};

#endif