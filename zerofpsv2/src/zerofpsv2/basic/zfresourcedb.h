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

*/
class BASIC_API ZFResourceDB : public ZFSubSystem {
	private:
		bool	m_bInstantExpire;		

		enum FuncId_e
		{
			FID_LISTRES,
			FID_LISTTYPES,
		};

		int						m_iNextID;

		list<ZFResourceInfo*>		m_kResources;
		vector<ResourceCreateLink>	m_kResourceFactory;

		ResourceCreateLink*	FindResourceTypeFromFullName(string strResName);

		void RunCommand(int cmdid, const CmdArgument* kCommand);

	public:
		ZFResourceDB();
		~ZFResourceDB();

		ResourceCreateLink*	FindResourceType(string strName);
		ZFResource*	CreateResource(string strName);
		void RegisterResource(string strName, ZFResource* (*Create)());

		ZFResourceInfo*	GetResourceData(string strResName);

		ZFResourceInfo* FindResource(string strResName);
		bool IsResourceLoaded(string strResName);		
		
		void GetResource(ZFResourceHandle& kResHandle, string strResName);
		//ZFResourceHandle GetResource(string strResName);
		void FreeResource(ZFResourceHandle& kResHandle);

		bool Refresh();

		ZFResource* GetResourcePtr(ZFResourceHandle& kResHandle);

		bool StartUp();
		bool ShutDown();
		bool IsValid();

		int	GetResSizeInBytes();
};

#endif
