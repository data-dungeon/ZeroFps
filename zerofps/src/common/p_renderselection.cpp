#include "p_renderselection.h"

P_RenderSelection::P_RenderSelection()
{
	strcpy(m_acName,"P_RenderSelection");
	
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_bEnabled =	false;
}

void P_RenderSelection::Update()
{
	if(m_bEnabled)
	{
		glDisable(GL_LIGHTING);
		m_pkRender->Sphere(m_pkObject->GetPos(), m_pkObject->GetRadius(),10,Vector3(0,1,1),false);
		glEnable(GL_LIGHTING);
	}

}


COMMON_API Property* Create_P_RenderSelection()
{
	return new P_RenderSelection();
}



