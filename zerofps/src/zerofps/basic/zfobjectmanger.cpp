#include <iostream>
#include <string>
using namespace std;

#include "zfobjectmanger.h"

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



ZFObjectManger::ZFObjectManger()
{
#ifdef _DEBUG
	cout << "Starting Object System" << endl;
#endif
}

ZFObjectManger::~ZFObjectManger()
{
#ifdef _DEBUG
	cout << "Closing Object System" << endl;
#endif

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
}

ZFObject* ZFObjectManger::GetObjectPtr(char* acName)
{
	string Test(acName);

	for(int i=0; i < kObjectNames.size();i++) {
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

	for(int i=0; i < kObjectNames.size();i++) {
		cout << kObjectNames[i].m_strName << " , " << kObjectNames[i].m_iNumOfRequests <<  endl;
	}

}

void ZFObjectManger::PrintObjectsHer(void)
{
	cout << "OBJECTS: "<< endl;

	for(int i=0; i < kObjectNames.size();i++) {
		if(kObjectNames[i].pkObject->m_pkParent == NULL) {
			kObjectNames[i].pkObject->PrintChilds("");
		}
	}

}
