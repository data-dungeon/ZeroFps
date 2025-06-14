#include <iostream>
#include <string>
#include <cstdio>
#include <cstdarg>

#include "zfobjectmanger.h"
#include "basicconsole.h"
#include "globals.h"
using namespace std;

ZFObjectManger* ZFObjectManger::pkInstance;














/*
Manger:
	Register:		Registrerar object i object name sys.
	Unregister:		Removes object fr�n object name sys.

	Link:			L�nkar ett object till ett annat som child.
	UnLink:			Tar bort child fr�n en parent.

	IsOk:

Object:
	Name				Namn p� object.
	Childs:				Lista p� children.
	Parent:				Pekare till parent.
	
	GetParent()			Get ptr to parent of object.
	GetNumChildren()	Totalt antal children.
	GetChild()			Pekare till child i.
	GetName()			Returnerar namnet p� object.

	Get

Object Data:
	Objekt:				Pekare till object.

*/

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







ZFObjectManger::ZFObjectManger()
{
	ZFObjectManger::pkInstance = this;

	m_pkLogFile = fopen("zerofps.txt", "wt");	// Open Master Log File.
	setvbuf(m_pkLogFile, NULL, _IONBF, 0);		// Set Non buffer mode.

#ifdef _DEBUG
	g_Logf("Starting Object System\n");
#endif

//	g_ZFObjSys.Register_Cmd("set",		FID_SET,this, "set name value", 2);
//	g_ZFObjSys.Register_Cmd("varlist",	FID_VARLIST,this);
}

ZFObjectManger::~ZFObjectManger()
{
#ifdef _DEBUG
	g_Logf("Closing Object System\n");
#endif

	// Check that we don't have any objects left.
	if(kObjectNames.size() > 0) {
		g_Logf("WARNING: Some Engine Systems have not been closed down\n");
		PrintObjects();
		}

	Log_DestroyAll();
	fclose(m_pkLogFile);						// Close Log Fil
}


ZFObjectManger* ZFObjectManger::GetInstance()
{
	return ZFObjectManger::pkInstance;
}


void ZFObjectManger::Register(ZFObject* pkObject, char* acName, ZFObject* pkParent)
{
#ifdef _DEBUG
	Logf("Register '%s'", acName);
#endif

	// Make sure that there is no other object with the same name.
	ZFObject* pkCheck = GetObjectPtr(acName);
	if(pkCheck) {
		Logf("There is already a object with name '%s'\n", acName);
		return;
		}

	NameObject kObjName;
	kObjName.m_strName = string(acName);
	kObjName.pkObject = pkObject;
	kObjName.m_iNumOfRequests = 0;
	
	kObjectNames.push_back(kObjName);
	pkObject->m_strZFpsName = string(acName);

	

#ifdef _DEBUG
	g_Logf("ok\n");
#endif
}

void ZFObjectManger::UnRegister(ZFObject* pkObject)
{
#ifdef _DEBUG
	Logf("UnRegister '%s' ", pkObject->m_strZFpsName.c_str());
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

#ifdef _DEBUG
	g_Logf("Ok\n");
#endif

	UnRegister_Cmd(pkObject);
}

ZFObject* ZFObjectManger::GetObjectPtr(char* acName)
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



void ZFObjectManger::Link(ZFObject* pkParent, ZFObject* pkObject)
{
	pkObject->m_pkParent = pkParent;
	pkParent->m_akChild.push_back(pkObject);
}

void ZFObjectManger::UnLink(ZFObject* pkObject)
{
	if(pkObject->m_pkParent == NULL)	
		return;

	pkObject->m_pkParent = NULL;
	
}

void ZFObjectManger::PrintObjects(void)
{
	for(unsigned int i=0; i < kObjectNames.size();i++) {
		Logf(" %s, %d\n", kObjectNames[i].m_strName.c_str(), kObjectNames[i].m_iNumOfRequests );
	}

}

void ZFObjectManger::PrintObjectsHer(void)
{
	g_Logf("ZFObjectManger::PrintObjectsHer\n");

	for(unsigned int i=0; i < kObjectNames.size();i++) {
		if(kObjectNames[i].pkObject->m_pkParent == NULL) {
			kObjectNames[i].pkObject->PrintChilds("");
		}
	}

}

ZFCmdData* ZFObjectManger::FindArea(const char* szName)
{
	for(unsigned int i=0; i<m_kCmdDataList.size(); i++) {
		if(m_kCmdDataList[i].m_strName == szName)
			return &m_kCmdDataList[i];
	}

	return NULL;
}


