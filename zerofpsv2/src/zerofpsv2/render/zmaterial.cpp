#include "zmaterial.h"


ZMaterial::ZMaterial()
{
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