#include <iostream>
#include <string>
#include <cstdio>
#include <cstdarg>

#include <SDL/SDL.h>

#include "zfsystem.h"
#include "cmdsystem.h"
#include "basicconsole.h"
#include "globals.h"
#include "../basic/zfini.h"
#include "zfassert.h"

using namespace std;

ZFSystem* ZFSystem::pkInstance;

/*
	Print all system variables.
*/
void ZFSystem::PrintVariables()
{
	BasicConsole*		m_pkCon;
	m_pkCon = static_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("ZSSConsole"));


	string strLastSubSysName;
	m_pkCon->Printf("### variable list ###");
	for(unsigned int i=0; i<m_kConCommands.size(); i++) 
	{
		ConCommand *pkConCommand = m_kConCommands[i];
		if(!pkConCommand->IsCommand())
		{
			string strSubSystem = pkConCommand->GetSubSystemName();
			
			//if no subsystem, print it as global
			if(strSubSystem.empty()) strSubSystem = "GLOBAL";
			
			//only print subsystem name if its not the same as the last
			if(strSubSystem != strLastSubSysName)
			{
				m_pkCon->Printf(" ");
				m_pkCon->Printf("%s",strSubSystem.c_str());
			}
				
			//do we want to print any help text?
			if(!pkConCommand->GetHelp().empty())
			{
				m_pkCon->Printf("   %s - %s",
					pkConCommand->GetName().c_str(),
					pkConCommand->GetHelp().c_str());
			}
			else
			{
				m_pkCon->Printf("   %s",pkConCommand->GetName().c_str());						
			}
								
			strLastSubSysName = strSubSystem;
		}
	}
}

void ZFSystem::PrintCommands()
{
	BasicConsole*		m_pkCon;
	m_pkCon = static_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("ZSSConsole"));

	m_pkCon->Printf("### Command list ###");
	
	string strLastSubSysName;
	for(unsigned int i=0; i<m_kConCommands.size(); i++) 
	{
		ConCommand *pkConCommand = m_kConCommands[i];
		if(pkConCommand->IsCommand())
		{		
			string strSubSystem = pkConCommand->GetSubSystemName();
			
			//if no subsystem, print it as global
			if(strSubSystem.empty()) strSubSystem = "GLOBAL";
			
			//only print subsystem name if its not the same as the last
			if(strSubSystem != strLastSubSysName)
			{
				m_pkCon->Printf(" ");
				m_pkCon->Printf("%s",strSubSystem.c_str());
			}
				
			//do we want to print any help text?
			if(!pkConCommand->GetHelp().empty())
			{
				m_pkCon->Printf("   %s - %s",
					pkConCommand->GetName().c_str(),
					pkConCommand->GetHelp().c_str());
			}
			else
			{
				m_pkCon->Printf("   %s",pkConCommand->GetName().c_str());						
			}
								
			strLastSubSysName = strSubSystem;
		}	
	}
}

void ZFSystem::PrintObjects(void)
{
		for(map<string,NameObject>::iterator it = m_kObjectNames.begin();it != m_kObjectNames.end();it++)
		{
			g_Logf(" %s, %d\n",(*it).second.m_strName.c_str(), (*it).second.m_iNumOfRequests );
		}
}

void ZFSystem::LogVariables(void)
{
	for(unsigned int i=0; i < m_kCmdDataList.size();i++) {
		g_Logf(" %s\n", m_kCmdDataList[i].m_strName.c_str());
	}
}

/**	\brief	Runs App Arguments as cmd's into the ZFSystem.

	Takes the arguments of the application and runs them. Should only be called
	once. the arguments is then stored in AppArguments.
*/
void ZFSystem::HandleArgs(int iNrOfArgs, char** paArgs)
{
	string	strFullArg;
	string	strArg;
	bool		bFoundArg = false;

	for(int i = 0; i < iNrOfArgs; i++) 
	{
		strArg = string(paArgs[i]);

		//add to raw argumentlist
		m_kRawArguments.push_back(strArg);
		
		if(strArg.c_str()[0] == '-') 
		{
			bFoundArg = true;
			// Start of new argument.
			if(strFullArg.size())
				AppArguments.push_back(strFullArg);
	
			strFullArg = "";
			strArg.erase(0,1);
		}

		if(bFoundArg)
			strFullArg = strFullArg + " " + strArg;
	}

	if(strFullArg.size())
		AppArguments.push_back(strFullArg);

	//cout<<"Nr of arguments: "<< strFullArg.size() <<endl;
	for(unsigned int ia = 0; ia < AppArguments.size(); ia++) 
	{
		//cout << "Argument[" << ia << "]: "<< AppArguments[ia] << endl;
		RunCommand(AppArguments[ia].c_str(), CSYS_SRC_CMDLINE);
	}
}

