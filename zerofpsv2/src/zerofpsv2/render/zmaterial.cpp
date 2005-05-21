
#include "zmaterial.h"
#include "../basic/globals.h"
#include "zshadersystem.h" 

map<string,int> ZMaterial::m_kEnums;
int ZMaterial::m_iNextID = 1;

ZMaterialSettings::ZMaterialSettings()
{
	m_kTUs[0] = new ZFResourceHandle();				// LEAK - MistClient, Level loaded.
	m_kTUs[1] = new ZFResourceHandle();
	m_kTUs[2] = new ZFResourceHandle();				// LEAK - MistClient, Level loaded.
	m_kTUs[3] = new ZFResourceHandle();				// LEAK - MistClient, Level loaded.
	
	m_pkVP = new ZFResourceHandle();					// LEAK - MistClient, Level loaded.
	m_pkFP = new ZFResourceHandle();
	
	m_pkSLP = new ZFResourceHandle();
	
	m_iTUTexCords[0] = CORDS_FROM_ARRAY_0;
	m_iTUTexCords[1] = CORDS_FROM_ARRAY_1;	
	m_iTUTexCords[2] = CORDS_FROM_ARRAY_2;	
	m_iTUTexCords[3] = CORDS_FROM_ARRAY_3;	
	
	m_iTUTexEnvMode[0] = TEXENV_MODE_MODULATE;
	m_iTUTexEnvMode[1] = TEXENV_MODE_MODULATE;
	m_iTUTexEnvMode[2] = TEXENV_MODE_MODULATE;
	m_iTUTexEnvMode[3] = TEXENV_MODE_MODULATE;
	
	m_fTUTexGenScale[0] = 1.0;
	m_fTUTexGenScale[1] = 1.0;
	m_fTUTexGenScale[2] = 1.0;
	m_fTUTexGenScale[3] = 1.0;
	
	m_iPolygonModeFront = FILL_POLYGON;
	m_iPolygonModeBack =	FILL_POLYGON;			
	
	m_kVertexColor.Set(1,1,1,1);
	
	m_fLineWidth = 	1.0;
	
	m_bColorMaterial= false;
	m_bLighting =		true;
	m_bAlphaTest =		false;
   m_bDepthTest = 	true;
	m_bStencilTest=	false;
	m_iDepthFunc =		DEPTHFUNC_LEQUAL;
	m_bFog=				true;
	
	m_iStencilOpFail=		STENCILOP_KEEP;
	m_iStencilOpZFail=	STENCILOP_KEEP;
	m_iStencilOpZPass = 	STENCILOP_KEEP;
	
	m_iStencilFunc = 		STENCILFUNC_ALWAYS;
	m_iStencilFuncRef =	0;
	m_iStencilFuncMask = 255;
	
	m_iCullFace =		CULL_FACE_BACK;
	
	m_bColorMask=		true;
	m_bDepthMask=		true;
	
	m_bBlend	= 			false;
	m_iBlendSrc = 		ONE_BLEND_SRC; 
	m_iBlendDst = 		ZERO_BLEND_DST; 
		
	m_kMatAmbient.Set(0.2,0.2,0.2,1.0);
	m_kMatDiffuse.Set(0.8,0.8,0.8,1.0);
	m_kMatSpecular.Set(0.0,0.0,0.0,1.0);
	m_kMatEmission.Set(0.0,0.0,0.0,1.0);
	m_fShininess = 0.0;
	
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
		
	if(m_pkSLP)
		delete m_pkSLP;
			
	m_pkVP = NULL;
	m_pkFP = NULL;
	m_pkSLP = NULL;
}

ZMaterial::ZMaterial()
{
	m_pkScript = NULL;
	m_pkMaterialScript = NULL;
	
	//setup material ID
	m_iID = m_iNextID;
	m_iNextID++;
	
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

	m_bRandomMovements 	= false;
	m_bWaves 				= false;	
	m_bTextureOffset 		= false;
	
	m_strName = "UnNamed";
	
	
	if(m_pkMaterialScript)
	{
		delete m_pkMaterialScript;
		m_pkMaterialScript = NULL;
	}
}


