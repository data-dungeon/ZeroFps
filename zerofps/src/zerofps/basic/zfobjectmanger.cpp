#include <iostream>
#include <string>
using namespace std;

#include "zfobjectmanger.h"

ZFObjectManger* ZFObjectManger::pkInstance;

/*
Manger:
	Register:		Registrerar object i object name sys.
	Unregister:		Removes object från object name sys.

	Link:			Länkar ett object till ett annat som child.
	UnLink:			Tar bort child från en parent.

	IsOk:

Object:
	Name				Namn på object.
	Childs:				Lista på children.
	Parent:				Pekare till parent.
	
	GetParent()			Get ptr to parent of object.
	GetNumChildren()	Totalt antal children.
	GetChild()			Pekare till child i.
	GetName()			Returnerar namnet på object.

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

#ifdef _DEBUG
	cout << "Starting Object System" << endl;
#endif

	m_pkLogFile = fopen("zerofps.txt", "wt");	// Open Master Log File.
	setvbuf(m_pkLogFile, NULL, _IONBF, 0);		// Set Non buffer mode.

}

ZFObjectManger::~ZFObjectManger()
{
#ifdef _DEBUG
	cout << "Closing Object System" << endl;
#endif

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
	cout << "Register '" << acName << "': ";
#endif

	// Make sure that there is no other object with the same name.
	ZFObject* pkCheck = GetObjectPtr(acName);
	if(pkCheck) {
		cout << "There is already a object with name " << acName << endl;
		return;
		}

	NameObject kObjName;
	kObjName.m_strName = string(acName);
	kObjName.pkObject = pkObject;
	kObjName.m_iNumOfRequests = 0;
	
	kObjectNames.push_back(kObjName);
	pkObject->m_strZFpsName = string(acName);

	

#ifdef _DEBUG
	cout << "Ok" << endl;
#endif
}

void ZFObjectManger::UnRegister(ZFObject* pkObject)
{
#ifdef _DEBUG
	cout << "UnRegister '" << pkObject->m_strZFpsName << "': ";
#endif

	vector<NameObject>::iterator itNames;
	for(itNames = kObjectNames.begin(); itNames != kObjectNames.end(); )
	{
		if(itNames->pkObject == pkObject) {
			itNames = kObjectNames.erase(itNames);
#ifdef _DEBUG
			cout << ".";
#endif
			}
		else
			itNames++;
	}

#ifdef _DEBUG
	cout << "Ok" << endl;
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
	cout << "OBJECTS: "<< endl;

	for(unsigned int i=0; i < kObjectNames.size();i++) {
		cout << kObjectNames[i].m_strName << " , " << kObjectNames[i].m_iNumOfRequests <<  endl;
	}

}

void ZFObjectManger::PrintObjectsHer(void)
{
	cout << "OBJECTS: "<< endl;

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


bool ZFObjectManger::Register_Cmd(char* szName, int iCmdID, ZFObject* kObject)
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

	m_kCmdDataList.push_back(kNewCmd);
	return true;
}

bool ZFObjectManger::UnRegister_Cmd(ZFObject* kObject)
{
	cout << "ZFObjectManger::UnRegister: " << kObject->m_strZFpsName << endl;
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
			kCmdData->m_pkObject->RunCommand(kCmdData->m_iCmdID, &kcmdargs);
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
	for(int i=0; i<m_kLogFiles.size(); i++) {
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
	fprintf(m_pkLogFile, szMessage);
}

void ZFObjectManger::Log(const char* szName, const char* szMessage)
{
	ZFLogFile* pkLog = Log_Find( szName );
	if(!pkLog)
		return;

	fprintf(pkLog->m_pkFilePointer, szMessage);
}