ZFSystem::ZFSystem()
{
	ZFSystem::pkInstance = this;

	m_pkLogFile = NULL;
	m_strPreLogName = "Zerofps";

#ifdef _DEBUG
	g_Logf("Starting ZeroFps Object System\n");
#endif

	m_pkCmdSystem = new ZSSCmdSystem;	
	m_iTotalTime = 		0;	
		
	m_kLogEnabled.Register(NULL,		"e_log",			"0");
	m_kProfileEnabled.Register(NULL,	"e_profile",	"0");
}

ZFSystem::~ZFSystem()
{
	delete m_pkCmdSystem;

#ifdef _DEBUG
	g_Logf("Closing ZeroFps Object System\n");
#endif

	// Check that we don't have any objects left.
 	if(m_kObjectNames.size() > 0) {
		g_Logf("WARNING: Some Engine Systems have not been closed down\n");
		PrintObjects();
		}

	// Check that we don't have any objects left.
	if(m_kCmdDataList.size() > 0) {
		g_Logf("WARNING: Some Variables Systems have not unregistred\n");
		LogVariables();
		}

	Log_DestroyAll();
	if(m_pkLogFile)
		fclose(m_pkLogFile);						// Close Log Fil
	m_pkLogFile = NULL;

	m_pkConsole = NULL;
}

/**	\brief	Registers a SubSystem

	Registers a SubSystem to be accsessible t the ZFSystem. Set the name and parent
	subsystem (NULL) if no parent. Name must be uniqe or reg will fail.
*/
void ZFSystem::Register(ZFSubSystem* pkObject, char* acName)
{
#ifdef _DEBUG
	g_Logf("Register '%s'", acName);
#endif

	// Make sure that there is no other object with the same name.
// 	ZFSubSystem* pkCheck = GetObjectPtr(acName,false);
// 	if(pkCheck) 
	if(GetObjectPtr(acName,false))
	{
		g_Logf("Fail\n");
		g_Logf("Warning: There was already a object with name '%s'\n", acName);
		return;
	}

	static int iRegisterOrder = 0;
	
	NameObject kObjName;
	kObjName.m_strName			= string(acName);
	kObjName.pkObject				= pkObject;
	kObjName.m_iNumOfRequests	= 0;
	kObjName.m_bStarted			= false;
	kObjName.m_iRegisterOrder	= iRegisterOrder;
	
	//increse register order
	iRegisterOrder++;
	
	m_kObjectNames.insert(map<const char*,NameObject>::value_type(kObjName.m_strName.c_str(),kObjName));
	
	pkObject->m_strZFpsName		= string(acName);
	pkObject->m_pkSystem = this;

	
#ifdef _DEBUG
	g_Logf("ok\n");
#endif
}

/**	\brief	UnRegister a SubSystem

  Unregister all the names that the subsystem pointed to by pkObject have registred.
*/
void ZFSystem::UnRegister(ZFSubSystem* pkObject)
{
#ifdef _DEBUG
	g_Logf("UnRegister '%s' ", pkObject->m_strZFpsName.c_str());
#endif

	cout<<"unregistering "<<pkObject->m_strZFpsName.c_str()<<endl;

	map<string,NameObject>::iterator itNames;
	for(itNames = m_kObjectNames.begin(); itNames != m_kObjectNames.end(); itNames++)
	{
		if((*itNames).second.pkObject == pkObject) 
		{
// 			itNames = m_kObjectNames.erase(itNames);
			m_kObjectNames.erase(itNames);
			break;
#ifdef _DEBUG
			g_Logf(".");
#endif
		}
	}



	UnRegister_Cmd(pkObject);

#ifdef _DEBUG
	g_Logf("Ok\n");
#endif
}

/**	\brief	Get pointer to SubSystem by name.

	Get a Ptr to the SubSystem with the choosen name. Null if no SubSystem found with this name.
*/
ZFSubSystem* ZFSystem::GetObjectPtr(const char* acName,bool bWarning)
{
// cout<<"requesting:"<<acName<<endl;
	static map<string,NameObject>::iterator it;
	
	it = m_kObjectNames.find(acName);
	
	if(it == m_kObjectNames.end())
	{
		if(bWarning)
		{
			//cout<<"WARNING: Requested system pointer not found:"<<acName<<endl;
		}
		
		//cout << "System Not Found: " << acName << endl;
		//ZFAssert(0, "Requested system pointer not found");
		return NULL;
	}

	return it->second.pkObject;

}

/**	\brief	Startup all SubSystems.
	Loop all subsystems from last created to first and run their startup function. 
	If any subsystems fail and return false.
*/
bool ZFSystem::StartUp()
{
	g_Logf("Start ZeroFps Engine SubSystems: \n");

	for(int i = m_kObjectNames.size()-1;i >= 0;i--)
	{
		for(map<string,NameObject>::iterator it = m_kObjectNames.begin();it != m_kObjectNames.end();it++)
		{
			if(it->second.m_iRegisterOrder == i)
			{
			
				g_Logf(" -  %s: ",it->second.m_strName.c_str());
			
				if(!it->second.pkObject->StartUp()) 
				{
					g_Logf("Fail\n");
					cerr<<"ERROR: Subsystem "<<it->second.m_strName<<" failed StartUp"<<endl;
					return false;
				}			
				
				it->second.m_bStarted = true;
				
				g_Logf("Ok\n");
				
				break;
			}	
		}
	}
	
	
	return true;
}

