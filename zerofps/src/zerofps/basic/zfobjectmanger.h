#ifndef _BASIC_ZFOBJECTMANGER_H_
#define _BASIC_ZFOBJECTMANGER_H_

#include "basic_x.h"
#include "os.h"

#include <vector>
#include "zfobject.h"

using namespace std;

class ZFObject;

/// Links Names to ZFObjects
struct NameObject
{
	string		m_strName;			// Name of object.
	ZFObject*	pkObject;			// Ptr to object.
	int			m_iNumOfRequests;	// Num times name was checked.
};

/// Type of Command
enum ZFCmdDataType 
{
	CSYS_NONE = 0,					// None valid data.
	CSYS_FUNCTION,					// Function ptr.
	CSYS_INT,						// int.
	CSYS_FLOAT,						// float.
	CSYS_BOOL,						// bool.
	CSYS_STRING,					// /0 terminated string.
};

/// Contains data about a stored command.
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


#define DECLARE_SINGLETON( TClass)	\
	static TClass* pkInstance;		\
	static TClass* GetInstance();	\


/// Tracks all Engine Systems objects.
class BASIC_API ZFObjectManger
{
private:
	vector<NameObject>		kObjectNames;		///< List of all object names/ptrs.
	vector<ZFCmdData>		m_kCmdDataList;		///< List of all cmd functions/variables.

public:
	public:

	//DECLARE_SINGLETON(ZFObjectManger);

	static ZFObjectManger* pkInstance;
	static ZFObjectManger* GetInstance();

	ZFObjectManger();
	~ZFObjectManger();

	void Register(ZFObject* pkObject, char* acName, ZFObject* pkParent);	///< Register a Object.
	void UnRegister(ZFObject* pkObject);									///< UnRegister a objects.
	ZFObject* GetObjectPtr(char* acName);									///< Get pointer to object by name.

	void Link(ZFObject* pkParent, ZFObject* pkObject);						///< Links a object as a child to another.
	void UnLink(ZFObject* pkObject);										///< Unlinks a object from another.

	void PrintObjects(void);
	void PrintObjectsHer(void);

// Cmd / Functions.
	ZFCmdData* FindArea(const char* szName);
	bool Register_Cmd(char* szName, int iCmdID, ZFObject* kObject);			///< Register a Cmd and object that will handle it.
	bool UnRegister_Cmd(ZFObject* kObject);									///< UnRegister all cmd's bound to a object.
	bool RunCommand(const char* szCmdArg);									///< Run a cmd by passing it along to the correct object

};

extern BASIC_API ZFObjectManger g_ZFObjSys;

#endif



