//#include "../engine/zerofps.h"
 #include <SDL/SDL.h>
#include "zfresourcedb.h"
#include "globals.h"
#include "zfsystem.h"
#include "assert.h"
#include "concommand.h"

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
	if(m_pkResource) 
		delete m_pkResource;
}

// ZFResourceHandle ***********************************************************

bool ZFResourceHandle::GetResDB()
{
	if(m_pkResDB)
		return true;

	m_pkResDB = static_cast<ZSSResourceDB*>(g_ZFObjSys.GetObjectPtr("ZSSResourceDB"));
	
	if(!m_pkResDB)
	{
		cerr<<"WARNING: ZFResourceHandle could not get resDB pointer"<<endl;
		return false;
	}
	
	return true;
}

ZFResourceHandle::ZFResourceHandle()
{
 	m_pkResDB = NULL;
	m_pkResource = NULL;
}

ZFResourceHandle::ZFResourceHandle(const ZFResourceHandle& kOther)
{
	m_pkResDB = NULL;	
	m_pkResource = NULL;
	
	SetRes(kOther.m_strName);
}

ZFResourceHandle& ZFResourceHandle::operator=(const ZFResourceHandle& kOther)
{
 	if(!kOther.m_pkResource)
		FreeRes();
	else
		SetRes(kOther.m_strName);
		
	return *this;
}

ZFResourceHandle::~ZFResourceHandle()
{
	FreeRes();
}

bool ZFResourceHandle::SetRes(const string& strName)
{
	FreeRes();

	if(!GetResDB())
		return false;

   m_pkResDB->GetResource(*this,strName);

	if(!m_pkResource)
		return false;
		
	return true;
}

void ZFResourceHandle::FreeRes()
{
	if(!m_pkResource)
		return;

	if(!GetResDB())
		return;
		
	m_pkResDB->FreeResource(*this);
}



// ZFResourceDB ***********************************************************

ResourceCreateLink*	ZSSResourceDB::FindResourceTypeFromFullName(string strResName)
{
	char szFullName[256];
	strcpy(szFullName, strResName.c_str());
	Gemens(szFullName);


	char* pcExt = strrchr(szFullName, '.');
	if(!pcExt)
		return NULL;

	return FindResourceType(string(pcExt));
}

void ZSSResourceDB::RunCommand(int cmdid, const ConCommandLine* kCommand)
{
	vector<ZFResourceInfo*>::iterator it;

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
			if(kCommand->m_kSplitCommand.size() >= 2)
			{
				ReloadAllResorces(kCommand->m_kSplitCommand[1]);		
			}
			else
				ReloadAllResorces();		
			
			break;
		};
}

ZSSResourceDB::ZSSResourceDB() : ZFSubSystem("ZSSResourceDB") 
{
	m_iNextID			= 0;
	m_bInstantExpire	= false;

	Register_Cmd("res_list",		FID_LISTRES,			CSYS_FLAG_SRC_ALL);
	Register_Cmd("res_types",		FID_LISTTYPES,			CSYS_FLAG_SRC_ALL);
	Register_Cmd("res_reload",		FID_RESRELOAD,			CSYS_FLAG_SRC_ALL);
	Register_Cmd("res_reloadall",	FID_RESRELOADALL,		CSYS_FLAG_SRC_ALL);

	

}

ZSSResourceDB::~ZSSResourceDB()
{
	
}

bool ZSSResourceDB::StartUp()	
{ 
	g_ZFObjSys.Log_Create("resdb");
	return true; 
}

bool ZSSResourceDB::ShutDown() 
{ 
	m_bInstantExpire = true;

	printf("Unloading Res: ");
	do 
	{
		printf(".");
	} while( Refresh());
	printf("\n");

	
	if(m_kResources.size()) 
	{
		//Logf("resdb", "ZFResourceDB::ShutDown: There are res left\n");
		
		vector<ZFResourceInfo*>::iterator it;

		for(it = m_kResources.begin(); it != m_kResources.end(); it++ )
			g_ZFObjSys.Logf("resdb", " [%d] = %s\n", (*it)->m_iNumOfUsers, (*it)->m_strName.c_str() );
	}

	return true; 
}

