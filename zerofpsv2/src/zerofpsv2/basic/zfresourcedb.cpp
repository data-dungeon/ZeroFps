//#include "../engine/zerofps.h"
#include <SDL/SDL.h>
#include "zfresourcedb.h"
#include "globals.h"
#include "zfsystem.h"

#define	RES_EXPIRE_TIME	5.0

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
	g_ZFObjSys.Logf("resdb", "ZFResourceHandle::SetRes %d to  %s\n", m_iHandleID, strName.c_str());
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

ZFResourceDB::ZFResourceDB()
 : ZFSubSystem("ZFResourceDB") 
{
	m_iNextID = 0;

	Register_Cmd("res_list",	FID_LISTRES,	CSYS_FLAG_SRC_ALL);
	Register_Cmd("res_types",	FID_LISTTYPES,	CSYS_FLAG_SRC_ALL);
	g_ZFObjSys.Log_Create("resdb");
}

bool ZFResourceDB::StartUp()	
{ 
//	m_pkZeroFps = dynamic_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	return true; 
}

bool ZFResourceDB::ShutDown() 
{ 
	if(m_kResources.size()) {
		Logf("zerofps", "ZFResourceDB::ShutDown: There are res left\n");
		
		list<ZFResourceInfo*>::iterator it;

		for(it = m_kResources.begin(); it != m_kResources.end(); it++ )
			Logf("zerofps", " [] = %s\n", (*it)->m_strName.c_str() );
		}


	return true; 
}

bool ZFResourceDB::IsValid()	{ return true; }


ZFResourceDB::~ZFResourceDB()
{
	
}

void ZFResourceDB::Refresh()
{
	float fTime = float(SDL_GetTicks()/1000.0);

	list<ZFResourceInfo*>::iterator it;

	for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) {
		// If someone still using this res.
		if((*it)->m_iNumOfUsers != 0)	continue;

		// No one is using it. Check for expire time.
		if((*it)->m_fExpireTimer == 0) {
			(*it)->m_fExpireTimer = fTime + RES_EXPIRE_TIME;
			cout << "Set Expire: '" << (*it)->m_strName << "'" << endl;
			}
		else {
			if((*it)->m_fExpireTimer < fTime) {
				// Time to die.
				g_ZFObjSys.Logf("resdb", "Remove %s\n", (*it)->m_strName.c_str());
				cout << "Expires: '" << (*it)->m_strName << "'" << endl;
				delete (*it);
				it = m_kResources.erase(it);
				}
			}
		}

	for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) {
//		m_pkZeroFps->DevPrintf("res", "%s - %d", (*it)->m_strName.c_str(), (*it)->m_iNumOfUsers);
		}
}



ZFResourceInfo*	ZFResourceDB::GetResourceData(string strResName)
{
	return FindResource(strResName);
/*	if(iResID == -1)
		return NULL;

	return m_kResources[iResID];*/

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
		g_ZFObjSys.Logf("resdb", "Failed to create resource %s\n", strResName.c_str());
//		cout<<"Error: failed to create resource "<<strResName<<endl;
		return;
		}

	//Mad_Core* pkCore = new Mad_Core;
	if(pkRes->Create(strResName.c_str()) == false) {
		g_ZFObjSys.Logf("resdb", "Failed to Load resource %s\n", strResName.c_str());
		return;
		}

	g_ZFObjSys.Logf("resdb", "Resource %s loaded\n", strResName.c_str());

	ZFResourceInfo* kResInfo = new ZFResourceInfo;
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
		g_ZFObjSys.Logf("resdb", "FreeResource on non valid handle.\n", kResHandle.m_strName.c_str());
		return;
		}

	pkRes->m_iNumOfUsers --;
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

	ZFResource* pkRes = pkLink->Create();
	return pkRes;
}

void ZFResourceDB::RegisterResource(string strName, ZFResource* (*Create)())
{
	ResourceCreateLink kResLink;
	kResLink.m_strName = strName;
	kResLink.Create = Create;

	m_kResourceFactory.push_back(kResLink);
}


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
			string hora = (*it)->m_strName.c_str();
			cout << "Hora: " <<  hora << endl;

			GetSystem().Printf("- %s - %d - %d",(*it)->m_strName.c_str(),(*it)->m_iNumOfUsers, (*it)->m_pkResource->GetSize());
			}
			break;

		case FID_LISTTYPES:
			GetSystem().Printf("Resource types: ");
			for(unsigned int i=0; i<m_kResourceFactory.size(); i++) {
				GetSystem().Printf(" [%d] = %s",i, m_kResourceFactory[i].m_strName.c_str());
				}
			break;
		};
}

int ZFResourceDB::GetResSizeInBytes()
{
	int iTotalBytes = 0;

	list<ZFResourceInfo*>::iterator it;

	for(it = m_kResources.begin(); it != m_kResources.end(); it++ )
		iTotalBytes += (*it)->m_pkResource->GetSize();

	return iTotalBytes;
}