/**	\brief	Shutdown all SubSystem.
*/
bool ZFSystem::ShutDown()
{
	g_Logf("ShutDown Engine SubSystems: \n");

	// Engine Systems Shutdown backwards.
	for(unsigned int i = 0;i < m_kObjectNames.size();i++)
	{
		for(map<string,NameObject>::iterator it = m_kObjectNames.begin();it != m_kObjectNames.end();it++)
		{
			if(it->second.m_iRegisterOrder == i)
			{
				if(it->second.m_bStarted == false)
					continue;
				
				g_Logf(" -  %s: ",it->second.m_strName.c_str());
				if(!it->second.pkObject->ShutDown()) 
				{
					g_Logf("Fail\n");
					return false;
				}
				
				g_Logf("Ok\n");				

				break;
			}	
		}
	}	

	
	return true;
}

bool ZFSystem::IsValid()
{
	for(map<string,NameObject>::iterator it = m_kObjectNames.begin();it != m_kObjectNames.end();it++)
		if(!it->second.pkObject->IsValid())
			return false;
	

	return true;
}

ZFCmdData* ZFSystem::FindArea(const char* szName)
{
	for(unsigned int i=0; i<m_kCmdDataList.size(); i++) {
		if(m_kCmdDataList[i].m_strName == szName)
			return &m_kCmdDataList[i];
	}

	return NULL;
}

ConCommand* ZFSystem::FindConCommand(const char* szName)
{
	for(unsigned int i=0; i<m_kConCommands.size(); i++) 
	{
		if(m_kConCommands[i]->GetName() == szName)
			return m_kConCommands[i];
	}

	return NULL;
}

ConVar* ZFSystem::GetConVar(const char* szName)
{
	for(unsigned int i=0; i<m_kConCommands.size(); i++) 
	{
		if(m_kConCommands[i]->GetName() == szName && !m_kConCommands[i]->IsCommand())
			return dynamic_cast<ConVar*>(m_kConCommands[i]);
	}

	return NULL;

}


void ZFSystem::RegisterConCommand(ConCommand* pkConCommand)
{
	if(FindConCommand(pkConCommand->GetName().c_str())) 
		return;
	
	m_kConCommands.push_back( pkConCommand );
}


/**	\brief	Registers a Command.

  Registers a new command and the SubSystem that will handle the commands. Also
  set options for the cmd.

	iCmdID:	This is the value that will be sent to ZFSubSystem::RunCommand.<BR>
	iFlags:	Options for cmd.<BR>
	szHelp:	Help text to be displayed to user.<BR>
	iNumArg:	Min num of arguments this commands need.<BR>
*/
bool ZFSystem::Register_Cmd(char* szName, int iCmdID, ZFSubSystem* kObject,int iFlags, char* szHelp, int iNumOfArg)
{
	ConFunction* pkConFunc = new ConFunction();
	pkConFunc->Register(kObject, szName,iCmdID);
	return true;

/*	// Validate parameters
	if(szName == NULL)	return false;
	if(kObject == NULL)	return false;

	// Check if there is already something with this name.
	if(FindArea(szName)) 
		return false;
	

	ZFCmdData kNewCmd;
	kNewCmd.m_strName			= string(szName);
	kNewCmd.m_eType			= CSYS_FUNCTION;
	kNewCmd.m_iFlags			= iFlags;
	kNewCmd.m_vValue			= NULL;
	kNewCmd.m_pkObject		= kObject;
	kNewCmd.m_iCmdID			= iCmdID;
	kNewCmd.m_iMinNumOfArgs = iNumOfArg;

	if(szHelp)
		kNewCmd.m_strHelpText = string(szHelp);
	else
		kNewCmd.m_strHelpText = string("");

	m_kCmdDataList.push_back(kNewCmd);

	
#ifdef _DEBUG
	g_Logf("Command '%s' registred to be handled by '%s'\n", szName, kObject->m_strZFpsName.c_str());
#endif
	return true;
*/
}

/**	\brief	Unregisters Commands.
	
	Unregister all cmd data registred by this SubSystem.
*/
bool ZFSystem::UnRegister_Cmd(ZFSubSystem* kObject)
{
	vector<ZFCmdData>::iterator itCmds;
	for(itCmds = m_kCmdDataList.begin(); itCmds != m_kCmdDataList.end(); )
	{
		if(itCmds->m_pkObject == kObject) {
			itCmds = m_kCmdDataList.erase(itCmds);
#ifdef _DEBUG
			g_Logf(".");
#endif
			}
		else
			itCmds++;
	}

	return true;
}

