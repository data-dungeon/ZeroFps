#include "zmaterial.h"
#include "zshader.h"
#include "../basic/globals.h"

ZMaterialSettings::ZMaterialSettings()
{
/*	m_iTUs[0] = -1;
	m_iTUs[1] = -1;		
	m_iTUs[2] = -1;			
	m_iTUs[3] = -1;			*/
	cout<<"bla:"<<endl;
	m_kTUs[0] = new ZFResourceHandle();
	m_kTUs[1] = new ZFResourceHandle();
	m_kTUs[2] = new ZFResourceHandle();
	m_kTUs[3] = new ZFResourceHandle();
	
	m_iTUTexCords[0] = CORDS_FROM_ARRAY_0;
	m_iTUTexCords[1] = CORDS_FROM_ARRAY_1;	
	m_iTUTexCords[2] = CORDS_FROM_ARRAY_2;	
	m_iTUTexCords[3] = CORDS_FROM_ARRAY_3;	
	
	m_iPolygonModeFront = LINE_POLYGON;
	m_iPolygonModeBack =	LINE_POLYGON;			
	
	m_bLighting =	 false;
	m_bCullFace = 	true;
	m_bAlphaTest =	false;
	m_iDepthFunc =	LESS_DEPTH;
	
	m_bBlend	= false;
	m_iBlendSrc = ONE_BLEND_SRC; 
	m_iBlendDst = ZERO_BLEND_DST; 
};

ZMaterialSettings::~ZMaterialSettings()
{
	for(int i=0;i<4;i++)
	{
		cout<<"deleting:"<<i<<endl;
		if(m_kTUs[i])
			delete m_kTUs[i];	
			
		m_kTUs[i] = NULL;
		
	}
}

ZMaterial::ZMaterial()
{
	Clear();

	ZMaterialSettings* first = AddPass();	
}

ZMaterial::~ZMaterial()
{
	Clear();
}

ZMaterialSettings* ZMaterial::GetPass(int iPass)
{
	if(iPass >= m_kPasses.size())
		return NULL;
	
	return m_kPasses[iPass];
}

ZMaterialSettings* ZMaterial::AddPass()
{
	m_kPasses.push_back(new ZMaterialSettings());
	return m_kPasses.back();
}

void ZMaterial::Clear()
{
	for(int i=0;i<m_kPasses.size();i++)
	{
		if(m_kPasses[i])
			delete m_kPasses[i];	
	}
	m_kPasses.clear();

	m_bCopyData = false;
	m_bRandomMovements = false;
	m_bWaves = false;	
}

bool ZMaterial::LoadShader(const char* acFile)
{
	bool open=false;
	
	//try to open material file
	if(m_kIni.Open(acFile,0))		
		open=true;
	else		//if file didt open, try to load a default texture
	{
		cout<<"Error loading shader: "<<acFile<<" using nomaterial"<<endl;
		if(m_kIni.Open("../data/material/nomaterial.zmt",0))
			open=true;
	}

	if(open)
	{
		Clear();
	
		
		if(!LoadGlobalSection())
			return false;
			 
		
		int iPass=0;	
		while(LoadPass(iPass))
		{
			iPass++;
		}
				
		//cout<<"shader "<<acFile<<" loaded: found "<<iPass<<" passes"<<endl;
		return true;
	}
	else
	{
		cout<<"error loading shader:"<<acFile<<endl;
		return false;
	}

}

bool ZMaterial::LoadGlobalSection()
{
	if(!m_kIni.SectionExist("global"))
	{
		cout<<"global section is missing"<<endl;
		return false;
	}

	//read shader global settings

	if(m_kIni.KeyExist("global","copydata"))
		if(m_kIni.GetBoolValue("global","copydata"))
			m_bCopyData = true;
		else
			m_bCopyData = false;		

	if(m_kIni.KeyExist("global","waves"))
		if(m_kIni.GetBoolValue("global","waves"))
			m_bWaves = true;
		else
			m_bWaves = false;

	if(m_kIni.KeyExist("global","randommovements"))
		if(m_kIni.GetBoolValue("global","randommovements"))
			m_bRandomMovements = true;
		else
			m_bRandomMovements = false;


	return true;
}