bool ZSSResourceDB::IsValid()	{ return true; }

/**	Register a new Resource and the function used to create it. */
void ZSSResourceDB::RegisterResource(const string& strName, ZFResource* (*Create)())
{
	ResourceCreateLink kResLink;
	kResLink.m_strName = strName;
	kResLink.Create = Create;
	kResLink.m_iActive = 0;
	kResLink.m_iIndex = m_kResourceFactory.size();

	m_kResourceFactory.push_back(kResLink);
}

/**	Refresh the resource DB by unloading unused resources. */
bool ZSSResourceDB::Refresh()
{
	bool bWasUnloaded = false;
	bool bHaveLoaded = false;
	
	float fTime = float(SDL_GetTicks()/1000.0);
	
	for(vector<ZFResourceInfo*>::iterator it = m_kResources.begin();it != m_kResources.end();it++)
	{
		ZFResourceInfo* pkRes = 	*it;
	
		//check if created
		if(!pkRes->m_bIsCreated)
		{
			if(!bHaveLoaded)
			{
				//cout<<"asyncronus loading of :"<<pkRes->m_strName<<endl;			
				pkRes->m_pkResource->Create( pkRes->m_strName );
				pkRes->m_bIsCreated = true;
				bHaveLoaded = true;
			}
		}
	
	
		//still in use
		if(pkRes->m_iNumOfUsers != 0)	continue;
		
		//update timeout
		if( (!m_bInstantExpire)  && (pkRes->m_fExpireTimer == 0) ) 
		{
			pkRes->m_fExpireTimer = fTime + float(RES_EXPIRE_TIME);
			//cout << "Set Expire: '" << (*it)->m_strName << "'" << endl;
		}
		else 
		{
			//resource has expired
			if(m_bInstantExpire || (pkRes->m_fExpireTimer < fTime) ) 
			{				
				m_kResourceFactory[pkRes->m_pkResource->m_iTypeIndex].m_iActive --;
				delete pkRes;
				
				m_kResources.erase(it);
				it = m_kResources.begin();
				
				bWasUnloaded = true;
			}
		}			
	}
					

	return bWasUnloaded;
}

ZFResourceInfo*	ZSSResourceDB::GetResourceData(const string& strResName)
{
	return FindResource(strResName);
}

ZFResourceInfo*	ZSSResourceDB::FindResource(const string& strResName)
{
	for(unsigned int i = 0;i<m_kResources.size();i++)
		if(m_kResources[i]->m_strName == strResName)
			return m_kResources[i];

	return NULL;
}

bool ZSSResourceDB::IsResourceLoaded(string strResName)
{
	if(FindResource(strResName) != NULL)
		return true;
	
	return false;
}

void ZSSResourceDB::ReloadResource(ZFResourceInfo* pkResInfo)
{
	pkResInfo->m_pkResource->Create(pkResInfo->m_strName);

}


void ZSSResourceDB::ReloadResource(string strResName)
{
	ZFResourceInfo* pkResInfo = GetResourceData(strResName);

	if(!pkResInfo)
		return;

	ReloadResource( pkResInfo );
}

void ZSSResourceDB::ReloadAllResorces(const string& strName)
{
	if(strName.empty())
		for(unsigned int i = 0;i<m_kResources.size();i++)
			ReloadResource(m_kResources[i]->m_strName);
	else
		for(unsigned int i = 0;i<m_kResources.size();i++)
		{
// 			if(m_kResources[i]->m_strName.find(strName) != -1)
			if(strstr(m_kResources[i]->m_strName.c_str(),strName.c_str()) != 0 )
			{
				cout<<"Reloading :"<<m_kResources[i]->m_strName<<endl;
				ReloadResource(m_kResources[i]->m_strName);
			}
		}

// 	list<ZFResourceInfo*>::iterator it;
// 	for(it = m_kResources.begin(); it != m_kResources.end(); it++ ) 
// 	{
// 		ReloadResource((*it)->m_strName);
// 	}	
}


