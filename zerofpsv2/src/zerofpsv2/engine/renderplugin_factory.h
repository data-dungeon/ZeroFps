#ifndef _RENDERPLUGIN_FACTORY_H_
#define _RENDERPLUGIN_FACTORY_H_

#include <string>
#include <vector>
#include "engine_x.h"

using namespace std;

class ENGINE_API Plugin
{
	
	public:
		virtual ~Plugin() {};
		virtual void ImPlugin() {};
};

class ENGINE_API PluginInfo
{
	public:
		string		m_strName;
		int			m_iUseCounter;		

		Plugin*		(*CreateFunction)();
		
		PluginInfo()
		{
			m_iUseCounter = 0;
		};
};


class ENGINE_API PluginFactory
{
	private:
		vector<PluginInfo>	m_kPlugins;
	
	public:
		bool IsRegistered(const string& strName);
		void RegisterPlugin(const string& strName,Plugin*	(*CreateFunction)());

		Plugin* CreatePluginInstans(const string strName);
		
		vector<string> GetPluginList();
};


#endif

