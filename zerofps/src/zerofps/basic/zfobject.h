#ifndef _BASIC_ZFOBJECT_H_XXXX
#define _BASIC_ZFOBJECT_H_XXXX

#include <string>
#include <vector>
#include "basic_x.h"
#include "os.h"

using namespace std;

#define NOOBJECT_INDEX	-1

class ZFObject;

class BASIC_API CmdArgument
{
public:
	string				m_strFullCommand;
	vector<string>		m_kSplitCommand;

	void Set(const char* szCmdArgs);
};

/**
	Base class for all Engine Systems like graphic, audio,...
*/
 
class BASIC_API ZFObject
{
private:
	string					m_strZFpsName;			///<	Name of this objekt.
	void DestroyChildren();							///<	Remove and delete children.
		
protected:
	vector<ZFObject*>		m_akChild;				///<	List of all object we own.
	ZFObject*				m_pkParent;				///<	Ptr to object that own us.

	ZFObject(char *szName);					

public:
	virtual void RunCommand(int cmdid, const CmdArgument* kCommand) {};

	ZFObject*	GetParent() const;					///< Get ptr to object parent.

	int GetNumChildren() const;						///< Get num of children we have.
	int GetChildIndex(ZFObject* pkChild);			///< Get index for child.
	int GetChildIndex(char* szName);				///< Get index for child.
	ZFObject* GetChildPtr(int iIndex);				///< Get ptr to child.
	ZFObject* GetChildPtr(char* szName);			///< Get ptr to child.

	void AddChild(ZFObject* pkObject);				///< Make a object child to this.
	void RemoveChild(ZFObject* pkObject);			///< Remove one of our children.

	void PrintChilds(const char* szParentName);		///< Debug: Prints childs from this object. 

	virtual ~ZFObject();

	friend class ZFObjectManger;

	virtual bool StartUp();
	virtual bool ShutDown();
	virtual bool IsValid();
};

#endif






