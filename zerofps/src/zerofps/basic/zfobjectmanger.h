#ifndef _ZFOBJECTMANGER_H_
#define _ZFOBJECTMANGER_H_

#include "basic_x.h"
#include "os.h"

#include <vector>
#include "zfobject.h"

using namespace std;

class ZFObject;

struct NameObject
{
	string		m_strName;			// Name of object.
	ZFObject*	pkObject;			// Ptr to object.
	int			m_iNumOfRequests;	// Num times name was checked.
};

enum ZFCmdDataType 
{
	CSYS_NONE = 0,			// None valid data.
	CSYS_FUNCTION,			// Function ptr.
	CSYS_INT,				// int.
	CSYS_FLOAT,				// float.
	CSYS_BOOL,				// bool.
	CSYS_STRING,			// /0 terminated string.
};
// Contains data about a stored command.
class BASIC_API ZFCmdData
{
public:
//	ZFCmdData();
//	~ZFCmdData();
	
//	ZFCmdData(const ZFCmdData& pack);
//	ZFCmdData&  operator=(const ZFCmdData& pack);

	string			m_strName;				// Name for this data/commmand.
	ZFCmdDataType	m_eType;				// Type of data.
	int				m_iFlags;				// flags.

	void*			m_vValue;				// Pekare till data.
	ZFObject*		m_pkObject;				// Ptr to zfps object that owns this data (if any).
	int				m_iCmdID;				// ID.
};



class BASIC_API ZFObjectManger
{
private:
	vector<NameObject>		kObjectNames;		// List of all object names/ptrs.
	vector<ZFCmdData>		m_kCmdDataList;		// List of all cmd functions/variables.

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


// Cmd / Functions.
	ZFCmdData* FindArea(const char* szName);
	bool Register_Cmd(char* szName, int iCmdID, ZFObject* kObject);
	bool UnRegister_Cmd(ZFObject* kObject);
	bool RunCommand(const char* szCmdArg);

};

extern BASIC_API ZFObjectManger g_ZFObjSys;

#endif



