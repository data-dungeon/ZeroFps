#include "zmaterial.h"
#include "zshader.h"
#include "../basic/globals.h"
 

map<string,int> ZMaterial::m_kEnums;

ZMaterialSettings::ZMaterialSettings()
{
	m_kTUs[0] = new ZFResourceHandle();				// LEAK - MistClient, Level loaded.
	m_kTUs[1] = new ZFResourceHandle();
	m_kTUs[2] = new ZFResourceHandle();				// LEAK - MistClient, Level loaded.
	m_kTUs[3] = new ZFResourceHandle();				// LEAK - MistClient, Level loaded.
	
	m_pkVP = new ZFResourceHandle();					// LEAK - MistClient, Level loaded.
	m_pkFP = new ZFResourceHandle();
	
	m_iTUTexCords[0] = CORDS_FROM_ARRAY_0;
	m_iTUTexCords[1] = CORDS_FROM_ARRAY_1;	
	m_iTUTexCords[2] = CORDS_FROM_ARRAY_2;	
	m_iTUTexCords[3] = CORDS_FROM_ARRAY_3;	
	
	m_iPolygonModeFront = LINE_POLYGON;
	m_iPolygonModeBack =	LINE_POLYGON;			
	
	m_kVertexColor.Set(1,1,1,1);
	
	m_bColorMaterial = false;
	m_bLighting =	 true;
	m_bCullFace = 	true;
	m_bAlphaTest =	false;
   m_bDepthTest = true;
	m_iDepthFunc =	LESS_DEPTH;
	
	m_bBlend	= false;
	m_iBlendSrc = ONE_BLEND_SRC; 
	m_iBlendDst = ZERO_BLEND_DST; 
		
	m_iTextureColorEffect = -1;
	
};

ZMaterialSettings::~ZMaterialSettings()
{
	for(int i=0;i<4;i++)
	{
		if(m_kTUs[i])
			delete m_kTUs[i];	
			
		m_kTUs[i] = NULL;
		
	}

	if(m_pkVP)
		delete m_pkVP;

	if(m_pkFP)
		delete m_pkFP;
		
			
	m_pkVP = NULL;
	m_pkFP = NULL;
}

ZMaterial::ZMaterial()
{
	SetupEnums();
	Clear();

	ZMaterialSettings* first = AddPass();	
	
	
	//cout<<"bah:"<<GetTranslateEnum("SRC_ALPHA_BLEND_DST")<<endl;;
	
}

ZMaterial::~ZMaterial()
{
	Clear();
}

ZMaterialSettings* ZMaterial::GetPass(int iPass)
{
	if(iPass >= (int) m_kPasses.size())
		return NULL;
	
	return m_kPasses[iPass];
}

ZMaterialSettings* ZMaterial::AddPass()
{
	m_kPasses.push_back(new ZMaterialSettings());		// LEAK - MistClient, Level loaded.
	return m_kPasses.back();
}

void ZMaterial::Clear()
{
	for(unsigned int i=0;i<m_kPasses.size();i++)
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
		if(m_kIni.Open("data/material/nomaterial.zmt",0))
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

	// load textureoffset values
	if(m_kIni.KeyExist("global","textureoffset_u"))
		m_faTextureOffset[0] = m_kIni.GetFloatValue("global","textureoffset_u");
	else
		m_faTextureOffset[0] = 0;

	if(m_kIni.KeyExist("global","textureoffset_v"))
		m_faTextureOffset[1] = m_kIni.GetFloatValue("global","textureoffset_v");
	else
		m_faTextureOffset[1] = 0;

	if( (m_faTextureOffset[0] + m_faTextureOffset[1]) != 0 )
	{
		m_bTextureOffset = true;
	}
	else
		m_bTextureOffset = false;




	return true;
}

