#ifndef _ZFOBJECTMANGER_H_
#define _ZFOBJECTMANGER_H_

#include "vector"
#include "zfobject.h"
#include "basic_x.h"

using namespace std;

class ZFObject;

struct NameObject
{
	string		m_strName;			// Name of object.
	ZFObject*	pkObject;			// Ptr to object.
	int			m_iNumOfRequests;	// Num times name was checked.
};

class BASIC_API ZFObjectManger
{
private:
	vector<NameObject>		kObjectNames;	// List of all object names/ptrs.

public:
	ZFObjectManger();
	~ZFObjectManger();

	void Register(ZFObject* pkObject, char* acName, ZFObject* pkParent);
	void UnRegister(ZFObject* pkObject);
	ZFObject* GetObjectPtr(char* acName);

	void Link(ZFObject* pkParent, ZFObject* pkObject);
	void UnLink(ZFObject* pkObject);

	void PrintObjects(void);
	void PrintObjectsHer(void);

};

extern BASIC_API ZFObjectManger g_ZFObjSys;

#endif



