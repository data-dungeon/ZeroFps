#ifndef _BASIC_RESOURCE_H_
#define _BASIC_RESOURCE_H_

#include "engine_x.h"

class ENGINE_API ZFResource
{
private:
	int Hora;

public:
	ZFResource();
	virtual ~ZFResource();

	void Clear();	
	void Create();
	void Destroy();

	int GetSize() ;

	friend class ZFResourceDB;
};




#endif

