
#include "zmaterial.h"
#include "../basic/globals.h"
#include "zshadersystem.h" 

#include "../engine/zerofps.h"

map<string,int> ZMaterial::m_kEnums;
int ZMaterial::m_iNextID = 1;

ZMaterialSettings::ZMaterialSettings()
{
	m_pkTUs[0] = new ZFResourceHandle();		
	m_pkTUs[1] = new ZFResourceHandle();
	m_pkTUs[2] = new ZFResourceHandle();		
	m_pkTUs[3] = new ZFResourceHandle();		
	
	m_pkVP = new ZFResourceHandle();				
	m_pkFP = new ZFResourceHandle();
	
	m_pkSLP = new ZFResourceHandle();
	
	m_iTUTexCords[0] = CORDS_FROM_ARRAY_0;
	m_iTUTexCords[1] = CORDS_FROM_ARRAY_0;	
	m_iTUTexCords[2] = CORDS_FROM_ARRAY_0;	
	m_iTUTexCords[3] = CORDS_FROM_ARRAY_0;	
	
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
	
	m_fAlphaTreshold= 0.1;
	
	m_bColorMaterial= false;
	m_bLighting =		true;
	m_bAlphaTest =		false;
   m_bDepthTest = 	true;
	m_bStencilTest=	false;
	m_iDepthFunc =		DEPTHFUNC_LEQUAL;
	m_bFog=				true;
	m_bUseShader	=	true;
	
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
	m_fShininess = 7.5;
	
};

ZMaterialSettings::~ZMaterialSettings()
{
	for(int i=0;i<4;i++)
	{
		delete m_pkTUs[i];				
	}

	
	if(m_pkVP)
		delete m_pkVP;

	if(m_pkFP)
		delete m_pkFP;
		
	if(m_pkSLP)
		delete m_pkSLP;

}

ZMaterialSettings& ZMaterialSettings::operator=(const ZMaterialSettings& kOther)
{
	//copy textures
	for(int i =0;i<4;i++)
	{
		*(m_pkTUs[i]) = *(kOther.m_pkTUs[i]);			
	}

	*m_pkVP = 	*(kOther.m_pkVP);
	*m_pkFP = 	*(kOther.m_pkFP);
	*m_pkSLP = 	*(kOther.m_pkSLP);
		
	for(int i=0;i<4;i++)
	{
		m_iTUTexCords[i] = 	kOther.m_iTUTexCords[i];
		m_iTUTexEnvMode[i] = kOther.m_iTUTexEnvMode[i];
		m_fTUTexGenScale[i] =kOther.m_fTUTexGenScale[i];
	}
		
	m_kVertexColor = 		kOther.m_kVertexColor;		
	m_kMatAmbient = 		kOther.m_kMatAmbient;
	m_kMatDiffuse = 		kOther.m_kMatDiffuse;
	m_kMatSpecular = 		kOther.m_kMatSpecular;
	m_kMatEmission =		kOther.m_kMatEmission;
	m_fShininess = 		kOther.m_fShininess;		
	m_bColorMaterial = 	kOther.m_bColorMaterial;
	m_fLineWidth = 		kOther.m_fLineWidth;
	m_iPolygonModeFront =kOther.m_iPolygonModeFront;
	m_iPolygonModeBack = kOther.m_iPolygonModeBack;
	m_iDepthFunc = 		kOther.m_iDepthFunc;
	m_bLighting = 			kOther.m_bLighting;
	m_iCullFace =			kOther.m_iCullFace;
	m_bAlphaTest = 		kOther.m_bAlphaTest;
	m_fAlphaTreshold = 	kOther.m_fAlphaTreshold;
	m_bFog = 				kOther.m_bFog;
	m_bBlend = 				kOther.m_bBlend;
	m_bDepthTest = 		kOther.m_bDepthTest;
	m_bStencilTest = 		kOther.m_bStencilTest;
	m_iStencilOpFail = 	kOther.m_iStencilOpFail;
	m_iStencilOpZFail = 	kOther.m_iStencilOpZFail;
	m_iStencilOpZPass = 	kOther.m_iStencilOpZPass;
	m_iStencilFunc = 		kOther.m_iStencilFunc;
	m_iStencilFuncRef = 	kOther.m_iStencilFuncRef;
	m_iStencilFuncMask = kOther.m_iStencilFuncMask;
	m_iBlendSrc = 			kOther.m_iBlendSrc;
	m_iBlendDst = 			kOther.m_iBlendDst;
	m_bColorMask = 		kOther.m_bColorMask;
	m_bDepthMask = 		kOther.m_bDepthMask;
	m_bUseShader=			kOther.m_bUseShader;
			
	return (*this);
}


