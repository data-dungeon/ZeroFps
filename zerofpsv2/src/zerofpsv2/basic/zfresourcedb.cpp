//#include "../engine/zerofps.h"
 #include <SDL/SDL.h>
#include "zfresourcedb.h"
#include "globals.h"
#include "zfsystem.h"
#include "assert.h"

#define	RES_EXPIRE_TIME	30.0

int	g_iResourceHandleID;

ZFResourceInfo::ZFResourceInfo()
{
	m_iID				= -1;
	m_iNumOfUsers	= 0;
	m_fExpireTimer	= 0.0;
	m_pkResource	= NULL;
}

ZFResourceInfo::~ZFResourceInfo()
{
	if(m_pkResource) {
		delete m_pkResource;
		}
}

// ZFResourceHandle ***********************************************************

ZFResourceHandle::ZFResourceHandle()
{
	m_iHandleID = g_iResourceHandleID ++;
	m_iID = -1;
}

ZFResourceHandle::ZFResourceHandle(const ZFResourceHandle& kOther)
{
	m_iHandleID = g_iResourceHandleID ++;
	m_iID = -1;
	SetRes(kOther.m_strName);
}

ZFResourceHandle& ZFResourceHandle::operator=(const ZFResourceHandle& kOther)
{
	SetRes(kOther.m_strName);
	return *this;
}

ZFResourceHandle::~ZFResourceHandle()
{
	FreeRes();
}

bool ZFResourceHandle::SetRes(string strName)
{
	FreeRes();
//	g_ZFObjSys.Logf("resdb", "ZFResourceHandle::SetRes %d to  %s\n", m_iHandleID, strName.c_str());
//	cout << "ZFResourceHandle::SetRes: " << m_iHandleID << " to " << strName << endl;

	m_strName = strName;
	ZFResourceDB* pkResDB = static_cast<ZFResourceDB*>(g_ZFObjSys.GetObjectPtr("ZFResourceDB"));
   pkResDB->GetResource(*this,strName);

	if(m_iID == -1)
		return false;
		
	return true;
}

void ZFResourceHandle::FreeRes()
{
	if(m_iID == -1)
		return;

//	g_ZFObjSys.Logf("resdb", "ZFResourceHandle::FreeRes %d %s\n", m_iHandleID, m_strName.c_str());

	ZFResourceDB* pkResDB = static_cast<ZFResourceDB*>(g_ZFObjSys.GetObjectPtr("ZFResourceDB"));
	pkResDB->FreeResource(*this);
	m_iID = -1;
	m_strName = "";
}

bool  ZFResourceHandle::IsValid()
{
	if(m_iID == -1)
		return false;

	return true;
}

ZFResource* ZFResourceHandle::GetResourcePtr()
{
	if(m_iID == -1)
		return NULL;
	
	ZFResourceDB* pkResDB = static_cast<ZFResourceDB*>(g_ZFObjSys.GetObjectPtr("ZFResourceDB"));
	return pkResDB->GetResourcePtr(*this);
}


// ZFResourceDB ***********************************************************

ResourceCreateLink*	ZFResourceDB::FindResourceTypeFromFullName(string strResName)
{
	char szFullName[256];
	strcpy(szFullName, strResName.c_str());
	Gemens(szFullName);


	char* pcExt = strrchr(szFullName, '.');
	if(!pcExt)
		return NULL;

	return FindResourceType(string(pcExt));
}

void ZFResourceDB::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	list<ZFResourceInfo*>::iterator it;

	switch (cmdid) {
		case FID_LISTRES:

		for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) {
			GetSystem().Printf("- %s - %d - %d",(*it)->m_strName.c_str(),(*it)->m_iNumOfUsers, (*it)->m_pkResource->GetSize());
			}
			break;

		case FID_LISTTYPES:
			GetSystem().Printf("Resource types: ");
			for(unsigned int i=0; i<m_kResourceFactory.size(); i++) {
				GetSystem().Printf(" [%d] = %s, %d",i, m_kResourceFactory[i].m_strName.c_str(),m_kResourceFactory[i].m_iActive);
				}
			break;

		case FID_RESRELOAD:
			if(kCommand->m_kSplitCommand.size() >= 2)
			{
				ReloadResource(kCommand->m_kSplitCommand[1]);
			}
			break;

		case FID_RESRELOADALL:
				ReloadAllResorces();
			break;
		};
}

