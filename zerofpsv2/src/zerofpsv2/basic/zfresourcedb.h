#ifndef _ENGINE_ZFRESDB_H_
#define _ENGINE_ZFRESDB_H_

#include "zfresource.h"
#include "basic_x.h"
#include "zfobject.h"

#include <list>

using namespace std;

/// Links a resource type with a creation function
class ResourceCreateLink
{
public:
	string		m_strName;			// Resource type name.
	ZFResource*	(*Create)();		// Create function for resource type.
};


class ZFResourceInfo
{
private:
	string	m_strName;
	int		m_iID;
	int		m_iNumOfUsers;
	float	m_fExpireTimer;
	
	ZFResource*		m_pkResource;

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




class BASIC_API ZFResourceDB : public ZFSubSystem {
	private:
		enum FuncId_e
		{
			FID_LISTRES,
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

		void Refresh();

		ZFResource* GetResourcePtr(ZFResourceHandle& kResHandle);

		bool StartUp();
		bool ShutDown();
		bool IsValid();

};

#endif
