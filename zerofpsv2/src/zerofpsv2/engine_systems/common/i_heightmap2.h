// iheightmap2.h: interface for the IHeightMap2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IHEIGHTMAP2_H__E0152B70_906A_4C33_8A35_B32617FB3D2B__INCLUDED_)
#define AFX_IHEIGHTMAP2_H__E0152B70_906A_4C33_8A35_B32617FB3D2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../basic/vector3.h"
#include "../engine_systems_x.h"
#include "../../basic/zfresource.h"

class ENGINE_SYSTEMS_API I_HeightMap2 
{
public:
	I_HeightMap2();
	virtual ~I_HeightMap2();

	virtual bool Loaded() = 0;
	virtual void UpdateRecLodLevel(Vector3 kCamPos) = 0;
	virtual Vector3 GetPos() = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual float GetScale() = 0;
	virtual ZFResourceHandle* GetMaterial() = 0;

};

#endif // !defined(AFX_IHEIGHTMAP2_H__E0152B70_906A_4C33_8A35_B32617FB3D2B__INCLUDED_)
