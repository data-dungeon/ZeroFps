#include "zmaterial.h"
#include "zshader.h"
#include "../basic/globals.h"

ZMaterialSettings::ZMaterialSettings()
{
	m_iTUs[0] = -1;
	m_iTUs[1] = -1;		
	m_iTUs[2] = -1;			
	m_iTUs[3] = -1;			
	
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
};

ZMaterial::ZMaterial()
{
	Clear();

	ZMaterialSettings* first = AddPass();	
	first->m_iTUs[0] = 1; 


}

ZMaterialSettings* ZMaterial::GetPass(int iPass)
{
	if(iPass >= m_kPasses.size())
		return NULL;
		
	return &m_kPasses[iPass];
}

ZMaterialSettings* ZMaterial::AddPass()
{
	ZMaterialSettings temp;	
	m_kPasses.push_back(temp);

	return &m_kPasses.back();
}

void ZMaterial::Clear()
{
	m_kPasses.clear();

	m_bCopyData = false;
	m_bRandomMovements = false;
	m_bWaves = false;	
}

bool ZMaterial::LoadShader(const char* acFile)
{
	if(m_kIni.Open(acFile,0))
	{
		Clear();
	
		
		if(!LoadGlobalSection())
			return false;
			 
		
		int iPass=0;	
		while(LoadPass(iPass))
		{
			iPass++;
		}
	
	
		cout<<"shader loaded: found "<<iPass<<endl;
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
	
	cout<<"loading pass "<<passname<<endl;
	
	if(!m_kIni.SectionExist(passname.c_str()))
	{
		cout<<passname.c_str()<<" section is missing"<<endl;
		return false;
	}
	
	//load pass settings
	ZMaterialSettings* newpass = AddPass();
	
	
	if(m_kIni.KeyExist(passname.c_str(),"lighting"))
		newpass->m_bLighting = m_kIni.GetBoolValue(passname.c_str(),"lighting");
	if(m_kIni.KeyExist(passname.c_str(),"cullface"))
		newpass->m_bCullFace = m_kIni.GetBoolValue(passname.c_str(),"cullface");
	if(m_kIni.KeyExist(passname.c_str(),"alphatest"))
		newpass->m_bAlphaTest = m_kIni.GetBoolValue(passname.c_str(),"alphatest");
	
	
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
	
	
	return true;
}
