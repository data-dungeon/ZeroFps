#ifndef _ENGINE_PROPERTYFACTORY_H_
#define _ENGINE_PROPERTYFACTORY_H_

#include <string>
#include <iostream>
#include <vector>

#include "engine_x.h"
//#include "engine.pkg"
#include "../basic/zfsubsystem.h"

using namespace std;

class ZSSNetWork;
class Property;

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

/**	\brief	Engine System that creates propertys.
	 \ingroup Engine

This class keeps a list of all type of propertys that exist in the game and creates
them when requested by the ObjectManger.
*/
class ENGINE_API ZSSPropertyFactory : public ZFSubSystem
{
private:
	vector<ProperyCreateLink>	m_kProperyLinks; 
	void RunCommand(int cmdid, const CmdArgument* kCommand) { }

	ZSSNetWork*		m_pkNetWork;

public:
	ZSSPropertyFactory();
	~ZSSPropertyFactory() { }

	ProperyCreateLink*	GetPropertyLink(const char* szName);
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
