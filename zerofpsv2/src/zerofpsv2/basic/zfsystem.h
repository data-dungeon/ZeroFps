#ifndef _BASIC_ZFOBJECTMANGER_H_
#define _BASIC_ZFOBJECTMANGER_H_

#include "basic_x.h"
//#include "os.h"

#include <vector>
#include "zfsubsystem.h"
#include "cstdio"
#include "concommand.h"
//#include <hash_map> 
#include <map> 


using namespace std;

class ZFSubSystem;
class BasicConsole;
class ZSSCmdSystem;

#pragma warning(disable: 4267) // disable warning C4267: possible loss of data, okay to ignore

/// A Log File.
class ZFLogFile
{
public:
	string	m_strName;
	string	m_strFileName;

	FILE*		m_pkFilePointer;
};

/// Links Names to ZFObjects
struct NameObject
{
	string			m_strName;				// Name of object.
	ZFSubSystem*	pkObject;				// Ptr to object.
	int				m_iNumOfRequests;		// Num times name was checked.
	bool				m_bStarted;				// True if this subsystem has started ok.
	int				m_iRegisterOrder;		// place this system was registered
};

/// Contains data about a stored command.
class BASIC_API ZFCmdData
{
public:
	string			m_strName;				// Name for this data/commmand.
	ZFCmdDataType	m_eType;					// Type of data.
	int				m_iFlags;				// flags.

	void*				m_vValue;				// Pekare till data.
	ZFSubSystem*	m_pkObject;				// Ptr to zfps object that owns this data (if any).
	int				m_iCmdID;				// ID.

	int				m_iMinNumOfArgs;		// Minimum num of arguments this func need.
	string			m_strHelpText;			// Text printed if some args are missing.
};


class BASIC_API ProfileTimer
{
	public:
		int			m_iTotalTime;		
		int			m_iStartime;
		
		ProfileTimer()
		{
			m_iTotalTime = 0;
			m_iStartime	=	-1;
		}
};

class BASIC_API TimerInfo
{
	public:
		string	m_strName;
		int		m_iTime;
};

	struct BASIC_API eqstr
	{
		bool operator()(const char* s1, const char* s2) const
		{
			return strcmp(s1, s2) == 0;
		}
	};

#define DECLARE_SINGLETON( TClass)	\
	static TClass* pkInstance;		\
	static TClass* GetInstance();	\

/**	\brief	Tracks all Engine Sub Systems objects.
		\ingroup Basic

ZFSystem is the master objekt that keep track of all other subsystems in ZeroFPS. 
There should only be one of this in the program. 

- Have a list of all SubSystems.<BR>
- Handles cmd functions and variables.<BR>
- Handles Log files.<BR>
- Handles ini files.<BR>
*/
class BASIC_API ZFSystem	/*	ZFSystem	*/
{
private:
	ZSSCmdSystem*				m_pkCmdSystem;

	//want to log
	ConVar						m_kLogEnabled;
	
	//profileing
	ConVar							m_kProfileEnabled;	
	map<string,ProfileTimer>	m_kTimers;
	int								m_iTotalTime;
	
	vector<ConCommand*>	m_kConCommands; 

	vector<ZFCmdData>		m_kCmdDataList;		///< List of all cmd functions/variables.
	vector<ZFLogFile>		m_kLogFiles;			///< List of all Log files.
	FILE*						m_pkLogFile;			///< Master Log File (zerofps.txt).
	string					m_strPreLogName;

	void PrintVariables();
	void PrintCommands();
	void PrintObjects(void);
	void LogVariables(void);

	bool SetVariable(const char* szName, const char* szValue);
	void SetValue(ZFCmdData* pkArea, const char* szValue);
	void SetString(ZFCmdData* pkArea, const char* szValue);
	void* GetVar(ZFCmdData* pkArea);
	string GetVarValue(ZFCmdData* pkArea);
	
protected:
	vector<string>			AppArguments;			///< Arguments sent to app at startup.
	vector<string>			m_kRawArguments;
	BasicConsole*			m_pkConsole;			///< Ptr to ZeroFps Console.

public:
	//vector<NameObject>		kObjectNames;			///< List of all object names/ptrs.
	map<string,NameObject>	m_kObjectNames;			///< List of all object names/ptrs.

	void HandleArgs(int iNrOfArgs, char** paArgs);

	static ZFSystem* pkInstance;
	static ZFSystem* GetInstance() { return ZFSystem::pkInstance; }

	ZFSystem();
	~ZFSystem();

	void Register(ZFSubSystem* pkObject, char* acName /*, ZFSubSystem* pkParent*/);		// Register a SubSystem.
	void UnRegister(ZFSubSystem* pkObject);													// UnRegister a objects.
	ZFSubSystem* GetObjectPtr(const char* acName,bool bWarning = true);													// Get pointer to SubSystem by name.
	bool StartUp();
	bool ShutDown();
	bool IsValid();

	//profiling
	void DontUseStartProfileTimer(const char* czName);
	void DontUseStopProfileTimer(const char* czName);
	void ClearProfileTimers();
	void GetProfileTimers(vector<TimerInfo >* pkTimers);
	int GetTotalTime();					/*{	return SDL_GetTicks() - m_iTotalTime;	};*/	
	bool  GetProfileEnabled()			{	return m_kProfileEnabled.GetBool();		};
	
	//program arguments
	int GetNumberOfArguments()			{	return m_kRawArguments.size();			};
	string GetArgument(int iArg)		{	return m_kRawArguments[iArg];				};
	
	//message system
	bool SendSystemMessage(const string& strSystem,const string& strType,int iNrOfParam,const void** pkParam);
	
	// Cmd / Functions.
	ZFCmdData* FindArea(const char* szName);	// GALLA
	/// Register a Cmd and object that will handle it.
	bool Register_Cmd(char* szName, int iCmdID, ZFSubSystem* kObject, int iFlags, char* szHelp = NULL, int iNumOfArg = 0);	
	bool UnRegister_Cmd(ZFSubSystem* kObject);												///< UnRegister all cmd's bound to a object.
	bool RunCommand(const char* szCmdArg, ZFCmdSource iCmdSource);						///< Run a cmd by passing it along to the correct object

	bool AutoComplete(const char* szCmdArg, vector<string>* pkCmds, int iMaxFinds);

	ConCommand* FindConCommand(const char* szName);	
	ConVar* GetConVar(const char* szName);	
	void RegisterConCommand(ConCommand* pkConCommand);

// Variables
	bool RegisterVariable(const char* szName, void* pvAddress, ZFCmdDataType eType, ZFSubSystem* kObject, int iFlags);

// Log Files
	bool Log_Create(const char* szName);
	void Log_Destory(const char* szName);
	ZFLogFile*	Log_Find(const char* szName);
	void Log_DestroyAll();

	void Log(const char* szMessage);
	void Log(const char* szName, const char* szMessage);
	void Logf(const char* szName, const char* szMessageFmt,...);

	// Config Files
	void Config_Save(string strFileName);
	void Config_Load(string strFileName);

	void Printf(const char* szMessageFmt,...);			// Print to console if created.

	void SetPreLogName(string strPreLogName);
	string GetPreLogName()	{ return m_strPreLogName; }

	friend class ZSSCmdSystem;
};

extern BASIC_API ZFSystem g_ZFObjSys;

#endif



