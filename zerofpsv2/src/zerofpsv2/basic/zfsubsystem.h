#ifndef _BASIC_ZFSubSystem_H_XXXX
#define _BASIC_ZFSubSystem_H_XXXX

#include <string>
#include <vector>
#include "basic_x.h"
#include "os.h"

using namespace std;

#define NOOBJECT_INDEX	-1

class ZFSubSystem;
class ZFSystem;

/**	\brief	Type of Command

	Describes the type of data stored.
*/
enum ZFCmdDataType 
{
	CSYS_NONE = 0,					///< None valid data.
	CSYS_FUNCTION,					///< Function ptr.
	CSYS_INT,						///< int.
	CSYS_FLOAT,						///< float.
	CSYS_DOUBLE,					///< float.
	CSYS_LONG,						///< float.
	CSYS_BOOL,						///< bool.
	CSYS_STRING,					///< /0 terminated string.
	CSYS_BOOLVECTOR,				///< vector<bool>
};

/**	\brief	The src of a command.

	The src from were the commands comes. 
*/
enum ZFCmdSource
{
	CSYS_SRC_CMDLINE  = 1,				///< Cmd is from the program cmd line.
	CSYS_SRC_INITFILE = 2,				///< Cmd is from the ini files that is run at startup
	CSYS_SRC_CONSOLE  = 4,				///< Cmd is from the console.
	CSYS_SRC_SUBSYS   = 8,				///< Cmd was sent from a subsystem.
	CSYS_SRC_UNKNOWN  = 16,				///< Unknown Src. Will be ignored and written to log.
};

/* Flags changes settings for variables/commands.
		SRC* flags are used to select from what src's a variable/command can be changed.
*/
#define	CSYS_FLAG_SRC_CMDLINE	1
#define	CSYS_FLAG_SRC_INITFILE	2
#define	CSYS_FLAG_SRC_CONSOLE	4
#define	CSYS_FLAG_SRC_SUBSYS		8
#define	CSYS_FLAG_SRC_UNKNOWN	16
#define	CSYS_FLAG_SRC_ALL			(CSYS_FLAG_SRC_CMDLINE | CSYS_FLAG_SRC_INITFILE | CSYS_FLAG_SRC_CONSOLE | CSYS_FLAG_SRC_SUBSYS | CSYS_FLAG_SRC_UNKNOWN)

class BASIC_API CmdArgument
{
public:
	ZFCmdSource			m_eSource;
	string				m_strFullCommand;
	vector<string>		m_kSplitCommand;

	void Set(const char* szCmdArgs);
};

/**	\brief	Base class for all engine sub systems like graphic, audio, input.
		\ingroup Basic

This class is the base for all engine subsystems. A engine subsystem is a single object 
in the engine that is used in many other places. All Subsystems can be accessed by using
the ZFSystem. A subsystem can make its variables changeable from the console and
other places and also respond to commands sent from console, cmd line and init files.

To create a subsystem one need to create the following functions:

Constructor:	Init variables, register variables and commands. Do NOT acces any other
					subsystem.<BR>
StartUp:			Get Ptr's to other subsystems we will use. Startup ourself. Only use subsystems
					that started before ourself.<BR>
ShutDown:		Sutdown ourself.<BR>
Destructor:		<BR>

RunCommand:		This is used to handles the commands that we have registred. Evey commands
					is registred by a name and is sent to us as a Int ID that we can specify when
					we registred it.

To get access to the other subsystem one should use GetSystem() to get a ptr to 
the ZeroFps System and request a ptr to the other subsystem.
*/
class BASIC_API ZFSubSystem
{
private:
	string					m_strZFpsName;					///<	Name of this objekt.
		
protected:
	ZFSubSystem(char *szName);		
	ZFSystem*				m_pkSystem;						///<  Ptr to the System.

public:
	
	ZFSystem&	GetSystem();

	bool Register_Cmd(char* szName, int iCmdID, int iFlags = CSYS_FLAG_SRC_ALL, char* szHelp = NULL, int iNumOfArg = 0);		
	bool RegisterVariable(const char* szName, void* pvAddress, ZFCmdDataType eType, int iFlags = CSYS_FLAG_SRC_ALL);		
 	void Logf(const char* szName, const char* szMessageFmt,...);

	virtual ~ZFSubSystem();

	friend class ZFSystem;

	
	/**	\brief	Runs Cmd Sent to this SubSystem from ZFSystem.

		This function is called when a command is sent to the subsystem. cmdid is set to
		the id used when the command was reg. kCommand contains the whole text command.
	*/
	virtual void RunCommand(int cmdid, const CmdArgument* kCommand) {};

	/**	\brief	Starts up this SubSystem.
	*/
	virtual bool StartUp()  = 0;

	/**	\brief	Shut Down this SubSystem.
	*/
	virtual bool ShutDown() = 0;

	/**	\brief	Check if SubSystem is Ok.
	*/
	virtual bool IsValid()  = 0;
};

#endif