ZFResourceDB::ZFResourceDB()
 : ZFSubSystem("ZFResourceDB") 
{
	m_iNextID			= 0;
	m_bInstantExpire	= false;

	Register_Cmd("res_list",		FID_LISTRES,			CSYS_FLAG_SRC_ALL);
	Register_Cmd("res_types",		FID_LISTTYPES,			CSYS_FLAG_SRC_ALL);
	Register_Cmd("res_reload",		FID_RESRELOAD,			CSYS_FLAG_SRC_ALL);
	Register_Cmd("res_reloadall",	FID_RESRELOADALL,		CSYS_FLAG_SRC_ALL);

	

	g_ZFObjSys.Log_Create("resdb");
}

ZFResourceDB::~ZFResourceDB()
{
	
}

bool ZFResourceDB::StartUp()	
{ 
	return true; 
}

bool ZFResourceDB::ShutDown() 
{ 
	m_bInstantExpire = true;

	printf("Unloading Res: ");
	do {
		printf(".");
	} while( Refresh());
	printf("\n");

	
	if(m_kResources.size()) 
	{
		//Logf("resdb", "ZFResourceDB::ShutDown: There are res left\n");
		
		list<ZFResourceInfo*>::iterator it;

		for(it = m_kResources.begin(); it != m_kResources.end(); it++ )
			LOGF("resdb", " [%d] = %s\n", (*it)->m_iNumOfUsers, (*it)->m_strName.c_str() );
	}

	return true; 
}

bool ZFResourceDB::IsValid()	{ return true; }

/**	Register a new Resource and the function used to create it. */
void ZFResourceDB::RegisterResource(string strName, ZFResource* (*Create)())
{
	ResourceCreateLink kResLink;
	kResLink.m_strName = strName;
	kResLink.Create = Create;
	kResLink.m_iActive = 0;
	kResLink.m_iIndex = m_kResourceFactory.size();

	m_kResourceFactory.push_back(kResLink);
}

/**	Refresh the resource DB by unloading unused resources. */
bool ZFResourceDB::Refresh()
{
	bool bWasUnloaded = false;

	float fTime = float(SDL_GetTicks()/1000.0);

	list<ZFResourceInfo*>::iterator it;

	for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) {
		// If someone still using this res.
		if((*it)->m_iNumOfUsers != 0)	continue;

		// No one is using it. Check for expire time.
		if(m_bInstantExpire == false && (*it)->m_fExpireTimer == 0) {
			(*it)->m_fExpireTimer = fTime + float(RES_EXPIRE_TIME);
//			cout << "Set Expire: '" << (*it)->m_strName << "'" << endl;
			}
		else {
			if(m_bInstantExpire == true || (*it)->m_fExpireTimer < fTime) {
				// Time to die.
				// g_ZFObjSys.Logf("resdb", "Remove %s\n", (*it)->m_strName.c_str());
				//	cout << "Expires: '" << (*it)->m_strName << "'" << endl;
				
				m_kResourceFactory[(*it)->m_pkResource->m_iTypeIndex].m_iActive --;
				delete (*it);
				it = m_kResources.erase(it);
				bWasUnloaded = true;
				}
			}
		}

	for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) {
		//m_pkZeroFps->DevPrintf("res", "%s - %d", (*it)->m_strName.c_str(), (*it)->m_iNumOfUsers);
		}

	return bWasUnloaded;
}

ZFResourceInfo*	ZFResourceDB::GetResourceData(string strResName)
{
	return FindResource(strResName);
}

ZFResourceInfo*	ZFResourceDB::FindResource(string strResName)
{
	list<ZFResourceInfo*>::iterator it;

	for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) {
		if(strResName == (*it)->m_strName)
			return (*it);
		}

	return NULL;
}

bool ZFResourceDB::IsResourceLoaded(string strResName)
{
	if(FindResource(strResName) != NULL)
		return true;
	
	return false;
}

void ZFResourceDB::ReloadResource(ZFResourceInfo* pkResInfo)
{
	// Remove Old Resource
	delete pkResInfo->m_pkResource;
	pkResInfo->m_pkResource = NULL;

	// Create new resource.
	ZFResource* pkRes = CreateResource(pkResInfo->m_strName);

	// Failed to create resource.
	if(!pkRes) {
		LOGF("resdb", "Failed to create resource %s\n", pkResInfo->m_strName.c_str());
		return;
		}

	if(pkRes->Create(pkResInfo->m_strName.c_str()) == false) {
		LOGF("resdb", "Failed to Load resource %s\n", pkResInfo->m_strName.c_str());
		return;
		}

	 pkResInfo->m_pkResource = pkRes;
}


