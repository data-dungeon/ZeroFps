#ifndef _BASIC_RESOURCE_H_
#define _BASIC_RESOURCE_H_

#include <string>
#include "basic_x.h"

class ZFResourceDB;

using namespace std;

/**	\brief	Base class for all resources.

A resource is data/code that are shared and used in many places of the code. 
It's is unloaded when no one is using it anymore. A resource need to be based
on the ZFResource class and there are some rules that it must follow. Each type
of resource need to be registred in the ZFResourceDB.
*/
class BASIC_API ZFResource
{
private:
	int		m_iSizeInBytes;						// The size in bytes that this res takes. Does not include sub res.

public:
	int	m_iTypeIndex;								// Index of this resouce type in the resDB

	ZFResource();
	virtual ~ZFResource();

	/**
		Loads the Res. Returns false if loads fails.
		*/
	virtual bool Create(string strName) = 0;	

	/**
		Calc the num of bytes this res takes. This should not include the size of other Res that are used.
		*/
	virtual int  CalculateSize() = 0;			

	/**
		Returns the size in bytes of this res.
		*/
	int GetSize();									

	friend class ZFResourceDB;
};

/**	\brief	A Handle to a Resource.

*/
class BASIC_API ZFResourceHandle
{
private:
	ZFResourceDB*	m_pkResDB;

	int		m_iHandleID;
	string	m_strName;
	int		m_iID;
	
	ZFResource*	m_pkResource;

public:
	ZFResourceHandle();
	ZFResourceHandle(const ZFResourceHandle& kOther);
	~ZFResourceHandle();
	ZFResourceHandle& operator=(const ZFResourceHandle& kOther);

	bool SetRes(const string& strName);
	string GetRes() 							{return m_strName;	}
	void FreeRes();
	bool IsValid()								{return m_iID != -1;	}

	ZFResource*	GetResourcePtr() const	{return m_pkResource;}

	friend class ZFResourceDB;
};

#endif

