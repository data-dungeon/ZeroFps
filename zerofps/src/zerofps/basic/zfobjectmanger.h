#ifndef _BASIC_ZFOBJECTMANGER_H_
#define _BASIC_ZFOBJECTMANGER_H_

#include "basic_x.h"
#include "os.h"

#include <vector>
#include "zfobject.h"
#include "cstdio"

using namespace std;

class ZFObject;

class ZFLogFile
{
public:
	string	m_strName;
	string	m_strFileName;

	FILE*	m_pkFilePointer;
};

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
	CSYS_DOUBLE,					// float.
	CSYS_LONG,						// float.
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
	ZFCmdDataType	m_eType;					// Type of data.
	int				m_iFlags;				// flags.

	void*				m_vValue;				// Pekare till data.
	ZFObject*		m_pkObject;				// Ptr to zfps object that owns this data (if any).
	int				m_iCmdID;				// ID.

	int				m_iMinNumOfArgs;		// Minimum num of arguments this func need.
	string			m_strHelpText;			// Text printed if some args are missing.
};


#define DECLARE_SINGLETON( TClass)	\
	static TClass* pkInstance;		\
	static TClass* GetInstance();	\


/// Tracks all Engine Systems objects.
class BASIC_API ZFObjectManger
{
private:
	vector<NameObject>	kObjectNames;		///< List of all object names/ptrs.
	vector<ZFCmdData>		m_kCmdDataList;		///< List of all cmd functions/variables.

	vector<ZFLogFile>		m_kLogFiles;
	FILE*						m_pkLogFile;

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
	bool Register_Cmd(char* szName, int iCmdID, ZFObject* kObject, char* szHelp = NULL, int iNumOfArg = 0);	///< Register a Cmd and object that will handle it.
	bool UnRegister_Cmd(ZFObject* kObject);									///< UnRegister all cmd's bound to a object.
	bool RunCommand(const char* szCmdArg);									///< Run a cmd by passing it along to the correct object

// Variables
	bool RegisterVariable(const char* szName, void* pvAddress, ZFCmdDataType eType);
	bool SetVariable(const char* szName, const char* szValue);

	void SetValue(ZFCmdData* pkArea, const char* szValue);
	void SetString(ZFCmdData* pkArea, const char* szValue);

	void PrintVariables();
	void* GetVar(ZFCmdData* pkArea);
	string GetVarValue(ZFCmdData* pkArea);

// Log Files
	void Log(const char* szMessage);
	bool Log_Create(const char* szName);
	void Log_Destory(const char* szName);
	ZFLogFile*	Log_Find(const char* szName);
	void Log_DestroyAll();
	void Log(const char* szName, const char* szMessage);
	void Logf(const char* szName, const char* szMessageFmt,...);

	bool StartUp();
	bool ShutDown();
	bool IsValid();

};

extern BASIC_API ZFObjectManger g_ZFObjSys;

#endif



