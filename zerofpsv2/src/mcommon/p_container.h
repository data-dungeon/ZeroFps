#ifndef _ENGINE_P_CONTAINER_H_
#define _ENGINE_P_CONTAINER_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include <vector>
   using namespace std;

class Entity;
class Property;
class ItemStats;

/**	\brief	Da P_Container
		\ingroup Common
*/
class MCOMMON_API P_Container: public Property 
{
private:
   vector<int> m_kContainedObjects;

public:
   int m_iCapacity; // number of objets, change to weight??
   int m_iContainerID;

   unsigned int m_uiVersion;

 	void Update();
	void CloneOf(Property* pkProperty) {}
	vector<PropertyValues> GetPropertyValues();

   void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
   void PackTo(NetPacket* pkNetPacket, int iConnectionID );

   void Load(ZFIoInterface* pkPackage);
   void Save(ZFIoInterface* pkPackage);

   // returns true if there was room for the object
   bool AddObject (int iAddToContainer );

   // returns false if the object wasn't found in the container
   bool RemoveObject ( int iRemoveFromContainer );

   bool IsEmpty()                      { return m_kContainedObjects.empty(); }

   void AddObjectToBeLoaded (int iAddToContainer);

   // empties container
   void Clear()                        { m_kContainedObjects.clear(); }

   void GetAllItemsInContainer( vector<Entity*>* kObjectIDs );

   // throws out all objects in container, or something :)
   void Empty();

   void Init();

   void AddSendsData ( int iClientID );

   void RequestUpdateFromServer();

   int Items()                         { return m_kContainedObjects.size(); }

   vector<int>* GetItemsInContainer()  { return &m_kContainedObjects; }

   P_Container();


};

MCOMMON_API Property* Create_P_Container();

#endif










