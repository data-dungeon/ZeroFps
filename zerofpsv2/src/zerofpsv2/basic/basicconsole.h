#ifndef _BASIC_BASICCONSOLE_H_
#define _BASIC_BASICCONSOLE_H_

#include "zfsubsystem.h"
#include "basic_x.h"
#include <vector>

using namespace std;

// #define TEXT_MAX_LENGHT 256

/**	\brief	A base class to create objects that you can print text to.
		\ingroup Basic
*/
class BASIC_API BasicConsole : public ZFSubSystem 
{
private:
		virtual void PrintOneRow(const char* aText);
		virtual void PrintWordWrap(const char* aText);
		virtual void Print(const char* aText);								///< Print text to console.

protected:
		vector<char*>	m_kText;													///< Text that the console have.
		int				m_iMaxWidth;											// Max number of char this console can show in width before word wrap will kick in.
		bool				m_bLog;													// True if console printouts logs to file.

public:
		BasicConsole(char* szName);
		virtual ~BasicConsole();
		
		// Print output to console.
		virtual void Printf(const char *fmt, ...);						///< Print text with formation commands to console.
		
		void RunCommand(int cmdid, const ConCommandLine* kCommand) { }
};

#endif




