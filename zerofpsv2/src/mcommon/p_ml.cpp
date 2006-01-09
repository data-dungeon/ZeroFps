#include "p_ml.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "p_characterproperty.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 

P_Ml::P_Ml() : Property("P_Ml")
{
	m_iType=0;//PROPERTY_TYPE_NORMAL;
	m_iSide=0;//PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;
	
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			

	m_bNetwork = true;	
	m_iVersion = 4;
	


}

void P_Ml::Init()
{
}

P_Ml::~P_Ml()
{
}


void P_Ml::Update()
{
}


void P_Ml::AddAction(const char* csAction)
{
	m_kActions.push_back(string(csAction));	
	SetNetUpdateFlag(true);		
}


void P_Ml::PackTo( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	int nr = m_kActions.size();	
	pkNetPacket->Write(&nr,sizeof(nr));
	
	for(unsigned int i=0;i<m_kActions.size();i++)
	{
		pkNetPacket->Write_Str(m_kActions[i].c_str());
	}

	SetNetUpdateFlag(iConnectionID,false);
} 

void P_Ml::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	int nr;		
	pkNetPacket->Read(&nr,sizeof(nr));
	
	m_kActions.clear();
	for(int i=0;i<nr;i++)
	{
		char temp[128];
		pkNetPacket->Read_Str(temp);
		m_kActions.push_back(string(temp));
	}
}

void P_Ml::Save(ZFIoInterface* pkPackage)
{	
	int nr = m_kActions.size();
	pkPackage->Write(nr);
	for(int i = 0;i<nr;i++)
		pkPackage->Write_Str(m_kActions[i]);
}

void P_Ml::Load(ZFIoInterface* pkPackage,int iVersion)
{
	switch(iVersion)
	{
		case 1:
		{
			int iDummy;			
			bool bDummy;
			pkPackage->Read((void*)&iDummy,sizeof(iDummy),1);		
			pkPackage->Read((void*)&iDummy,sizeof(iDummy),1);	
			pkPackage->Read((void*)&bDummy,sizeof(bDummy),1);		
			break;
		}
			
		case 2:
		{
			bool bDummy;
			pkPackage->Read((void*)&bDummy,sizeof(bDummy),1);		
			break;
		}
		
		case 3:
		{
			bool bDummy;
			pkPackage->Read((void*)&bDummy,sizeof(bDummy),1);		
			
			
			int nr;
			pkPackage->Read(nr);
			m_kActions.clear();
			string temp;
			for(int i = 0;i<nr;i++)
			{
				pkPackage->Read_Str(temp);
				m_kActions.push_back(temp);
			}
				
			break;
		}		
		case 4:
		{
			int nr;
			pkPackage->Read(nr);
			m_kActions.clear();
			string temp;
			for(int i = 0;i<nr;i++)
			{
				pkPackage->Read_Str(temp);
				m_kActions.push_back(temp);
			}
				
			break;
		}		
	
	}
}






// SCRIPT INTERFACE FOR P_Ml
using namespace ObjectManagerLua;

namespace SI_P_Ml
{
	int AddActionLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua, 2))
			return 0;
			//if(g_pkScript->GetNumArgs(pkLua) != 2)
				//	return 0;
		
		int id;
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		id = (int)dTemp;
		
		Entity* pkObject = g_pkObjMan->GetEntityByID(id);
		if(pkObject)
		{
			P_Ml* pe = (P_Ml*)pkObject->GetProperty("P_Ml");	
			
			if(pe)
			{
				char	acEvent[128];
				g_pkScript->GetArgString(pkLua, 1, acEvent);
			
				pe->AddAction(acEvent);
				return 0;
			}
			else
				cout<<"Error tried to add action on object whitout P_Ml property"<<endl;
		}
		
		return 0;
	}


}


Property* Create_P_Ml()
{
	return new P_Ml;
}

void Register_P_Ml(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Ml", Create_P_Ml);					

	// Register Property Script Interface
	g_pkScript->ExposeFunction("AddAction",	SI_P_Ml::AddActionLua);

}







// vector<PropertyValues> P_Ml::GetPropertyValues()
// {
// 	vector<PropertyValues> kReturn(1);
// 		
// 	kReturn[0].kValueName = "showtext";
// 	kReturn[0].iValueType = VALUETYPE_BOOL;
// 	kReturn[0].pkValue    = (void*)&m_bShowText;
// 
// 	return kReturn;
// }