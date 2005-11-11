#ifndef _BASIC_ZFCONCOMMAND_H_
#define _BASIC_ZFCONCOMMAND_H_

#include <vector>
#include <string>
#include "basic_x.h"
#include "zfio.h"

using namespace std;

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

class BASIC_API ConCommandLine
{
public:
	ZFCmdSource			m_eSource;
	string				m_strFullCommand;
	vector<string>		m_kSplitCommand;

public:
	void Set(const char* szCmdArgs);

	int		argc() const;
	string	argv(const int iIndex) const;
	string	GetLine() const;
};

class ZFSubSystem;

// Console command is the base class for all things used from the console.
class BASIC_API ConCommand
{
protected:
	string			m_strName;
	string			m_strHelp;
	ZFSubSystem*	m_pkSubSystem;			// Subsystem this concommand is part of. Can be NULL.

public:
	ConCommand();
	~ConCommand();

	virtual void Create(string strName);
	virtual bool IsCommand() = 0;

	string	GetName()			{	return m_strName; }
	string	GetHelp()			{	return m_strHelp; }
	string   GetSubSystemName();
	ZFSubSystem* GetSubsystem()	{ return m_pkSubSystem; }

private:
	bool				m_bOwnedBySystem;
};	

// Function that can be called from the console.
typedef void ( *ConFunctionCallback ) ( const ConCommandLine* pkCmd );

class BASIC_API ConFunction : public ConCommand
{
protected:

public:
	ConFunction();
	ConFunction(char* szName, ConFunctionCallback pCallBack);
	ConFunction(char* szName, ConFunctionCallback pCallBack, char* szHelp);
	~ConFunction();

	void Register(ZFSubSystem* pkSubSys, char* szName, ConFunctionCallback pCallBack);
	void Register(ZFSubSystem* pkSubSys, char* szName, ConFunctionCallback pCallBack, char* szHelp);

	void Register(ZFSubSystem* pkSubSys, char* szName, int iCmdID);

	bool IsCommand()	{	return true;	}

	void Run( const ConCommandLine* pkCmd );

private:
	ConFunctionCallback	m_pCallBackFunction;		// Function to call when running command.
	int	m_iCmdID;		
};	

// Variable that are put in zfsystem and can be used from the console.
class BASIC_API ConVar : public ConCommand
{
public:
	ConVar();
	ConVar(char* szName, char* szDefaultValue);
	ConVar(char* szName, char* szDefaultValue, char* szHelp);
	~ConVar();

	void Register(ZFSubSystem* pkSubSys, char* szName, char* szDefaultValue);
	void Register(ZFSubSystem* pkSubSys, char* szName, char* szDefaultValue, char* szHelp);

	bool	GetBool()		const;
	int	GetInt()			const;
	float	GetFloat()		const;
	string GetString()	const;
	vector<bool>	GetBoolVector()	const;

	void SetBool(bool bValue);
	void SetInt(int iValue);
	void SetFloat(float fValue);
	void SetString(string strValue);
	void SetBoolVector(vector<bool> kValue);

	bool IsCommand()	{	return false;	}

private:


	string	m_strValue;
	int		m_iValue;
	float		m_fValue;

	string	m_strDefaultValue;

};

#endif



