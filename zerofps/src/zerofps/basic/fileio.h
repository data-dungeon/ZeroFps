#ifndef _FILEIO_H_
#define _FILEIO_H_

#include <iostream>
#include <string>
#include "basic.pkg"

class BASIC_API FileIo {
	private:
		char fil[256];

	public:
		char* File(char* aFile);

};



#endif






