// pathfindproperty.h: interface for the PathFindProperty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHFINDPROPERTY_H__9D005728_8255_440B_99C3_C8128F732F45__INCLUDED_)
#define AFX_PATHFINDPROPERTY_H__9D005728_8255_440B_99C3_C8128F732F45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common_x.h"
#include "../zerofps/engine/engine.pkg"

class PathFindProperty  
{
public:
	PathFindProperty();
	~PathFindProperty();

	void Update();
	void CloneOf(Property* pkProperty) { }
	void Init();

};

#endif // !defined(AFX_PATHFINDPROPERTY_H__9D005728_8255_440B_99C3_C8128F732F45__INCLUDED_)
