#ifndef _BASIC_BASICCONSOLE_H_
#define _BASIC_BASICCONSOLE_H_

#include "os.h"
#include "zfobject.h"
#include "basic_x.h"
#include <iostream>
#include <vector>

using namespace std;

#define TEXT_MAX_LENGHT 256

/*
 Basic console är ett object som kan ta in kommandon och skriva ut resultat 
 från användaren. 
*/
class BASIC_API BasicConsole : public ZFObject {
protected:
		vector<char*>	m_kText;

public:
		BasicConsole(char* szName);
		
		// Print output to console.
		virtual void Print(const char* aText);
		virtual void Printf(const char *fmt, ...);
		
		enum FuncId_e
			{
			FID_VERSION,
			FID_HELP
			};

		void RunCommand(int cmdid, const CmdArgument* kCommand);

};

#endif