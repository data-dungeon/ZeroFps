#ifndef _ENGINE_ZFRESDB_H_
#define _ENGINE_ZFRESDB_H_

#include "../basic/basic.pkg"
#include "zfresource.h"
#include "engine_x.h"

#include <vector>

using namespace std;


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

class ZFResourceHandle
{
private:
	int		m_iHandleID;
	string	m_strName;
	int		m_iID;

public:
	ZFResourceHandle();
	~ZFResourceHandle();

	bool SetRes(string strName);
	void FreeRes();
	bool IsValid();

	ZFResource*	GetResourcePtr();

	friend class ZFResourceDB;


};



class ENGINE_API ZFResourceDB : public ZFObject {
	private:
		int						m_iNextID;

		vector<ZFResourceInfo*>	m_kResources;

	public:
		ZFResourceDB();
		~ZFResourceDB();


		ZFResourceInfo*	GetResourceData(string strResName);

		int	 FindResource(string strResName);
		bool IsResourceLoaded(string strResName);		
		
		void GetResource(ZFResourceHandle& kResHandle, string strResName);
		//ZFResourceHandle GetResource(string strResName);
		void FreeResource(ZFResourceHandle& kResHandle);

		void Refresh();

		ZFResource* GetResourcePtr(ZFResourceHandle& kResHandle);
};

#endif
