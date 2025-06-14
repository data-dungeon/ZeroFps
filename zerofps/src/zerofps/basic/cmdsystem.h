#ifndef _BASIC_CMDSYSTEM_H_
#define _BASIC_CMDSYSTEM_H_

#include <vector>
#include <iostream>
#include <cstring>
#include <string.h>
#include "basic_x.h"
#include "os.h"
#include "zfobject.h"
#include "basicconsole.h"

using namespace std;

class BASIC_API CmdSystem : public ZFObject  {
	private:
		BasicConsole*			m_pkCon;

	public:
		enum FuncId_e
			{
			FID_SET,
			FID_VARLIST
			};

		void RunCommand(int cmdid, const CmdArgument* kCommand);

		CmdSystem(void);
		
		bool Set(const char* aName,const char* acData);							//set variable aName to dData
};


#endif