ZMaterial::ZMaterial()
{
	m_pkScript = NULL;
	
	//setup material ID
	m_iID = m_iNextID;
	m_iNextID++;
	
	SetupEnums();
	Clear();

	ZMaterialSettings* first = AddPass();	
	
}

ZMaterial::~ZMaterial()
{
	Clear();
}

ZMaterial& ZMaterial::operator=(const ZMaterial& kOther)
{
	Clear();

	int iPasses = kOther.GetNrOfPasses();	
	for(int i =0;i<iPasses;i++)
	{
		ZMaterialSettings* pkNew = AddPass();
		(*pkNew) = *kOther.GetPass(i); 
	}
	
	int iValues = kOther.m_kMaterialParameters.size();
	for(int i =0;i<iValues;i++)
	{
		m_kMaterialParameters.push_back(kOther.m_kMaterialParameters[i]);	
	}
	
	m_pkScript = 			NULL;
	
	
	m_bCastShadow 		= kOther.m_bCastShadow;
	m_bBoneTransform 	= kOther.m_bBoneTransform;
	m_bTextureOffset	= kOther.m_bTextureOffset;
	m_bReceiveShadow	= kOther.m_bReceiveShadow;
	
	return (*this);
}


ZMaterialSettings* ZMaterial::GetPass(int iPass) const
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
 		delete m_kPasses[i];	
	}
	
	m_kPasses.clear();

	m_bBoneTransform		= true;
	m_bCastShadow			= true;
	m_bTextureOffset		= false;
	m_bReceiveShadow		= true;
	
	m_strName = "UnNamed";
	
	m_kMaterialParameters.clear();

}


bool ZMaterial::LoadLuaMaterial(const string& strFile,bool bDontClear)
{
	if(!bDontClear)
		Clear();

	//get script system pointer
	if(!m_pkScript)
		m_pkScript = static_cast<ZSSScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZSSScriptSystem"));

	if(SI_ZMATERIAL::g_pkMaterialScript)
	{
		cerr<<"Tried to load material, but another material is already being loaded"<<endl;
		return false;
	}
		
		
	//create script object
	SI_ZMATERIAL::g_pkMaterialScript = new ZFScript;
	
	
	//load script
	if(SI_ZMATERIAL::g_pkMaterialScript->Create(strFile))
	{
 		SI_ZMATERIAL::g_pkCurrentMaterial = this;
 		SI_ZMATERIAL::g_bMaterialLoadError = false;
 		
 		m_strName = strFile;
		if(m_pkScript->Call(SI_ZMATERIAL::g_pkMaterialScript, "Main",0,0))
		{
			//finaly get global parameters
			LuaMaterialGetGlobals();
		
			//clear and remove script
			delete SI_ZMATERIAL::g_pkMaterialScript;
			SI_ZMATERIAL::g_pkMaterialScript = NULL;
			SI_ZMATERIAL::g_pkCurrentMaterial = NULL;	
			
			//check if there was any material loading errors
			if(SI_ZMATERIAL::g_bMaterialLoadError)
			{
				return false;
			}
			
			if(m_kPasses.size() == 0)
				cerr<<"Warning: material "<<strFile<<" has no passes"<<endl;
			
			return true;		
		}
		
		cerr<<"Error: Lua material "<<strFile<< " is broken"<<endl;
		
	}

	
	delete SI_ZMATERIAL::g_pkMaterialScript;
	SI_ZMATERIAL::g_pkMaterialScript = NULL;
	SI_ZMATERIAL::g_pkCurrentMaterial = NULL;
	
	return false;
}

