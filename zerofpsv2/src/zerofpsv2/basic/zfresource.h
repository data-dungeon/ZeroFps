#ifndef _BASIC_RESOURCE_H_
#define _BASIC_RESOURCE_H_

#include "basic_x.h"
#include <string>

using namespace std;

/**	\brief	Base class for all resources.

*/
class BASIC_API ZFResource
{
private:
	int		m_iSizeInBytes;						// The size in bytes that this res takes. Does not include sub res.

public:
	ZFResource();
	virtual ~ZFResource();

	virtual bool Create(string strName) = 0;	// Loads the res. Return false if load fails.
	virtual int  CalculateSize() = 0;			// Calc the num of bytes this res takes.

	int GetSize();										// Returns the size in bytes of this res.

	friend class ZFResourceDB;
	
	int	m_iTypeIndex;							// Index of this resouce type in the resDB
};

/**	\brief	A Handle to a Resource.

*/
class BASIC_API ZFResourceHandle
{
private:
	int		m_iHandleID;
	string	m_strName;
	int		m_iID;

public:
	ZFResourceHandle();
	~ZFResourceHandle();
	ZFResourceHandle& operator=(const ZFResourceHandle& kOther);

	bool SetRes(string strName);
	string GetRes() {return m_strName;};
	void FreeRes();
	bool IsValid();

	ZFResource*	GetResourcePtr();

	friend class ZFResourceDB;
};

#endif

