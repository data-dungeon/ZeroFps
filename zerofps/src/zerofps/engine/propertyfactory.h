#ifndef _ENGINE_PROPERTYFACTORY_H_
#define _ENGINE_PROPERTYFACTORY_H_

#include "../basic/basic.pkg"
#include <string>
#include <iostream>
#include <vector>

#include "engine_x.h"
//#include "engine.pkg"

using namespace std;

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
class ENGINE_API PropertyFactory : public ZFObject
{
private:
	vector<ProperyCreateLink>	m_kProperyLinks; 

	void RunCommand(int cmdid, const CmdArgument* kCommand) { }

public:
	PropertyFactory() : ZFObject("PropertyFactory") { }
	~PropertyFactory() { }

	Property*	CreateProperty(const char* szName);
	void		Register(char* szName, Property*	(*Create)());
	
	void		Display();

	void GetAllProperties(vector<string>& vkList);
};

class ZeroFps;

#endif
