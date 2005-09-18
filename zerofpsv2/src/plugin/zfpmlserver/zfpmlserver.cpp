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

#if defined(WIN32) 

#ifdef ZPGPLUGIN_EXPORTS
#define ZPGPLUGIN_API __declspec(dllexport)
#else
#define ZPGPLUGIN_API __declspec(dllimport)
#endif

#endif

#if !defined(WIN32) 
#define ZPGPLUGIN_API 
#endif
extern "C" 
{
	void ZPGPLUGIN_API Plugin_Load()
	{
		cout << "Loading plugin" << endl;
		Register_P_Ost( dynamic_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps")));
	}

	void ZPGPLUGIN_API Plugin_Unload()
	{
		cout << "Unloading plugin" << endl;
	}
}

