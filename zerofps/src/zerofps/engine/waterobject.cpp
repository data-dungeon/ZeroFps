#include "waterobject.h"

WaterObject::WaterObject(int iSize,int iStep,const char* acTexture)
{
	AddProperty(new WaterRenderProperty(iSize,iStep,acTexture));
}

void WaterObject::SetTexture(const char* acTexture)
{
	WaterRenderProperty *water = static_cast<WaterRenderProperty*>(GetProperty("WaterRenderProperty"));
	water->SetTexture(acTexture);
}



