#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#include "object.h"


using namespace std;


class ENGINE_API ObjectManager : public ZFObject{
	struct Property_Less : public binary_function<Property*, Property*, bool> {
		bool operator()(Property* x, Property* y) { return *x < *y; };
	} Less_Property;
	
	
	
	list<Object*>	m_akObjects;
	vector<Object*> m_akDeleteList;
	list<Property*> m_akPropertys;

	void RunCommand(int cmdid, const CmdArgument* kCommand) { }

	int	iNextObjectID;

	public:
		ObjectManager();
		~ObjectManager() { }


		void GetPropertys(int iType,int iSide);
		void Update(int iType,int iSide,bool bSort);

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
		int	GetNumOfObjects();

};


#endif




