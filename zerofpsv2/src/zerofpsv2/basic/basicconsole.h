#ifndef _BASIC_BASICCONSOLE_H_
#define _BASIC_BASICCONSOLE_H_

#include "os.h"
#include "zfsubsystem.h"
#include "basic_x.h"
#include <iostream>
#include <vector>

using namespace std;

#define TEXT_MAX_LENGHT 256

/**	\brief	A base class to create objects that you can print text to.
		\ingroup Basic
*/
class BASIC_API BasicConsole : public ZFSubSystem 
{
protected:
		vector<char*>	m_kText;										///< Text that the console have.

public:
		BasicConsole(char* szName);
		virtual ~BasicConsole();
		
		// Print output to console.
		virtual void Print(const char* aText);							///< Print text to console.
		virtual void Printf(const char *fmt, ...);						///< Print text with formation commands to console.
		
		void RunCommand(int cmdid, const CmdArgument* kCommand) { }
};

#endif