void ZMaterial::LuaMaterialGetGlobals()
{
	if(!SI_ZMATERIAL::g_pkMaterialScript || !SI_ZMATERIAL::g_pkCurrentMaterial)
	{
		cerr<<"ERROR: no material is being loaded"<<endl;
		return;
	}

	char czTemp[128];

	if(PassGetLuaChar("bonetransform",czTemp))
		m_bBoneTransform = GetTranslateEnum(czTemp);
		
	if(PassGetLuaChar("castshadow",czTemp))
		m_bCastShadow = GetTranslateEnum(czTemp);
		
	if(PassGetLuaChar("receiveshadow",czTemp))
		m_bReceiveShadow = GetTranslateEnum(czTemp);
	
		
}

bool ZMaterial::LuaMaterialEndPass(int iPass)
{
	ZMaterialSettings* newpass =  GetPass(iPass);
	
	if(!newpass)
	{
		cout<<"ERROR: LuaMaterialEndPass  missin pass , this shuld not hapen"<<endl;
		return false;	
	}
		
	char 		ctemp[128];	
	double 	dtemp;
	
	//LOAD PASS SPECIFIC SETTINGS
	
	//GLSL program
	string strSLVS;
	string strSLFS;
	if(PassGetLuaChar("slvertexshader",ctemp))
		strSLVS = ctemp;
	if(PassGetLuaChar("slfragmentshader",ctemp))
		strSLFS = ctemp;
	if(!strSLVS.empty() || !strSLFS.empty())
	{
		if(!newpass->m_pkSLP->SetRes(strSLVS+string("#")+strSLFS+string(".glsl")))
			return false;
	}

	// ARB vertex program
	if(PassGetLuaChar("vertexprogram",ctemp))
	{
		if(!newpass->m_pkVP->SetRes(ctemp))
			return false;
	}
	// ARB fragment program	
	if(PassGetLuaChar("fragmentprogram",ctemp))
	{
		if(!newpass->m_pkFP->SetRes(ctemp))
			return false;
	}
			
	//textures
	if(PassGetLuaChar("texture0",ctemp))
		newpass->m_pkTUs[0]->SetRes(ctemp);
	if(PassGetLuaChar("texture1",ctemp))
		newpass->m_pkTUs[1]->SetRes(ctemp);
	if(PassGetLuaChar("texture2",ctemp))
		newpass->m_pkTUs[2]->SetRes(ctemp);
	if(PassGetLuaChar("texture3",ctemp))
		newpass->m_pkTUs[3]->SetRes(ctemp);

	//texutre cordinats
	if(PassGetLuaChar("texcords0",ctemp))
		newpass->m_iTUTexCords[0] = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("texcords1",ctemp))
		newpass->m_iTUTexCords[1] = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("texcords2",ctemp))
		newpass->m_iTUTexCords[2] = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("texcords3",ctemp))
		newpass->m_iTUTexCords[3] = GetTranslateEnum(ctemp);

	
	//texutre enviroment mode
	if(PassGetLuaChar("texenvmode0",ctemp))
		newpass->m_iTUTexEnvMode[0] = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("texenvmode1",ctemp))
		newpass->m_iTUTexEnvMode[1] = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("texenvmode2",ctemp))
		newpass->m_iTUTexEnvMode[2] = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("texenvmode3",ctemp))
		newpass->m_iTUTexEnvMode[3] = GetTranslateEnum(ctemp);

		
	//texture generation scale
	if(PassGetLuaDouble("texgenscale0",dtemp))
		newpass->m_fTUTexGenScale[0] = float(dtemp);
	if(PassGetLuaDouble("texgenscale1",dtemp))
		newpass->m_fTUTexGenScale[1] = float(dtemp);
	if(PassGetLuaDouble("texgenscale2",dtemp))
		newpass->m_fTUTexGenScale[2] = float(dtemp);
	if(PassGetLuaDouble("texgenscale3",dtemp))
		newpass->m_fTUTexGenScale[3] = float(dtemp);

		
	if(PassGetLuaChar("polygonmodefront",ctemp))
		newpass->m_iPolygonModeFront = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("polygonmodeback",ctemp))
		newpass->m_iPolygonModeBack = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("depthfunc",ctemp))
		newpass->m_iDepthFunc = GetTranslateEnum(ctemp);

	//lots a bools
	if(PassGetLuaChar("fog",ctemp))
		newpass->m_bFog = bool(GetTranslateEnum(ctemp));
	if(PassGetLuaChar("colormask",ctemp))
		newpass->m_bColorMask = bool(GetTranslateEnum(ctemp));
	if(PassGetLuaChar("depthmask",ctemp))
		newpass->m_bDepthMask = bool(GetTranslateEnum(ctemp));		
	if(PassGetLuaChar("depthtest",ctemp))
		newpass->m_bDepthTest = bool(GetTranslateEnum(ctemp));
	if(PassGetLuaChar("stenciltest",ctemp))
		newpass->m_bStencilTest = bool(GetTranslateEnum(ctemp));		
	if(PassGetLuaChar("lighting",ctemp))
		newpass->m_bLighting = bool(GetTranslateEnum(ctemp));
	if(PassGetLuaChar("cullface",ctemp))
		newpass->m_iCullFace = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("alphatest",ctemp))
		newpass->m_bAlphaTest = bool(GetTranslateEnum(ctemp));		
	if(PassGetLuaChar("colormaterial",ctemp))
		newpass->m_bColorMaterial = bool(GetTranslateEnum(ctemp));		
				
	if(PassGetLuaChar("useshader",ctemp))
		newpass->m_bUseShader = bool(GetTranslateEnum(ctemp));
		
		
		
	//blending
	if(PassGetLuaChar("blend",ctemp))
		newpass->m_bBlend = bool(GetTranslateEnum(ctemp));
	if(PassGetLuaChar("blendsrc",ctemp))
		newpass->m_iBlendSrc = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("blenddst",ctemp))
		newpass->m_iBlendDst = GetTranslateEnum(ctemp);

		
	//stencil stuff
	if(PassGetLuaChar("stencilopfail",ctemp))
		newpass->m_iStencilOpFail = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("stencilopzfail",ctemp))
		newpass->m_iStencilOpZFail = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("stencilopzpass",ctemp))
		newpass->m_iStencilOpZPass = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("stencilfunc",ctemp))
		newpass->m_iStencilFunc = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("stencilfuncref",ctemp))
		newpass->m_iStencilFuncRef = GetTranslateEnum(ctemp);
	if(PassGetLuaChar("stencilfuncmask",ctemp))
		newpass->m_iStencilFuncMask = GetTranslateEnum(ctemp);

	//alphatest treshold
	if(PassGetLuaDouble("alphatreshold",dtemp))
		newpass->m_fAlphaTreshold = float(dtemp);

	//line width
	if(PassGetLuaDouble("linewidth",dtemp))
		newpass->m_fLineWidth = float(dtemp);
	

	//get vertexcolor R G B A values
	if(PassGetLuaDouble("vertexcolorr",dtemp))
		newpass->m_kVertexColor.x = float(dtemp);
	if(PassGetLuaDouble("vertexcolorg",dtemp))
		newpass->m_kVertexColor.y = float(dtemp);
	if(PassGetLuaDouble("vertexcolorb",dtemp))
		newpass->m_kVertexColor.z = float(dtemp);
	if(PassGetLuaDouble("vertexcolora",dtemp))
		newpass->m_kVertexColor.w = float(dtemp);

	
	//material settings
	//ambient
	if(PassGetLuaDouble("matambientr",dtemp))
		newpass->m_kMatAmbient.x = float(dtemp);
	if(PassGetLuaDouble("matambientg",dtemp))
		newpass->m_kMatAmbient.y = float(dtemp);
	if(PassGetLuaDouble("matambientb",dtemp))
		newpass->m_kMatAmbient.z = float(dtemp);
	if(PassGetLuaDouble("matambienta",dtemp))
		newpass->m_kMatAmbient.w = float(dtemp);
	
	//diffuse
	if(PassGetLuaDouble("matdiffuser",dtemp))
		newpass->m_kMatDiffuse.x = float(dtemp);
	if(PassGetLuaDouble("matdiffuseg",dtemp))
		newpass->m_kMatDiffuse.y = float(dtemp);
	if(PassGetLuaDouble("matdiffuseb",dtemp))
		newpass->m_kMatDiffuse.z = float(dtemp);
	if(PassGetLuaDouble("matdiffusea",dtemp))
		newpass->m_kMatDiffuse.w = float(dtemp);

	
	//specular
	if(PassGetLuaDouble("matspecularr",dtemp))
		newpass->m_kMatSpecular.x = float(dtemp);
	if(PassGetLuaDouble("matspecularg",dtemp))
		newpass->m_kMatSpecular.y = float(dtemp);
	if(PassGetLuaDouble("matspecularb",dtemp))
		newpass->m_kMatSpecular.z = float(dtemp);
	if(PassGetLuaDouble("matspeculara",dtemp))
		newpass->m_kMatSpecular.w = float(dtemp);
	
	//emission
	if(PassGetLuaDouble("matemissionr",dtemp))
		newpass->m_kMatEmission.x = float(dtemp);
	if(PassGetLuaDouble("matemissiong",dtemp))
		newpass->m_kMatEmission.y = float(dtemp);
	if(PassGetLuaDouble("matemissionb",dtemp))
		newpass->m_kMatEmission.z = float(dtemp);
	if(PassGetLuaDouble("matemissiona",dtemp))
		newpass->m_kMatEmission.w = float(dtemp);

				
	//shininess
	if(PassGetLuaDouble("matshininess",dtemp))
		newpass->m_fShininess = float(dtemp);
		
		
	return true;
}

