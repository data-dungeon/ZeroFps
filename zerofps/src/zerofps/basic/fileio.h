#ifndef _BASIC_FILEIO_H_
#define _BASIC_FILEIO_H_

#include <iostream>
#include <string>
#include "basic_x.h"
#include "os.h"

class BASIC_API FileIo {
	private:
		char fil[256];

	public:
		char* File(char* aFile);

};



#endif






