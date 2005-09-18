#include "zerofps.h"
#include "../engine_systems/propertys/p_mad.h"
#include "../engine_systems/propertys/p_light.h"
#include "network.h"
#include "../basic/zfsystem.h"
 
ZSSPropertyFactory::ZSSPropertyFactory() :
 ZFSubSystem("ZSSPropertyFactory") 
{
	
}

bool ZSSPropertyFactory::StartUp()	
{ 
	m_pkNetWork = static_cast<ZSSNetWork*>(g_ZFObjSys.GetObjectPtr("ZSSNetWork"));		

	return true;
}

bool ZSSPropertyFactory::ShutDown()	
{ 
	
	return true;	
}

bool ZSSPropertyFactory::IsValid()	{ return true;	}

ProperyCreateLink* ZSSPropertyFactory::GetPropertyLink(const char* szName)
{
	for(unsigned int i=0; i<m_kProperyLinks.size(); i++)
	{
		if(strcmp(m_kProperyLinks[i].m_szName, szName) == 0)
			return &m_kProperyLinks[i];
	}

	return NULL;
}


Property* ZSSPropertyFactory::CreateProperty(const char* szName)
{
	ProperyCreateLink* pkPl = GetPropertyLink(szName);
	if(! pkPl)
	{
		ZFWarning("Property '%s' is not registered.", szName);
		return NULL;
	}

	// Create Property
	pkPl->iCount++;
	pkPl->iTotalCreated++;
	Property* pkProp = pkPl->Create();
	
	if(!pkProp)
	{
		ZFWarning("Failed to create property '%s'.", szName);
		return NULL;
	}

	if(strcmp(szName, pkProp->m_acName) != 0)
	{
		ZFWarning("Internal property name '%s' does not match whit registered property name '%s'.",
			pkProp->m_acName, szName);
	}

	return pkProp;
}

void ZSSPropertyFactory::Register(char* szName, Property*	(*Create)())
{
	ProperyCreateLink NewLink;
	strcpy(NewLink.m_szName, szName);
	NewLink.Create = Create;
	
	m_kProperyLinks.push_back(NewLink);
	
//	GetSystem().Logf("zerorts", "Property added: %s", szName);
	g_Logf("zerofps", "Property added %s", szName);
//	cout<<"Property added "<<szName<<endl;
	
	m_pkNetWork->NetString_Add(szName);
	
}

void ZSSPropertyFactory::Display()
{
	cout << "Listing properys : 'name' : 'active' : 'total created'" << endl;

	int iSize = 0;

	for(unsigned int i=0; i<m_kProperyLinks.size(); i++)
	{
		cout << m_kProperyLinks[i].m_szName << " : " << m_kProperyLinks[i].iCount 
			<< " : " << m_kProperyLinks[i].iTotalCreated << endl;
		iSize += strlen( m_kProperyLinks[i].m_szName ) + 1;
	}
	
	cout << "Total Property Name Size: " <<  iSize << endl;
}

void ZSSPropertyFactory::GetAllProperties(vector<string>& vkList)
{
	vkList.reserve( m_kProperyLinks.size() + 1 );

	for(unsigned int i=0; i<m_kProperyLinks.size(); i++)
		vkList.push_back( m_kProperyLinks[i].m_szName ); 
}