bool ZMaterial::PassGetLuaDouble(char* czName,double& dTemp)
{
	if(!SI_ZMATERIAL::g_pkMaterialScript)
	{
		cerr<<"Error: PassGetLuaDouble called but no material is being loaded"<<endl;
	}

	if(m_pkScript->GetGlobal(SI_ZMATERIAL::g_pkMaterialScript->m_pkLuaState,czName,dTemp))
	{
		m_pkScript->RemoveGlobal(SI_ZMATERIAL::g_pkMaterialScript->m_pkLuaState,czName);
		return true;
	}
	
	return false;
}

bool ZMaterial::PassGetLuaChar(char* czName,char* cTemp)
{
	if(!SI_ZMATERIAL::g_pkMaterialScript)
	{
		cerr<<"Error: PassGetLuaChar called but no material is being loaded"<<endl;
	}

	if(m_pkScript->GetGlobal(SI_ZMATERIAL::g_pkMaterialScript->m_pkLuaState,czName,cTemp))
	{
		m_pkScript->RemoveGlobal(SI_ZMATERIAL::g_pkMaterialScript->m_pkLuaState,czName);
		return true;
	}
	
	return false;
}

bool ZMaterial::LoadIniMaterial(const string& strFile,bool bDontClear)
{
	
	
	//TRY to load ZLM material first
	string zlmfile = strFile;
	zlmfile = zlmfile.substr(0,zlmfile.length()-3)+string("zlm");
	if(LoadLuaMaterial(zlmfile.c_str(),bDontClear))
		return true;
		

	bool open=false;
	
	//try to open material file
	if(m_kIni.Open(strFile.c_str(),0))		
		open=true;
	else		//if file didt open, try to load a default texture
	{
		cerr<<"Error loading material: "<<strFile<<" using nomaterial"<<endl;
		if(m_kIni.Open("data/material/nomaterial.zmt",0))
			open=true;
	}

	if(open)
	{
		if(!bDontClear)
			Clear();
	
		m_strName = strFile;
		
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
		cout<<"error loading shader:"<<strFile<<endl;
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

	if(m_kIni.KeyExist("global","bonetransform"))
		m_bBoneTransform = m_kIni.GetBoolValue("global","bonetransform");
	
	if(m_kIni.KeyExist("global","castshadow"))
		m_bCastShadow = m_kIni.GetBoolValue("global","castshadow");
	
	if(m_kIni.KeyExist("global","receiveshadow"))
		m_bReceiveShadow = m_kIni.GetBoolValue("global","receiveshadow");
		

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
		newpass->m_pkTUs[0]->SetRes(m_kIni.GetValue(passname.c_str(),"tu0"));
	if(m_kIni.KeyExist(passname.c_str(),"tu1"))
		newpass->m_pkTUs[1]->SetRes(m_kIni.GetValue(passname.c_str(),"tu1"));
	if(m_kIni.KeyExist(passname.c_str(),"tu2"))
		newpass->m_pkTUs[2]->SetRes(m_kIni.GetValue(passname.c_str(),"tu2"));
	if(m_kIni.KeyExist(passname.c_str(),"tu3"))
		newpass->m_pkTUs[3]->SetRes(m_kIni.GetValue(passname.c_str(),"tu3"));

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
	
	if(m_kIni.KeyExist(passname.c_str(),"useshader"))
		newpass->m_bUseShader = m_kIni.GetBoolValue(passname.c_str(),"useshader");
	
	
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


bool ZMaterial::Create(const string& strFile)
{
	return LoadMaterial(strFile);
} 

bool ZMaterial::LoadMaterial(string strName, bool bDontClear)
{

	if(strName.find("data/material/") == -1)
		strName ="data/material/" + strName;
// 	else
// 		cerr<<"Warning: old resource path:"<<strName<<endl;
	
	bool bLoaded = false;
	
	if(strName.substr(strName.length()-4) == ".zmt")
	{
		//cout<<"loading ini material"<<endl;
	 	if(LoadIniMaterial(strName.c_str(),bDontClear))
	 		bLoaded = true;
	}
	else if(strName.substr(strName.length()-4) == ".zlm")
	{
		//cout<<"loading lua material"<<endl;
	 	if(LoadLuaMaterial(strName,bDontClear))
	 		bLoaded = true;
	}
	
	if(bLoaded)
		return true;
	else
	{
 		cerr<<"WARNING: Missing material "<<strName<<" using nomaterial.zmt"<<endl;
 		return LoadIniMaterial("data/material/nomaterial.zmt",bDontClear);	
	}
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
	m_kEnums["ZERO_BLEND_DST"] = 						0;
	m_kEnums["ONE_BLEND_DST"] = 						1;
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

int ZMaterial::GetTranslateEnum(const string& strEnum)
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

void ZMaterial::SetParameterValue(const string& strName,const string& strValue)
{
	//check if already added
	for(int i= 0;i<m_kMaterialParameters.size();i++)
	{
		if(m_kMaterialParameters[i].first == strName)
		{
			m_kMaterialParameters[i].second = strValue;
			return;
		}
	}

	//add value
	m_kMaterialParameters.push_back(pair<string,string>(strName,strValue));		
}

string ZMaterial::GetParameterValue(const string& strName)
{
	//check if already added
	for(int i= 0;i<m_kMaterialParameters.size();i++)
	{
		if(m_kMaterialParameters[i].first == strName)
			return m_kMaterialParameters[i].second;
	}


	//cerr<<"Warning: material "<<m_strName<<" tried to get unknown material parameter "<<strName<<endl;

	return "none";
}

ZFResource* Create__Material()
{
	return new ZMaterial;						// LEAK - MistClient, Level loaded.
}







/**	\class Material 
		\ingroup NewSi

		Scriptfunctions that can only be used inside a material script (.zlm). 
*/

// #include "../engine_systems/script_interfaces/si_objectmanager.h" 
// using namespace ObjectManagerLua;
// extern ZFScriptSystem* 	g_pkScript;

//MATERIAL LUA INTERFACE
namespace SI_ZMATERIAL
{
	ZFScript*			g_pkMaterialScript = 	NULL;
	ZMaterial*			g_pkCurrentMaterial = 	NULL;
	int					g_iCurrentMaterialPass = -1;
	bool					g_bMaterialLoadError = 	false;
	ZSSScriptSystem*	g_pkScript = 				NULL;
	ZSSZeroFps*			g_pkZeroFps = 				NULL;
	ZShaderSystem*		g_pkZShaderSystem = 		NULL;
	
/**	\fn PassBegin( iPass )
		\param iPass Rendering pass to start setting parameters for.
		\brief Starts a new rendering pass in a material file.
		\relates Material
*/	
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

/**	\fn PassEnd( )
		\brief Ends the current rendering pass in a material file.
		\relates Material
*/	
	int PassEndLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,0))
			return 0;		

		if(!g_pkCurrentMaterial)
		{
			cout<<"you can only call this function in a material script"<<endl;
			return 0;
		}			
					
		//return value
		double dRet = 1;	
					
		//cout<<"ending pass"<<endl;
		if(!g_pkCurrentMaterial->LuaMaterialEndPass(g_iCurrentMaterialPass))
		{
			g_bMaterialLoadError = true;
			dRet = 0;
		}	
						
						
		g_pkScript->AddReturnValue(pkLua, dRet);		
		return 1;		
	}		
	
/**	\fn SupportGLSLProgram( )
		\return Returns 1 if current rendering system supports GL Slang programs. 0 if not.
		\brief Used to see if gl slang is supported.
		\relates Material
*/		
	int SupportGLSLProgramLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,0))
			return 0;		

		double dRet = 0;
			
		if(g_pkZShaderSystem->SupportGLSLProgram())
			dRet = 1;	
		
		g_pkScript->AddReturnValue(pkLua, dRet);				
		return 1;		
	}		
	
