// izfresourcedb.h: interface for the IZFResourceDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IZFRESOURCEDB_H__B1096AB8_8750_4BC2_8F7B_B2CA5E718C1D__INCLUDED_)
#define AFX_IZFRESOURCEDB_H__B1096AB8_8750_4BC2_8F7B_B2CA5E718C1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../basic/zfobject.h"
#include "engine_x.h"
#include "zfresource.h"

class ResourceCreateLink;
class ZFResourceInfo;

class ENGINE_API I_ZFResourceDB : public ZFObject 
{
public:
	I_ZFResourceDB(char* szName);
	virtual ~I_ZFResourceDB();

	virtual ResourceCreateLink*	FindResourceType(string strName) = 0;
	virtual ZFResource*	CreateResource(string strName) = 0;
	virtual void RegisterResource(string strName, ZFResource* (*Create)()) = 0;

	virtual ZFResourceInfo*	GetResourceData(string strResName) = 0;

	//virtual ZFResourceInfo* FindResource(string strResName) = 0;
	virtual bool IsResourceLoaded(string strResName) = 0;		
	
	virtual void GetResource(ZFResourceHandle& kResHandle, string strResName) = 0;
	virtual void FreeResource(ZFResourceHandle& kResHandle) = 0;

	virtual void Refresh() = 0;

	virtual ZFResource* GetResourcePtr(ZFResourceHandle& kResHandle) = 0;

};

#endif // !defined(AFX_IZFRESOURCEDB_H__B1096AB8_8750_4BC2_8F7B_B2CA5E718C1D__INCLUDED_)