bool ZFSystem::AutoComplete(const char* szCmdArg, vector<string>* pkCmds, int iMaxFinds)
{
	ConCommand *pkConCommand = FindConCommand( szCmdArg );

	//ZFCmdData* kCmdData = FindArea( szCmdArg );
	if(pkConCommand)
	{
		// This is already a complete command. Simply return.
		return false;
	}

	/*
	for(unsigned int i=0; i<m_kCmdDataList.size(); i++) 
	{
		// If this command fits so far add it.
		if(strncmp(szCmdArg, m_kCmdDataList[i].m_strName.c_str() ,strlen(szCmdArg)) == 0)
		{
			pkCmds->push_back(m_kCmdDataList[i].m_strName);
		}
	}
	*/
	for(unsigned int i=0; i<m_kConCommands.size(); i++) 
	{
		ConCommand *pkConCommand = m_kConCommands[i];
		if(strncmp(szCmdArg, pkConCommand->GetName().c_str() ,strlen(szCmdArg)) == 0)
		{
			pkCmds->push_back(pkConCommand->GetName());
		}
	}

	return true;
}



/**	\brief	Runs a Cmd into the ZFSystem.
	
	
*/
bool ZFSystem::RunCommand(const char* szCmdArg, ZFCmdSource iCmdSource)
{
	ConCommandLine kcmdargs;
	kcmdargs.Set(szCmdArg);
	kcmdargs.m_eSource = iCmdSource;
	string strValue;

	if(kcmdargs.m_kSplitCommand.size() == 0)
		return false;

	ZFCmdData* kCmdData = FindArea(kcmdargs.m_kSplitCommand[0].c_str());

	if(!kCmdData)
	{
		ConCommand* pkConCom = FindConCommand(kcmdargs.m_kSplitCommand[0].c_str());
		if(!pkConCom)
			return false;

		if(pkConCom->IsCommand())
		{
			ConFunction* pkConFunc = dynamic_cast<ConFunction*>(pkConCom);
			ConCommandLine kLine;
			kLine.Set( kcmdargs.m_strFullCommand.c_str() );
			pkConFunc->Run(&kLine);
			return true;
		}
		else
		{
				ConVar* pkConVar = dynamic_cast<ConVar*>(pkConCom);
				int iHora = kcmdargs.m_kSplitCommand.size();
				if(iHora == 1)
				{
					Printf("Show");
					Printf(" %s.%s = %s",pkConVar->GetSubSystemName().c_str(),kcmdargs.m_kSplitCommand[0].c_str(),pkConVar->GetString().c_str());
				}
				if(iHora == 2)
				{
					Printf("Set");
					string strConCommand = kcmdargs.m_kSplitCommand[0].c_str();
					strValue = kcmdargs.m_kSplitCommand[1].c_str();
					pkConVar = dynamic_cast<ConVar*>(FindConCommand(strConCommand.c_str()));
					pkConVar->SetString( strValue );
					Printf(" %s.%s = %s", pkConVar->GetSubSystemName().c_str(), strConCommand.c_str(),pkConVar->GetString().c_str());
				}
		}

		return true;
	}

	// Check so we have the rights to run this command.
	if( (iCmdSource & kCmdData->m_iFlags) == 0)
		return false;

	switch(kCmdData->m_eType) {
		case CSYS_FUNCTION:	
			if(kcmdargs.m_kSplitCommand.size() <= (unsigned int)kCmdData->m_iMinNumOfArgs) {
				BasicConsole* con = static_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("ZSSConsole"));
				con->Printf(kCmdData->m_strHelpText.c_str());
				}
			else {
				kCmdData->m_pkObject->RunCommand(kCmdData->m_iCmdID, &kcmdargs);
				}
			break;

		default:
			if(kCmdData->m_eType == CSYS_NONE)
				break;

			if(kcmdargs.m_kSplitCommand.size() == 1)
			{
				strValue = GetVarValue(kCmdData);
				BasicConsole* con = static_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("ZSSConsole"));
				con->Printf(" %s = [ %s]",kCmdData->m_strName.c_str(), strValue.c_str());
			}
			
			//dvoid haxorar
			if(kcmdargs.m_kSplitCommand.size() >= 2 )
			{
				SetVariable(kcmdargs.m_kSplitCommand[0].c_str(),kcmdargs.m_kSplitCommand[1].c_str());
				break;
			}

			/*			
			if(kcmdargs.m_kSplitCommand.size() >= 3 && kcmdargs.m_kSplitCommand[1] == string("=") )
			{
				SetVariable(kcmdargs.m_kSplitCommand[0].c_str(),kcmdargs.m_kSplitCommand[2].c_str());
			}
			*/
			
			break;

	}

	return true;
}

