#include "p_clientunit.h"
#include "p_renderselection.h"


P_ClientUnit::P_ClientUnit()
{
	strcpy(m_acName,"P_ClientUnit");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_ALL;
	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	
	bNetwork = true;

	m_kInfo.m_cTeam = 		0;
	m_kInfo.m_cHealth = 	255;
	m_kInfo.m_cWeapon = 	0;
	m_kInfo.m_cArmor = 		0;
	m_kInfo.m_cPropultion =0;
	m_kInfo.m_kName = 		"NoName";
	
	m_bSelected =	false;
	
	m_bCurrentSelectionRenderState = false;

}


void P_ClientUnit::Update()
{
	if(m_pkFps->m_bClientMode)
	{
		//enable rendering of selection
		if(m_bSelected && !m_bCurrentSelectionRenderState)
			EnableSelectionRender();
		
		//disable rendering of selection
		if(!m_bSelected && m_bCurrentSelectionRenderState)
			DisableSelectionRender();
	}

}


void P_ClientUnit::EnableSelectionRender()
{
	P_RenderSelection* rs = (P_RenderSelection*)m_pkObject->GetProperty("P_RenderSelection");
	
	if(rs == NULL)
	{
		m_pkObject->AddProperty("P_RenderSelection");
		rs = (P_RenderSelection*)m_pkObject->GetProperty("P_RenderSelection");
	}
	
	if(rs == NULL) 	
		cout << "Something is wrong with P_RenderSelection"<<endl;
	
	rs->m_bEnabled = true;
	m_bCurrentSelectionRenderState = true;
}


void P_ClientUnit::DisableSelectionRender()
{
	P_RenderSelection* rs = (P_RenderSelection*)m_pkObject->GetProperty("P_RenderSelection");
	
	if(rs == NULL)
	{
		m_pkObject->AddProperty("P_RenderSelection");
		rs = (P_RenderSelection*)m_pkObject->GetProperty("P_RenderSelection");
	}
	
	if(rs == NULL) 	
		cout << "Something is wrong with P_RenderSelection"<<endl;
	
	rs->m_bEnabled = false;
	m_bCurrentSelectionRenderState = false;

}

void P_ClientUnit::Save(ZFMemPackage* pkPackage)
{

}

void P_ClientUnit::Load(ZFMemPackage* pkPackage)
{

}

void P_ClientUnit::PackTo(NetPacket* pkNetPacket)
{
	pkNetPacket->Write(&m_kInfo, sizeof(m_kInfo));
}
 
void P_ClientUnit::PackFrom(NetPacket* pkNetPacket)
{
	pkNetPacket->Read(&m_kInfo, sizeof(m_kInfo));
}


COMMON_API Property* Create_P_ClientUnit()
{
	return new P_ClientUnit();

}
