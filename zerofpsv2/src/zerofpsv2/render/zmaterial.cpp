#include "zmaterial.h"
#include "zshader.h"

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
	
	m_iPolygonModeFront = GL_LINE;
	m_iPolygonModeBack = GL_LINE;			
	
	m_bLighting = false;
	m_bCullFace = true;
	m_iDepthFunc = GL_LESS;
};

ZMaterial::ZMaterial()
{
	ZMaterialSettings* first = AddPass();
	
	first->m_iTUs[0] = 1; 

	m_bCopyData = false;
	m_bRandomMovements = false;
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