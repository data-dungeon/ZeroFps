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
		enum FuncId_e
		{
			FID_LOGOHTREE,
			FID_LOGACTIVEPROPERTYS,
			FID_SENDMESSAGE,
		};

		struct Property_Less : public binary_function<Property*, Property*, bool> {
			bool operator()(Property* x, Property* y) { return *x < *y; };
		} Less_Property;
	
		ZeroFps*	m_pkZeroFps;

		Object*	m_pkWorldObject;											///< Top level object.
		
		// Object ArcheTypes
		list<ObjectArcheType*>	m_akArcheTypes;						///< List of all object Archetypes.
		ObjectArcheType*	GetArcheType(string strName);				///< Get ptr to AT. NULL if not found.
		void AddArchPropertys(Object* pkObj, string strName);

		list<Object*>		m_akObjects;									///< List of all objects.
		vector<Object*>	m_akDeleteList;								///< List of objects that will be destroyed at end of frame.
		vector<int>			m_aiNetDeleteList;

		vector<Property*>	m_akPropertys;									///< List of Active Propertys.	
		int					m_iNrOfActivePropertys;						///> Size of akProperty list.

		list<ObjectDescriptor*> m_akTemplates;							///< List of templates.


		int	iNextObjectID;													///< Next free object ID.
		bool	m_bUpdate;														///< Disable all updates except RENDER.
	
		void RunCommand(int cmdid, const CmdArgument* kCommand);

		void GetPropertys(int iType,int iSide);						///< Fill propery list.

		void TESTVIM_LoadArcheTypes(char* szFileName);

	public:
		int	m_iForceNetUpdate;					
		float	m_fEndTimeForceNet;

		int	m_iUpdateFlags;												///< Flags of active update.
		int	m_iRole;															///< I'm i a server or a client or both.

		int	m_iTotalNetObjectData;
		int	m_iNumOfNetObjects;

		ObjectManager();
		~ObjectManager();
	

		// Add/Remove Objects
		void Add(Object* pkNewObject);									///< Add object to the manager
		void Delete(Object* pkNewObject);								///< Adds an object to delete qeue
		void Remove(Object* pkObject);									///< Dont use this..use Delete instead
		void Clear();															///< Delete all objects.

		// Updates
		void Update(int iType,int iSide,bool bSort);					///< Run update on selected propertys.
		void UpdateDelete();													///< Deletes objects in delete qeue	
		void UpdateGameMessages(void);									///< Update game messages.
		void SetUpdate(bool bUpdate) { m_bUpdate=bUpdate; };		

			// Create 
		Object* CreateObject();												///< Create a empty object.
		Object* CreateObject(const char* acName);						///< Create object from template.
		Object* CreateObject(ObjectDescriptor* pkObjDesc);			///< Create object from ObjectDescriptor
		Object* CreateObjectByNetWorkID(int iNetID);					///< Create object with selected NetworkID
		Object* CreateObjectByArchType(const char* acName);		///< Create object from archtype
//		Object* CloneObject(int iNetID);									

		// Template
		void AddTemplate(ObjectDescriptor* pkNewTemplate);
		int GetNrOfTemplates();
		void GetTemplateList(vector<string>* paList);
		bool MakeTemplate(const char* acName,Object* pkObject, bool bForce = false);
		void ClearTemplates();
		ObjectDescriptor* GetTemplate(const char* acName);
		bool LoadTemplate(const char* acFile);
		bool SaveTemplate(const char* acName,const char* acFile);

		// Arch types
		bool IsA(Object* pkObj, string strStringType);

		// Load/Save Objects
		bool SaveAllObjects(const char* acFile);
		bool LoadAllObjects(const char* acFile);

		// Gets
		Object* GetWorldObject()	{	return m_pkWorldObject;				};
		int	GetNumOfObjects()		{	return m_akObjects.size();			}
		int	GetActivePropertys() {	return m_iNrOfActivePropertys;	};
		void GetAllObjects(vector<Object*> *pakObjects);
		Object* GetObject(const char* acName);							///< Get a ptr to object by name
		Object*	GetObjectByNetWorkID(int iNetID);					///< Get a ptr to object by networkID
		void	GetArchObjects(vector<string>* pkFiles, string strParentName);

		// NetWork
		void UpdateState(NetPacket* pkNetPacket);						//Updates objects.
		void UpdateDeleteList(NetPacket* pkNetPacket);
		void PackToClients();												//Packs and Sends to ALL clients.

		// Debug / Help Functions		
		void DisplayTree();
		void DumpActiverPropertysToLog(char* szMsg);					///< Log all propertys in m_akPropertys.

		// Message System.
		void SendMsg();
		void RouteMessage(GameMessage& Msg);

		// Get Strings.
		char* GetUpdateStatusName(int eStatus);				
		char* GetObjectTypeName(int eType);
		char* GetPropertyTypeName(int iType);
		char* GetPropertySideName(int iSide);

		Object* ObjectManager::CloneObject(int iNetID);

		bool TestLine(vector<Object*>* pkObList,Vector3 kPos,Vector3 kVec);

		void OwnerShip_Take(Object* pkObj);		
		void OwnerShip_Give(Object* pkObj);
		
		friend class Object;
};

#endif




