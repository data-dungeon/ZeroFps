#include "zerofps.h"
#include "../basic/zfobjectmanger.h"

ZFResource* Create__Mad_Core();
ZFResource* Create__ResTexture();

int	g_iResourceHandleID;

ZFResourceInfo::ZFResourceInfo()
{
	m_iID			= -1;
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
	ZFResourceDB* pkResDB = static_cast<ZFResourceDB*>(g_ZFObjSys.GetObjectPtr("ZFResourceDB"));
	return pkResDB->GetResourcePtr(*this);
}


// ZFResourceDB ***********************************************************

ZFResourceDB::ZFResourceDB()
 : ZFObject("ZFResourceDB") 
{
	m_iNextID = 0;

	RegisterResource( string(".mad"), Create__Mad_Core		);
	RegisterResource( string(".tga"), Create__ResTexture	);
	RegisterResource( string(".bmp"), Create__ResTexture	);

	g_ZFObjSys.Register_Cmd("res_list",FID_LISTRES,this);
	g_ZFObjSys.Log_Create("resdb");
}

ZFResourceDB::~ZFResourceDB()
{
	
}

void ZFResourceDB::Refresh()
{
	list<ZFResourceInfo*>::iterator it;

	for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) {
		if((*it)->m_iNumOfUsers == 0) {
			g_ZFObjSys.Logf("resdb", "Remove %s\n", (*it)->m_strName.c_str());
			delete (*it);
			it = m_kResources.erase(it);
			}
		}

	ZeroFps* pkZeofps = dynamic_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) {
		pkZeofps->DevPrintf("res", "%s - %d", (*it)->m_strName.c_str(), (*it)->m_iNumOfUsers);
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
	for(int i=0; i<m_kResourceFactory.size(); i++) {
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
	ZeroFps* pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	list<ZFResourceInfo*>::iterator it;

	switch (cmdid) {
		case FID_LISTRES:

		for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) {
			pkZeroFps->DevPrintf("res", "- %s - %d",(*it)->m_strName.c_str(),(*it)->m_iNumOfUsers);
			}
			break;

		};
}









