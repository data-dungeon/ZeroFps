#ifndef _BASIC_RESOURCE_H_
#define _BASIC_RESOURCE_H_

#include "engine_x.h"
#include <string>

using namespace std;

class ENGINE_API ZFResource
{
private:
	int Hora;

public:
	ZFResource();
	virtual ~ZFResource();

	void Clear();	
	virtual bool Create(string strName) = 0;
	void Destroy();

	int GetSize() ;

	friend class ZFResourceDB;
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


#endif

