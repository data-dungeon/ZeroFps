#ifndef _ENGINE_PROPERTYFACTORY_H_
#define _ENGINE_PROPERTYFACTORY_H_

#include <string>
#include <iostream>
#include <vector>

#include "engine_x.h"
//#include "engine.pkg"

using namespace std;

class NetWork;

/// Links property name to a create function.
class ProperyCreateLink
{
public:
	char		m_szName[50];
	Property*	(*Create)();

	int			iTotalCreated;
	int			iCount;

	ProperyCreateLink()
	{
		iTotalCreated = 0;
		iCount = 0;
	}
};

/// Engine System that creates propertys.
class ENGINE_API PropertyFactory : public ZFSubSystem
{
private:
	vector<ProperyCreateLink>	m_kProperyLinks; 
	void RunCommand(int cmdid, const CmdArgument* kCommand) { }

	NetWork*			m_pkNetWork;

public:
	PropertyFactory();
	~PropertyFactory() { }

	Property*	CreateProperty(const char* szName);
	void		Register(char* szName, Property*	(*Create)());
	
	void		Display();

	void GetAllProperties(vector<string>& vkList);

	bool StartUp();
	bool ShutDown();
	bool IsValid();

};

class ZeroFps;

#endif
