#ifndef _BASIC_BASICCONSOLE_H_
#define _BASIC_BASICCONSOLE_H_

#include "os.h"
#include "zfobject.h"
#include "basic_x.h"
#include <iostream>
#include <vector>

using namespace std;

#define TEXT_MAX_LENGHT 256

/// A base class to create objects that you can print text to.
class BASIC_API BasicConsole : public ZFObject {
protected:
		vector<char*>	m_kText;										///< Text that the console have.

public:
		BasicConsole(char* szName);
		
		// Print output to console.
		virtual void Print(const char* aText);					///< Print text to console.
		virtual void Printf(const char *fmt, ...);			///< Print text with formation commands to console.
		
/*		enum FuncId_e
			{
			FID_HELP
			};*/

		void RunCommand(int cmdid, const CmdArgument* kCommand);
};

#endif