/**	\fn UseShadowMapLua( )
		\brief *** Unknown ZMaterial.cpp:965.
		\relates Material
*/		
	int UseShadowMapLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,0))
			return 0;		

		double dRet = 0;					
		if(g_pkZeroFps->GetShadowMap())
			dRet = 1;	
		
		g_pkScript->AddReturnValue(pkLua, dRet);				
		return 1;		
	}			
	
	
/**	\fn SupportSpecMapLua( )
		\brief *** Unknown ZMaterial.cpp:965.
		\relates Material
*/		
	int SupportSpecMapLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,0))
			return 0;		

		double dRet = 0;					
		if(g_pkZShaderSystem->SupportGLSLProgram() && g_pkZShaderSystem->UseSpecMap())
			dRet = 1;	
		
		g_pkScript->AddReturnValue(pkLua, dRet);				
		return 1;		
	}	
				
/**	\fn SupportNormalMapLua( )
		\brief *** Unknown ZMaterial.cpp:965.
		\relates Material
*/		
	int SupportNormalMapLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,0))
			return 0;		

		double dRet = 0;					

		if(g_pkZShaderSystem->SupportGLSLProgram() && g_pkZShaderSystem->UseNormalMap())
			dRet = 1;	
		
		g_pkScript->AddReturnValue(pkLua, dRet);				
		return 1;		
	}					
				