/**	\brief	Register a variable.

  Registers a variable to be accsessible t the ZFSystem. Set the name, type, object
  that own it and flags for the variable.
*/
bool ZFSystem::RegisterVariable(const char* szName, void* pvAddress, ZFCmdDataType eType, ZFSubSystem* kObject,int iFlags)
{
	// Validate parameters
	if(szName == NULL)		return false;
	if(pvAddress == NULL)	return false;

	// Check if there is already something with this name.
	if(FindArea(szName)) 
		return false;

	ZFCmdData kNewCmd;
	kNewCmd.m_strName			= string(szName);
	kNewCmd.m_eType			= eType;
	kNewCmd.m_iFlags			= iFlags;
	kNewCmd.m_vValue			= pvAddress;
	kNewCmd.m_pkObject		= kObject;
	kNewCmd.m_iCmdID			= 0;
	kNewCmd.m_iMinNumOfArgs = 0;

	m_kCmdDataList.push_back(kNewCmd);

#ifdef _DEBUG
	g_Logf("Variable '%s' registred.\n", szName);
#endif
	return true;
}

bool ZFSystem::SetVariable(const char* szName, const char* szValue)
{
	ZFCmdData* pkArea = FindArea(szName);
	if(!pkArea)
		return false;

	if(pkArea->m_eType == CSYS_STRING)
		SetString (pkArea, szValue);											
	else 
		SetValue  (pkArea, szValue);											

	return true;
}


void ZFSystem::SetValue(ZFCmdData* pkArea, const char* szValue) 
{
	float dData = float(atof(szValue));
	bool	bValue = false;

	switch(pkArea->m_eType) {
		case CSYS_INT:
			*(int*)pkArea->m_vValue = (int)dData;break;
		case CSYS_FLOAT:
			*(float*)pkArea->m_vValue=(float)dData;break;
		case CSYS_DOUBLE:
			*(double*)pkArea->m_vValue=(double)dData;break;
		case CSYS_LONG:
			*(long*)pkArea->m_vValue=(long)dData;break;
		case CSYS_BOOL:
			if(IsSameIgnoreCase(szValue, "false"))
				*(bool*)pkArea->m_vValue = false;
			else if(IsSameIgnoreCase(szValue, "true"))
				*(bool*)pkArea->m_vValue = true;
			else if(IsSameIgnoreCase(szValue, "toggle")) {
				bValue = *(bool*)pkArea->m_vValue;
				*(bool*)pkArea->m_vValue = !bValue;
				}
			else {
				if(dData != 0.0)
					bValue = true;
				*(bool*)pkArea->m_vValue = bValue;
				}

			break;
		case CSYS_STRING:
		{
			string* pstrValue = (string*)pkArea->m_vValue;
			*pstrValue = szValue;
			break;	
		}
		
		case CSYS_BOOLVECTOR:
		{
			vector<bool>* pkBoolVec = 	(vector<bool>*)pkArea->m_vValue;
			int iBVL = pkBoolVec->size();
			int iStrLen = strlen(szValue);
						
			if(iStrLen != iBVL)
			{
				Printf("Wrong size of input bool vector");
				break;
			}					
			
			for(int i = 0;i<iStrLen;i++)
			{
				if(szValue[i] == '1')
					(*pkBoolVec)[i] = true;
				else if(szValue[i] == '0')
					(*pkBoolVec)[i] = false;
				else
				{
					Printf("Bad value, shuld be 1/0");
					break;
				}									
			}				
				
			break;
		}
	}
}	

void ZFSystem::SetString(ZFCmdData* pkArea, const char* szValue) 
{
	(*(string*)pkArea->m_vValue)=szValue;
}

void* ZFSystem::GetVar(ZFCmdData* pkArea)
 {
	return (void*)pkArea->m_vValue;
}

string ZFSystem::GetVarValue(ZFCmdData* pkArea)
{
	char szValue[256];
	strcpy(szValue, "");
	string strValue;

	switch(pkArea->m_eType) 
	{
		case CSYS_FUNCTION:
			sprintf(szValue, "(%s)", pkArea->m_strHelpText.c_str());
			strValue = szValue;
			break;
		case CSYS_FLOAT:
			sprintf(szValue, "%.3f", *(float*)pkArea->m_vValue);
			strValue = szValue;
			break;
		case CSYS_DOUBLE:
			sprintf(szValue, "%.3d", *(double*)pkArea->m_vValue);
			strValue = szValue;
			break;
		case CSYS_LONG:
			sprintf(szValue, "%l", *(long*)pkArea->m_vValue);
			strValue = szValue;
			break;
		case CSYS_BOOL:
			sprintf(szValue, "%d", *(bool*)pkArea->m_vValue);
			strValue = szValue;
			break;
		case CSYS_INT:
			sprintf(szValue, "%d", *(int*)pkArea->m_vValue);
			strValue = szValue;
			break;
		case CSYS_STRING:
		{
			//string* pstrValue = (string*)pkArea->m_vValue;
			//sprintf(szValue, "%s", pstrValue->c_str());		
			
			strValue = *(string*)pkArea->m_vValue;
			break;
		}
		case CSYS_BOOLVECTOR:
		{
			vector<bool>* pkBoolVec = 	(vector<bool>*)pkArea->m_vValue;
			
			for(unsigned int i = 0;i< pkBoolVec->size();i++)
			{
				if((*pkBoolVec)[i])
					strValue += "1";
				else
					strValue += "0";
			}			
			
			break;	
		}
	}

	
	return strValue;
}

