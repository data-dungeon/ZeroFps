#include <iostream>
#include <string>
#include <cstdio>
#include <cstdarg>

#include "zfsystem.h"
#include "basicconsole.h"
#include "globals.h"
#include "../basic/zfini.h"

using namespace std;

ZFSystem* ZFSystem::pkInstance;

/*
	Print all system variables.
*/
void ZFSystem::PrintVariables()
{
	BasicConsole*		m_pkCon;
	m_pkCon = dynamic_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("Console"));
	string strValue;

	m_pkCon->Printf("### variable list ###");

	for(unsigned int i=0; i<m_kCmdDataList.size(); i++) {
		if(m_kCmdDataList[i].m_eType == CSYS_NONE)		continue; // We don't print none valid data.
		if(m_kCmdDataList[i].m_eType == CSYS_FUNCTION)	continue; // We don't print functions.

		strValue = GetVarValue(&m_kCmdDataList[i]);
		m_pkCon->Printf(" %s = [ %s]",m_kCmdDataList[i].m_strName.c_str(), strValue.c_str());
	}
}


void ZFSystem::PrintCommands()
{
	BasicConsole*		m_pkCon;
	m_pkCon = dynamic_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("Console"));
	string strValue;

	m_pkCon->Printf("### variable list ###");

	for(unsigned int i=0; i<m_kCmdDataList.size(); i++) {
		if(m_kCmdDataList[i].m_eType == CSYS_FUNCTION) {
			strValue = GetVarValue(&m_kCmdDataList[i]);
			m_pkCon->Printf(" %s = [ %s]",m_kCmdDataList[i].m_strName.c_str(), strValue.c_str());
			}
	}
}

