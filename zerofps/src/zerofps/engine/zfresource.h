#ifndef _BASIC_RESOURCE_H_
#define _BASIC_RESOURCE_H_

#include "engine_x.h"
#include <string>

using namespace std;


class ENGINE_API ZFResource
{
private:
	int Hora;

public:
	ZFResource();
	virtual ~ZFResource();

	void Clear();	
	virtual bool Create(string strName) = 0;
	void Destroy();

	int GetSize() ;

	friend class ZFResourceDB;
};




#endif

