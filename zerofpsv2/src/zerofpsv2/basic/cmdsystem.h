#ifndef _BASIC_CMDSYSTEM_H_
#define _BASIC_CMDSYSTEM_H_

#include <vector>
#include <iostream>
#include <cstring>
#include <string.h>
#include "basic_x.h"
#include "os.h"
#include "zfsubsystem.h"
#include "basicconsole.h"

using namespace std;

/**	\brief	Used by ZFSystem to register it's own commands.
		\ingroup Basic
*/
class BASIC_API CmdSystem : public ZFSubSystem  
{
	private:
		BasicConsole*			m_pkCon;

	public:
		enum FuncId_e
			{
			FID_SET,	
			FID_VARLIST,
			FID_COMMANDS,
			FID_SYS
			};

		void RunCommand(int cmdid, const CmdArgument* kCommand);

		CmdSystem(void);
		
		bool Set(const char* aName,const char* acData);							//set variable aName to dData

		friend class ZFSystem;

		bool StartUp();
		bool ShutDown();
		bool IsValid();
};


#endif