/**	\fn SetParameterLua(string,string )
		\brief adds a custom value to the material , like surface sounds
		\relates Material
*/		
	int SetParameterLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,2))
			return 0;		

		string strValueName;
		string strValue;

		g_pkScript->GetArgString(pkLua,0,strValueName);
		g_pkScript->GetArgString(pkLua,1,strValue);

		g_pkCurrentMaterial->SetParameterValue(strValueName,strValue);
		return 0;		
	}	
	
/**	\fn GetParameterLua(string,string )
		\brief returns parameter value
		\relates Material
*/		
	int GetParameterLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,1))
			return 0;		

		string strValueName;
		
		//get parameter name
		g_pkScript->GetArgString(pkLua,0,strValueName);

		//get parameter value
		string strValue = g_pkCurrentMaterial->GetParameterValue(strValueName);

		//return value
		g_pkScript->AddReturnValue(pkLua, (char*)strValue.c_str(), strValue.size());

		return 1;		
	}		
	
/**	\fn LoadMaterialLua(string,string )
		\brief loads a parentmaterial
		\relates Material
*/		
	int LoadMaterialLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,1))
			return 0;		

		string strMaterial;
		
		//get parameter name
		g_pkScript->GetArgString(pkLua,0,strMaterial);

		//backup material script pointer and material name
		ZFScript* pkScriptBakup = 		g_pkMaterialScript;
		ZMaterial* pkMaterialBakup = 	g_pkCurrentMaterial;
		string strNameBakup = 			g_pkCurrentMaterial->GetName();
		
		//clear current material script		
		g_pkMaterialScript = NULL;				
		
		//return value
		double dRet = 1;					
		
		//load parent material 
		if(!g_pkCurrentMaterial->LoadMaterial(strMaterial,true))
		{
			cerr<<"Warning: parent material "<<strMaterial<<" could not be loaded for material "<<strNameBakup<<endl;
			dRet = 0;
		}
		
		//restore pointers and name
		g_pkMaterialScript = 	pkScriptBakup;
		g_pkCurrentMaterial = 	pkMaterialBakup;
		g_pkCurrentMaterial->SetName(strNameBakup);

		g_pkScript->AddReturnValue(pkLua, dRet);				
		return 1;		
	}		
}


