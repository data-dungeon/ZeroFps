#ifndef _ZFOBJECT_H_
#define _ZFOBJECT_H_

#include <string>
#include <vector>
#include "basic_x.h"

using namespace std;

class ZFObjectManger;
class ZFObject;

#define NOOBJECT_INDEX	-1

class BASIC_API ZFObject
{
private:
	string					m_strZFpsName;	// Name of this objekt.
	void DestroyChildren();					// Remove and delete children.
		
protected:
	vector<ZFObject*>		m_akChild;		// List of all object we own.
	ZFObject*				m_pkParent;		// Ptr to object that own us.

	ZFObject(char *szName);					

public:
	ZFObject*	GetParent() const;			// Get ptr to object parent.

	int GetNumChildren() const;				// Get num of children we have.
	int GetChildIndex(ZFObject* pkChild);	// Get index for child.
	int GetChildIndex(char* szName);		// Get index for child.
	ZFObject* GetChildPtr(int iIndex);		// Get ptr to child.
	ZFObject* GetChildPtr(char* szName);	// Get ptr to child.

	void AddChild(ZFObject* pkObject);		// Make a object child to this.
	void RemoveChild(ZFObject* pkObject);	// Remove one of our children.

	void PrintChilds(const char* szParentName);

	virtual ~ZFObject();

	friend ZFObjectManger;
};

#endif