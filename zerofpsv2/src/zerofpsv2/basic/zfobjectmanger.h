#ifndef _BASIC_ZFOBJECTMANGER_H_
#define _BASIC_ZFOBJECTMANGER_H_

#include "basic_x.h"
#include "os.h"

#include <vector>
#include "zfobject.h"
#include "cstdio"

using namespace std;

class ZFSubSystem;
class BasicConsole;

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
	string			m_strName;				// Name of object.
	ZFSubSystem*	pkObject;				// Ptr to object.
	int				m_iNumOfRequests;		// Num times name was checked.
	int				m_bStarted;				// True if this subsystem has started ok.
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
	ZFSubSystem*	m_pkObject;				// Ptr to zfps object that owns this data (if any).
	int				m_iCmdID;				// ID.

	int				m_iMinNumOfArgs;		// Minimum num of arguments this func need.
	string			m_strHelpText;			// Text printed if some args are missing.
};


#define DECLARE_SINGLETON( TClass)	\
	static TClass* pkInstance;		\
	static TClass* GetInstance();	\

/// 
/**	\brief	Tracks all Engine Sub Systems objects.

*/
class BASIC_API ZFSystem	/*	ZFSystem	*/
{
private:
	vector<ZFCmdData>		m_kCmdDataList;		///< List of all cmd functions/variables.

	vector<ZFLogFile>		m_kLogFiles;
	FILE*						m_pkLogFile;

protected:
	vector<string>			AppArguments;		
	BasicConsole*			m_pkConsole;

public:
	public:
	vector<NameObject>	kObjectNames;		///< List of all object names/ptrs.

	//DECLARE_SINGLETON(ZFSystem);

	void HandleArgs(int iNrOfArgs, char** paArgs);


	static ZFSystem* pkInstance;
	static ZFSystem* GetInstance();

	ZFSystem();
	~ZFSystem();

	void Register(ZFSubSystem* pkObject, char* acName, ZFSubSystem* pkParent);	///< Register a Object.
	void UnRegister(ZFSubSystem* pkObject);									///< UnRegister a objects.
	ZFSubSystem* GetObjectPtr(char* acName);									///< Get pointer to object by name.

	void Link(ZFSubSystem* pkParent, ZFSubSystem* pkObject);						///< Links a object as a child to another.
	void UnLink(ZFSubSystem* pkObject);										///< Unlinks a object from another.

	void PrintObjects(void);
	void PrintObjectsHer(void);
	void LogVariables(void);

// Cmd / Functions.
	ZFCmdData* FindArea(const char* szName);
	bool Register_Cmd(char* szName, int iCmdID, ZFSubSystem* kObject, int iFlags, char* szHelp = NULL, int iNumOfArg = 0);	///< Register a Cmd and object that will handle it.
	bool UnRegister_Cmd(ZFSubSystem* kObject);									///< UnRegister all cmd's bound to a object.
	///< Run a cmd by passing it along to the correct object
	bool RunCommand(const char* szCmdArg, ZFCmdSource iCmdSource);				

// Variables
	bool RegisterVariable(const char* szName, void* pvAddress, ZFCmdDataType eType, ZFSubSystem* kObject, int iFlags);
	bool SetVariable(const char* szName, const char* szValue);

	void SetValue(ZFCmdData* pkArea, const char* szValue);
	void SetString(ZFCmdData* pkArea, const char* szValue);

	void PrintVariables();
	void PrintCommands();
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

	void Printf(const char* szMessageFmt,...);			// Print to console if created.


	bool StartUp();
	bool ShutDown();
	bool IsValid();

	// Config Files
	void Config_Save(string strFileName);
	void Config_Load(string strFileName);
};

extern BASIC_API ZFSystem g_ZFObjSys;

#endif



