#include "zerofps.h"
#include "../engine_systems/propertys/madproperty.h"
#include "../engine_systems/propertys/lightproperty.h"
#include "network.h"
#include "../basic/zfobjectmanger.h"

PropertyFactory::PropertyFactory() :
 ZFSubSystem("PropertyFactory") 
{
	
}

bool PropertyFactory::StartUp()	
{ 
	m_pkNetWork = static_cast<NetWork*>(g_ZFObjSys.GetObjectPtr("NetWork"));		

	return true;
}

bool PropertyFactory::ShutDown()	
{ 
	
	return true;	
}

bool PropertyFactory::IsValid()	{ return true;	}

Property* PropertyFactory::CreateProperty(const char* szName)
{
	for(unsigned int i=0; i<m_kProperyLinks.size(); i++)
	{
		if(strcmp(m_kProperyLinks[i].m_szName, szName) == 0)
		{
			m_kProperyLinks[i].iCount++;
			m_kProperyLinks[i].iTotalCreated++;
			return m_kProperyLinks[i].Create();
		}
		
	}

	return NULL;
}

void PropertyFactory::Register(char* szName, Property*	(*Create)())
{
	ProperyCreateLink NewLink;
	strcpy(NewLink.m_szName, szName);
	NewLink.Create = Create;
	
	m_kProperyLinks.push_back(NewLink);
	
	cout<<"Property added "<<szName<<endl;
	
	m_pkNetWork->NetString_Add(szName);
	
}

void PropertyFactory::Display()
{
	cout << "Listing properys : 'name' : 'active' : 'total created'" << endl;

	for(unsigned int i=0; i<m_kProperyLinks.size(); i++)
	{
		cout << m_kProperyLinks[i].m_szName << " : " << m_kProperyLinks[i].iCount 
			<< " : " << m_kProperyLinks[i].iTotalCreated << endl;
	}
	
}

/*
void RegisterPropertys(ZeroFps* zfps)
{
	zfps->m_kPropertyFactory.Register("MadProperty", Create_MadProperty);
	zfps->m_kPropertyFactory.Register("ModelProperty", Create_ModelProperty);
	zfps->m_kPropertyFactory.Register("LightProperty", Create_LightProperty);

}
*/

void PropertyFactory::GetAllProperties(vector<string>& vkList)
{
	vkList.reserve( m_kProperyLinks.size() + 1 );

	for(unsigned int i=0; i<m_kProperyLinks.size(); i++)
		vkList.push_back( m_kProperyLinks[i].m_szName ); 
}