bool ZMaterial::LoadLuaMaterial(const char* acFile)
{
	Clear();

	//get script system pointer
	if(!m_pkScript)
		m_pkScript = static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));

		
	//create script object
	if(!m_pkMaterialScript)
		m_pkMaterialScript = new ZFScript;
	
	//load script
	if(m_pkMaterialScript->Create(acFile))
	{
 		SI_ZMATERIAL::g_pkCurrentMaterial = this;
		if(m_pkScript->Call(m_pkMaterialScript, "Main",0,0))
		{
			SI_ZMATERIAL::g_pkCurrentMaterial = NULL;	
			return true;		
		}
		else
		{
			cout<<"Lua material "<<acFile<< " did not contain any Main() function"<<endl;
		}
		
		SI_ZMATERIAL::g_pkCurrentMaterial = NULL;
	}

	
	return false;
}

void ZMaterial::LuaMaterialEndPass(int iPass)
{
	ZMaterialSettings* newpass =  GetPass(iPass);
	
	if(!newpass)
	{
		cout<<"ERROR: LuaMaterialEndPass  missin pass , this shuld not hapen"<<endl;
		return;	
	}
		
	char 		ctemp[128];	
	double 	dtemp;
	
	//LOAD PASS SPECIFIC SETTINGS
	
	//GLSL program
	string strSLVS;
	string strSLFS;
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"slvertexshader",ctemp))
		strSLVS = ctemp;
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"slfragmentshader",ctemp))
		strSLFS = ctemp;
	if(!strSLVS.empty() || !strSLFS.empty())
		newpass->m_pkSLP->SetRes(strSLVS+string("#")+strSLFS+string(".glsl"));

	// ARB vertex program
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"vertexprogram",ctemp))
		newpass->m_pkVP->SetRes(ctemp);		
	// ARB fragment program	
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"fragmentprogram",ctemp))
		newpass->m_pkFP->SetRes(ctemp);
	
			
	//textures
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texture0",ctemp))
		newpass->m_kTUs[0]->SetRes(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texture1",ctemp))
		newpass->m_kTUs[1]->SetRes(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texture2",ctemp))
		newpass->m_kTUs[2]->SetRes(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texture3",ctemp))
		newpass->m_kTUs[3]->SetRes(ctemp);

	//texutre cordinats
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texcords0",ctemp))
		newpass->m_iTUTexCords[0] = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texcords1",ctemp))
		newpass->m_iTUTexCords[1] = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texcords2",ctemp))
		newpass->m_iTUTexCords[2] = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texcords3",ctemp))
		newpass->m_iTUTexCords[3] = GetTranslateEnum(ctemp);

	
	//texutre enviroment mode
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texenvmode0",ctemp))
		newpass->m_iTUTexEnvMode[0] = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texenvmode1",ctemp))
		newpass->m_iTUTexEnvMode[1] = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texenvmode2",ctemp))
		newpass->m_iTUTexEnvMode[2] = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texenvmode3",ctemp))
		newpass->m_iTUTexEnvMode[3] = GetTranslateEnum(ctemp);

		
	//texture generation scale
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texgenscale0",dtemp))
		newpass->m_fTUTexGenScale[0] = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texgenscale1",dtemp))
		newpass->m_fTUTexGenScale[1] = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texgenscale2",dtemp))
		newpass->m_fTUTexGenScale[2] = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"texgenscale3",dtemp))
		newpass->m_fTUTexGenScale[3] = float(dtemp);

		
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"polygonmodefront",ctemp))
		newpass->m_iPolygonModeFront = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"polygonmodeback",ctemp))
		newpass->m_iPolygonModeBack = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"depthfunc",ctemp))
		newpass->m_iDepthFunc = GetTranslateEnum(ctemp);

	//lots a bools
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"fog",ctemp))
		newpass->m_bFog = bool(GetTranslateEnum(ctemp));
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"colormask",ctemp))
		newpass->m_bColorMask = bool(GetTranslateEnum(ctemp));
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"depthmask",ctemp))
		newpass->m_bDepthMask = bool(GetTranslateEnum(ctemp));		
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"depthtest",ctemp))
		newpass->m_bDepthTest = bool(GetTranslateEnum(ctemp));
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"stenciltest",ctemp))
		newpass->m_bStencilTest = bool(GetTranslateEnum(ctemp));		
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"lighting",ctemp))
		newpass->m_bLighting = bool(GetTranslateEnum(ctemp));
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"cullface",ctemp))
		newpass->m_iCullFace = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"alphatest",ctemp))
		newpass->m_bAlphaTest = bool(GetTranslateEnum(ctemp));		
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"colormaterial",ctemp))
		newpass->m_bColorMaterial = bool(GetTranslateEnum(ctemp));		
				
		
		
	//blending
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"blend",ctemp))
		newpass->m_bBlend = bool(GetTranslateEnum(ctemp));
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"blendsrc",ctemp))
		newpass->m_iBlendSrc = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"blenddst",ctemp))
		newpass->m_iBlendDst = GetTranslateEnum(ctemp);

		
	//stencil stuff
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"stencilop-fail",ctemp))
		newpass->m_iStencilOpFail = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"stencilop-zfail",ctemp))
		newpass->m_iStencilOpZFail = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"stencilop-zpass",ctemp))
		newpass->m_iStencilOpZPass = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"stencilfunc",ctemp))
		newpass->m_iStencilFunc = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"stencilfunc-ref",ctemp))
		newpass->m_iStencilFuncRef = GetTranslateEnum(ctemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"stencilfunc-mask",ctemp))
		newpass->m_iStencilFuncMask = GetTranslateEnum(ctemp);


	//line width
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"linewidth",dtemp))
		newpass->m_fLineWidth = float(dtemp);
	

	//get vertexcolor R G B A values
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"vertexcolor-r",dtemp))
		newpass->m_kVertexColor.x = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"vertexcolor-g",dtemp))
		newpass->m_kVertexColor.y = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"vertexcolor-b",dtemp))
		newpass->m_kVertexColor.z = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"vertexcolor-a",dtemp))
		newpass->m_kVertexColor.w = float(dtemp);

	
	//material settings
	//ambient
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matambient-r",dtemp))
		newpass->m_kMatAmbient.x = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matambient-g",dtemp))
		newpass->m_kMatAmbient.y = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matambient-b",dtemp))
		newpass->m_kMatAmbient.z = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matambient-a",dtemp))
		newpass->m_kMatAmbient.w = float(dtemp);
	
	//diffuse
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matdiffuse-r",dtemp))
		newpass->m_kMatDiffuse.x = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matdiffuse-g",dtemp))
		newpass->m_kMatDiffuse.y = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matdiffuse-b",dtemp))
		newpass->m_kMatDiffuse.z = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matdiffuse-a",dtemp))
		newpass->m_kMatDiffuse.w = float(dtemp);

	
	//specular
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matspecular-r",dtemp))
		newpass->m_kMatSpecular.x = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matspecular-g",dtemp))
		newpass->m_kMatSpecular.y = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matspecular-b",dtemp))
		newpass->m_kMatSpecular.z = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matspecular-a",dtemp))
		newpass->m_kMatSpecular.w = float(dtemp);
	
	//emission
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matemission-r",dtemp))
		newpass->m_kMatEmission.x = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matemission-g",dtemp))
		newpass->m_kMatEmission.y = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matemission-b",dtemp))
		newpass->m_kMatEmission.z = float(dtemp);
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matemission-a",dtemp))
		newpass->m_kMatEmission.w = float(dtemp);

				
	//shininess
	if(m_pkScript->GetGlobal(m_pkMaterialScript->m_pkLuaState,"matshininess",dtemp))
		newpass->m_fShininess = float(dtemp);
		
}

