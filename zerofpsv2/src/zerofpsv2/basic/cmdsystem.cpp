#include "basic_x.h"
#include "cmdsystem.h"
#include "zfsystem.h"
#include "globals.h"
#include "zfini.h"

#if defined(WIN32) 
#include <windows.h> 
#else
#include <dlfcn.h>
#endif



ZSSCmdSystem::ZSSCmdSystem()
: ZFSubSystem("ZSSCmdSystem") 
{
//	m_pkCon = NULL;

	Register_Cmd("set",			FID_SET,						CSYS_FLAG_SRC_ALL,	"set name value", 2);
	Register_Cmd("varlist",		FID_VARLIST,				CSYS_FLAG_SRC_ALL);
	Register_Cmd("commands",	FID_COMMANDS,				CSYS_FLAG_SRC_ALL);
	Register_Cmd("sys",			FID_SYS,						CSYS_FLAG_SRC_ALL);
	Register_Cmd("exec",			FID_EXECUTECOMMANDS,		CSYS_FLAG_SRC_ALL);
	Register_Cmd("pload",		FID_PLUGINLOAD,			CSYS_FLAG_SRC_ALL);
	Register_Cmd("punload",		FID_PLUGINUNLOAD,			CSYS_FLAG_SRC_ALL);
	Register_Cmd("plist",		FID_PLUGINLIST,			CSYS_FLAG_SRC_ALL);
}

bool ZSSCmdSystem::StartUp()	
{ 
//	m_pkCon = dynamic_cast<BasicConsole*>((GetSystem().GetObjectPtr("Console")));
	return true; 
}

bool ZSSCmdSystem::ShutDown() 
{ 
	return true; 
}

bool ZSSCmdSystem::IsValid()	
{ 
	return true; 
}

bool ZSSCmdSystem::Set(const char* acName,const char* acData)
{
	return GetSystem().SetVariable(const_cast<char*>(acName),const_cast<char*>(acData));
}

void ZSSCmdSystem::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) 
	{
		case FID_SET:
			if(!Set(kCommand->m_kSplitCommand[1].c_str(),&kCommand->m_strFullCommand.c_str()[kCommand->m_kSplitCommand[0].length() + kCommand->m_kSplitCommand[1].length() + 2])){
				//if(m_pkCon)
					GetSystem().Printf("Variable not found");
				return;
			} else {
				//if(m_pkCon)
					GetSystem().Printf("Setting %s = %s",kCommand->m_kSplitCommand[1].c_str(),kCommand->m_kSplitCommand[2].c_str());
			}
			
			
			break;

		case FID_VARLIST:		GetSystem().PrintVariables();	break;
		case FID_COMMANDS:	GetSystem().PrintCommands();	break;

		case FID_PLUGINLOAD:	
			Plugin_LoadSubSystem(kCommand->m_kSplitCommand[1]);
			break;

		case FID_PLUGINUNLOAD:	
			Plugin_UnLoadSubSystem(kCommand->m_kSplitCommand[1]);
			break;

		case FID_PLUGINLIST:	Plugin_List();	break;
		


		case FID_SYS:
			// Draw All Systems to console.
			for(map<string,NameObject>::iterator it = m_pkSystem->m_kObjectNames.begin();it != m_pkSystem->m_kObjectNames.end();it++)
			{
				GetSystem().Printf(" %s, %d\n",(*it).second.m_strName.c_str(), (*it).second.m_iNumOfRequests );
			}


			break;

		case FID_EXECUTECOMMANDS:
			ZFIni kCfgFile;
			if(kCommand->m_kSplitCommand.size() > 1) {
				kCfgFile.ExecuteCommands( kCommand->m_kSplitCommand[1].c_str() );
				}
			break;

	}
}









class	ModuleInfo
{
public:
#ifdef WIN32
	HMODULE	m_kModule;
#else
	void*		m_kModule;
#endif
	
	string	m_strModuleName;
};

map<string,ModuleInfo>	g_kModule;			

typedef bool (*ZPF_PluginLoad)();
typedef bool (*ZPF_PluginUnLoad)();

void ZSSCmdSystem::Plugin_LoadSubSystem(string strPluginName)
{
	ModuleInfo	kInfo;
	kInfo.m_strModuleName = strPluginName;

	string strFullPluginName;

#ifdef WIN32
	strFullPluginName = strPluginName + string(".dll");
	kInfo.m_kModule = LoadLibrary( strFullPluginName.c_str() );
#else
	strFullPluginName = strPluginName + string(".so");
	kInfo.m_kModule = dlopen( strFullPluginName.c_str(), RTLD_NOW);
#endif
		
	if(!kInfo.m_kModule)
	{
		GetSystem().Printf("Failed to load plugin '%s'\n", strPluginName.c_str());
		return;
	}

	// Call plugin load function.
#ifdef WIN32
	ZPF_PluginLoad pkPluginLoad = (ZPF_PluginLoad) GetProcAddress(kInfo.m_kModule, "Plugin_Load");
#else
	ZPF_PluginLoad pkPluginLoad = (ZPF_PluginLoad) dlsym(kInfo.m_kModule, "Plugin_Load");
#endif

	if(!pkPluginLoad)
	{
		GetSystem().Printf("Failed to init plugin '%s'\n", strPluginName.c_str());
#ifdef WIN32
		FreeLibrary( kInfo.m_kModule );
#else
		dlclose( kInfo.m_kModule );
#endif
		return;
	}

	pkPluginLoad();
	g_kModule.insert(map<const char*,ModuleInfo>::value_type(kInfo.m_strModuleName.c_str(),kInfo));

	GetSystem().Printf("Plugin '%s' is now loaded.\n", strPluginName.c_str());
}

void ZSSCmdSystem::Plugin_UnLoadSubSystem(string strPluginName)
{
	// First we check so module is really loaded.
	static map<string,ModuleInfo>::iterator it;
	it = g_kModule.find(strPluginName);

	if(it == g_kModule.end())
	{
		GetSystem().Printf("Module '%s' is not loaded.\n", strPluginName.c_str());
		return;
	}

	// Call Unload function
#ifdef WIN32	
	ZPF_PluginUnLoad pkPluginUnLoad = (ZPF_PluginUnLoad) GetProcAddress(it->second.m_kModule, "Plugin_Unload");
#else
	ZPF_PluginUnLoad pkPluginUnLoad = (ZPF_PluginUnLoad) dlsym(it->second.m_kModule, "Plugin_Unload");
#endif	
	
	if(pkPluginUnLoad)
		pkPluginUnLoad();

	// Unload Module
#ifdef WIN32
		FreeLibrary( it->second.m_kModule );
#else
		dlclose( it->second.m_kModule );
#endif	
	
	g_kModule.erase(it);
	GetSystem().Printf("Plugin '%s' is now unloaded.\n", strPluginName.c_str());
}

void ZSSCmdSystem::Plugin_List()
{
	GetSystem().Printf("Loaded Modules\n");

	map<string,ModuleInfo>::iterator itModule;
	for(itModule = g_kModule.begin(); itModule != g_kModule.end(); itModule++)
	{
		GetSystem().Printf(" %s", (*itModule).second.m_strModuleName.c_str());
	}
}

// void CmdSystem::Plugin_LoadSubSystem(string strPluginName)		{ }
// void CmdSystem::Plugin_UnLoadSubSystem(string strPluginName)	{ }
// void CmdSystem::Plugin_List()												{ }