bool ZMaterial::LoadPass(int iPass)
{
	string passname;
	passname="pass-";	
	char nr;
	IntToChar(&nr,iPass);			
	passname+=nr;
	
	if(!m_kIni.SectionExist(passname.c_str()))
	{
		//cout<<passname.c_str()<<" section is missing"<<endl;
		return false;
	}
	
	cout<<"loading pass: "<<iPass<<endl;
	
	//load pass settings
	ZMaterialSettings* newpass = AddPass();
	
	if(m_kIni.KeyExist(passname.c_str(),"lighting"))
		newpass->m_bLighting = m_kIni.GetBoolValue(passname.c_str(),"lighting");
	if(m_kIni.KeyExist(passname.c_str(),"cullface"))
		newpass->m_bCullFace = m_kIni.GetBoolValue(passname.c_str(),"cullface");
	if(m_kIni.KeyExist(passname.c_str(),"alphatest"))
		newpass->m_bAlphaTest = m_kIni.GetBoolValue(passname.c_str(),"alphatest");
	
	if(m_kIni.KeyExist(passname.c_str(),"tu0"))
		newpass->m_kTUs[0]->SetRes(m_kIni.GetValue(passname.c_str(),"tu0"));
	if(m_kIni.KeyExist(passname.c_str(),"tu1"))
		newpass->m_kTUs[1]->SetRes(m_kIni.GetValue(passname.c_str(),"tu1"));
	if(m_kIni.KeyExist(passname.c_str(),"tu2"))
		newpass->m_kTUs[2]->SetRes(m_kIni.GetValue(passname.c_str(),"tu2"));
	if(m_kIni.KeyExist(passname.c_str(),"tu3"))
		newpass->m_kTUs[3]->SetRes(m_kIni.GetValue(passname.c_str(),"tu3"));

	
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords0"))
		newpass->m_iTUTexCords[0] = m_kIni.GetIntValue(passname.c_str(),"tutexcords0");
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords1"))
		newpass->m_iTUTexCords[1] = m_kIni.GetIntValue(passname.c_str(),"tutexcords1");
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords2"))
		newpass->m_iTUTexCords[2] = m_kIni.GetIntValue(passname.c_str(),"tutexcords2");
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords3"))
		newpass->m_iTUTexCords[3] = m_kIni.GetIntValue(passname.c_str(),"tutexcords3");
	
	
	if(m_kIni.KeyExist(passname.c_str(),"polygonmodefront"))
		newpass->m_iPolygonModeFront = m_kIni.GetIntValue(passname.c_str(),"polygonmodefront");
	if(m_kIni.KeyExist(passname.c_str(),"polygonmodeback"))
		newpass->m_iPolygonModeBack = m_kIni.GetIntValue(passname.c_str(),"polygonmodeback");
	if(m_kIni.KeyExist(passname.c_str(),"depthfunc"))
		newpass->m_iDepthFunc = m_kIni.GetIntValue(passname.c_str(),"depthfunc");
	
	if(m_kIni.KeyExist(passname.c_str(),"blend"))
		newpass->m_bBlend = m_kIni.GetBoolValue(passname.c_str(),"blend");
	if(m_kIni.KeyExist(passname.c_str(),"blendsrc"))
		newpass->m_iBlendSrc = m_kIni.GetIntValue(passname.c_str(),"blendsrc");
	if(m_kIni.KeyExist(passname.c_str(),"blenddst"))
		newpass->m_iBlendDst = m_kIni.GetIntValue(passname.c_str(),"blenddst");
	
	
	return true;
}

bool ZMaterial::Create(string strName)
{
	return LoadShader(strName.c_str());
}

ZFResource* Create__Material()
{
	return new ZMaterial;
}





