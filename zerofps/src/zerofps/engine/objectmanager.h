#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#include "object.h"


using namespace std;


class ENGINE_API ObjectManager : public ZFObject{
	
	list<Object*>	m_akObjects;
	vector<Object*> m_akDeleteList;

	void RunCommand(int cmdid, const CmdArgument* kCommand) { }

	int	iNextObjectID;

	public:
		ObjectManager();
		~ObjectManager() { }

		void Add(Object* pkNewObject);				//add object to the manager
		void Delete(Object* pkNewObject);			//adds an object to delete qeue
		void Remove(Object* pkObject);				//dont use this..use Delete instead
		void Update();								//update all objects in manager
		void Update(int iType);						//update all objects of specified type
		void UpdateDelete();						//deletes objects in delete qeue	
		
		void UpdateState(NetPacket* pkNetPacket);	//Updates objects.
		void PackToClients();						//Packs and Sends to ALL clients.

		Object*	GetObjectByNetWorkID(int iNetID);
		Object* CreateObjectByNetWorkID(int iNetID);		

};


#endif