bool ZFSystem::Log_Create(const char* szName)
{
	ZFLogFile NewLogFile;
	NewLogFile.m_strName = szName;
	NewLogFile.m_strFileName = m_strPreLogName + "-" + string(szName) + ".txt";
	
	NewLogFile.m_strFileName = "logs/" + NewLogFile.m_strFileName;

	NewLogFile.m_pkFilePointer = fopen(NewLogFile.m_strFileName.c_str(), "wt");
	setvbuf(NewLogFile.m_pkFilePointer , NULL, _IONBF, 0);		// Set Non buffer mode.
	
	m_kLogFiles.push_back( NewLogFile );

	return true;
}

void ZFSystem::Log_Destory(const char* szName)
{
	vector<ZFLogFile>::iterator it;

	for(it = m_kLogFiles.begin(); it != m_kLogFiles.end(); it++ ) {
		if(it->m_strName == string(szName)) {
			fclose(it->m_pkFilePointer);
			it = m_kLogFiles.erase(it);
			}
		}
}

ZFLogFile*	ZFSystem::Log_Find(const char* szName)
{
	string strHataStl;

	for(unsigned int i=0; i<m_kLogFiles.size(); i++) {
		strHataStl = m_kLogFiles[i].m_strName ;
		if(m_kLogFiles[i].m_strName == string(szName)) 
			return &m_kLogFiles[i];
		}

	cout<<"WARNING: ZFSystem::Log_Find , log "<<szName<< " not found"<<endl;
		
	return NULL;
}

void ZFSystem::Log_DestroyAll()
{
	for(unsigned int i=0; i<m_kLogFiles.size(); i++) {
		fclose(m_kLogFiles[i].m_pkFilePointer);
		}
}

void ZFSystem::Log(const char* szMessage)
{
	if(!m_pkLogFile)
		return;

	///////////////////////////////////////////////////
	// Zeb was here
	static bool s_bNewLine = true;

	if(s_bNewLine)
	{
		static int s_counter = 0;
		static int s_iMaxPreZeros = (int) log10((float)9999);
		int curr = (int) log10((float)s_counter);

		int iNumPreZeros = s_iMaxPreZeros-curr;
		for(int j=0; j<iNumPreZeros; j++)
			fprintf(m_pkLogFile, "0");

		fprintf(m_pkLogFile, "%i: ", s_counter++);
	}

	if(szMessage[strlen(szMessage)-1] == '\n')
		s_bNewLine = true;
	else
		s_bNewLine = false;
	///////////////////////////////////////////////////

	fprintf(m_pkLogFile, szMessage);
}

void ZFSystem::Log(const char* szName, const char* szMessage)
{
	if(!m_kLogEnabled.GetBool())
		return;
	
	ZFLogFile* pkLog = Log_Find( szName );
	if(!pkLog)
		return;

	fprintf(pkLog->m_pkFilePointer, szMessage);
}

char g_LogFormatTxt2[4096];	