bool ZMaterial::LoadIniMaterial(const char* acFile)
{
	//TRY to load ZLM material first
	string zlmfile = acFile;
	zlmfile = zlmfile.substr(0,zlmfile.length()-3)+string("zlm");
	if(LoadLuaMaterial(zlmfile.c_str()))
		return true;
		

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
	
		m_strName = acFile;
		
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

	if( m_faTextureOffset[0] != 0 || m_faTextureOffset[1] != 0)
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
		newpass->m_iCullFace = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"cullface"));
		
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

		
	string strSLVS;
	string strSLFS;
	if(m_kIni.KeyExist(passname.c_str(),"slvertexshader"))
		strSLVS = m_kIni.GetValue(passname.c_str(),"slvertexshader");
	if(m_kIni.KeyExist(passname.c_str(),"slfragmentshader"))
		strSLFS = m_kIni.GetValue(passname.c_str(),"slfragmentshader");

	if(!strSLVS.empty() || !strSLFS.empty())
	{
		newpass->m_pkSLP->SetRes(strSLVS+string("#")+strSLFS+string(".glsl"));
	}			
			
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords0"))
		newpass->m_iTUTexCords[0] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexcords0"));
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords1"))
		newpass->m_iTUTexCords[1] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexcords1"));
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords2"))
		newpass->m_iTUTexCords[2] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexcords2"));
	if(m_kIni.KeyExist(passname.c_str(),"tutexcords3"))
		newpass->m_iTUTexCords[3] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexcords3"));
	
	if(m_kIni.KeyExist(passname.c_str(),"tutexenvmode0"))
		newpass->m_iTUTexEnvMode[0] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexenvmode0"));
	if(m_kIni.KeyExist(passname.c_str(),"tutexenvmode1"))
		newpass->m_iTUTexEnvMode[1] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexenvmode1"));
	if(m_kIni.KeyExist(passname.c_str(),"tutexenvmode2"))
		newpass->m_iTUTexEnvMode[2] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexenvmode2"));
	if(m_kIni.KeyExist(passname.c_str(),"tutexenvmode3"))
		newpass->m_iTUTexEnvMode[3] = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"tutexenvmode3"));
			
	if(m_kIni.KeyExist(passname.c_str(),"tutexgenscale0"))
		newpass->m_fTUTexGenScale[0] = m_kIni.GetFloatValue(passname.c_str(),"tutexgenscale0");
	if(m_kIni.KeyExist(passname.c_str(),"tutexgenscale1"))
		newpass->m_fTUTexGenScale[1] = m_kIni.GetFloatValue(passname.c_str(),"tutexgenscale1");
	if(m_kIni.KeyExist(passname.c_str(),"tutexgenscale2"))
		newpass->m_fTUTexGenScale[2] = m_kIni.GetFloatValue(passname.c_str(),"tutexgenscale2");
	if(m_kIni.KeyExist(passname.c_str(),"tutexgenscale3"))
		newpass->m_fTUTexGenScale[3] = m_kIni.GetFloatValue(passname.c_str(),"tutexgenscale3");
				

	if(m_kIni.KeyExist(passname.c_str(),"polygonmodefront"))
		newpass->m_iPolygonModeFront = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"polygonmodefront"));
	if(m_kIni.KeyExist(passname.c_str(),"polygonmodeback"))
		newpass->m_iPolygonModeBack = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"polygonmodeback"));
	if(m_kIni.KeyExist(passname.c_str(),"depthfunc"))
		newpass->m_iDepthFunc =GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"depthfunc"));
	
	if(m_kIni.KeyExist(passname.c_str(),"fog"))
		newpass->m_bFog = m_kIni.GetBoolValue(passname.c_str(),"fog");
	
	if(m_kIni.KeyExist(passname.c_str(),"colormask"))
		newpass->m_bColorMask = m_kIni.GetBoolValue(passname.c_str(),"colormask");
	if(m_kIni.KeyExist(passname.c_str(),"depthmask"))
		newpass->m_bDepthMask = m_kIni.GetBoolValue(passname.c_str(),"depthmask");
	
	if(m_kIni.KeyExist(passname.c_str(),"blend"))
		newpass->m_bBlend = m_kIni.GetBoolValue(passname.c_str(),"blend");
	if(m_kIni.KeyExist(passname.c_str(),"blendsrc"))
		newpass->m_iBlendSrc = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"blendsrc"));
	if(m_kIni.KeyExist(passname.c_str(),"blenddst"))
		newpass->m_iBlendDst = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"blenddst"));
	
   if(m_kIni.KeyExist(passname.c_str(),"depthtest"))
      newpass->m_bDepthTest = m_kIni.GetBoolValue(passname.c_str(),"depthtest");
   
	if(m_kIni.KeyExist(passname.c_str(),"stenciltest"))
      newpass->m_bStencilTest = m_kIni.GetBoolValue(passname.c_str(),"stenciltest");

	if(m_kIni.KeyExist(passname.c_str(),"stencilop-fail"))
		newpass->m_iStencilOpFail = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"stencilop-fail"));
	if(m_kIni.KeyExist(passname.c_str(),"stencilop-zfail"))
		newpass->m_iStencilOpZFail = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"stencilop-zfail"));
	if(m_kIni.KeyExist(passname.c_str(),"stencilop-zpass"))
		newpass->m_iStencilOpZPass = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"stencilop-zpass"));
		
	if(m_kIni.KeyExist(passname.c_str(),"stencilfunc"))
		newpass->m_iStencilFunc = GetTranslateEnum(m_kIni.GetValue(passname.c_str(),"stencilfunc"));
	if(m_kIni.KeyExist(passname.c_str(),"stencilfunc-ref"))
		newpass->m_iStencilFuncRef = m_kIni.GetIntValue(passname.c_str(),"stencilfunc-ref");
	if(m_kIni.KeyExist(passname.c_str(),"stencilfunc-mask"))
		newpass->m_iStencilFuncMask = m_kIni.GetIntValue(passname.c_str(),"stencilfunc-mask");

					
	if(m_kIni.KeyExist(passname.c_str(),"colormaterial"))
      newpass->m_bColorMaterial = m_kIni.GetBoolValue(passname.c_str(),"colormaterial");		

	//line width
	if(m_kIni.KeyExist(passname.c_str(),"linewidth"))
      newpass->m_fLineWidth = m_kIni.GetFloatValue(passname.c_str(),"linewidth");	

			
	//get vertexcolor R G B A values
	if(m_kIni.KeyExist(passname.c_str(),"vertexcolor-r"))
      newpass->m_kVertexColor.x = m_kIni.GetFloatValue(passname.c_str(),"vertexcolor-r");	
	if(m_kIni.KeyExist(passname.c_str(),"vertexcolor-g"))
      newpass->m_kVertexColor.y = m_kIni.GetFloatValue(passname.c_str(),"vertexcolor-g");	
	if(m_kIni.KeyExist(passname.c_str(),"vertexcolor-b"))
      newpass->m_kVertexColor.z = m_kIni.GetFloatValue(passname.c_str(),"vertexcolor-b");	
	if(m_kIni.KeyExist(passname.c_str(),"vertexcolor-a"))
      newpass->m_kVertexColor.w = m_kIni.GetFloatValue(passname.c_str(),"vertexcolor-a");	
	
	//material settings
	//ambient
	if(m_kIni.KeyExist(passname.c_str(),"matambient-r"))
      newpass->m_kMatAmbient.x = m_kIni.GetFloatValue(passname.c_str(),"matambient-r");	
	if(m_kIni.KeyExist(passname.c_str(),"matambient-g"))
      newpass->m_kMatAmbient.y = m_kIni.GetFloatValue(passname.c_str(),"matambient-g");	
	if(m_kIni.KeyExist(passname.c_str(),"matambient-b"))
      newpass->m_kMatAmbient.z = m_kIni.GetFloatValue(passname.c_str(),"matambient-b");	
	if(m_kIni.KeyExist(passname.c_str(),"matambient-a"))
      newpass->m_kMatAmbient.w = m_kIni.GetFloatValue(passname.c_str(),"matambient-a");	

	//diffuse
	if(m_kIni.KeyExist(passname.c_str(),"matdiffuse-r"))
      newpass->m_kMatDiffuse.x = m_kIni.GetFloatValue(passname.c_str(),"matdiffuse-r");	
	if(m_kIni.KeyExist(passname.c_str(),"matdiffuse-g"))
      newpass->m_kMatDiffuse.y = m_kIni.GetFloatValue(passname.c_str(),"matdiffuse-g");	
	if(m_kIni.KeyExist(passname.c_str(),"matdiffuse-b"))
      newpass->m_kMatDiffuse.z = m_kIni.GetFloatValue(passname.c_str(),"matdiffuse-b");	
	if(m_kIni.KeyExist(passname.c_str(),"matdiffuse-a"))
      newpass->m_kMatDiffuse.w = m_kIni.GetFloatValue(passname.c_str(),"matdiffuse-a");	
	
	//specular
	if(m_kIni.KeyExist(passname.c_str(),"matspecular-r"))
      newpass->m_kMatSpecular.x = m_kIni.GetFloatValue(passname.c_str(),"matspecular-r");	
	if(m_kIni.KeyExist(passname.c_str(),"matspecular-g"))
      newpass->m_kMatSpecular.y = m_kIni.GetFloatValue(passname.c_str(),"matspecular-g");	
	if(m_kIni.KeyExist(passname.c_str(),"matspecular-b"))
      newpass->m_kMatSpecular.z = m_kIni.GetFloatValue(passname.c_str(),"matspecular-b");	
	if(m_kIni.KeyExist(passname.c_str(),"matspecular-a"))
      newpass->m_kMatSpecular.w = m_kIni.GetFloatValue(passname.c_str(),"matspecular-a");	
	
	//emission
	if(m_kIni.KeyExist(passname.c_str(),"matemission-r"))
      newpass->m_kMatEmission.x = m_kIni.GetFloatValue(passname.c_str(),"matemission-r");	
	if(m_kIni.KeyExist(passname.c_str(),"matemission-g"))
      newpass->m_kMatEmission.y = m_kIni.GetFloatValue(passname.c_str(),"matemission-g");	
	if(m_kIni.KeyExist(passname.c_str(),"matemission-b"))
      newpass->m_kMatEmission.z = m_kIni.GetFloatValue(passname.c_str(),"matemission-b");	
	if(m_kIni.KeyExist(passname.c_str(),"matemission-a"))
      newpass->m_kMatEmission.w = m_kIni.GetFloatValue(passname.c_str(),"matemission-a");	
				
	//shininess
	if(m_kIni.KeyExist(passname.c_str(),"matshininess"))
      newpass->m_fShininess = m_kIni.GetFloatValue(passname.c_str(),"matshininess");	
	
		
	
	return true;
}


