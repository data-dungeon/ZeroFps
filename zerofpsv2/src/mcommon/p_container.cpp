#include "p_clientcontrol.h"
#include "p_container.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "mlcontainer.h"

// -----------------------------------------------------------------------------------------------

P_Container::P_Container()
{
	m_pkEntityMan=			static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));			
	
	strcpy(m_acName,"P_Container");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	m_bNetwork = 	false;
	m_iVersion = 	1;

	m_bFirstUpdate = true;
	
	m_pkContainer = NULL;
	m_iSizeX  = 6;
	m_iSizeY  = 6;	
	
}

P_Container::~P_Container()
{
	delete m_pkContainer;

}


void P_Container::Init()
{
	m_pkContainer		=	new MLContainer(m_pkEntityMan,GetEntity()->GetEntityID(),m_iSizeX,m_iSizeY,true);	
}

void P_Container::Update()
{
	//try to find items on load
	if(m_bFirstUpdate)
	{
		m_bFirstUpdate = false;
		m_pkContainer->FindMyItems();	
	}

}

vector<PropertyValues> P_Container::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
			
	kReturn[0].kValueName = "sizex";
	kReturn[0].iValueType = VALUETYPE_INT;
	kReturn[0].pkValue    = &m_iSizeX;		
	
	kReturn[1].kValueName = "sizey";
	kReturn[1].iValueType = VALUETYPE_INT;
	kReturn[1].pkValue    = &m_iSizeY;		
	
	return kReturn;	
}


bool P_Container::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "sizex") == 0) {
		m_iSizeX = atoi(kValue.c_str()) ;
		return true;
	}
	
	if(strcmp(kValueName.c_str(), "sizey") == 0) {
		m_iSizeY = atoi(kValue.c_str()) ;
		return true;
	}

	m_pkContainer->SetSize(m_iSizeX,m_iSizeY);
	
	return false;
}




void P_Container::Save(ZFIoInterface* pkPackage)
{	
	//save container settings
	m_pkContainer->Save(pkPackage);
}

void P_Container::Load(ZFIoInterface* pkPackage,int iVersion)
{
	//save container settings
	m_pkContainer->Load(pkPackage);
}

Property* Create_P_Container()
{
	return new P_Container;
}