void ZFSystem::Logf(const char* szName, const char* szMessageFmt,...)
{
	if(!m_kLogEnabled.GetBool())
		return;


	ZFLogFile* pkLog = Log_Find( szName );
	if(!pkLog)
		return;

	va_list		ap;							// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (szMessageFmt == NULL)	return;					

	va_start(ap, szMessageFmt);						// Parses The String For Variables
		vsprintf(g_LogFormatTxt2, szMessageFmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	fprintf(pkLog->m_pkFilePointer, g_LogFormatTxt2);
}


void ZFSystem::Config_Save(string strFileName)
{
	string strVar;
	FILE* fp = fopen(strFileName.c_str(), "wt");

	for(map<string,NameObject>::iterator it = m_kObjectNames.begin(); it != m_kObjectNames.end();it++)
	{
		// Write Header to config
		fprintf(fp,"\n\n[%s]\n", it->second.m_strName.c_str());

		for(unsigned int i=0; i<m_kConCommands.size(); i++)
		{	
			if(!m_kConCommands[i]->IsCommand())
			{
				ConVar* pkConVar = dynamic_cast<ConVar*>(m_kConCommands[i]);
				if(pkConVar->GetSubsystem() == it->second.pkObject) 
				{
					string strVarName  = pkConVar->GetName();
					string strVarValue = pkConVar->GetString();
					fprintf(fp,"%s=%s\n",strVarName.c_str(), strVarValue.c_str());
				}
			}
		}
	}

	// Write Globals
	fprintf(fp,"\n\n[%s]\n", "Global");
	for(unsigned int i=0; i<m_kConCommands.size(); i++)
	{	
		if(!m_kConCommands[i]->IsCommand())
		{
			ConVar* pkConVar = dynamic_cast<ConVar*>(m_kConCommands[i]);
			if(pkConVar->GetSubsystem() == NULL) 
			{
				string strVarName  = pkConVar->GetName();
				string strVarValue = pkConVar->GetString();
				fprintf(fp,"%s=%s\n",strVarName.c_str(), strVarValue.c_str());
			}
		}
	}

	fclose(fp);
}

void ZFSystem::Config_Load(string strFileName)
{
	ZFIni			m_kIni;
	if(m_kIni.Open(strFileName.c_str(), 0) == 0) 
	{
		cout << "Failed to find config file" << endl;
		return;
	}

	char* pkVal;

	for(map<string,NameObject>::iterator it = m_kObjectNames.begin(); it != m_kObjectNames.end();it++)
	{
		for(unsigned int i=0; i<m_kConCommands.size(); i++)
		{	
			if(!m_kConCommands[i]->IsCommand())
			{
				ConVar* pkConVar = dynamic_cast<ConVar*>(m_kConCommands[i]);
				string strVarName  = pkConVar->GetName();
				pkVal = m_kIni.GetValue(it->second.m_strName.c_str(), strVarName.c_str());
				if(pkVal) 
					pkConVar->SetString(pkVal);
			}
		}
	}	

	// Load globals
	for(unsigned int i=0; i<m_kConCommands.size(); i++)
	{	
		if(!m_kConCommands[i]->IsCommand())
		{
			ConVar* pkConVar = dynamic_cast<ConVar*>(m_kConCommands[i]);
			string strVarName  = pkConVar->GetName();
			pkVal = m_kIni.GetValue("Global", strVarName.c_str());
			if(pkVal) 
				pkConVar->SetString(pkVal);
		}
	}
}

void ZFSystem::Printf(const char* szMessageFmt,...)
{
	if(!m_pkConsole) {
 		m_pkConsole = static_cast<BasicConsole*>(GetObjectPtr("ZSSConsole")); 	 	
		if(!m_pkConsole)
			return;
		}

	va_list		ap;							// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (szMessageFmt == NULL)	return;					

	va_start(ap, szMessageFmt);						// Parses The String For Variables
		vsprintf(g_LogFormatTxt2, szMessageFmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	m_pkConsole->Printf(g_LogFormatTxt2);
}

void ZFSystem::SetPreLogName(string strPreLogName)
{
	m_strPreLogName = strPreLogName;

	// Open master logfile.
	string strMasterLogFile = m_strPreLogName + ".txt";
	m_pkLogFile = fopen(strMasterLogFile.c_str(), "wt");	// Open Master Log File.
	setvbuf(m_pkLogFile, NULL, _IONBF, 0);						// Set Non buffer mode.
}




void CmdArgument::Set(const char* szCmdArgs)
{
	m_strFullCommand = string(szCmdArgs);
//	m_strFullCommand.replace(m_strFullCommand.begin(), m_strFullCommand.end(),9,32);	// Replace tab with space.
	m_kSplitCommand.clear();
	string	kNewArg;

	if(strlen(szCmdArgs) == 0) {
		m_kSplitCommand.push_back("");
		return;
	}

	int args = 0;	// Number of arguments
	kNewArg = "";

	for(unsigned int i=0; i<strlen(szCmdArgs); i++) {

		while((unsigned int)(szCmdArgs[i]) != 32 && i < strlen(szCmdArgs) ) {	//loop until space
			kNewArg.append(1,szCmdArgs[i]);						//add to argument nr args
			i++;
		}

		if(kNewArg.size() !=0 ) {
		//if nothing was added to the argument use it in the next loop
			m_kSplitCommand.push_back(kNewArg);		
			kNewArg = "";
		}
			args++;
	}
}


void ZFSystem::DontUseStartProfileTimer(const char* czName)
{
	if(!m_kProfileEnabled.GetBool())
		return;

	m_kTimers[czName].m_iStartime = SDL_GetTicks();
}

void ZFSystem::DontUseStopProfileTimer(const char* czName)
{
	if(!m_kProfileEnabled.GetBool())
		return;
		
		
	ProfileTimer* pkTimer = &m_kTimers[czName];
	
	if(pkTimer->m_iStartime == -1)
	{
		cout<<"WARNING: tried to stop , already stoped profile timer:"<<czName<<endl;
		return;
	}

	//add time to total time	
	pkTimer->m_iTotalTime += (SDL_GetTicks() - pkTimer->m_iStartime);
	pkTimer->m_iStartime = -1;
}

void ZFSystem::ClearProfileTimers() 
{
	if(!m_kProfileEnabled.GetBool())
		return;


	m_kTimers.clear();
	m_iTotalTime = SDL_GetTicks(); 
}

void ZFSystem::GetProfileTimers(vector<TimerInfo >* pkTimers)
{
	if(!m_kProfileEnabled.GetBool())
		return;

	TimerInfo kTemp;
	
	map<string,ProfileTimer>::iterator it;
	for(it = m_kTimers.begin(); it != m_kTimers.end(); it++ )
	{
		//cout<<(*it).first<< " : "<<(*it).second.m_iTotalTime<<endl;
		kTemp.m_strName = 	(*it).first;
		kTemp.m_iTime = 		(*it).second.m_iTotalTime;
		pkTimers->push_back( kTemp );
	}
	
}

int ZFSystem::GetTotalTime()			
{	
	return SDL_GetTicks() - m_iTotalTime;	
}


bool ZFSystem::SendSystemMessage(const string& strSystem,const string& strType,int iNrOfParam,const void** pkParam)
{
	if(ZFSubSystem* pkSystem = GetObjectPtr(strSystem.c_str(),false))
	{
		pkSystem->OnSystemMessage(strType,iNrOfParam,pkParam);		
		return true;
	}
	
	return false;
}







// graveyard

// 	for(unsigned int i=0; i < kObjectNames.size();i++) {
// 		g_Logf(" %s, %d\n", kObjectNames[i].m_strName.c_str(), kObjectNames[i].m_iNumOfRequests );
// 	}



// 	vector<NameObject>::iterator itNames;
// 	for(itNames = kObjectNames.begin(); itNames != kObjectNames.end(); )
// 	{
// 		if(itNames->pkObject == pkObject) {
// 			itNames = kObjectNames.erase(itNames);
// #ifdef _DEBUG
// 			g_Logf(".");
// #endif
// 			}
// 		else
// 			itNames++;
// 	}

	
// 	for(unsigned int i=0; i < kObjectNames.size();i++) 
// 	{
// 		if(kObjectNames[i].m_strName == acName) 
// 		{
// 			kObjectNames[i].m_iNumOfRequests ++;
// 			return kObjectNames[i].pkObject;
// 		}
// 	}
// 
// 	if(bWarning)
// 		cout<<"WARNING: Requested system pointer not found:"<<acName<<endl;
// 		
//	return NULL;

// 	for( int i = (kObjectNames.size() - 1); i >= 0; i--) {
// 		g_Logf(" - %s: ",kObjectNames[i].m_strName.c_str());
// 
// 		if(!kObjectNames[i].pkObject->StartUp()) {
// 			g_Logf("Fail\n");
// 			return false;
// 			}
// 
// 		kObjectNames[i].m_bStarted = true;
// 		g_Logf("Ok\n");
// 	}

// 	for(unsigned int SubIndex=0; SubIndex < kObjectNames.size();SubIndex++) 
// 	{
// 		//cout << "[section] : " << kObjectNames[SubIndex].m_strName << endl;
// 
// 		for(unsigned int i=0; i<m_kCmdDataList.size(); i++) {
// 			if(m_kCmdDataList[i].m_eType == CSYS_NONE)		continue; // We don't save none valid data.
// 			if(m_kCmdDataList[i].m_eType == CSYS_FUNCTION)	continue; // We don't save functions.
// 			
// 			pkVal = m_kIni.GetValue(kObjectNames[SubIndex].m_strName.c_str(), m_kCmdDataList[i].m_strName.c_str());
// 			if(pkVal) {
// 				//cout << "Setting " << m_kCmdDataList[i].m_strName.c_str();
// 				//cout << " " << pkVal << endl;
// 				SetVariable(m_kCmdDataList[i].m_strName.c_str(),pkVal);
// 				}
// 			}
// 	}

		
// 	for(unsigned int SubIndex=0; SubIndex < kObjectNames.size();SubIndex++) 
// 	{
// 		// Write Header to config
// 		fprintf(fp,"\n\n[%s]\n", kObjectNames[SubIndex].m_strName.c_str());
// 
// 		for(unsigned int i=0; i<m_kCmdDataList.size(); i++) 
// 		{
// 			if(m_kCmdDataList[i].m_eType == CSYS_NONE)		continue; // We don't save none valid data.
// 			if(m_kCmdDataList[i].m_eType == CSYS_FUNCTION)	continue; // We don't save functions.
// 			
// 			// Check so this variable is owned by the current section.
// 			if(m_kCmdDataList[i].m_pkObject == kObjectNames[SubIndex].pkObject) 
// 			{
// 				strVar = GetVarValue(&m_kCmdDataList[i]);
// 				fprintf(fp,"%s=%s\n",m_kCmdDataList[i].m_strName.c_str(), strVar.c_str());
// 			}
// 		}
// 	}

// 	for(unsigned int i=0; i < kObjectNames.size();i++) {
// 		if(!kObjectNames[i].pkObject->IsValid())
// 			return false;
// 	}

	
// 	for(unsigned int i=0; i < kObjectNames.size();i++) {
// 		strcpy(szName, kObjectNames[i].m_strName.c_str());
// 		if(kObjectNames[i].m_bStarted == false)	continue;
// 
// 		g_Logf(" -  %s: ",kObjectNames[i].m_strName.c_str());
// 		if(!kObjectNames[i].pkObject->ShutDown()) {
// 			g_Logf("Fail\n");
// 			return false;
// 			}
// 		g_Logf("Ok\n");
// 	}