void ZFResourceDB::ReloadResource(string strResName)
{
	ZFResourceInfo* pkResInfo = GetResourceData(strResName);

	if(!pkResInfo)
		return;

	ReloadResource( pkResInfo );
}

void ZFResourceDB::ReloadAllResorces()
{
	list<ZFResourceInfo*>::iterator it;

	for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) 
	{
		ReloadResource((*it)->m_strName);
	}	
}


void ZFResourceDB::GetResource(ZFResourceHandle& kResHandle, string strResName)
{
	ZFResourceInfo* pkResInfo = GetResourceData(strResName);
	
	// If Resource is loaded we inc ref and return handle to it.
	if(pkResInfo) {
		pkResInfo->m_iNumOfUsers++;

		kResHandle.m_strName	= pkResInfo->m_strName;
		kResHandle.m_iID		= pkResInfo->m_iID;
		return;
		}

	// Create Res Class
	ZFResource* pkRes = CreateResource(strResName);

	// Failed to create resource.
	if(!pkRes) {
		LOGF("resdb", "Failed to create resource %s\n", strResName.c_str());
//		cout<<"Error: failed to create resource "<<strResName<<endl;
		return;
		}

	//Mad_Core* pkCore = new Mad_Core;
	if(pkRes->Create(strResName.c_str()) == false) {
		LOGF("resdb", "Failed to Load resource %s\n", strResName.c_str());
		return;
		}

	//g_ZFObjSys.Logf("resdb", "Resource %s loaded\n", strResName.c_str());

	ZFResourceInfo* kResInfo = new ZFResourceInfo;			// LEAK - MistServer, Nothing loaded.
	kResInfo->m_iID			= m_iNextID++;
	kResInfo->m_iNumOfUsers	= 1;
	kResInfo->m_strName		= strResName;
	kResInfo->m_pkResource	= pkRes;

	m_kResources.push_back(kResInfo);

	kResHandle.m_strName	= kResInfo->m_strName;
	kResHandle.m_iID		= kResInfo->m_iID;
	return;
}

void ZFResourceDB::FreeResource(ZFResourceHandle& kResHandle)
{
	ZFResourceInfo* pkRes = GetResourceData(kResHandle.m_strName);
	if(pkRes == NULL) {
		LOGF("resdb", "FreeResource on non valid handle.\n", kResHandle.m_strName.c_str());
		return;
		}

	pkRes->m_iNumOfUsers --;
	//g_ZFObjSys.Logf("resdb", "Res '%s' - %d.\n", pkRes->m_strName.c_str(), pkRes->m_iNumOfUsers);

	//if(pkRes->m_iNumOfUsers < 0)
	//	assert(0);
	kResHandle.m_iID = -1;
	kResHandle.m_strName = "";
}

ZFResource* ZFResourceDB::GetResourcePtr(ZFResourceHandle& kResHandle)
{
	ZFResourceInfo* pkRes = GetResourceData(kResHandle.m_strName);
	if(!pkRes)
		return NULL;

	return pkRes->m_pkResource;
}

ResourceCreateLink*	ZFResourceDB::FindResourceType(string strName)
{
	for(unsigned int i=0; i<m_kResourceFactory.size(); i++) {
		if(m_kResourceFactory[i].m_strName == strName)
			return &m_kResourceFactory[i];
	}

	return NULL;
}

ZFResource*	ZFResourceDB::CreateResource(string strName)
{
	ResourceCreateLink* pkLink = FindResourceTypeFromFullName(strName);
	if(pkLink == NULL)
		return NULL;

	pkLink->m_iActive ++;
	ZFResource* pkRes = pkLink->Create();
	if(pkRes)
		pkRes->m_iTypeIndex = pkLink->m_iIndex;
	return pkRes;
}



/**	Returns total size of all loaded resources. */
int ZFResourceDB::GetResSizeInBytes()
{
	int iTotalBytes = 0;

	list<ZFResourceInfo*>::iterator it;

	for(it = m_kResources.begin(); it != m_kResources.end(); it++ )
		iTotalBytes += (*it)->m_pkResource->GetSize();

	return iTotalBytes;
}







