#ifndef _ENGINE_OBJECTMANAGER_H_
#define _ENGINE_OBJECTMANAGER_H_

#include "object.h"
#include "property.h"
#include <vector>
#include <list>
#include "../basic/basic.pkg"

using namespace std;

class PropertyDescriptor;
class ObjectDescriptor;
class ZeroFps;
class GameMessage;

class ENGINE_API ObjectManager : public ZFObject{
	private:
		struct Property_Less : public binary_function<Property*, Property*, bool> {
			bool operator()(Property* x, Property* y) { return *x < *y; };
		} Less_Property;
	
		Object*			m_pkWorldObject;		
	
		list<ObjectArcheType*>	m_akArcheTypes;						// List of all object Archetypes.
		ObjectArcheType*	GetArcheType(string strName);				// Get ptr to AT. NULL if not found.
		void AddArchPropertys(Object* pkObj, string strName);

		list<Object*>	m_akObjects;					// List of all objects.
		vector<Object*> m_akDeleteList;				// List of objects that will be destroyed at end of frame.

		list<Property*> m_akPropertys;				// List of Active Propertys.	
		int m_iNrOfActivePropertys;					// Size of akProperty list.

		list<ObjectDescriptor*> m_akTemplates;		// List of templates.


		int	iNextObjectID;							// Next avil object ID.
		bool m_bUpdate;							// Disable all updates except RENDER.
	
		void RunCommand(int cmdid, const CmdArgument* kCommand) { }

		void GetPropertys(int iType,int iSide);		// Fill propery list.

		ZeroFps*	m_pkZeroFps;

		void TESTVIM_SpawnArcheTypes();
		void TESTVIM_LoadArcheTypes(char* szFileName);

	public:
		ObjectManager();
		~ObjectManager() { }


		// Add/Remove Objects
		void Add(Object* pkNewObject);				// Add object to the manager
		void Delete(Object* pkNewObject);			// Adds an object to delete qeue
		void Remove(Object* pkObject);				// Dont use this..use Delete instead
		void Clear();								// Delete all objects.

		// Updates
		void Update(int iType,int iSide,bool bSort);					// Update selected propertys.
		void SetUpdate(bool bUpdate) { m_bUpdate=bUpdate; };
		void UpdateDelete();											// Deletes objects in delete qeue	
		void UpdateGameMessages(void);

			// Create 
		Object* CreateObject(const char* acName);
		Object* CreateObject(ObjectDescriptor* pkObjDesc);
		Object* CreateObjectByNetWorkID(int iNetID);	
		Object* CreateObjectByArchType(const char* acName);

		// Template
		void AddTemplate(ObjectDescriptor* pkNewTemplate);
		int GetNrOfTemplates();
		void GetTemplateList(vector<string>* paList);
		bool MakeTemplate(const char* acName,Object* pkObject, bool bForce = false);
		void ClearTemplates();
		ObjectDescriptor* GetTemplate(const char* acName);
		bool LoadTemplate(const char* acFile);
		bool SaveTemplate(const char* acName,const char* acFile);

		void Create_OT(int iID);
		void Destory_OT(int iID);
		void LoadGameObjects(const char* szFileName);
		void SaveGameObjects(const char* szFileName);

		// Load/Save Objects
		bool SaveAllObjects(const char* acFile);
		bool LoadAllObjects(const char* acFile);

		// Gets
		Object* GetWorldObject() {return m_pkWorldObject;};
		void GetAllObjects(list<Object*> *pakObjects);
		Object* GetObject(const char* acName);
		Object*	GetObjectByNetWorkID(int iNetID);
		int	GetNumOfObjects() { return m_akObjects.size(); }
		int	GetActivePropertys() {return m_iNrOfActivePropertys;};

		// NetWork
		void UpdateState(NetPacket* pkNetPacket);	//Updates objects.
		void PackToClients();						//Packs and Sends to ALL clients.

		// Debug / Help Functions		
		void DisplayTree();

		void SendMsg();
		void RouteMessage(GameMessage& Msg);

		char* GetUpdateStatusName(int eStatus);
		char* GetObjectTypeName(int eType);

		char* GetPropertyTypeName(int iType);
		char* GetPropertySideName(int iSide);

};

//		void Update();								//update all objects in manager
//		void Update(int iType);						//update all objects of specified type


#endif




