#ifndef _PROPERTYFACTORY_H_
#define _PROPERTYFACTORY_H_

#include <string.h>
#include <iostream>
#include <vector>

#include "engine_x.h"
#include "property.h"

using namespace std;


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

class ENGINE_API PropertyFactory : public ZFObject
{
private:
	vector<ProperyCreateLink>	m_kProperyLinks; 

public:
	PropertyFactory() : ZFObject("PropertyFactory") { }
	~PropertyFactory() { }

	Property*	CreateProperty(char* szName);
	void		Register(char* szName, Property*	(*Create)());

	void		Display();
};

class ZeroFps;
void RegisterPropertys(ZeroFps* zfps);

#endif
