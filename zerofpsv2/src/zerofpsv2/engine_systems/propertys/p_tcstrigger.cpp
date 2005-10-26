#include "p_tcstrigger.h"
#include "../tcs/tcs.h"


P_TcsTrigger::P_TcsTrigger()
{
	strcpy(m_acName,"P_TcsTrigger");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;	
	
	m_bNetwork	= false;
	m_iVersion	= 1;
		
	m_pkTcs	= 		static_cast<Tcs*>(g_ZFObjSys.GetObjectPtr("Tcs"));
	m_pkRender = 	static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));
	
	
	
	m_iTriggerType = eSPHERE;
	m_fRadius = 0.5;
	m_kBoxSize.Set(0.5,0.5,0.5);
	m_iTriggerID = 1;
	
}

P_TcsTrigger::~P_TcsTrigger()
{
	Disable();
}

void P_TcsTrigger::Init()
{

}

void P_TcsTrigger::Enable()
{
	m_pkTcs->AddTrigger(this);
}
void P_TcsTrigger::Disable()
{
	m_pkTcs->RemoveTrigger(this);
}


void P_TcsTrigger::Update()
{	
	Enable();
	
	m_iType=0;
	m_iSide=0;

}

void P_TcsTrigger::Trigger(P_Tcs* pkTcs)
{
	static vector<ScriptFuncArg> args(1);
	static int iID;

	iID = pkTcs->GetEntity()->GetEntityID();
	args[0].m_kType.m_eType = tINT;
	args[0].m_pData = &iID;	
	m_pkEntityManager->CallFunction(m_pkEntity, "Trigger",&args);
}


vector<PropertyValues> P_TcsTrigger::GetPropertyValues()
{
	vector<PropertyValues> kReturn(4);
	
	
	kReturn[0].kValueName="type";
	kReturn[0].iValueType=VALUETYPE_INT;
	kReturn[0].pkValue=(void*)&m_iTriggerType;	
	
	kReturn[1].kValueName="radius";
	kReturn[1].iValueType=VALUETYPE_FLOAT;
	kReturn[1].pkValue=(void*)&m_fRadius;	

	kReturn[2].kValueName="boxsize";
	kReturn[2].iValueType=VALUETYPE_VECTOR3;
	kReturn[2].pkValue=(void*)&m_kBoxSize;				
	
	kReturn[3].kValueName="id";
	kReturn[3].iValueType=VALUETYPE_INT;
	kReturn[3].pkValue=(void*)&m_iTriggerID;				
	
	return kReturn;
	
}

void P_TcsTrigger::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(m_iTriggerType);
	pkPackage->Write(m_fRadius);
	pkPackage->Write(m_kBoxSize);
	pkPackage->Write(m_iTriggerID);
}

void P_TcsTrigger::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read(m_iTriggerType);
	pkPackage->Read(m_fRadius);
	pkPackage->Read(m_kBoxSize);
	pkPackage->Read(m_iTriggerID);
}


Property* Create_P_TcsTrigger()
{
	return new P_TcsTrigger;
}

void ENGINE_SYSTEMS_API Register_PTcsTrigger(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_TcsTrigger", Create_P_TcsTrigger);	

	// Register Constants
	pkZeroFps->m_pkScript->RegisterConstant("TRIGGER_SPHERE", eSPHERE);
	pkZeroFps->m_pkScript->RegisterConstant("TRIGGER_BOX", eBOX);
}
