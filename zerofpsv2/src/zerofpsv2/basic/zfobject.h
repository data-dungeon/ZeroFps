#ifndef _BASIC_ZFSubSystem_H_XXXX
#define _BASIC_ZFSubSystem_H_XXXX

#include <string>
#include <vector>
#include "basic_x.h"
#include "os.h"

using namespace std;

#define NOOBJECT_INDEX	-1

class ZFSubSystem;
class ZFObjectManger;

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

class BASIC_API CmdArgument
{
public:
	string				m_strFullCommand;
	vector<string>		m_kSplitCommand;

	void Set(const char* szCmdArgs);
};

/// Base class for all engine sub systems like graphic, audio, input...
class BASIC_API ZFSubSystem
{
private:
	string					m_strZFpsName;					///<	Name of this objekt.
	void DestroyChildren();									///<	Remove and delete children.
		
protected:
	vector<ZFSubSystem*>		m_akChild;					///<	List of all object we own.
	ZFSubSystem*				m_pkParent;					///<	Ptr to object that own us.

	ZFSubSystem(char *szName);		
			

public:
	ZFObjectManger*			m_pkSystem;	
	
	ZFObjectManger&	GetSystem();

	virtual void RunCommand(int cmdid, const CmdArgument* kCommand) {};

	ZFSubSystem*	GetParent() const;					///< Get ptr to object parent.

	int GetNumChildren() const;							///< Get num of children we have.
	int GetChildIndex(ZFSubSystem* pkChild);			///< Get index for child.
	int GetChildIndex(char* szName);						///< Get index for child.
	ZFSubSystem* GetChildPtr(int iIndex);				///< Get ptr to child.
	ZFSubSystem* GetChildPtr(char* szName);			///< Get ptr to child.

	void AddChild(ZFSubSystem* pkObject);				///< Make a object child to this.
	void RemoveChild(ZFSubSystem* pkObject);			///< Remove one of our children.

	void PrintChilds(const char* szParentName);		///< Debug: Prints childs from this object. 
	bool Register_Cmd(char* szName, int iCmdID, char* szHelp = NULL, int iNumOfArg = 0);	///< Register a Cmd for this SubSys.
	bool RegisterVariable(const char* szName, void* pvAddress, ZFCmdDataType eType);			///< Register a var for this SubSys

	void Logf(const char* szName, const char* szMessageFmt,...);

	virtual ~ZFSubSystem();

	friend class ZFObjectManger;

	virtual bool StartUp()  = 0;
	virtual bool ShutDown() = 0;
	virtual bool IsValid()  = 0;
};

#endif






