#ifndef _BASIC_FILEIO_H_
#define _BASIC_FILEIO_H_

#include <iostream>
#include <string>
#include "zfobject.h"
#include "basic_x.h"
#include "os.h"

class BASIC_API FileIo  : public ZFObject {
	private:
		char fil[256];

	public:
		FileIo();
		char* File(const char* aFile);

		bool StartUp()	{ return true;	}
		bool ShutDown()	{ return true;	}
		bool IsValid()	{ return true;	}

};



#endif






