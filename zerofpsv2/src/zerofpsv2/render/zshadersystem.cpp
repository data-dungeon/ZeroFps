#include "zshadersystem.h"

ZShaderSystem::ZShaderSystem() : ZFSubSystem("ZShaderSystem")
{
	
};

bool ZShaderSystem::StartUp()
{
 	m_pkTexMan	= static_cast<TextureManager*>(GetSystem().GetObjectPtr("TextureManager"));
 	m_pkLight	= static_cast<Light*>(GetSystem().GetObjectPtr("Light"));

	return true;
}

bool ZShaderSystem::ShutDown()
{
	
	return true;
}