bool ZMaterial::Create(string strName)
{
	if(strName.substr(strName.length()-4) == ".zmt")
	{
		//cout<<"loading ini material"<<endl;
	 	return LoadIniMaterial(strName.c_str());	
	}
	else if(strName.substr(strName.length()-4) == ".zlm")
	{
		//cout<<"loading lua material"<<endl;
	 	return LoadLuaMaterial(strName.c_str());		
	}

// 	return LoadShader(strName.c_str());
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
		
	m_kEnums["true"] = 								1;	
	m_kEnums["false"] = 								0;			
	m_kEnums["TRUE"] = 								1;	
	m_kEnums["FALSE"] = 								0;		
	m_kEnums["True"] = 								1;	
	m_kEnums["False"] = 								0;		
		
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
	m_kEnums["AUTO_SPHERE"] = 						6;
	
	
	m_kEnums["CORDS_FROM_ARRAY_0"] = 			0;
	m_kEnums["CORDS_FROM_ARRAY_1"] = 			1;
	m_kEnums["CORDS_FROM_ARRAY_2"] = 			2;
	m_kEnums["CORDS_FROM_ARRAY_3"] = 			3;	
	m_kEnums["CORDS_OBJECT_LINEAR"] = 			4;
	m_kEnums["CORDS_EYE_LINEAR"] = 				5;
	m_kEnums["CORDS_SPHERE_MAP"] = 				6;	
	
	//texture env mode
	m_kEnums["TEXENV_MODE_MODULATE"] = 			0;
	m_kEnums["TEXENV_MODE_DECAL"] = 				1;
	m_kEnums["TEXENV_MODE_BLEND"] = 				2;
	m_kEnums["TEXENV_MODE_REPLACE"] = 			3;
	
	//polygon mode
	m_kEnums["FILL"] = 								0;
	m_kEnums["LINE"] = 								1;
	m_kEnums["POINT"] = 								2;
	
	//depth func
	m_kEnums["DEPTHFUNC_NEVER"] = 				0;
	m_kEnums["DEPTHFUNC_LESS"] = 					1;
	m_kEnums["DEPTHFUNC_EQUAL"] = 				2;
	m_kEnums["DEPTHFUNC_LEQUAL"] = 				3;
	m_kEnums["DEPTHFUNC_GREATER"] = 				4;
	m_kEnums["DEPTHFUNC_NOTEQUAL"] = 			5;
	m_kEnums["DEPTHFUNC_GEQUAL"] = 				6;
	m_kEnums["DEPTHFUNC_ALWAYS"] = 				7;
	

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
	
	//cullface
	m_kEnums["CULL_FACE_NONE"] = 						0;
	m_kEnums["CULL_FACE_BACK"] = 						1;
	m_kEnums["CULL_FACE_FRONT"] = 					2;		
	m_kEnums["CULL_FACE_ALL"] = 						3;		
	
	//stencil operations
	m_kEnums["STENCILOP_KEEP"] = 						0;
	m_kEnums["STENCILOP_ZERO"] = 						1;
	m_kEnums["STENCILOP_REPLACE"] = 					2;
	m_kEnums["STENCILOP_INCR"] = 						3;
	m_kEnums["STENCILOP_DECR"] = 						4;
	m_kEnums["STENCILOP_INVERT"] = 					5;	

	//stencil functions
	m_kEnums["STENCILFUNC_NEVER"] = 					0;
	m_kEnums["STENCILFUNC_LESS"] = 					1;
	m_kEnums["STENCILFUNC_LEQUAL"] = 				2;
	m_kEnums["STENCILFUNC_GREATER"] = 				3;
	m_kEnums["STENCILFUNC_GEQUAL"] = 				4;
	m_kEnums["STENCILFUNC_EQUAL"] = 					5;	
	m_kEnums["STENCILFUNC_NOTEQUAL"] = 				6;	
	m_kEnums["STENCILFUNC_ALWAYS"] = 				7;	
	
	
}

