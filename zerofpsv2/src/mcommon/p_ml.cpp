#include "p_ml.h" 
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"

P_Ml::P_Ml()
{
	strcpy(m_acName,"P_Ml");		
	m_iType=PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;
	
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			

	bNetwork = true;	
	m_iVersion = 2;
	
	m_bShowText =	false;	
	m_strText = "";
}

void P_Ml::Init()
{
	if(m_strText == "")
		m_strText = m_pkObject->GetName();

}

P_Ml::~P_Ml()
{

}


void P_Ml::Update()
{
	static ZMaterial* pkText = NULL;
	if(!pkText)
	{
		pkText = new ZMaterial;
		pkText->GetPass(0)->m_kTUs[0]->SetRes("data/textures/text/mltext.tga");
		pkText->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
		pkText->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
		pkText->GetPass(0)->m_bLighting = 				false;		
		pkText->GetPass(0)->m_bColorMaterial = 		true;
		pkText->GetPass(0)->m_bFog = 						true;		
		pkText->GetPass(0)->m_bAlphaTest =				true;		
		pkText->GetPass(0)->m_bDepthTest = 				true;				//needs to disable z-test , else marker wont work
	}
	
	if(m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER))
	{
		if(m_bShowText)
		{
			m_pkZShaderSystem->BindMaterial(pkText);			
			m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),GetObject()->GetIWorldPosV()+Vector3(0,GetObject()->GetRadius(),0),GetObject()->GetName().c_str(),0.2,true);
		}
	}
}

void P_Ml::AddAction(const char* csAction)
{
	//cout<<"registering action:"<<csAction<<endl;
	m_kActions.push_back(string(csAction));	
	SetNetUpdateFlag(true);		
}


void P_Ml::PackTo( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Write(&m_bShowText,sizeof(m_bShowText));

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
	pkNetPacket->Read(&m_bShowText,sizeof(m_bShowText));

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
	pkPackage->Write((void*)&m_bShowText,sizeof(m_bShowText),1);		
}

void P_Ml::Load(ZFIoInterface* pkPackage,int iVersion)
{
	switch(iVersion)
	{
		case 1:
		{
			int iDummy;			
			pkPackage->Read((void*)&iDummy,sizeof(iDummy),1);		
			pkPackage->Read((void*)&iDummy,sizeof(iDummy),1);	
			pkPackage->Read((void*)&m_bShowText,sizeof(m_bShowText),1);		
			break;
		}
			
		case 2:
		{
			pkPackage->Read((void*)&m_bShowText,sizeof(m_bShowText),1);		
			break;
		}
	}
}



vector<PropertyValues> P_Ml::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
		
	kReturn[0].kValueName = "showtext";
	kReturn[0].iValueType = VALUETYPE_BOOL;
	kReturn[0].pkValue    = (void*)&m_bShowText;

	return kReturn;
}

Property* Create_P_Ml()
{
	return new P_Ml;
}