bool ZMaterial::LoadPass(int iPass)
{
	string passname;

	// Funkar inte i VC7...
	//passname="pass-";	
	//char nr; 
	//IntToChar(&nr,iPass);			
	//passname+=nr;

	char temp[50];
	sprintf(temp, "pass-%i", iPass);
	passname = temp;
	
	if(!m_kIni.SectionExist(passname.c_str()))
	{
		//cout<<passname.c_str()<<" section is missing"<<endl;
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
	
	if(m_kIni.KeyExist(passname.c_str(),"tu0"))
		newpass->m_kTUs[0]->SetRes(m_kIni.GetValue(passname.c_str(),"tu0"));
	if(m_kIni.KeyExist(passname.c_str(),"tu1"))
		newpass->m_kTUs[1]->SetRes(m_kIni.GetValue(passname.c_str(),"tu1"));
	if(m_kIni.KeyExist(passname.c_str(),"tu2"))
		newpass->m_kTUs[2]->SetRes(m_kIni.GetValue(passname.c_str(),"tu2"));
	if(m_kIni.KeyExist(passname.c_str(),"tu3"))
		newpass->m_kTUs[3]->SetRes(m_kIni.GetValue(passname.c_str(),"tu3"));

	if(m_kIni.KeyExist(passname.c_str(),"vertexprogram"))
		newpass->m_pkVP->SetRes(m_kIni.GetValue(passname.c_str(),"vertexprogram"));

	if(m_kIni.KeyExist(passname.c_str(),"fragmentprogram"))
		newpass->m_pkFP->SetRes(m_kIni.GetValue(passname.c_str(),"fragmentprogram"));

				
			
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords0"))
		newpass->m_iTUTexCords[0] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexcords0"));
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords1"))
		newpass->m_iTUTexCords[1] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexcords1"));
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords2"))
		newpass->m_iTUTexCords[2] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexcords2"));
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords3"))
		newpass->m_iTUTexCords[3] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexcords3"));
	
	
	if(m_kIni.KeyExist(passname.c_str(),"polygonmodefront"))
		newpass->m_iPolygonModeFront = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"polygonmodefront"));
	if(m_kIni.KeyExist(passname.c_str(),"polygonmodeback"))
		newpass->m_iPolygonModeBack = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"polygonmodeback"));
	if(m_kIni.KeyExist(passname.c_str(),"depthfunc"))
		newpass->m_iDepthFunc =GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"depthfunc"));
	
	if(m_kIni.KeyExist(passname.c_str(),"blend"))
		newpass->m_bBlend = m_kIni.GetBoolValue(passname.c_str(),"blend");
	if(m_kIni.KeyExist(passname.c_str(),"blendsrc"))
		newpass->m_iBlendSrc = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"blendsrc"));
	if(m_kIni.KeyExist(passname.c_str(),"blenddst"))
		newpass->m_iBlendDst = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"blenddst"));
	
   if(m_kIni.KeyExist(passname.c_str(),"depthtest"))
      newpass->m_bDepthTest = m_kIni.GetBoolValue(passname.c_str(),"depthtest");
	
	if(m_kIni.KeyExist(passname.c_str(),"colormaterial"))
      newpass->m_bColorMaterial = m_kIni.GetBoolValue(passname.c_str(),"colormaterial");		
	
	//get vertexcolor R G B A values
	if(m_kIni.KeyExist(passname.c_str(),"vertexcolor-r"))
      newpass->m_kVertexColor.x = m_kIni.GetFloatValue(passname.c_str(),"vertexcolor-r");	
	if(m_kIni.KeyExist(passname.c_str(),"vertexcolor-g"))
      newpass->m_kVertexColor.y = m_kIni.GetFloatValue(passname.c_str(),"vertexcolor-g");	
	if(m_kIni.KeyExist(passname.c_str(),"vertexcolor-b"))
      newpass->m_kVertexColor.z = m_kIni.GetFloatValue(passname.c_str(),"vertexcolor-b");	
	if(m_kIni.KeyExist(passname.c_str(),"vertexcolor-a"))
      newpass->m_kVertexColor.w = m_kIni.GetFloatValue(passname.c_str(),"vertexcolor-a");	
	
	
	//get effects	
	if(m_kIni.KeyExist(passname.c_str(),"coloreffect"))
		newpass->m_iTextureColorEffect = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"coloreffect"));
	
	
	return true;
}

bool ZMaterial::Create(string strName)
{
	return LoadShader(strName.c_str());
} 

