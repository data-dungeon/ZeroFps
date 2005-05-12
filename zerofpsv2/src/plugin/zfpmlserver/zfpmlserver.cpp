#include <vector>

#include "../../zerofpsv2/engine/application.h"
#include "../../zerofpsv2/gui/zgui.h"
#include "../../zerofpsv2/engine/entitymanager.h"
#include "p_ost.h"

using namespace std;

class I_NameGenerator
{
public:
	virtual string GetName() = 0;
};

class NameGenerator : public ZFSubSystem, public I_NameGenerator  
{
private:

protected:

public:
		NameGenerator();
		virtual ~NameGenerator();
			
		void RunCommand(int cmdid, const CmdArgument* kCommand) { }
		bool StartUp() { return true; }
		bool ShutDown() {  return true;  }
		bool IsValid() { return true; } 
		string GetName();
};


NameGenerator::NameGenerator() : ZFSubSystem("NameGenerator")  
{

}

NameGenerator::~NameGenerator()
{

}

string NameGenerator::GetName()
{
	return "Smurfomatic";
}

NameGenerator	g_CoolSubSystem;

extern "C" 
{
	void Plugin_Load()
	{
		cout << "Loading plugin" << endl;
		Register_P_Ost( dynamic_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps")));
	}

	void  Plugin_Unload()
	{
		cout << "Unloading plugin" << endl;
	}
}

