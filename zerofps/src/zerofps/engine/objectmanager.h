#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#include "object.h"
#include <vector>
#include <list>
#include "../basic/basic.pkg"

using namespace std;

class PropertyDescriptor;
class ObjectDescriptor;


class ENGINE_API ObjectManager : public ZFObject{
	private:
		struct Property_Less : public binary_function<Property*, Property*, bool> {
			bool operator()(Property* x, Property* y) { return *x < *y; };
		} Less_Property;
	
		Object* m_pkWorldObject;
	
		list<Object*>	m_akObjects;
		vector<Object*> m_akDeleteList;
		list<Property*> m_akPropertys;

		list<ObjectDescriptor*> m_akTemplates;

		int	iNextObjectID;
		bool m_bNoUpdate;
		int m_iNrOfActivePropertys;
	
		void RunCommand(int cmdid, const CmdArgument* kCommand) { }


	public:
		ObjectManager();
		~ObjectManager() { }

		Object* GetWorldObject() {return m_pkWorldObject;};

		void GetPropertys(int iType,int iSide);
		void Update(int iType,int iSide,bool bSort);

		void Clear();
		void SetNoUpdate(bool bNoUpdate) {m_bNoUpdate=bNoUpdate;};
		void Add(Object* pkNewObject);				//add object to the manager
		void Delete(Object* pkNewObject);			//adds an object to delete qeue
		void Remove(Object* pkObject);				//dont use this..use Delete instead
//		void Update();								//update all objects in manager
//		void Update(int iType);						//update all objects of specified type
		void UpdateDelete();						//deletes objects in delete qeue	
		
		void AddTemplate(ObjectDescriptor* pkNewTemplate);
		int GetNrOfTemplates();
		void GetTemplateList(vector<string>* paList);
		bool MakeTemplate(const char* acName,Object* pkObject);
		void ClearTemplates();
		ObjectDescriptor* GetTemplate(const char* acName);
		Object* CreateObject(const char* acName);
		bool LoadTemplate(const char* acFile);
		bool SaveTemplate(const char* acName,const char* acFile);
		bool SaveAllObjects(const char* acFile);
		
		void UpdateState(NetPacket* pkNetPacket);	//Updates objects.
		void PackToClients();						//Packs and Sends to ALL clients.

		Object*	GetObjectByNetWorkID(int iNetID);
		Object* CreateObjectByNetWorkID(int iNetID);	
		int	GetNumOfObjects();
		int	GetActivePropertys() {return m_iNrOfActivePropertys;};

};


#endif




