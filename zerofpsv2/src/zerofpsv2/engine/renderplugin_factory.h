#ifndef _RENDERPLUGIN_FACTORY_H_
#define _RENDERPLUGIN_FACTORY_H_

#include <string>
#include <vector>

using namespace std;

class Plugin
{
	
	public:
		virtual void ImPlugin() {};
};

class PluginInfo
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


class PluginFactory
{
	private:
		vector<PluginInfo>	m_kPlugins;
	
	public:
		bool IsRegistered(const string& strName);
		void RegisterPlugin(const string& strName,Plugin*	(*CreateFunction)());

		Plugin* CreatePluginInstans(const string strName);
};


#endif

