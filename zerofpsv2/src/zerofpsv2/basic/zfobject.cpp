#include <iostream>
#include <string>

#include <iostream>
#include <string>

#include "basic_x.h"
#include "os.h"
#include "zfobject.h"
#include "zfobjectmanger.h"

using namespace std;

BASIC_API ZFObjectManger g_ZFObjSys;


ZFSubSystem::ZFSubSystem(char *szName)
{
	m_strZFpsName = string("");
	m_pkParent = NULL;
	m_pkObjectManger = NULL;
	g_ZFObjSys.Register(this, szName, NULL);
}

ZFSubSystem::~ZFSubSystem()
{
	DestroyChildren();
	g_ZFObjSys.UnRegister(this);
}

int ZFSubSystem::GetNumChildren() const
{
	return m_akChild.size();
}

ZFSubSystem*	ZFSubSystem::GetParent() const
{
	return m_pkParent;
}

void ZFSubSystem::PrintChilds(const char* szParentName)
{
	string FullName = string(szParentName) + "/" + m_strZFpsName;
	cout << FullName << endl;

	for(unsigned int i=0; i < m_akChild.size(); i++) {
		m_akChild[i]->PrintChilds(FullName.c_str());
		}
}

void ZFSubSystem::AddChild(ZFSubSystem* pkObject)
{
	m_akChild.push_back(pkObject);
}

void ZFSubSystem::RemoveChild(ZFSubSystem* pkObject)
{
	int iIndex = GetChildIndex(pkObject);
	if(iIndex == NOOBJECT_INDEX)
		return;

//	m_akChild.erase(&m_akChild[iIndex]);
	pkObject->m_pkParent = NULL;
}

int ZFSubSystem::GetChildIndex(ZFSubSystem* pkChild)
{
	for(unsigned int i=0; i < m_akChild.size(); i++) {
		if(pkChild == m_akChild[i])
			return i;
		}

	return NOOBJECT_INDEX;
}

int ZFSubSystem::GetChildIndex(char* szName)
{
	string strName(szName);

	for(unsigned int i=0; i < m_akChild.size(); i++) {
		if(strName == m_akChild[i]->m_strZFpsName)
			return i;
		}

	return NOOBJECT_INDEX;
}

ZFSubSystem* ZFSubSystem::GetChildPtr(int iIndex)
{
	if(iIndex >= 0 || (unsigned int)iIndex < m_akChild.size())
		return m_akChild[iIndex];

	return NULL;
}


ZFSubSystem* ZFSubSystem::GetChildPtr(char* szName)
{
	string strName(szName);

	for(unsigned int i=0; i < m_akChild.size(); i++) {
		if(strName == m_akChild[i]->m_strZFpsName)
			return m_akChild[i];
		}

	return NULL;
}

void ZFSubSystem::DestroyChildren()
{
	vector<ZFSubSystem*>::iterator itChild;
	
	for(itChild = m_akChild.begin(); itChild != m_akChild.end(); itChild++)
	{
		(*itChild)->DestroyChildren();
		delete (*itChild);
	}

}

/**
	dfsad
  */
bool ZFSubSystem::StartUp()
{
	return true;
}

/**
	sgdsfg
  */
bool ZFSubSystem::ShutDown()
{
	return true;
	
}

/**
	dghdfj
*/
bool ZFSubSystem::IsValid()
{
	return true;

}
