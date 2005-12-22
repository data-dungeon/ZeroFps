#include "renderplugin_factory.h"
#include <iostream>



bool PluginFactory::IsRegistered(const string& strName)
{
	for(int i = 0;i<m_kPlugins.size();i++)
		if(m_kPlugins[i].m_strName == strName)
			return true;

	return false;
}

void PluginFactory::RegisterPlugin(const string& strName,Plugin*	(*CreateFunction)())
{
	if(IsRegistered(strName))
		return;

	PluginInfo kTemp;	
	
	kTemp.m_strName = strName;
	kTemp.CreateFunction = CreateFunction;
	
	m_kPlugins.push_back(kTemp);
}

Plugin* PluginFactory::CreatePluginInstans(const string strName)
{
	for(int i = 0;i<m_kPlugins.size();i++)
		if(m_kPlugins[i].m_strName == strName)
		{
			m_kPlugins[i].m_iUseCounter++;
			return m_kPlugins[i].CreateFunction();
		}
	
	cerr<<"WARNING: missing plugin "<<strName<<endl;
	
	return NULL;
}


vector<string> PluginFactory::GetPluginList()
{
	vector<string> kPluginList;

	for(int i = 0;i<m_kPlugins.size();i++)
		kPluginList.push_back(m_kPlugins[i].m_strName);	

	return kPluginList;
}