void RegisterSI_Material()
{
	SI_ZMATERIAL::g_pkScript = 			static_cast<ZSSScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZSSScriptSystem"));
	SI_ZMATERIAL::g_pkZeroFps = 			static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	SI_ZMATERIAL::g_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	
	SI_ZMATERIAL::g_pkScript->ExposeFunction("PassBegin",				SI_ZMATERIAL::PassBeginLua);
	SI_ZMATERIAL::g_pkScript->ExposeFunction("PassEnd",				SI_ZMATERIAL::PassEndLua);
	SI_ZMATERIAL::g_pkScript->ExposeFunction("SupportGLSLProgram",	SI_ZMATERIAL::SupportGLSLProgramLua);
	SI_ZMATERIAL::g_pkScript->ExposeFunction("UseShadowMap",			SI_ZMATERIAL::UseShadowMapLua);
	SI_ZMATERIAL::g_pkScript->ExposeFunction("SupportSpecMap",		SI_ZMATERIAL::SupportSpecMapLua);
	SI_ZMATERIAL::g_pkScript->ExposeFunction("SupportNormalMap",	SI_ZMATERIAL::SupportNormalMapLua);

	SI_ZMATERIAL::g_pkScript->ExposeFunction("SetMaterialParameter",	SI_ZMATERIAL::SetParameterLua);
	SI_ZMATERIAL::g_pkScript->ExposeFunction("GetMaterialParameter",	SI_ZMATERIAL::GetParameterLua);
	SI_ZMATERIAL::g_pkScript->ExposeFunction("LoadMaterial",				SI_ZMATERIAL::LoadMaterialLua);
};








