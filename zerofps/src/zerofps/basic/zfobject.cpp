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


ZFObject::ZFObject(char *szName)
{
	m_strZFpsName = string("");
	m_pkParent = NULL;
	g_ZFObjSys.Register(this, szName, NULL);
}

ZFObject::~ZFObject()
{
	DestroyChildren();
	g_ZFObjSys.UnRegister(this);
}

int ZFObject::GetNumChildren() const
{
	return m_akChild.size();
}

ZFObject*	ZFObject::GetParent() const
{
	return m_pkParent;
}

void ZFObject::PrintChilds(const char* szParentName)
{
	string FullName = string(szParentName) + "/" + m_strZFpsName;
	cout << FullName << endl;

	for(unsigned int i=0; i < m_akChild.size(); i++) {
		m_akChild[i]->PrintChilds(FullName.c_str());
		}
}

void ZFObject::AddChild(ZFObject* pkObject)
{
	m_akChild.push_back(pkObject);
}

void ZFObject::RemoveChild(ZFObject* pkObject)
{
	int iIndex = GetChildIndex(pkObject);
	if(iIndex == NOOBJECT_INDEX)
		return;

//	m_akChild.erase(&m_akChild[iIndex]);
	pkObject->m_pkParent = NULL;
}

int ZFObject::GetChildIndex(ZFObject* pkChild)
{
	for(unsigned int i=0; i < m_akChild.size(); i++) {
		if(pkChild == m_akChild[i])
			return i;
		}

	return NOOBJECT_INDEX;
}

int ZFObject::GetChildIndex(char* szName)
{
	string strName(szName);

	for(unsigned int i=0; i < m_akChild.size(); i++) {
		if(strName == m_akChild[i]->m_strZFpsName)
			return i;
		}

	return NOOBJECT_INDEX;
}

ZFObject* ZFObject::GetChildPtr(int iIndex)
{
	if(iIndex >= 0 || (unsigned int)iIndex < m_akChild.size())
		return m_akChild[iIndex];

	return NULL;
}


ZFObject* ZFObject::GetChildPtr(char* szName)
{
	string strName(szName);

	for(unsigned int i=0; i < m_akChild.size(); i++) {
		if(strName == m_akChild[i]->m_strZFpsName)
			return m_akChild[i];
		}

	return NULL;
}

void ZFObject::DestroyChildren()
{
	vector<ZFObject*>::iterator itChild;
	
	for(itChild = m_akChild.begin(); itChild != m_akChild.end(); itChild++)
	{
		(*itChild)->DestroyChildren();
		delete (*itChild);
	}

}