void ZSSResourceDB::GetResource(ZFResourceHandle& kResHandle,const string& strResName)
{
	
	//check if resource is already loaded			
	if(ZFResourceInfo* pkResInfo = GetResourceData(strResName)) 
	{
		// If Resource is loaded we inc ref and return handle to it.
	
		pkResInfo->m_iNumOfUsers++;
		
		//poit handle to resource
		kResHandle.m_strName		= pkResInfo->m_strName;
// 		kResHandle.m_iID			= pkResInfo->m_iID;
		kResHandle.m_pkResource = pkResInfo->m_pkResource;		
		return;
	}
	
	
	//create resource
	ZFResource* pkRes = CreateResource(strResName);

	// Failed to create resource.
	if(!pkRes) 
	{
		g_ZFObjSys.Logf("resdb", "Failed to create resource %s\n", strResName.c_str());
		//cerr<<"Failed to create resource "<<strResName.c_str()<<endl;
		return;
	}

	//if resource is synced, load it now
	if(pkRes->m_bSynced)
		if(!pkRes->Create(strResName.c_str())) 
		{
			g_ZFObjSys.Logf("resdb", "Failed to Load resource %s\n", strResName.c_str());
			//cerr<<"Failed to Load resource "<<strResName.c_str()<<endl;
			//return; //warning, dont return, leakage
		}

	//g_ZFObjSys.Logf("resdb", "Resource %s loaded\n", strResName.c_str());

	//create new resource info
	ZFResourceInfo* kResInfo = new ZFResourceInfo;			// LEAK - MistServer, Nothing loaded.
	kResInfo->m_iID			= m_iNextID++;
	kResInfo->m_iNumOfUsers	= 1;
	kResInfo->m_strName		= strResName;
	kResInfo->m_pkResource	= pkRes;
	kResInfo->m_bIsCreated	= pkRes->m_bSynced;

	m_kResources.push_back(kResInfo);

	//poit handle to resource
	kResHandle.m_strName	= 		kResInfo->m_strName;
// 	kResHandle.m_iID		= 		kResInfo->m_iID;
	kResHandle.m_pkResource =	kResInfo->m_pkResource;
	
	return;
}

void ZSSResourceDB::FreeResource(ZFResourceHandle& kResHandle)
{
	if(ZFResourceInfo* pkRes = GetResourceData(kResHandle.m_strName))
	{
		pkRes->m_iNumOfUsers --;
	}
	else
	{
		g_ZFObjSys.Logf("resdb", "FreeResource on non valid handle.\n", kResHandle.m_strName.c_str());
	}

	//clear resource handle
// 	kResHandle.m_iID = -1;
	kResHandle.m_strName = "";
 	kResHandle.m_pkResource = NULL;
}

ZFResource* ZSSResourceDB::GetResourcePtr(ZFResourceHandle& kResHandle)
{
	ZFResourceInfo* pkRes = GetResourceData(kResHandle.m_strName);
	if(!pkRes)
		return NULL;

	return pkRes->m_pkResource;
}

ResourceCreateLink*	ZSSResourceDB::FindResourceType(const string& strName)
{
	for(unsigned int i=0; i<m_kResourceFactory.size(); i++) {
		if(m_kResourceFactory[i].m_strName == strName)
			return &m_kResourceFactory[i];
	}

	return NULL;
}

ZFResource*	ZSSResourceDB::CreateResource(const string& strName)
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
int ZSSResourceDB::GetResSizeInBytes()
{
	int iTotalBytes = 0;

	
	for(unsigned int i=0;i<m_kResources.size();i++)
		iTotalBytes += m_kResources[i]->m_pkResource->GetSize();
		
// 	list<ZFResourceInfo*>::iterator it;
// 
// 	for(it = m_kResources.begin(); it != m_kResources.end(); it++ )
// 		iTotalBytes += (*it)->m_pkResource->GetSize();

	return iTotalBytes;
}