bool ZFObjectManger::Register_Cmd(char* szName, int iCmdID, ZFObject* kObject,char* szHelp, int iNumOfArg)
{
	// Validate parameters
	if(szName == NULL)	return false;
	if(kObject == NULL)	return false;

	// Check if there is already something with this name.
	if(FindArea(szName)) 
		return false;
	

	ZFCmdData kNewCmd;
	kNewCmd.m_strName	= string(szName);
	kNewCmd.m_eType		= CSYS_FUNCTION;
	kNewCmd.m_iFlags	= 0;
	kNewCmd.m_vValue	= NULL;
	kNewCmd.m_pkObject	= kObject;
	kNewCmd.m_iCmdID	= iCmdID;
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

bool ZFObjectManger::UnRegister_Cmd(ZFObject* kObject)
{
#ifdef _DEBUG
	g_Logf("'%s' will no longer handle any commands\n", kObject->m_strZFpsName);
#endif
	return true;
}

bool ZFObjectManger::RunCommand(const char* szCmdArg)
{
	CmdArgument kcmdargs;
	kcmdargs.Set(szCmdArg);

	ZFCmdData* kCmdData = FindArea(kcmdargs.m_kSplitCommand[0].c_str());

	if(!kCmdData)	return false;

	switch(kCmdData->m_eType) {
		case CSYS_FUNCTION:	
			if(kcmdargs.m_kSplitCommand.size() <= kCmdData->m_iMinNumOfArgs) {
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

bool ZFObjectManger::Log_Create(const char* szName)
{
	ZFLogFile NewLogFile;
	NewLogFile.m_strName = szName;
	NewLogFile.m_strFileName = string(szName) + ".txt";
	
	NewLogFile.m_pkFilePointer = fopen(NewLogFile.m_strFileName.c_str(), "wt");
	setvbuf(NewLogFile.m_pkFilePointer , NULL, _IONBF, 0);		// Set Non buffer mode.
	
	m_kLogFiles.push_back( NewLogFile );

	return true;
}

void ZFObjectManger::Log_Destory(const char* szName)
{
	vector<ZFLogFile>::iterator it;

	for(it = m_kLogFiles.begin(); it != m_kLogFiles.end(); it++ ) {
		if(it->m_strName == string(szName)) {
			fclose(it->m_pkFilePointer);
			it = m_kLogFiles.erase(it);
			}
		}
}

ZFLogFile*	ZFObjectManger::Log_Find(const char* szName)
{
	string strHataStl;

	for(int i=0; i<m_kLogFiles.size(); i++) {
		strHataStl = m_kLogFiles[i].m_strName ;
		if(m_kLogFiles[i].m_strName == string(szName)) 
			return &m_kLogFiles[i];
		}

	return NULL;
}

void ZFObjectManger::Log_DestroyAll()
{
	for(int i=0; i<m_kLogFiles.size(); i++) {
		fclose(m_kLogFiles[i].m_pkFilePointer);
		}
}

void ZFObjectManger::Log(const char* szMessage)
{
	if(!m_pkLogFile)
		return;

	fprintf(m_pkLogFile, szMessage);
}

void ZFObjectManger::Log(const char* szName, const char* szMessage)
{
	ZFLogFile* pkLog = Log_Find( szName );
	if(!pkLog)
		return;

	fprintf(pkLog->m_pkFilePointer, szMessage);
}

char g_LogFormatTxt2[4096];	

void ZFObjectManger::Logf(const char* szName, const char* szMessageFmt,...)
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





bool ZFObjectManger::RegisterVariable(const char* szName, void* pvAddress, ZFCmdDataType eType)
{
	// Validate parameters
	if(szName == NULL)		return false;
	if(pvAddress == NULL)	return false;

	// Check if there is already something with this name.
	if(FindArea(szName)) 
		return false;

	ZFCmdData kNewCmd;
	kNewCmd.m_strName		= string(szName);
	kNewCmd.m_eType			= eType;
	kNewCmd.m_iFlags		= 0;
	kNewCmd.m_vValue		= pvAddress;
	kNewCmd.m_pkObject		= NULL;
	kNewCmd.m_iCmdID		= 0;
	kNewCmd.m_iMinNumOfArgs = 0;

	m_kCmdDataList.push_back(kNewCmd);

#ifdef _DEBUG
	Logf("Variable '%s' registred.\n", szName);
#endif
	return true;
	
}

bool ZFObjectManger::SetVariable(const char* szName, const char* szValue)
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


void ZFObjectManger::SetValue(ZFCmdData* pkArea, const char* szValue) 
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

void ZFObjectManger::SetString(ZFCmdData* pkArea, const char* szValue) 
{
	(*(string*)pkArea->m_vValue)=szValue;
}

void* ZFObjectManger::GetVar(ZFCmdData* pkArea)
 {
	return (void*)pkArea->m_vValue;
}

string ZFObjectManger::GetVarValue(ZFCmdData* pkArea)
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



void ZFObjectManger::PrintVariables()
{
	BasicConsole*		m_pkCon;
	m_pkCon = dynamic_cast<BasicConsole*>(g_ZFObjSys.GetObjectPtr("Console"));
	string strValue;

	m_pkCon->Printf("### variable list ###");

	for(unsigned int i=0; i<m_kCmdDataList.size(); i++) {
			strValue = GetVarValue(&m_kCmdDataList[i]);
			m_pkCon->Printf(" %s = [ %s]",m_kCmdDataList[i].m_strName.c_str(), strValue.c_str());
	}
}


bool ZFObjectManger::StartUp()
{
	g_Logf("Start Engine SubSystems: \n");


	for(unsigned int i=0; i < kObjectNames.size();i++) {
		Logf("Start %s: ",kObjectNames[i].m_strName.c_str());

		if(!kObjectNames[i].pkObject->StartUp())
			return false;

		g_Logf("Ok\n");
	}
	
	return true;
}

bool ZFObjectManger::ShutDown()
{
	g_Logf("ShutDown Engine SubSystems: \n");
	for(unsigned int i=0; i < kObjectNames.size();i++) {
		g_Logf("ShutDown %s: ",kObjectNames[i].m_strName.c_str());
		if(!kObjectNames[i].pkObject->ShutDown())
			return false;
		g_Logf("Ok\n");
	}
	
	return true;
}

bool ZFObjectManger::IsValid()
{
	for(unsigned int i=0; i < kObjectNames.size();i++) {
		if(!kObjectNames[i].pkObject->IsValid())
			return false;
	}
	
	return true;
}