void ZFSystem::PrintObjects(void)
{
	for(unsigned int i=0; i < kObjectNames.size();i++) {
		g_Logf(" %s, %d\n", kObjectNames[i].m_strName.c_str(), kObjectNames[i].m_iNumOfRequests );
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

	for(int i = 0; i < iNrOfArgs; i++) {
		strArg = string(paArgs[i]);

		if(strArg.c_str()[0] == '-') {
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

	cout<<"Nr of arguments: "<< strFullArg.size() <<endl;
	for(int ia = 0; ia < AppArguments.size(); ia++) {
		cout << "Argument[" << ia << "]: "<< AppArguments[ia] << endl;
		RunCommand(AppArguments[ia].c_str(), CSYS_SRC_CMDLINE);
		}
}

ZFSystem::ZFSystem()
{
	ZFSystem::pkInstance = this;

	m_pkLogFile = fopen("zerofps.txt", "wt");	// Open Master Log File.
	setvbuf(m_pkLogFile, NULL, _IONBF, 0);		// Set Non buffer mode.

#ifdef _DEBUG
	g_Logf("Starting ZeroFps Object System\n");
#endif
}

ZFSystem::~ZFSystem()
{
#ifdef _DEBUG
	g_Logf("Closing ZeroFps Object System\n");
#endif

	// Check that we don't have any objects left.
	if(kObjectNames.size() > 0) {
		g_Logf("WARNING: Some Engine Systems have not been closed down\n");
		PrintObjects();
		}

	// Check that we don't have any objects left.
	if(m_kCmdDataList.size() > 0) {
		g_Logf("WARNING: Some Variables Systems have not unregistred\n");
		LogVariables();
		}

	Log_DestroyAll();
	fclose(m_pkLogFile);						// Close Log Fil

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
	ZFSubSystem* pkCheck = GetObjectPtr(acName);
	if(pkCheck) {
		g_Logf("Fail\n");
		Logf("Warning: There was already a object with name '%s'\n", acName);
		return;
		}

	NameObject kObjName;
	kObjName.m_strName			= string(acName);
	kObjName.pkObject				= pkObject;
	kObjName.m_iNumOfRequests	= 0;
	kObjName.m_bStarted			= false;
	
	kObjectNames.push_back(kObjName);
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

	vector<NameObject>::iterator itNames;
	for(itNames = kObjectNames.begin(); itNames != kObjectNames.end(); )
	{
		if(itNames->pkObject == pkObject) {
			itNames = kObjectNames.erase(itNames);
#ifdef _DEBUG
			g_Logf(".");
#endif
			}
		else
			itNames++;
	}

	UnRegister_Cmd(pkObject);

#ifdef _DEBUG
	g_Logf("Ok\n");
#endif
}

/**	\brief	Get pointer to SubSystem by name.

	Get a Ptr to the SubSystem with the choosen name. Null if no SubSystem found with this name.
*/
ZFSubSystem* ZFSystem::GetObjectPtr(char* acName)
{
	string Test(acName);

	for(unsigned int i=0; i < kObjectNames.size();i++) {
		if(kObjectNames[i].m_strName == Test) {
			kObjectNames[i].m_iNumOfRequests ++;
			return kObjectNames[i].pkObject;
			}
		}

	return NULL;
}

/**	\brief	Startup all SubSystems.
	Loop all subsystems from last created to first and run their startup function. 
	If any subsystems fail and return false.
*/
bool ZFSystem::StartUp()
{
	cout << "Start ZeroFps Engine SubSystems:" << endl;
	g_Logf("Start ZeroFps Engine SubSystems: \n");

	int iSize = kObjectNames.size();
	ZFSubSystem* pkTestObject;

	for( int i = (kObjectNames.size() - 1); i >= 0; i--) {
		g_Logf(" - %s: ",kObjectNames[i].m_strName.c_str());

		pkTestObject = kObjectNames[i].pkObject;

		if(!kObjectNames[i].pkObject->StartUp()) {
			g_Logf("Fail\n");
			return false;
			}

		kObjectNames[i].m_bStarted = true;
		g_Logf("Ok\n");
	}
	
	return true;
}

/**	\brief	Shutdown all SubSystem.
*/
bool ZFSystem::ShutDown()
{
	g_Logf("ShutDown Engine SubSystems: \n");

	// Engine Systems Shutdown backwards.
	for(unsigned int i=0; i < kObjectNames.size();i++) {
		if(kObjectNames[i].m_bStarted == false)	continue;

		g_Logf(" -  %s: ",kObjectNames[i].m_strName.c_str());
		if(!kObjectNames[i].pkObject->ShutDown()) {
			g_Logf("Fail\n");
			return false;
			}
		g_Logf("Ok\n");
	}


	
	return true;
}

bool ZFSystem::IsValid()
{
	for(unsigned int i=0; i < kObjectNames.size();i++) {
		if(!kObjectNames[i].pkObject->IsValid())
			return false;
	}
	
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
	// Validate parameters
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
	Logf("Command '%s' registred to be handled by '%s'\n", szName, kObject->m_strZFpsName);
#endif
	return true;
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

/**	\brief	Runs a Cmd into the ZFSystem.
	
	
*/
bool ZFSystem::RunCommand(const char* szCmdArg, ZFCmdSource iCmdSource)
{
	CmdArgument kcmdargs;
	kcmdargs.Set(szCmdArg);
	kcmdargs.m_eSource = iCmdSource;

	if(kcmdargs.m_kSplitCommand.size() == 0)
		return false;

	ZFCmdData* kCmdData = FindArea(kcmdargs.m_kSplitCommand[0].c_str());

	if(!kCmdData)	return false;

	switch(kCmdData->m_eType) {
		case CSYS_FUNCTION:	
			if(kcmdargs.m_kSplitCommand.size() <= (unsigned int)kCmdData->m_iMinNumOfArgs) {
				BasicConsole* con = static_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("Console"));
				con->Printf(kCmdData->m_strHelpText.c_str());
				}
			else {
				kCmdData->m_pkObject->RunCommand(kCmdData->m_iCmdID, &kcmdargs);
				}
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
	Logf("Variable '%s' registred.\n", szName);
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
	bool	bValue;

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
			else 
				*(bool*)pkArea->m_vValue=(bool)dData;

			break;
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

	switch(pkArea->m_eType) {
		case CSYS_FUNCTION:
			sprintf(szValue, "(%s)", pkArea->m_strHelpText.c_str());
			break;
		case CSYS_FLOAT:
			sprintf(szValue, "%.3f", *(float*)pkArea->m_vValue);
			//m_pkCon->Printf(" %s = [%.3f]",kVars[i]->aName,*(float*)GetVar(i));break;
			break;
		case CSYS_DOUBLE:
			sprintf(szValue, "%.3d", *(double*)pkArea->m_vValue);
			//	m_pkCon->Printf(" %s = [%.3d]",kVars[i]->aName,*(double*)GetVar(i));break;
			break;
		case CSYS_LONG:
			sprintf(szValue, "%l", *(long*)pkArea->m_vValue);
			//m_pkCon->Printf(" %s = [%l]",kVars[i]->aName,*(long*)GetVar(i));break;										
			break;
		case CSYS_BOOL:
			sprintf(szValue, "%d", *(bool*)pkArea->m_vValue);
			break;
		case CSYS_INT:
			sprintf(szValue, "%d", *(int*)pkArea->m_vValue);
			break;
		case CSYS_STRING:
			sprintf(szValue, "%s", (string*)pkArea->m_vValue);
			//m_pkCon->Printf(" %s = [%s]",kVars[i]->aName,((string*)GetVar(i))->c_str());break;
			break;
		}

	strValue = szValue;
	return strValue;
}

bool ZFSystem::Log_Create(const char* szName)
{
	ZFLogFile NewLogFile;
	NewLogFile.m_strName = szName;
	NewLogFile.m_strFileName = string(szName) + ".txt";
	
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
	ZFLogFile* pkLog = Log_Find( szName );
	if(!pkLog)
		return;

	fprintf(pkLog->m_pkFilePointer, szMessage);
}

char g_LogFormatTxt2[4096];	

void ZFSystem::Logf(const char* szName, const char* szMessageFmt,...)
{
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

	for(unsigned int SubIndex=0; SubIndex < kObjectNames.size();SubIndex++) {
		// Write Header to config
		fprintf(fp,"\n\n[%s]\n", kObjectNames[SubIndex].m_strName.c_str());

		for(unsigned int i=0; i<m_kCmdDataList.size(); i++) {
			if(m_kCmdDataList[i].m_eType == CSYS_NONE)		continue; // We don't save none valid data.
			if(m_kCmdDataList[i].m_eType == CSYS_FUNCTION)	continue; // We don't save functions.
			
			// Check so this variable is owned by the current section.
			if(m_kCmdDataList[i].m_pkObject == kObjectNames[SubIndex].pkObject) {
				strVar = GetVarValue(&m_kCmdDataList[i]);
				fprintf(fp,"%s=%s\n",m_kCmdDataList[i].m_strName.c_str(), strVar.c_str());
				}
			}
		}

	fclose(fp);
}

void ZFSystem::Config_Load(string strFileName)
{
	ZFIni			m_kIni;
	if(m_kIni.Open(strFileName.c_str(), 0) == 0) {
		cout << "Failed to find config file" << endl;
		return;
		}

	char* pkVal;

	for(unsigned int SubIndex=0; SubIndex < kObjectNames.size();SubIndex++) {
		//cout << "[section] : " << kObjectNames[SubIndex].m_strName << endl;

		for(unsigned int i=0; i<m_kCmdDataList.size(); i++) {
			if(m_kCmdDataList[i].m_eType == CSYS_NONE)		continue; // We don't save none valid data.
			if(m_kCmdDataList[i].m_eType == CSYS_FUNCTION)	continue; // We don't save functions.
			
			pkVal = m_kIni.GetValue(kObjectNames[SubIndex].m_strName.c_str(), m_kCmdDataList[i].m_strName.c_str());
			if(pkVal) {
				//cout << "Setting " << m_kCmdDataList[i].m_strName.c_str();
				//cout << " " << pkVal << endl;
				SetVariable(m_kCmdDataList[i].m_strName.c_str(),pkVal);
				}
			}
		}
}

void ZFSystem::Printf(const char* szMessageFmt,...)
{
	if(!m_pkConsole) {
 		m_pkConsole = static_cast<BasicConsole*>(GetObjectPtr("Console")); 	 	
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
	m_pkConsole->Print(g_LogFormatTxt2);
}



void CmdArgument::Set(const char* szCmdArgs)
{
	m_strFullCommand = string(szCmdArgs);
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