int ZMaterial::GetTranslateEnum(string strEnum)
{
	map<string,int> ::iterator it = m_kEnums.find(strEnum);
	
	if(it != m_kEnums.end())
		return (*it).second;
	else
	{
		cout<<"WARNING: could not find shader enum value for: "<<strEnum<<" in material: "<<m_strName<<endl;
		return -1;
	}
}


ZFResource* Create__Material()
{
	return new ZMaterial;						// LEAK - MistClient, Level loaded.
}









// #include "../engine_systems/script_interfaces/si_objectmanager.h" 
// using namespace ObjectManagerLua;
// extern ZFScriptSystem* 	g_pkScript;

//MATERIAL LUA INTERFACE
namespace SI_ZMATERIAL
{
	ZMaterial*			g_pkCurrentMaterial = NULL;
	int					g_iCurrentMaterialPass = -1;
	bool					g_bHaveGLSLSupport;
	ZFScriptSystem*	g_pkScript = NULL;
	
	
	int PassBeginLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,1))
			return 0;		
				
		if(!g_pkCurrentMaterial)
		{
			cout<<"you can only call this function in a material script"<<endl;
			return 0;
		}
			
		//get pass id
		g_pkScript->GetArgInt(pkLua, 0, &g_iCurrentMaterialPass);	
		
		
		//cout<<"starting new pass "<<g_iCurrentMaterialPass<<endl;
		
		while(g_pkCurrentMaterial->GetNrOfPasses() <= g_iCurrentMaterialPass)
		{
			//cout<<"adding new pass"<<endl;
			g_pkCurrentMaterial->AddPass();		
		}

			
		return 0;		
	}	

	int PassEndLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,0))
			return 0;		

		if(!g_pkCurrentMaterial)
		{
			cout<<"you can only call this function in a material script"<<endl;
			return 0;
		}			
					
		//cout<<"ending pass"<<endl;

		g_pkCurrentMaterial->LuaMaterialEndPass(g_iCurrentMaterialPass);
		
						
		return 0;		
	}		
	
	
	int SupportGLSLProgramLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,0))
			return 0;		

		double dRet = 0;
			
		if(ZShaderSystem* pkZShaderSystem =  static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem")))
			if(pkZShaderSystem->SupportGLSLProgram())
				dRet = 1;	
		
		g_pkScript->AddReturnValue(pkLua, dRet);				
		return 1;		
	}		
}


void RegisterSI_Material()
{
	SI_ZMATERIAL::g_pkScript = static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));
	
	SI_ZMATERIAL::g_pkScript->ExposeFunction("PassBegin",		SI_ZMATERIAL::PassBeginLua);
	SI_ZMATERIAL::g_pkScript->ExposeFunction("PassEnd",			SI_ZMATERIAL::PassEndLua);
	SI_ZMATERIAL::g_pkScript->ExposeFunction("SupportGLSLProgram",	SI_ZMATERIAL::SupportGLSLProgramLua);

};








