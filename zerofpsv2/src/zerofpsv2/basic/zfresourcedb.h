#ifndef _ENGINE_ZFRESDB_H_
#define _ENGINE_ZFRESDB_H_

#include "zfresource.h"
#include "basic_x.h"
#include "zfsubsystem.h"
#include <list>

using namespace std;

class ZeroFps;

/// Links a resource type with a creation function
class ResourceCreateLink
{
public:
	string		m_strName;			// Resource type name.
	ZFResource*	(*Create)();		// Create function for resource type.
	int			m_iActive;			// Num of res of this types active at the moment.
	int			m_iIndex;			// Index of resource link.
};

/**	\brief	Data that the Resource manger have about a resource.

*/
class ZFResourceInfo
{
private:
	string	m_strName;				// Name of the resource.
	int		m_iID;					
	int		m_iNumOfUsers;			// Num of handles that use resource.
	float		m_fExpireTimer;		// Time when res expires (if no users).
	
	ZFResource*		m_pkResource;	// ptr to resource.

public:

	ZFResourceInfo();
	~ZFResourceInfo();

	friend class ZFResourceDB;

	ZFResourceInfo& operator=(const ZFResourceInfo &kOther) {
		m_strName		= kOther.m_strName;
		m_iID			= kOther.m_iID;
		m_iNumOfUsers	= kOther.m_iNumOfUsers;
		m_fExpireTimer	= kOther.m_fExpireTimer;
		m_pkResource	= kOther.m_pkResource;
		return *this;
		}	
};

/**	\brief	Resource manger.
		\ingroup Basic

Handles all resources in the game. All Res types needs to be registred before they can be used.
To use a resource one must first use a ZFResourceHandle to load the Res and the get the Res ptr 
from it.   
*/
class BASIC_API ZFResourceDB : public ZFSubSystem {
	private:
		enum FuncId_e
		{
			FID_LISTRES,
			FID_LISTTYPES,
		};

		bool								m_bInstantExpire;		
		int								m_iNextID;
		list<ZFResourceInfo*>		m_kResources;
		vector<ResourceCreateLink>	m_kResourceFactory;

		ResourceCreateLink*	FindResourceTypeFromFullName(string strResName);

		void RunCommand(int cmdid, const CmdArgument* kCommand);

		// Creating Resources.
		ZFResource*	CreateResource(string strName);
		ResourceCreateLink*	FindResourceType(string strName);
		ZFResourceInfo*	GetResourceData(string strResName);
		ZFResourceInfo* FindResource(string strResName);

	public:
		ZFResourceDB();
		~ZFResourceDB();

		bool StartUp();
		bool ShutDown();
		bool IsValid();

		// Resource Types
		void RegisterResource(string strName, ZFResource* (*Create)());

		// Accessing / Using.
		bool IsResourceLoaded(string strResName);		
		ZFResource* GetResourcePtr(ZFResourceHandle& kResHandle);

		
		void GetResource(ZFResourceHandle& kResHandle, string strResName);
		void FreeResource(ZFResourceHandle& kResHandle);

		bool Refresh();

		// Stats
		int	GetResSizeInBytes();
};

#endif