int ZMaterial::CalculateSize()
{
	int iSizeInByes = 0;
	iSizeInByes += sizeof(ZMaterial);

	// Add Size for all pass.
	return iSizeInByes;

}

void ZMaterial::SetupEnums()
{
	if(!m_kEnums.empty())
		return;
		
	//numbervalues
	m_kEnums["-1"] = 									-1;	
	m_kEnums["0"] = 									0;
	m_kEnums["1"] = 									1;
	m_kEnums["2"] = 									2;
	m_kEnums["3"] = 									3;
	m_kEnums["4"] = 									4;
	m_kEnums["5"] = 									5;
	m_kEnums["6"] = 									6;
	m_kEnums["7"] = 									7;
	m_kEnums["8"] = 									8;
	m_kEnums["9"] = 									9;
	
	//texture cords
	m_kEnums["TEX_ARAY_0"] = 						0;
	m_kEnums["TEX_ARAY_1"] = 						1;
	m_kEnums["TEX_ARAY_2"] = 						2;
	m_kEnums["TEX_ARAY_3"] = 						3;
	m_kEnums["AUTO_OBJLIN"] = 						4;
	m_kEnums["AUTO_EYELIN"] = 						5;
	m_kEnums["AUTP_SPHERE"] = 						6;
	
	//polygon mode
	m_kEnums["FILL"] = 								0;
	m_kEnums["LINE"] = 								1;
	m_kEnums["POINT"] = 								2;
	
	//depth func
	m_kEnums["NEVER_DEPTH"] = 						0;
	m_kEnums["LESS_DEPTH"] = 						1;
	m_kEnums["EQUAL_DEPTH"] = 						2;
	m_kEnums["LEQUAL_DEPTH"] = 					3;
	m_kEnums["GREATER_DEPTH"] = 					4;
	m_kEnums["NOTEQUAL_DEPTH"] = 					5;
	m_kEnums["GEQUAL_DEPTH"] = 					6;
	m_kEnums["ALWAYS_DEPTH"] = 					7;
	

	//blends src
	m_kEnums["ZERO_BLEND_SRC"] = 						0;
	m_kEnums["ONE_BLEND_SRC"] = 						1;
	m_kEnums["DST_COLOR_BLEND_SRC"] = 				2;
	m_kEnums["ONE_MINUS_DST_COLOR_BLEND_SRC"] = 	3;
	m_kEnums["SRC_ALPHA_BLEND_SRC"] = 				4;
	m_kEnums["ONE_MINUS_SRC_ALPHA_BLEND_SRC"] = 	5;
	m_kEnums["DST_ALPHA_BLEND_SRC"] = 				6;
	m_kEnums["ONE_MINUS_DST_ALPHA_BLEND_SRC"] = 	7;
	m_kEnums["SRC_ALPHA_SATURATE_BLEND_SRC"] = 	8;

	//blends dst
	m_kEnums["ZERO_BLEND_SRC"] = 						0;
	m_kEnums["ONE_BLEND_SRC"] = 						1;
	m_kEnums["SRC_COLOR_BLEND_DST"] = 				2;
	m_kEnums["ONE_MINUS_SRC_COLOR_BLEND_DST"] = 	3;
	m_kEnums["SRC_ALPHA_BLEND_DST"] = 				4;
	m_kEnums["ONE_MINUS_SRC_ALPHA_BLEND_DST"] = 	5;
	m_kEnums["DST_ALPHA_BLEND_DST"] = 				6;
	m_kEnums["ONE_MINUS_DST_ALPHA_BLEND_DST"] = 	7;

	//color effect
	m_kEnums["DIABLED"] = 								-1;
	m_kEnums["FLICKERING"] = 							1;
	m_kEnums["PULSATING"] = 							2;		
}

int ZMaterial::GetTranslateEnum(string strEnum)
{
	map<string,int> ::iterator it = m_kEnums.find(strEnum);
	
	if(it != m_kEnums.end())
		return (*it).second;
	else
	{
		cout<<"WARNING: could not find shader enum value for: "<<strEnum<<endl;
		return -1;
	}
}


ZFResource* Create__Material()
{
	return new ZMaterial;						// LEAK - MistClient, Level loaded.
}





